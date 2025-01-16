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

#include <csp/csplib/data/GeoPos.h>
#include <csp/csplib/util/Testing.h>

using namespace csp;
using namespace csp::test;

/*
  static void ParseXML() {
      Quat I(Quat::IDENTITY);

	  CSP_EXPECT_EQ(I, I.inverse());
  }
*/

static void ParseXML() {
	// Arrange
	// XML is using . as standard for decimals. Check if the parseXML
	// method is independent of locale.
	std::locale old = std::locale::global(std::locale("sv_SE"));
	LLA lla;

	// Act
	lla.parseXML("1.1 1.2 1.3");

	// Assert
	std::locale::global(old);
	LLA lla_expected;
	lla_expected.setDegrees(1.1, 1.2, 1.3);
	CSP_EXPECT_EQ(lla_expected.latitude(), lla.latitude());
	CSP_EXPECT_EQ(lla_expected.longitude(), lla.longitude());
	CSP_EXPECT_EQ(1.3, lla.altitude());
}

static void ParseXMLWithMinSec() {
	// Arrange
	LLA lla;
	std::locale old = std::locale::global(std::locale("sv_SE"));

	// Act
	lla.parseXML("1'2\"3.4 2'3\"4.4 5.5");

	// Assert
	std::locale::global(old);
	CSP_EXPECT_EQ(toRadians(1.0 + 2.0 / 60.0 + 3.4 / 3600.0), lla.latitude());
	CSP_EXPECT_EQ(toRadians(2.0 + 3.0 / 60.0 + 4.4 / 3600.0), lla.longitude());
	CSP_EXPECT_EQ(5.5, lla.altitude());
}

__attribute__((constructor)) static void RegisterTests() {
	TestRegistry::addTest(TestInstance{"ParseXML", &ParseXML});
	TestRegistry::addTest(TestInstance{"ParseXMLWithMinSec", &ParseXMLWithMinSec});
}
