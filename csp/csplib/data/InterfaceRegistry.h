#pragma once
/* Combat Simulator Project
 * Copyright (C) 2002-2005 Mark Rose <mkrose@users.sf.net>
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
 * @file InterfaceRegistry.h
 * @brief Classes for storing and accessing object interfaces.
 */

#include <string>
#include <vector>

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Singleton.h>


namespace csp {


class InterfaceProxy;


/** Singleton class to store and access all ObjectInterfaces in the application.
 *
 *  ObjectInterfaces automatically register themselves with the global instance of this
 *  class at startup.  Interfaces can be accessed by name to provide dynamic object
 *  creation and variable assignment.  See @ref InterfaceDetails for details.
 */
class CSPLIB_EXPORT InterfaceRegistry {
friend class InterfaceProxy;

public:
	typedef std::vector<InterfaceProxy *> interface_list;

	/** Get an object interface by object class name.
	 *
	 *  @returns 0 if the interface is not found.
	 */
	InterfaceProxy *getInterface(const char *name);
	
	/** Get an object interface by object class hash.
	 *
	 *  @returns 0 if the interface is not found.
	 */
	InterfaceProxy *getInterface(hasht key);
	
	/** Test if an object interface is registered.
	 *
	 *  @param name The object class name.
	 */
	bool hasInterface(const char *name);
	
	/** Test if an object interface is registered.
	 *
	 *  @param key The object class hash.
	 */
	bool hasInterface(hasht key);
	
	/** Get a list of all object class names in the registry.
	 */
	std::vector<std::string> getInterfaceNames() const;

	/** Get a list of all interfaces in the registry.
	 */
	std::vector<InterfaceProxy *> getInterfaces() const;
	
	/** Get the interface registry singleton.
	 */
	static InterfaceRegistry &getInterfaceRegistry();

private:

	virtual ~InterfaceRegistry();

	/** Add an interface to the registry.
	 *
	 *  Interfaces are registered automatically by the XML definition macros
	 *  (@c CSP_XML_BEGIN and @c CSP_XML_END).
	 */
	void addInterface(const char *name, hasht id, InterfaceProxy *proxy);

	InterfaceRegistry();
	
	typedef HashMap<std::string, InterfaceProxy*>::Type proxy_map;
	typedef HashMap<hasht, InterfaceProxy*>::Type proxy_id_map;

	proxy_map __map;
	proxy_id_map __id_map;
	interface_list __list;

};


} // namespace csp
