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

#include <SimData/Link.h>
#include <SimData/Log.h>
#include <SimData/Pack.h>
#include <SimData/DataArchive.h>
#include <SimData/ns-simdata.h>

#include <sstream>

using std::cout;
using std::endl;

NAMESPACE_SIMDATA


//////////////////////////////////////////////////////////////////////////
// LinkBase


// SWIG python specific comparisons
bool LinkBase::__eq__(const LinkBase& other) { 
	return this->operator==(other); 
}

bool LinkBase::__ne__(const LinkBase& other) { 
	return !this->operator==(other); 
}

void LinkBase::pack(Packer &p) const {
	Path::pack(p);
	if (isNone()) {
		assert(!isNull());
		p.pack(_get()->getClassHash());
		_get()->pack(p);
	}
}

void LinkBase::unpack(UnPacker& p) {
	Path::unpack(p);
	DataArchive *archive = p._getArchive();
	assert(archive);
	if (isNone()) {
		ObjectID class_id;
		p.unpack(class_id);
		Object *pobj = archive->_createObject(class_id);
		pobj->unpack(p);
		pobj->postCreate();
		_assign_safe(pobj);
		// XXX should we also check that 'static' is not set?
		// (it makes no sense to have a static immediate object)
	} else {
		if (p._loadAll()) {
			_load(archive);
		}
	}
}

std::string LinkBase::asString() const {
	std::stringstream repr;
	repr << "<Path " << _path << ">";
	return repr.str();
}

void LinkBase::_load(DataArchive* archive, ObjectID path) {
	assert(archive != 0);
	if (path == 0) path = _path;
	if (path == 0) {
		_path = 0;
		_assign_safe(0);
	} else {
		//std::cerr << "loading pointerbase from " << path << " " << _path << std::endl;
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "loading pointerbase from " << path << " " << _path);
		Path _p(path);
		LinkBase _ppb;
		/*
		 * XXX this is a bit tricky.  if we don't surpress the exception,
		 * we probably leave partially constructed objects in an incomplete
		 * state.  if we eat the exception, the object ends up with a null
		 * pointer, but there's no way to globally catch the error...
		 * XXX for now, pass on the exception.
		 */
		//try {

			_ppb = archive->getObject(_p);
		//}
		/*
		catch (Exception e) {
			e.details();
			e.clear();
			return;
		}
		*/
		*this = _ppb;
		Object* p = _reference;
		assert(p);
		//printf("got %p (static=%d)\n", p, p->isStatic());
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "got 0x" << std::hex << int(p) << " (static=" << p->isStatic() << ")");
	}
}


//////////////////////////////////////////////////////////////////////////
// ReferencePointer

// SWIG python specific comparisons
bool ReferencePointer::__eq__(const ReferencePointer& other) { 
	return this->operator==(other); 
}

bool ReferencePointer::__ne__(const ReferencePointer& other) { 
	return !this->operator==(other); 
}


NAMESPACE_END // namespace simdata

