/* Combat Simulator Project
 * Copyright (C) 2006 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file test_Boolean.h
 * @brief Test for csplib/util/Boolean.h.
 */


#include <csp/csplib/util/Boolean.h>
#include <csp/csplib/util/Testing.h>

namespace csp {

CSP_TESTFIXTURE(TriBool) {

	CSP_TESTCASE(TestCasts) {
		tribool t;
		CSP_EXPECT(!static_cast<bool>(t));
		t = true;
		CSP_EXPECT(t);
		CSP_EXPECT(static_cast<bool>(t));
		t = false;
		CSP_EXPECT(!t);
		CSP_EXPECT(!static_cast<bool>(t));
	}

	CSP_TESTCASE(TestNegation) {
		tribool t;
		CSP_EXPECT(!static_cast<bool>(t));
		CSP_EXPECT(!static_cast<bool>(!t));
		t = true;
		CSP_EXPECT(static_cast<bool>(t));
		CSP_EXPECT(!static_cast<bool>(!t));
		t = false;
		CSP_EXPECT(!static_cast<bool>(t));
		CSP_EXPECT(static_cast<bool>(!t));
	}

	CSP_TESTCASE(TestEquality) {
		CSP_EXPECT(static_cast<bool>(tribool(true) == tribool(true)));
		CSP_EXPECT(static_cast<bool>(tribool(false) == tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool() == tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool(true) == tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool(false) == tribool(true)));
		CSP_EXPECT(!static_cast<bool>(tribool(true) == tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool(false) == tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool() == tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool() == tribool(true)));
	}

	CSP_TESTCASE(TestInequality) {
		CSP_EXPECT(!static_cast<bool>(tribool(true) != tribool(true)));
		CSP_EXPECT(!static_cast<bool>(tribool(false) != tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool() != tribool()));
		CSP_EXPECT(static_cast<bool>(tribool(true) != tribool(false)));
		CSP_EXPECT(static_cast<bool>(tribool(false) != tribool(true)));
		CSP_EXPECT(!static_cast<bool>(tribool(true) != tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool(false) != tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool() != tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool() != tribool(true)));
	}

	CSP_TESTCASE(TestAnd) {
		CSP_EXPECT(static_cast<bool>(tribool(true) && tribool(true)));
		CSP_EXPECT(!static_cast<bool>(tribool(false) && tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool() && tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool(true) && tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool(false) && tribool(true)));
		CSP_EXPECT(!static_cast<bool>(tribool(true) && tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool(false) && tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool() && tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool() && tribool(true)));
	}

	CSP_TESTCASE(TestOr) {
		CSP_EXPECT(static_cast<bool>(tribool(true) || tribool(true)));
		CSP_EXPECT(!static_cast<bool>(tribool(false) || tribool(false)));
		CSP_EXPECT(!static_cast<bool>(tribool() || tribool()));
		CSP_EXPECT(static_cast<bool>(tribool(true) || tribool(false)));
		CSP_EXPECT(static_cast<bool>(tribool(false) || tribool(true)));
		CSP_EXPECT(static_cast<bool>(tribool(true) || tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool(false) || tribool()));
		CSP_EXPECT(!static_cast<bool>(tribool() || tribool(false)));
		CSP_EXPECT(static_cast<bool>(tribool() || tribool(true)));
	}

	CSP_TESTCASE(TestComplement) {
		CSP_EXPECT(~tribool(true));
		CSP_EXPECT(~tribool(false));
		CSP_EXPECT(!~tribool());
	}
};

} // namespace csp


