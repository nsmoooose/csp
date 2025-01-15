/* Combat Simulator Project
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
 * @file test_Quat.h
 * @brief Test Quaternion methods.
 */

#include <csp/csplib/data/Real.h>
#include <csp/csplib/util/Testing.h>

using namespace csp;
using namespace csp::test;

CSP_TESTFIXTURE(Real) {
	CSP_TESTCASE(ParseXMLWithOneValue) {
		// Arrange
		// XML is using . as standard for decimals. Check if the parseXML
		// method is independent of locale.
		std::locale old = std::locale::global(std::locale("sv_SE"));

		// Act
		Real r;
		r.parseXML("1.2");

		// Assert
		std::locale::global(old);
		CSP_EXPECT_EQ(static_cast<float>(1.2), r.mean());
	}

	CSP_TESTCASE(ParseXMLWithTwoValues) {
		// Arrange
		// XML is using . as standard for decimals. Check if the parseXML
		// method is independent of locale.
		std::locale old = std::locale::global(std::locale("sv_SE"));

		// Act
		Real r;
		r.parseXML("1.2:1.3");

		// Assert
		std::locale::global(old);
		CSP_EXPECT_EQ(static_cast<float>(1.2), r.mean());
		CSP_EXPECT_EQ(static_cast<float>(1.3), r.sigma());
	}
};

__attribute__((constructor)) static void RegisterTests() {
}
