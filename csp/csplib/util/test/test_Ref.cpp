/* Combat Simulator Project
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file test_Ref.h
 * @brief Test reference counting.
 */


#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Testing.h>


using namespace csp;
using namespace csp::test;

struct R: public csp::Referenced {
	int &m_count;
	R(int &count): m_count(count) { ++m_count; }
	~R() { --m_count; }
};

typedef csp::Ref<R> RefR;

static void Count() {
	int count = 0;
	CSP_VERIFY_EQ(count, 0);
	RefR x = new R(count);
	CSP_VERIFY_EQ(count, 1);
	{
		RefR y = x;
		CSP_VERIFY_EQ(count, 1);
	}
	CSP_VERIFY_EQ(count, 1);
	{
		RefR y = new R(count);
		CSP_VERIFY_EQ(count, 2);
		y = x;
		CSP_VERIFY_EQ(count, 1);
		y = new R(count);
		CSP_VERIFY_EQ(count, 2);
	}
	CSP_VERIFY_EQ(count, 1);
	x = 0;
	CSP_VERIFY_EQ(count, 0);
}

__attribute__((constructor)) static void RegisterTests() {
	TestRegistry::addTest(TestInstance{"Ref_Count", &Count});
}
