// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file SimpleConfig.cpp
 *
 * There are a number of aspects of these classes that are not ideal,
 * however the interface is simple enough to be stable and the
 * implementation can be "purified" later (if desired) without 
 * affecting its use.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning (disable : 4786)
# endif

#ifdef _WIN32
#define snprintf _snprintf
#endif

#include "SimpleConfig.h"
#include "Platform.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <assert.h>

#include "compiler.h"
#include STL_SSTREAM


// characters surrounding section names ('[' and ']' for typical windows .ini)
#define SECTION_START	'['
#define SECTION_END	']'
#define COMMENT_CHAR	';'
#define EQUAL_CHAR	'='


const std::string WHITESPACE = " \t\r\n";

// trim leading and trailing whitespace
static std::string Trim(const std::string &s) {
	int i, j;
	if (s.length() == 0) return s;
	i = s.find_first_not_of(WHITESPACE);
	j = s.find_last_not_of(WHITESPACE);
	if (i < 0 || i > j) return "";
	return s.substr(i, j+1);
}


// class Error

void Error::setFullError(const std::string &s) {
	m_fullerror = s;
}

std::string Error::getFullError() {
	return m_fullerror;
}

std::string Error::getError() {
	return "Error: " + m_msg + ".";
}


Error::~Error() {
	if (m_display) {
		std::cerr << m_fullerror << std::endl;
	}
}


// class ConfigError

ConfigError::ConfigError(const std::string &msg, const std::string &filename, 
	    int line_no, const std::string &line):
	    Error(msg) {
	char lstr[20];
	m_filename = filename;
	m_line_no = line_no;
	m_line = line;
	snprintf(lstr, 20, "%d", m_line_no);
	setFullError(getError() +  "\n" + m_filename + ":" + lstr + ": " + m_line);
}


std::string ConfigError::getError() {
	return "Configuration file error: " + m_msg + ".";
}


// internal class for storing config data
class ConfigElement {
public:
	ConfigElement() {}
	virtual ~ConfigElement() {}
	virtual void write(std::ostream &os) const {}
};

// internal class for storing config comments
class ConfigComment: public ConfigElement {
public:
	ConfigComment(const std::string &comment) { m_comment = comment; }
	virtual ~ConfigComment() {}
	virtual void write(std::ostream &os) const {
		os << m_comment << std::endl;
	}
	const std::string &getComment() const { return m_comment; }
private:
	std::string m_comment;
};

// internal class for storing config section headers
class ConfigSection: public ConfigElement {
public:
	ConfigSection(const std::string &section) { m_section = section; }
	virtual ~ConfigSection() {}
	virtual void write(std::ostream &os) const {
		os << SECTION_START << m_section << SECTION_END << std::endl;
	}
	const std::string &getName() const { return m_section; }
private:
	std::string m_section;
};

// internal class for storing config key-value pairs
class ConfigValue: public ConfigElement {
public:
	ConfigValue(const std::string &section, const std::string &key,
	            const std::string &value) {
		m_section = section;
		m_key = key;
		setValue(value);
	}
	virtual ~ConfigValue() {}
	virtual void write(std::ostream &os) const {
		int n = m_value.length();
		if (n > 0 && (m_value[0] == ' ' || m_value[0] == '\t' || \
		              m_value[n-1] == ' ' || m_value[n-1] == '\t' || \
			      m_value[0] == '"')) {
			os << m_key << " = \"" << m_value << "\"" << std::endl;
		} else {
			os << m_key << " = " << m_value << std::endl;
		}
	}
	const std::string &getSection() const { return m_section; }
	const std::string &getKey() const { return m_key; }
	std::string getValue() const { return m_value; }
	void setValue(const std::string &value) {
		int n = value.length();
		m_value = value;
		if (n > 0 && value[0] == '"') {
			if (value[n-1] == '"') n--;
			m_value = m_value.substr(1, n-1);
		}
	}
private:
	std::string m_section;
	std::string m_key;
	std::string m_value;
};



// class SimpleConfig

void SimpleConfig::clear() {
	ElementList::iterator i;
	m_dict.clear();
	m_last.clear();
	for (i = m_elements.begin(); i != m_elements.end(); i++) {
		delete *i;
	}
	m_elements.clear();
	m_modified = false;
}

void SimpleConfig::dump(std::ostream &os) const {
	ElementList::const_iterator i;
	for (i=m_elements.begin(); i != m_elements.end(); i++) {
		(*i)->write(os);
	}
}

void SimpleConfig::_addComment(const std::string &comment) {
	m_elements.push_back(new ConfigComment(comment));
}

void SimpleConfig::_addSection(const std::string &section) {
	ConfigSection *element = new ConfigSection(section);
	m_elements.push_back(element);
	m_last[section] = element;
}

std::string SimpleConfig::_hash_index(const std::string &section, const std::string &key) const {
	return section + "^^^" + key;
}

void SimpleConfig::_addValue(const std::string &section, const std::string &key, const std::string &value) {
	if (key != "") {
		ConfigElement *element = new ConfigValue(section, key, value);
		ConfigDictionary::iterator last = m_last.find(section);
		if (last != m_last.end()) {
			ElementList::iterator at;
			at = std::find(m_elements.begin(), m_elements.end(), last->second);
			if (at != m_elements.end()) ++at;
			m_elements.insert(at, element);
		} else {
			_addComment("");
			_addSection(section);
			m_elements.push_back(element);
		}
		std::string index = _hash_index(section, key);
		m_dict[index] = element;
		m_last[section] = element;
	}
}

void SimpleConfig::_parse(std::istream &is) {
	std::vector<std::string> lines;
	std::vector<std::string>::iterator L;
	std::string line;
	int state = 0;
	line.reserve(256);
	while (is.good() && !is.eof()) {
		char buffer[1024];
		is.getline(buffer, 1024);
		buffer[1023] = 0;
		lines.push_back(buffer);
	}
	line = "";
	state = 0;
	std::string section;
	std::string comment;
	int line_no = 0;
	for (L = lines.begin(); L != lines.end(); L++) {
		line = Trim(*L);
		line_no++;
		if (line == "") {
			if (L+1 != lines.end()) {
				_addComment(line);
			}
		} else {
			char c = line[0];
			if (c == SECTION_START) {
				int e = line.find(SECTION_END);
				if (e < 0) {
					throw ConfigError("missing end-section tag", m_file, line_no, *L);
				}
				section = line.substr(1, e-1);
				_addSection(section);
			} else
			if (c == COMMENT_CHAR) {
				_addComment(line);
			} else {
				int eq = line.find_first_of(EQUAL_CHAR);
				if (eq < 0) {
					throw ConfigError("missing equal sign in key-value pair", m_file, line_no, *L);
				}
				std::string key = Trim(line.substr(0, eq));
				std::string value = Trim(line.substr(eq+1));
				_addValue(section, key, value);
			}
		} 
	}
}

ConfigValue *SimpleConfig::_find(const std::string &section, const std::string &key) {
	std::string index = _hash_index(section, key);
	ConfigDictionary::iterator i = m_dict.find(index);
	if (i == m_dict.end()) return 0;
	return dynamic_cast<ConfigValue*> ((*i).second);
}

void SimpleConfig::addComment(const std::string &comment) {
	_addComment(COMMENT_CHAR + comment);
}

int SimpleConfig::getInt(const std::string &section, const std::string &key, int default_, bool set) {
	ConfigValue *v = _find(section, key);
	if (v) {
		return atoi(v->getValue().c_str());
	} else 
	if (set) {
		setInt(section, key, default_);
	}
	return default_;
}

bool SimpleConfig::getBool(const std::string &section, const std::string &key, bool default_, bool set) {
	ConfigValue *v = _find(section, key);
	if (v) {
		std::string const &value = v->getValue();
		if (value == "true" || atoi(value.c_str())) {
			return true;
		} else {
			return false;
		}
	} else 
	if (set) {
		setBool(section, key, default_);
	}
	return default_;
}


double SimpleConfig::getFloat(const std::string &section, const std::string &key, double default_, bool set) {
	ConfigValue *v = _find(section, key);
	if (v) {
		return atof(v->getValue().c_str());
	} else 
	if (set) {
		setFloat(section, key, default_);
	}
	return default_;
}


std::string SimpleConfig::getString(const std::string &section, const std::string &key, std::string default_, bool set) {
	ConfigValue *v = _find(section, key);
	if (v) {
		return v->getValue();
	} else 
	if (set) {
		setString(section, key, default_);
	}
	return default_;
}

std::string SimpleConfig::getPath(const std::string &section, const std::string &key, std::string default_, bool set) {
	ConfigValue *v = _find(section, key);
	std::string native = ospath::filter(default_);
	if (v) {
		return ospath::filter(v->getValue());
	} else 
	if (set) {
		setString(section, key, native);
	}
	return native;
}

void SimpleConfig::setInt(const std::string &section, const std::string &key, int value) {
	char s[20];
	snprintf(s, 20, "%d", value);
	setString(section, key, s);
}

void SimpleConfig::setBool(const std::string &section, const std::string &key, bool value) {
	const char *s = "false";
	if (value) s = "true";
	setString(section, key, s);
}

void SimpleConfig::setFloat(const std::string &section, const std::string &key, double value) {
	char s[40];
	const char *fmt = "%lf";
	double positive = value> 0 ? value : -value;
	if (positive < 1e-4 || positive > 1e+4) fmt = "%le";
	snprintf(s, 40, fmt, value);
	setString(section, key, s);
}


void SimpleConfig::setString(const std::string &section, const std::string &key, const std::string &value) {
	ConfigValue *v = _find(section, key);
	if (v) {
		v->setValue(value);
	} else {
		_addValue(section, key, value);
	}
	m_modified = true;
	if (m_autosave) save();
}

void SimpleConfig::setPath(const std::string &section, const std::string &key, const std::string &value) {
	ConfigValue *v = _find(section, key);
	std::string native = ospath::filter(value);
	if (v) {
		v->setValue(native);
	} else {
		_addValue(section, key, native);
	}
	m_modified = true;
	if (m_autosave) save();
}

bool SimpleConfig::open(const std::string &fn) {
	if (fn != "") {
		std::filebuf fb;
		if (m_modified) save();
		clear();
		m_file = fn;
		fb.open(m_file.c_str(), std::ios::in);
		if (fb.is_open()) {
			std::istream is(&fb);
			_parse(is);
			fb.close();
			return true;
		}
	}
	return false;
}

void SimpleConfig::save() {
	if (m_modified && m_file != "") {
		std::filebuf fb;
		fb.open(m_file.c_str(), std::ios::out);
		if (fb.is_open()) {
			std::ostream os(&fb);
			dump(os);
			fb.close();
		}
	}
	m_modified = false;
}

bool SimpleConfig::hasKey(const char* section, const char* key) const {
	std::string index = _hash_index(section, key);
	ConfigDictionary::const_iterator i = m_dict.find(index);
	return (i != m_dict.end());
}

void SimpleConfig::delKey(const char* section, const char* key) {
	std::string index = _hash_index(section, key);
	ConfigDictionary::iterator i = m_dict.find(index);
	if (i != m_dict.end()) {
		ElementList::iterator at;
		at = std::find(m_elements.begin(), m_elements.end(), i->second);
		assert(at != m_elements.end());
		m_elements.erase(at);
		m_dict.erase(i);
		delete *at;
	}
}

bool SimpleConfig::hasSection(const char* section) const {
	ConfigDictionary::const_iterator i = m_last.find(section);
	return (i != m_last.end());
}


// This is a rather ugly and slow way to delete a section.  It
// can leave stay comments in the middle of sections, but will
// remove the section entirely, even if it is multiply declared.
// This whole function was a bit of an afterthought, since it 
// isn't very likely to be used in the application this class 
// was written for.  Sorry to all the purists out there.  :-(
void SimpleConfig::delSection(const char* section_name) {
	ConfigDictionary::iterator i = m_last.find(section_name);
	if (i != m_last.end()) {
		ElementList rebuild;
		ElementList::iterator at;
		bool blank = false;
		for (at = m_elements.begin(); at != m_elements.end(); at++) {
			ConfigSection *section = dynamic_cast<ConfigSection *>(*at);
			if (section && section->getName() == section_name) {
				delete *at;
				continue;
			}
			ConfigValue *value = dynamic_cast<ConfigValue *>(*at);
			if (value && value->getSection() == section_name) {
				std::string index = _hash_index(section_name, value->getKey());
				ConfigDictionary::iterator i = m_dict.find(index);
				if (i != m_dict.end()) m_dict.erase(i);
				delete *at;
				continue;
			}
			ConfigComment *comment = dynamic_cast<ConfigComment *>(*at);
			if (comment && comment->getComment() == "") {
				if (blank) {
					delete *at;
					continue;
				}
				blank = true;
			} else blank = false;
			rebuild.push_back(*at);
		}
		m_elements = rebuild;
		m_modified = true;
		if (m_autosave) save();
		m_last.erase(i);
	}
}


#ifdef __TEST__
/**
 * Configuration Test Script, compile with -D__TEST__ to run automatically.
 */
class config_test {
	public:
	config_test() {
		try {
			SimpleConfig c("test.ini");
			c.clear();
			c.save();
			if (c.isEmpty()) {
				c.addComment(" This file is automatically generated and update.  Do not modify!");
			}
			int i;
			i = c.getInt("section 1", "item 1", 1, true);
			i = c.getInt("section 1", "item 2", 2, true);
			i = c.getInt("section 1", "item 3", 3, true);
			c.setString("section 2", "item 1", "test 1 ");
			c.setString("section 2", "item 2", "test # 2");
			c.setInt("section 1", "item 1", 4);
			c.setFloat("section 2", "item 3", 0.00000000000031415);
			c.save();
			c.abort();
			c.open("test.ini");
			assert (c.getInt("section 1", "item 1") == 4);
			assert (c.getInt("section 1", "item 2") == 2);
			assert (c.getInt("section 1", "item 3") == 3);
			assert (c.getString("section 2", "item 1") == "test 1 ");
			assert (c.getString("section 2", "item 2") == "test # 2");
			c.delSection("section 3");
			c.dump(cout);
			cout << "All tests passed.\n";
		} catch (ConfigError &e) {
			cout << "Test failed.\n";
		}
	}
};
static config_test t;
#endif // __TEST__


