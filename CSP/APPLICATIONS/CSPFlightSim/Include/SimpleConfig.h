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
 * @file SimpleConfig.h
 *
 * There are a number of aspects of these classes that are not ideal,
 * however the interface is simple enough to be stable and the
 * implementation can be "purified" later (if desired) without 
 * affecting its use.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */


#ifndef __SIMPLECONFIG_H__
#define __SIMPLECONFIG_H__

#include <string>
#include <vector>
#include <hash_map>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>




/**
 * Basic exception class.
 *
 * The error message will automatically be dumped to stderr when the object 
 * is destroyed unless the clear() method is called.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class Error {
public:
	/**
	 * Set the error message.
	 */
	Error(const std::string &msg): m_msg(msg), m_display(true) {}

	/**
	 * Copy constructor.
	 *
	 * Maybe a little too fancy, but the clear() is needed so that
	 * multiple automatic copies yield only a single error message.
	 */
	Error(const Error &e): m_msg(e.m_msg), 
	                       m_fullerror(e.m_fullerror),
			       m_display(e.m_display) { e.clear(); }
	
	/**
	 * Display the error message on stderr unless clear() has been called.
	 */
	~Error();
	
	/**
	 * Prevent displaying the error on stderr.
	 */
	void clear() const { m_display = false; }
	
	/**
	 * Get the complete error message.
	 */
	std::string getFullError();
	
	/**
	 * Set the complete error message.
	 */
	void setFullError(const std::string &s);
	
	/**
	 * Get the basic error message.
	 */
	std::string getError();

protected:
	std::string m_msg, m_fullerror;
	mutable bool m_display;
};

/**
 * Thrown by SimpleConfig to report errors.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class ConfigError: public Error {
public:
	/**
	 * Set the error information.
	 */
	ConfigError(const std::string &msg, const std::string &filename, 
	            int line_no, const std::string &line);
	
	/**
	 * Get the basic error message.
	 */
	std::string getError();

	/**
	 * Get the offending line.
	 */
	std::string getErrorLine() { return m_line; }

	/**
	 * Get the line number of the offending line.
	 */
	int getLineNumber() { return m_line_no; }

	/**
	 * Get the filename in which the error occurred.
	 */
	std::string getFilename() { return m_filename; }

private:
	std::string m_filename, m_line;
	int m_line_no;
};


class ConfigElement;
class ConfigValue;

class HashString {
public:
	bool operator()(std::string const &str) const {
		return std::hash<char const *>()(str.c_str());
	}
};

typedef std::hash_map<std::string, ConfigElement*, HashString> ConfigDictionary;
typedef std::vector<ConfigElement *> ElementList;


/**
 * Provide a simple interface to text-based configuration files.
 *
 * This will often be created as a global object so that configuration
 * data can be accessed and saved throughout the program.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SimpleConfig {
public:
	
	/**
	 * Initialize configuration file.
	 *
	 * @param fn Path to the configuration file.
	 * @param autosave If true, every configuration change will be immediately
	 *  written to disk.
	 */
	SimpleConfig(const char *fn=0, bool autosave=true): m_modified(false) {
		m_autosave = autosave;
		open(fn);
	}
	
	/**
	 * Save if modified and internal cleanup.
	 */
	~SimpleConfig() {
		if (m_modified) save();
		clear();
	}

	/**
	 * Clear all configuration data.
	 */
	void clear();

	/**
	 * Abandon all changes and unbind from the configuration file.
	 */
	void abort() {
		clear();
		m_file = "";
	}

	/**
	 * Return true if the configuration file is completely empty (not even comments).
	 */
	bool isEmpty() const {
		return m_elements.size() == 0;
	}

	/**
	 * Add a comment at the end of the file.
	 */
	void addComment(const std::string &comment);

	/**
	 * Open a new configuration file.  Any changes to the previous file are saved 
	 * first.
	 */
	void open(const char *fn);

	/**
	 * Return true if there are any unsaved configuration changes.
	 */
	bool isModified() const { return m_modified; }

	/**
	 * Dump the configuration file to an output stream.
	 */
	void dump(ostream &os) const;

	/**
	 * Save the configuration file.
	 */
	void save();

	/**
	 * Returns true if a particular section-key pair is defined.
	 */
	bool hasKey(const char* section, const char* key) const;
	
	/**
	 * Remove a key-value pair.
	 */
	void delKey(const char* section, const char* key);
	
	/**
	 * Returns true if a particular section exists.
	 */
	bool hasSection(const char* section) const;

	/**
	 * Remove a section and all keys it contains.
	 */
	void delSection(const char* section);

	/**
	 * Get an integer value from the configuration file.
	 *
	 * @param section The section to look in.
	 * @param key The key to retrieve.
	 * @param default_ The value to return if the key is undefined.
	 * @param set If true will set undefined keys to the specified default value.
	 */
	int getInt(const std::string &section, const std::string &key, int default_=0, bool set=false);
	
	/**
	 * Get an floating-point value from the configuration file.
	 *
	 * @param section The section to look in.
	 * @param key The key to retrieve.
	 * @param default_ The value to return if the key is undefined.
	 * @param set If true will set undefined keys to the specified default value.
	 */
	double getFloat(const std::string &section, const std::string &key, double default_=0.0, bool set=false);
	
	/**
	 * Get a string value from the configuration file.
	 *
	 * @param section The section to look in.
	 * @param key The key to retrieve.
	 * @param default_ The value to return if the key is undefined.
	 * @param set If true will set undefined keys to the specified default value.
	 */
	std::string getString(const std::string &section, const std::string &key, std::string default_="", bool set=false);
	
	/**
	 * Set an integer value in the configuration file.
	 *
	 * @param section The section containing the key (will be created if non-existant).
	 * @param key The key to define or redefine.
	 * @param default_ The value to set.
	 */
	void setInt(const std::string &section, const std::string &key, int value);
	
	/**
	 * Set a floating-point value in the configuration file.
	 *
	 * @param section The section containing the key (will be created if non-existant).
	 * @param key The key to define or redefine.
	 * @param default_ The value to set.
	 */
	void setFloat(const std::string &section, const std::string &key, double value);

	/**
	 * Set a string value in the configuration file.
	 *
	 * @param section The section containing the key (will be created if non-existant).
	 * @param key The key to define or redefine.
	 * @param default_ The value to set.
	 */
	void setString(const std::string &section, const std::string &key, const std::string &value);
		
private:
	bool m_modified, m_autosave;
	std::string m_file;
	ConfigDictionary m_dict, m_last;
	ElementList m_elements;
	
	void _addComment(const std::string &comment); 
	void _addSection(const std::string &section); 
	std::string _hash_index(const std::string &section, const std::string &key) const;
	void _addValue(const std::string &section, const std::string &key, const std::string &value);
	void _parse(std::istream &is);
	ConfigValue *_find(const std::string &section, const std::string &key);

};


#endif // __SIMPLECONFIG_H__

