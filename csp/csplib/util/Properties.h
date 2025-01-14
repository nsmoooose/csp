#pragma once
/* Combat Simulator Project
 * Copyright (C) 2002-2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @brief Inheritable class properties.
 */

#include <csp/csplib/util/Export.h>

namespace csp {


/** Inheritable property for classes that cannot be copied.
 */
class CSPLIB_EXPORT NonCopyable {
protected:
	NonCopyable() { }
private:
	NonCopyable(NonCopyable const &);
	void operator = (NonCopyable const &);
};


/** Inheritable property for classes that cannot be (publicly) constructed.
 */
class CSPLIB_EXPORT NonConstructable: public NonCopyable {
private:
	NonConstructable();
};


/** Restrict template parameters to specific class hierarchies.
 *
 *  Example usage:
 *
 *  @code
 *  template <class FOO>
 *  class Widget: HasBase<FOO, Bar>, public WidgetBase {
 *  ...
 *  };
 *  @endcode
 *
 *  This will only allow Widget<FOO> to compile if FOO is a subclass of
 *  class Bar:
 *
 *  @code
 *  class BarChild: public Bar { };
 *  class NotBarChild { };
 *  Widget<BarChild> bar_widget;  // ok
 *  Widget<NotBarChild> not_bar_widget;  // compiler error
 *  @endcode
 */
template<class DERIVED, class BASE>
struct HasBase {
	static void constraint(DERIVED* pd, const BASE* pb) { pb = pd; }
	HasBase() { void(*p)(DERIVED*, const BASE*) = constraint; p; }
};


} // namespace csp
