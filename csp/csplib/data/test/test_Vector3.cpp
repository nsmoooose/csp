
/* Combat Simulator Project
 * Copyright (C) 2007 Mark Rose <mkrose@users.sf.net>
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
 * @file test_Vector3.h
 * @brief Test Vector3 methods.
 */

#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/Testing.h>

using namespace csp;
using namespace csp::test;

CSP_TESTFIXTURE(Vector3) {

	template <class T>
	static double delta(T const& lhs, T const& rhs) {
		return (lhs - rhs).length();
	}

	CSP_TESTCASE(Basics) {
		CSP_EXPECT_EQ("type::Vector3", Vector3().typeString());
		CSP_EXPECT_EQ(0.0, Vector3::ZERO.length());
		CSP_EXPECT_EQ(Vector3::ZERO, Vector3());
		CSP_EXPECT_TRUE(Vector3::ZERO.isZero());
		CSP_EXPECT_FALSE(Vector3::XAXIS.isZero());
		CSP_EXPECT_TRUE(Vector3::XAXIS.valid());
		CSP_EXPECT_FALSE(Vector3::XAXIS.isNaN());
	}

	CSP_TESTCASE(Axes) {
		CSP_EXPECT_DEQ(0, delta(Vector3(1, 0, 0), Vector3::XAXIS));
		CSP_EXPECT_DEQ(0, delta(Vector3(0, 1, 0), Vector3::YAXIS));
		CSP_EXPECT_DEQ(0, delta(Vector3(0, 0, 1), Vector3::ZAXIS));
	}

	CSP_TESTCASE(Arithmetic) {
		Vector3 v(1, 2, -3);
		CSP_EXPECT_DEQ(0, delta(Vector3(2.5, 5, -7.5), 2.5 * v));
		CSP_EXPECT_DEQ(0, delta(Vector3(2.5, 5, -7.5), v * 2.5));
		CSP_EXPECT_DEQ(0, delta(Vector3(2.5, 5, -7.5), v / 0.4));
		CSP_EXPECT_DEQ(0, delta(Vector3(-1, -2, 3), -v));
		CSP_EXPECT_DEQ(0, delta(Vector3(3, 6, -9), v + 2 * v));
		CSP_EXPECT_DEQ(0, (v - v).length());
		v += 1.5 * v;
		CSP_EXPECT_DEQ(0, delta(Vector3(2.5, 5, -7.5), v));
		v /= 2.5;
		CSP_EXPECT_DEQ(0, delta(Vector3(1, 2, -3), v));
		v *= 2.5;
		CSP_EXPECT_DEQ(0, delta(Vector3(2.5, 5, -7.5), v));
		v -= 0.6 * v;
		CSP_EXPECT_DEQ(0, delta(Vector3(1, 2, -3), v));
	}

	CSP_TESTCASE(Comparison) {
		Vector3 v(1, 2, -3);
		CSP_EXPECT_TRUE(v == v);
		CSP_EXPECT_FALSE(v == -v);
		CSP_EXPECT_FALSE(v != v);
		CSP_EXPECT_TRUE(v != -v);
	}

	CSP_TESTCASE(Components) {
		Vector3 v(1, 2, 3);
		CSP_EXPECT_EQ(1, v.x());
		CSP_EXPECT_EQ(2, v.y());
		CSP_EXPECT_EQ(3, v.z());
		v.x() = -4;
		v.y() = -3;
		v.z() = -2;
		CSP_EXPECT_EQ(-4, v.x());
		CSP_EXPECT_EQ(-3, v.y());
		CSP_EXPECT_EQ(-2, v.z());
		CSP_EXPECT_EQ(-4, v[0]);
		CSP_EXPECT_EQ(-3, v[1]);
		CSP_EXPECT_EQ(-2, v[2]);
		v[0] = 1;
		v[1] = 2;
		v[2] = 3;
		CSP_EXPECT_EQ(1, v.x());
		CSP_EXPECT_EQ(2, v.y());
		CSP_EXPECT_EQ(3, v.z());
	}

	CSP_TESTCASE(Assignment) {
		Vector3 v;
		v = Vector3(1, 2, 3);
		CSP_EXPECT_EQ(1, v.x());
		CSP_EXPECT_EQ(2, v.y());
		CSP_EXPECT_EQ(3, v.z());
		v = Vector3::ZERO;
		CSP_EXPECT_EQ(0, v.x());
		CSP_EXPECT_EQ(0, v.y());
		CSP_EXPECT_EQ(0, v.z());
	}

	CSP_TESTCASE(Set) {
		CSP_EXPECT_EQ(2, Vector3(2, 0, 0).x());
		CSP_EXPECT_EQ(2, Vector3(0, 2, 0).y());
		CSP_EXPECT_EQ(2, Vector3(0, 0, 2).z());

		Vector3 v;
		CSP_EXPECT_EQ(0, v.x());
		CSP_EXPECT_EQ(0, v.y());
		CSP_EXPECT_EQ(0, v.z());

		v.set(0.5, 0.25, 0.125);
		CSP_EXPECT_EQ(0.5, v.x());
		CSP_EXPECT_EQ(0.25, v.y());
		CSP_EXPECT_EQ(0.125, v.z());
	}

	CSP_TESTCASE(DotProduct) {
		CSP_EXPECT_DEQ(0.1, Vector3(4, -1, -1) * Vector3(0.25, 0.5, 0.4));
		CSP_EXPECT_DEQ(0.1, dot(Vector3(4, -1, -1), Vector3(0.25, 0.5, 0.4)));
		CSP_EXPECT_DEQ(-0.5, Vector3(1, 2, 3) * Vector3(-0.5, -6, 4));
		CSP_EXPECT_DEQ(-0.5, dot(Vector3(1, 2, 3), Vector3(-0.5, -6, 4)));
	}

	CSP_TESTCASE(CrossProduct) {
		CSP_EXPECT_DEQ(0, delta(Vector3(0.1, -1.85, 2.25), Vector3(4, -1, -1) ^ Vector3(0.25, 0.5, 0.4)));
		CSP_EXPECT_DEQ(0, delta(Vector3(0.1, -1.85, 2.25), cross(Vector3(4, -1, -1), Vector3(0.25, 0.5, 0.4))));
		CSP_EXPECT_DEQ(0, delta(Vector3(26, -5.5, -5), Vector3(1, 2, 3) ^ Vector3(-0.5, -6, 4)));
		CSP_EXPECT_DEQ(0, delta(Vector3(26, -5.5, -5), cross(Vector3(1, 2, 3), Vector3(-0.5, -6, 4))));
	}

	CSP_TESTCASE(Length) {
		CSP_EXPECT_EQ(0, Vector3::ZERO.length());
		CSP_EXPECT_EQ(0, Vector3::ZERO.length2());
		CSP_EXPECT_EQ(1, Vector3::XAXIS.length());
		CSP_EXPECT_EQ(1, Vector3::XAXIS.length2());
		CSP_EXPECT_DEQ(sqrt(14.0), Vector3(1, -2, 3).length());
		CSP_EXPECT_DEQ(14, Vector3(1, -2, 3).length2());
	}

	CSP_TESTCASE(Normalize) {
		Vector3 v(1, 2, 3);
		CSP_EXPECT_LT(1.0, v.length());
		CSP_EXPECT_DEQ(1.0, v.normalized().length());
		Vector3 n = v.normalized();
		CSP_EXPECT_DEQ(sqrt(14.0), v.normalize());
		CSP_EXPECT_DEQ(0, delta(v, n));
		CSP_EXPECT_DEQ(1.0, v.length());
		CSP_EXPECT_DEQ(v.y(), 2 * v.x());
		CSP_EXPECT_DEQ(v.z(), 3 * v.x());
	}

	CSP_TESTCASE(Bound) {
		Vector3 minv, maxv;

		Vector3(1, 2, 3).bound(minv, maxv);
		CSP_EXPECT_DEQ(0, delta(Vector3(0, 0, 0), minv));
		CSP_EXPECT_DEQ(0, delta(Vector3(1, 2, 3), maxv));

		Vector3(2, 4, 6).bound(minv, maxv);
		CSP_EXPECT_DEQ(0, delta(Vector3(0, 0, 0), minv));
		CSP_EXPECT_DEQ(0, delta(Vector3(2, 4, 6), maxv));

		Vector3(-1.1, 3.1, 7.1).bound(minv, maxv);
		CSP_EXPECT_DEQ(0, delta(Vector3(-1.1, 0, 0), minv));
		CSP_EXPECT_DEQ(0, delta(Vector3(2, 4, 7.1), maxv));

		Vector3(1.2, 3.7, 6.1).bound(minv, maxv);
		CSP_EXPECT_DEQ(0, delta(Vector3(-1.1, 0, 0), minv));
		CSP_EXPECT_DEQ(0, delta(Vector3(2, 4, 7.1), maxv));

		Vector3(-2.1, -3.1, -4.1).bound(minv, maxv);
		CSP_EXPECT_DEQ(0, delta(Vector3(-2.1, -3.1, -4.1), minv));
		CSP_EXPECT_DEQ(0, delta(Vector3(2, 4, 7.1), maxv));
	}

	CSP_TESTCASE(StarMatrix) {
		Vector3 v(1.1, 2.1, 3.1);
		Matrix3 m = v.starMatrix();
		CSP_EXPECT_EQ(0, m(0, 0));
		CSP_EXPECT_EQ(0, m(1, 1));
		CSP_EXPECT_EQ(0, m(2, 2));
		CSP_EXPECT_EQ(1.1, m(2, 1));
		CSP_EXPECT_EQ(-1.1, m(1, 2));
		CSP_EXPECT_EQ(2.1, m(0, 2));
		CSP_EXPECT_EQ(-2.1, m(2, 0));
		CSP_EXPECT_EQ(3.1, m(1, 0));
		CSP_EXPECT_EQ(-3.1, m(0, 1));
	}

	CSP_TESTCASE(ParseXML) {
		// Arrange
		std::locale old = std::locale::global(std::locale("sv_SE"));
		Vector3 v;
		char cdata[] = "1.3 2.4 3.5";

		// Act
		v.parseXML(cdata);

		// Assert
		std::locale::global(old);
		CSP_EXPECT_EQ(1.3, v.x());
		CSP_EXPECT_EQ(2.4, v.y());
		CSP_EXPECT_EQ(3.5, v.z());
	}
};

__attribute__((constructor)) static void RegisterTests() {
}
