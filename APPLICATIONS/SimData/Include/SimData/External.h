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

/**
 * @file External.h
 */

#ifndef __EXTERNAL_H__
#define __EXTERNAL_H__

#include <string>
#include <SimData/BaseType.h>


NAMESPACE_SIMDATA

/**
 * Holds the file system path to an external data source.
 *
 * Paths are stored interally in a platform independent format, and
 * automatically converted to and from the native format.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class External: public BaseType {
	static std::string toNative(const char *path);
	static std::string fromNative(const char *path);
protected:
	std::string _path;
	std::string _native_path;
public:
	virtual ~External();
#ifndef SWIG
	const External &operator=(std::string const &);
	const External &operator=(External const &);
#endif // SWIG
	void setSource(const char* path);
	const std::string& getSource() const;
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);
	virtual std::string asString() const;
};

NAMESPACE_END // namespace simdata


#endif //__EXTERNAL_H__

