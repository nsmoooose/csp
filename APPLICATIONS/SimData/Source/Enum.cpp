#include <SimData/Enum.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA

/*
 * TODO range checking!
 *
 */

// class Enum

Enum::Enum(const Enumeration& parent, const std::string &s, int idx): _parent(&parent), _name(s), _id(idx) {
}

Enum::Enum(): _parent(0), _id(0) {
}

Enum::Enum(const Enumeration& parent, const std::string &s): _parent(&parent) {
	*this = s;
}

Enum::Enum(const Enumeration& parent, int idx): _parent(&parent) {
	_id = idx;
	_name = parent[idx].asString();
}

Enum::~Enum() {
}

void Enum::__assign(const Enum* i) {
	assert(i);
	_parent = i->_parent;
	_id = i->_id;
	_name = i->_name;
}

const std::string& Enum::operator=(const std::string& s) {
	_name = s;
	if (_parent == 0) {
		_id = 0;
	} else {
		_id = _parent->_getID(s);
	}
	return s;
}

const Enum& Enum::operator=(const Enum& i) {
	if (_parent == 0) {
		 _parent = i._parent;
	}
	if (_parent == 0 || i._parent != _parent) {
		throw EnumTypeError("inequivalent Enum assignment");
	}
	this->operator=(i.asString());
	return i;
}

int Enum::asInt() const { return _id; }

std::string Enum::asString() const { return _name; }

void Enum::cycle() {
	if (_parent == 0) return;
	int n = _parent->size();
	if (_id == n) return;
	_id = (_id + 1) % n;
	_name = _parent->_getName(_id);
}

void Enum::cycleBack() {
	if (_parent == 0) return;
	if (_id == 0) _id = _parent->size();
	if (_id > 0) _id--;
	_name = _parent->_getName(_id);
}

void Enum::pack(Packer& p) const {
	p.pack(_name);
}

void Enum::unpack(UnPacker& p) {
	std::string name;
	p.unpack(name);
	*this = name;
}

void Enum::parseXML(const char* cdata) {
	std::string name(cdata);
	*this = name;
}

std::string Enum::__repr__() { return _name; }

bool Enum::__eq__(const std::string& x) { return x == _name; }
bool Enum::__ne__(const std::string& x) { return x != _name; }
bool Enum::__le__(const std::string& x) { return _parent != 0 && _parent->_getID(x) >= _id; }
bool Enum::__ge__(const std::string& x) { return _parent != 0 && _parent->_getID(x) <= _id; }
bool Enum::__lt__(const std::string& x) { return _parent != 0 && _parent->_getID(x) > _id; }
bool Enum::__gt__(const std::string& x) { return _parent != 0 && _parent->_getID(x) < _id; }
bool Enum::__eq__(const Enum& x) { return x._id == _id; }
bool Enum::__ne__(const Enum& x) { return x._id != _id; }
bool Enum::__le__(const Enum& x) { return x._id >= _id; }
bool Enum::__ge__(const Enum& x) { return x._id <= _id; }
bool Enum::__lt__(const Enum& x) { return x._id >  _id; }
bool Enum::__gt__(const Enum& x) { return x._id <  _id; }



// class Enumeration

Enumeration::Enumeration(const char* items) {
	const char *i;
	std::string item;
	int idx = 0;
	for (i = items; *i; items = ++i) {
		while (*i && *i != ' ') i++;
		item.assign(items, i-items);
		_enums.push_back(Enum(*this, item, idx));
		_map[item] = idx++;
		_strings.push_back(item);
		if (*i == 0) break;
	}
	if (_enums.size() == 0) throw EnumError("Enumeration(): empty enumeration list");
}

const std::vector<Enum> Enumeration::each() const { return _enums; }

const std::vector<std::string> Enumeration::eachString() const {
	return _strings;
}

const std::vector<std::string> Enumeration::getLabels() const {
	return _strings;
}

int Enumeration::size() const { return _enums.size(); }

int Enumeration::_getID(const std::string&s) const {
	string_map::const_iterator i;
	i = _map.find(s);
	if (i == _map.end()) throw EnumIndexError(s);
	return i->second;
}

const std::string& Enumeration::_getName(int idx) const {
	return _enums[idx]._name;
}

#ifndef SWIG
const Enum& Enumeration::operator[](int n) const { return _enums[n]; }

const Enum& Enumeration::operator[](const std::string& s) const { 
	int n = _getID(s);
	return _enums[n]; 
}
#endif // SWIG

const std::vector<Enum> Enumeration::irange(const Enum& a, const Enum& b) const {
	std::vector<Enum>::const_iterator i_a, i_b;
	i_a = _enums.begin() + a._id;
	i_b = _enums.begin() + b._id + 1;
	return std::vector<Enum>(i_a, i_b);
}
bool Enumeration::contains(const Enum& x) const {
	return contains(x._name);
}
bool Enumeration::contains(int x) const { return (x>=0 && x < size()); }
bool Enumeration::contains(const std::string& x) const { 
	string_map::const_iterator i;
	i = _map.find(x);
	return (i != _map.end());
}
const std::vector<Enum> Enumeration::__getslice__(int a, int b) const {
	std::vector<Enum>::const_iterator i_a, i_b;
	i_a = _enums.begin() + a;
	i_b = _enums.begin() + b + 1;
	return std::vector<Enum>(i_a, i_b);
}	
int Enumeration::__len__() const { return size(); }
bool Enumeration::__contains__(const Enum& i) const { return contains(i); }
bool Enumeration::__contains__(int i) const { return contains(i); }
bool Enumeration::__contains__(const std::string& i) const { return contains(i); }
const Enum& Enumeration::__getitem__(int n) const { return _enums[n]; }
const Enum& Enumeration::__getitem__(const std::string& s) const { return this->operator[](s); }
const Enum& Enumeration::__getattr_c__(const std::string& s) const {
	return this->operator[](s);
}


NAMESPACE_END // namespace simdata


/*
USING_SIMDATA
Enumeration AirSources("OFF NORM DUMP RAM");
Enumeration Power("POWER OFF ON");

main() {
	Enum source(AirSources);
	Enum power(Power);
	source = "NORM";
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycle();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycle();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycle();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycle();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycleBack();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycleBack();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycleBack();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycleBack();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());
	source.cycleBack();
	printf("source = %s (%d)\n", source.asString().c_str(), source.asInt());

	power = "OFF";
	if (power == Power["OFF"]) printf("equal\n");
};
*/



