/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
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


/**
 * @file Link.cpp
 * @brief Smart pointers to Object classes in data archives.
 */


#include <SimData/Link.h>
#include <SimData/Log.h>
#include <SimData/Archive.h>
#include <SimData/DataArchive.h>
#include <SimData/Namespace.h>

#include <sstream>


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

void LinkBase::serialize(Reader &reader) {
	Path::serialize(reader);
	ArchiveReader *arc = dynamic_cast<ArchiveReader*>(&reader);
	// XXX temporary assert for debugging; not exactly sure yet how to
	// handle Links outside of DataArchives.  the current idea (without
	// the assert) is to only read and write the path.
	assert(arc);
	if (arc) {
		DataArchive *data_archive = arc->_getArchive();
		assert(data_archive);
		if (isNone()) {
			ObjectID class_id;
			reader >> class_id;
			if (class_id == 0) {
				SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "loading Link with no path and no object");
			} else {
				Object *pobj = data_archive->_createObject(class_id);
				pobj->serialize(reader);
				// start reference counting before postCreate!
				_assign_safe(pobj);
				if (arc->_loadAll()) {
					pobj->postCreate();
				}
				// XXX should we also check that 'static' is not set?
				// (it makes no sense to have a static immediate object)
			}
		} else {
			if (arc->_loadAll()) {
				_load(data_archive);
			}
		}
	}
}

void LinkBase::serialize(Writer &writer) const {
	Path::serialize(writer);
	if (isNone()) {
		ArchiveWriter *arc = dynamic_cast<ArchiveWriter*>(&writer);
		// XXX temporary assert for debugging; not exactly sure yet how to
		// handle Links outside of DataArchives.  the current idea (without
		// the assert) is to only read and write the path.
		assert(arc);

		if (isNull()) {
			// saving a null+none link is now allowed.  in the context of xml interfaces,
			// however, required links cannot be null+none.  this restriction is enforced
			// by specialized validation classes in ObjectInterface.h.  here we just log
			// a debug message (although this is probably not very useful).
			SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "attempt to save Link with no path and no object");
			hasht class_hash = 0;
			writer << class_hash;
		} else {
			hasht class_hash = _get()->getClassHash();
			writer << class_hash;
			_get()->serialize(writer);
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
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "loaded " << p->getClassName() << " @ 0x" << std::hex << int(p));
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


NAMESPACE_SIMDATA_END

