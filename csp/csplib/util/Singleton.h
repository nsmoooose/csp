/* Combat Simulator Project
 * Copyright (C) 2003 Mark Rose <mkrose@users.sf.net>
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
 * @file Singleton.h
 * @brief Pattern for creating single global instances.
 */


#ifndef __CSPLIB_UTIL_SINGLETON_H__
#define __CSPLIB_UTIL_SINGLETON_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Properties.h>

namespace csp {

/** Creates a single, static instance of the templated class.
 *  TODO could stand lots of improvement (delete priority, thread
 *  safety, etc).
 */
template <class C>
class Singleton: public NonCopyable {
public:
	/** Get the one instance of the template class.
	 */
	static C& getInstance() {
		static C *__instance = 0;
		if (!__instance) __instance = new C();
		return *__instance;
	}

	/** Get the one instance of the template class as a const reference.
	 */
	static C const & getConstInstance() {
		return getInstance();
	}

protected:
	Singleton() {}
	virtual ~Singleton() {}
};

} // namespace csp

#endif // __CSPLIB_UTIL_SINGLETON_H__

