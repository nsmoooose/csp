/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002-2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Properties.h
 *
 * Inheritable class properties.
 */


#ifndef __SIMDATA_PROPERTIES_H__
#define __SIMDATA_PROPERTIES_H__

#include <SimData/Namespace.h>
#include <SimData/Export.h>


NAMESPACE_SIMDATA


/** Inheritable property for classes that cannot be copied.
 *
 *  @author Mark Rose <mkrose@users.sourceforge.net>
 */
class NonCopyable {
protected:
	NonCopyable() { }
private:
	NonCopyable(NonCopyable const &);
	void operator = (NonCopyable const &);
};


/** Inheritable property for classes that cannot be (publicly) constructed.
 *
 *  @author Mark Rose <mkrose@users.sourceforge.net>
 */
class NonConstructable: public NonCopyable {
private:
	NonConstructable();
};

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_OBJECT_H__

