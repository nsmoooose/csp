#include <SimData/Enum.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA

/*
 * TODO range checking!
 *
 */


// class Enum

Enum::Enum(const EnumerationCore& parent, const std::string &s, int idx): _core(&parent), _name(s), _id(idx) {
	_core->ref();
}

Enum::Enum(): _core(0), _id(0) {
}

Enum::Enum(const Enumeration& parent, const std::string &s): _core(0) {
	_core = parent._core;
	assert(_core);
	_core->ref();
	*this = s;
}

Enum::Enum(const Enumeration& parent, int idx): _core(0) {
	_core = parent._core;
	assert(_core);
	_core->ref();
	_id = idx;
	_name = _core->getName(idx);
}

Enum::Enum(const Enum &e): _core(0) {
	__assign(&e);
}

Enum::~Enum() {
	if (_core && _core->deref()) delete _core;
}

void Enum::__assign(const Enum* i) {
	assert(i);
	if (_core && _core->deref()) delete _core;
	_core = i->_core;
	if (_core) _core->ref();
	_id = i->_id;
	_name = i->_name;
}

const std::string& Enum::operator=(const std::string& s) {
	_name = s;
	if (_core == 0) {
		_id = 0;
	} else {
		_id = _core->getID(s);
	}
	return s;
}

const Enum& Enum::operator=(const Enum& i) {
	if (_core == 0) {
		 _core = i._core;
		if (_core) _core->ref();
	}
	if (_core == 0 || i._core != _core) {
		throw EnumTypeError("inequivalent Enum assignment");
	}
	this->operator=(i.asString());
	return i;
}

int  Enum::operator=(int x) {
	_id = x;
	if (_core == 0) {
		throw EnumTypeError("assignment of integer index to unbound Enum");
	} else {
		_name = _core->getName(x);
	}
	return x;
}

int Enum::asInt() const { return _id; }

std::string Enum::asString() const { return _name; }

void Enum::cycle() {
	if (_core == 0) return;
	int n = _core->size();
	if (_id == n) return;
	_id = (_id + 1) % n;
	_name = _core->getName(_id);
}

void Enum::cycleBack() {
	if (_core == 0) return;
	if (_id == 0) _id = _core->size();
	if (_id > 0) _id--;
	_name = _core->getName(_id);
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


bool Enum::operator==(std::string const &x) const { 
	return _core != 0 && _core->getID(x) == _id; 
}
bool Enum::operator!=(std::string const &x) const { 
	return _core != 0 && _core->getID(x) != _id; 
}
bool Enum::operator>=(std::string const &x) const { 
	return _core != 0 && _core->getID(x) >= _id; 
}
bool Enum::operator<=(std::string const &x) const { 
	return _core != 0 && _core->getID(x) <= _id; 
}
bool Enum::operator>(std::string const &x) const { 
	return _core != 0 && _core->getID(x) > _id; 
}
bool Enum::operator<(std::string const &x) const { 
	return _core != 0 && _core->getID(x) < _id;
}

bool Enum::operator>(int x) const { return _id > x; }
bool Enum::operator<(int x) const { return _id > x; }
bool Enum::operator>=(int x) const { return _id >= x; }
bool Enum::operator<=(int x) const { return _id <= x; }
bool Enum::operator==(int x) const { return _id == x; }
bool Enum::operator!=(int x) const { return _id != x; }

bool Enum::operator>(Enum const &x) const { return _id > x._id; }
bool Enum::operator<(Enum const &x) const { return _id > x._id; }
bool Enum::operator>=(Enum const &x) const { return _id >= x._id; }
bool Enum::operator<=(Enum const &x) const { return _id <= x._id; }
bool Enum::operator==(Enum const &x) const { return _id == x._id; }
bool Enum::operator!=(Enum const &x) const { return _id != x._id; }


EnumerationCore::EnumerationCore(const std::string &s, std::vector<Enum> &enums): _refs(1) {
	const char *i;
	const char *items = s.c_str();
	std::string item;
	int idx = 0;
	for (i = items; *i; items = ++i) {
		while (*i && *i != ' ') i++;
		item.assign(items, i-items);
		enums.push_back(Enum(*this, item, idx));
		_map[item] = idx++;
		_strings.push_back(item);
		if (*i == 0) break;
	}
	//if (_enums.size() == 0) throw EnumError("Enumeration(): empty enumeration list");
}

EnumerationCore::~EnumerationCore() {
	assert(_refs == 0);
	_map.clear(); 
	_strings.clear(); 
}

int EnumerationCore::getID(const std::string&s) const {
	string_map::const_iterator i;
	i = _map.find(s);
	if (i == _map.end()) throw EnumIndexError(s);
	return i->second;
}

const std::string& EnumerationCore::getName(int idx) const {
	return _strings[idx];
}

// class Enumeration

Enumeration::Enumeration(const char* items) {
	_core = new EnumerationCore(items, _enums);
}

Enumeration::~Enumeration() { 
	_enums.clear(); 
	if (_core && _core->deref()) delete _core;
}

const std::vector<Enum> Enumeration::each() const { 
	return _enums; 
}

const std::vector<std::string> Enumeration::eachString() const {
	assert(_core);
	return _core->strings();
}

const std::vector<std::string> Enumeration::getLabels() const {
	assert(_core);
	return _core->strings();
}

int Enumeration::size() const { 
	return _enums.size(); 
}

int Enumeration::_getID(const std::string&s) const {
	assert(_core);
	return _core->getID(s);
}

const std::string& Enumeration::_getName(int idx) const {
	assert(_core);
	return _core->getName(idx);
}

#ifndef SWIG
const Enum& Enumeration::operator[](int n) const { 
	return _enums[n]; 
}

const Enum& Enumeration::operator[](const std::string& s) const { 
	assert(_core);
	int n = _core->getID(s);
	return _enums[n]; 
}
#endif // SWIG

const std::vector<Enum> Enumeration::irange(const Enum& a, const Enum& b) const {
	std::vector<Enum>::const_iterator i_a, i_b;
	assert(_core);
	i_a = _enums.begin() + a._id;
	i_b = _enums.begin() + b._id + 1;
	return std::vector<Enum>(i_a, i_b);
}

const std::vector<Enum> Enumeration::range(int a, int b) const {
	if (a < 0) a = size() + a;
	if (b < 0) b = size() + b;
	if (b > size()) b = size();
	if (a < 0 || b < 0 || b <= a) {
		return std::vector<Enum>();
	}
	std::vector<Enum>::const_iterator i_a, i_b;
	assert(_core);
	i_a = _enums.begin() + a;
	i_b = _enums.begin() + b;
	return std::vector<Enum>(i_a, i_b);
}

bool Enumeration::contains(const Enum& x) const {
	return contains(x._name);
}

bool Enumeration::contains(int x) const { 
	return (x>=0 && x < size()); 
}

bool Enumeration::contains(const std::string& x) const { 
	EnumerationCore::string_map::const_iterator i;
	assert(_core);
	i = _core->map().find(x);
	return (i != _core->map().end());
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



