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

#include <SimData/Log.h>
#include <SimData/Path.h>
#include <SimData/DataArchive.h>
#include <SimData/ns-simdata.h>

#include <sstream>

using std::cout;
using std::endl;

NAMESPACE_SIMDATA


//////////////////////////////////////////////////////////////////////////
// Path

void Path::setPath(const char* path) {
	if (path != 0 && *path != 0) {
		setPath(hash_string(path));
	//	cout << "Path::setPath(" <<  path << ") = " << hash_string(path) << endl;
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


//////////////////////////////////////////////////////////////////////////
// PointerBase

PointerBase::PointerBase(): Path((const char*)0), ReferencePointer() {
}

PointerBase::PointerBase(const char* path): Path(path), ReferencePointer() {
}

PointerBase::PointerBase(const Path& path, Object* ptr): Path(path), ReferencePointer(ptr) {
}

PointerBase::PointerBase(const PointerBase& r): Path((const char*)0), ReferencePointer() {
	*this = r;
}

PointerBase::~PointerBase() {
}

PointerBase& PointerBase::operator=(const PointerBase& r) {
	ReferencePointer::operator=(r);
	_path = r.getPath();
	return *this;
}

// SWIG python specific comparisons
bool PointerBase::__eq__(const PointerBase& other) { 
	return this->operator==(other); 
}

bool PointerBase::__ne__(const PointerBase& other) { 
	return !this->operator==(other); 
}

void PointerBase::pack(Packer &p) const {
	Path::pack(p);
	if (isNone()) {
		assert(!isNull());
		p.pack(_get()->getClassHash());
		_get()->pack(p);
	}
}

void PointerBase::unpack(UnPacker& p) {
	Path::unpack(p);
	DataArchive *archive = p._getArchive();
	assert(archive);
	if (isNone()) {
		ObjectID class_id;
		p.unpack(class_id);
		Object *pobj = archive->_createObject(class_id);
		pobj->unpack(p);
		_assign(pobj);
	} else {
		if (p._loadAll()) {
			_load(archive);
		}
	}
}

std::string PointerBase::asString() const {
	std::stringstream repr;
	repr << "<Path " << _path << ">";
	return repr.str();
}

void PointerBase::setNull() {
	_assign(NULL);
}

void PointerBase::setNone() {
	_assign(NULL);
	Path::setNone();
}

void PointerBase::_load(DataArchive* archive, ObjectID path) {
	assert(archive != 0);
	if (path == 0) path = _path;
	if (path == 0) {
		_path = 0;
		_assign(0);
	} else {
		//std::cerr << "loading pointerbase from " << path << " " << _path << std::endl;
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "loading pointerbase from " << path << " " << _path);
		Path _p(path);
		PointerBase _ppb;
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
		//printf("got %p (static=%d)\n", p, p->isStatic());
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "got 0x" << std::hex << int(p) << " (static=" << p->isStatic() << ")");
	}
}


//////////////////////////////////////////////////////////////////////////
// ReferencePointer

ReferencePointer::ReferencePointer(): _reference(0) {
}

ReferencePointer::ReferencePointer(Object* ptr): _reference(0) {
	_assign(ptr);
}

ReferencePointer::ReferencePointer(const ReferencePointer& r) {
	*this = r;
}

ReferencePointer::~ReferencePointer() {
	_release();
}

bool ReferencePointer::unique() const {
	return (_reference ? _reference->getCount()==1 : true);
}
	
ReferencePointer& ReferencePointer::operator=(const ReferencePointer& r) {
	_assign(r._reference);
	return *this;
}

// SWIG python specific comparisons
bool ReferencePointer::__eq__(const ReferencePointer& other) { 
	return this->operator==(other); 
}

bool ReferencePointer::__ne__(const ReferencePointer& other) { 
	return !this->operator==(other); 
}

void ReferencePointer::_assign(Object* ptr) {
	_release();
	_update(ptr);
	if (!isNull()) _reference->ref();
}

void ReferencePointer::_release() {
	if (!isNull()) {
		if (_reference->deref() == 0) delete _reference;
		_update(0);
	}
}

void ReferencePointer::_update(Object* p) { 
	_reference = p; 
}

Object* ReferencePointer::_get() const { 
	return _reference; 
}


NAMESPACE_END // namespace simdata

