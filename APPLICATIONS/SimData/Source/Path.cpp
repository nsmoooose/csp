/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <SimData/Path.h>
#include <SimData/DataArchive.h>
#include <SimData/ns-simdata.h>

#include <sstream>

using std::cout;
using std::endl;

NAMESPACE_SIMDATA


// Path

void Path::setPath(const char* path) {
	if (path != 0 && *path != 0) {
		setPath(hash_string(path));
		cout << path << " = " << hash_string(path) << endl;
	} else {
		setNone();
	}
}

void Path::pack(Packer& p) const {
	p.pack(_path);
}

void Path::unpack(UnPacker& p) {
	p.unpack(_path);
}

std::string Path::asString() const {
	std::stringstream repr;
	repr << "<Path " << _path << ">";
	return repr.str();
}


// PathPointerBase

// SWIG python specific comparisons
bool PathPointerBase::__eq__(const PathPointerBase& other) { 
	return this->operator==(other); 
}

bool PathPointerBase::__ne__(const PathPointerBase& other) { 
	return !this->operator==(other); 
}

// SWIG python specific comparisons
bool PointerBase::__eq__(const PointerBase& other) { 
	return this->operator==(other); 
}

bool PointerBase::__ne__(const PointerBase& other) { 
	return !this->operator==(other); 
}

PointerBase::PointerBase(): _reference(0) {
}

PathPointerBase::PathPointerBase(): Path((const char*)0), PointerBase() {
}

PathPointerBase::PathPointerBase(const char* path): Path(path), PointerBase() {
}

PointerBase::PointerBase(Object* ptr): _reference(0) {
	_assign(ptr);
}

PathPointerBase::PathPointerBase(const Path& path, Object* ptr): Path(path), PointerBase(ptr) {
}

PointerBase::~PointerBase() {
	_release();
}
	
PathPointerBase::~PathPointerBase() {
}

bool PointerBase::unique() const {
	return (_reference ? _reference->getCount()==1 : true);
}

PointerBase::PointerBase(const PointerBase& r) {
	*this = r;
}

PathPointerBase::PathPointerBase(const PathPointerBase& r): Path((const char*)0), PointerBase() {
	*this = r;
}

PointerBase& PointerBase::operator=(const PointerBase& r) {
	_assign(r._reference);
	return *this;
}

PathPointerBase& PathPointerBase::operator=(const PathPointerBase& r) {
	PointerBase::operator=(r);
	_path = r.getPath();
	return *this;
}

void PathPointerBase::unpack(UnPacker& p) {
	Path::unpack(p);
	_load(p._getArchive());
}

std::string PathPointerBase::asString() const {
	std::stringstream repr;
	repr << "<Path " << _path << ">";
	return repr.str();
}

void PathPointerBase::setNull() {
	_assign(NULL);
}

void PathPointerBase::setNone() {
	_assign(NULL);
	Path::setNone();
}

void PointerBase::_assign(Object* ptr) {
	_release();
	_update(ptr);
	if (!isNull()) _reference->ref();
}

void PathPointerBase::_load(DataArchive* archive, ObjectID path) {
	assert(archive != 0);
	if (path == 0) path = _path;
	if (path == 0) {
		_path = 0;
		_assign(0);
	} else {
		std::cerr << "loading pathpointerbase from " << path << " " << _path << std::endl;
		Path _p(path);
		PathPointerBase _ppb;
		try {
			_ppb = archive->getObject(_p);
		}
		catch (Exception e) {
			e.details();
			e.clear();
			return;
		}
		*this = _ppb;
		Object* p = _reference;
		assert(p);
		printf("got %p (static=%d)\n", p, p->isStatic());
	}
}

void PointerBase::_release() {
	if (!isNull()) {
		if (_reference->deref() == 0) delete _reference;
		_update(0);
	}
}

void PointerBase::_update(Object* p) { 
	_reference = p; 
}

Object* PointerBase::_get() const { 
	return _reference; 
}


NAMESPACE_END // namespace simdata

