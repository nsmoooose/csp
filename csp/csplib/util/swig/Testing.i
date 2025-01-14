/* Combat Simulator Project
 * Copyright (C) 2005 Mark Rose <mkrose@users.sf.net>
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

%module Testing
%{
#include <csp/csplib/util/Testing.h>
%}

namespace csp {

namespace test {

class TestRegistry {
	TestRegistry();
public:
	static bool loadTestModule(const char *module);
	static bool runAll();
	static bool runOneTest(const char *test);
	static bool runOnePath(const char *path);
};

}

} // namespace csp
