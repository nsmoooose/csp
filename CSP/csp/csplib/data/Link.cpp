/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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


#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Archive.h>
#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Log.h>

#include <sstream>


CSP_NAMESPACE


//////////////////////////////////////////////////////////////////////////
// LinkCore


// SWIG python specific comparisons
bool LinkCore::__eq__(const LinkCore& other) {
	return this->operator==(other);
}

bool LinkCore::__ne__(const LinkCore& other) {
	return !this->operator==(other);
}

LinkCore LinkCore::_serialize(Reader &reader, DataArchive* &data_archive) {
	data_archive = 0;
	Path path;
	reader >> path;
	ArchiveReader *arc = dynamic_cast<ArchiveReader*>(&reader);
	// XXX temporary assert for debugging; not exactly sure yet how to
	// handle Links outside of DataArchives.  the current idea (without
	// the assert) is to only read and write the path.
	assert(arc);
	if (arc) {
		data_archive = arc->_getArchive();
		assert(data_archive);
		if (path.isNone()) {
			ObjectID class_id;
			reader >> class_id;
			if (class_id == 0) {
				CSPLOG(ERROR, ARCHIVE) << "loading Link with no path and no object";
			} else {
				Object *pobj = data_archive->_createObject(class_id);
				assert(pobj);
				CSPLOG(ERROR, ARCHIVE) << "loading inline object " << pobj->getClassName();
				pobj->serialize(reader);
				// start reference counting before postCreate!
				LinkCore link(path, pobj);
				if (arc->_loadAll()) {
					pobj->postCreate();
				}
				// XXX should we also check that 'static' is not set? (it makes no
				// sense to have a static immediate object.)  it is fairly convenient
				// sometimes to declare static objects inline (e.g., when testing),
				// but it can also lead to errors and should probably be disallowed.
				data_archive = 0;  // nothing more to load
				return link;
			}
		} else {
			CSPLOG(DEBUG, ARCHIVE) << "loading linked object" << (arc->_loadAll() ? "loadall" : "!loadall");
			if (!arc->_loadAll()) data_archive = 0;
		}
	}
	return LinkCore(path, 0);
}

void LinkCore::_serialize(Writer &writer) const {
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
			CSPLOG(DEBUG, ARCHIVE) << "attempt to save Link with no path and no object";
			hasht class_hash = 0;
			writer << class_hash;
		} else {
			hasht class_hash = _reference->getClassHash();
			writer << class_hash;
			_reference->serialize(writer);
		}
	}
}

LinkCore LinkCore::_internal_load(DataArchive* archive, ObjectID path) {
	assert(archive != 0);
	if (path == 0) path = _path;
	if (path == 0) return LinkCore();
	CSPLOG(DEBUG, ARCHIVE) << "loading LinkCore from " << path << " " << _path;
	Path _p(path);
	LinkCore _ppb;
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
	assert(_ppb.valid());
	Object *obj = _ppb._reference;
	CSPLOG(DEBUG, ARCHIVE) << "loaded " << obj->getClassName() << " @ 0x" << std::hex << reinterpret_cast<int>(obj);
	return _ppb;
}

CSP_NAMESPACE_END

