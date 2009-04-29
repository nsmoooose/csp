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
 * @file test_Quat.h
 * @brief Test Quaternion methods.
 */

#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/Testing.h>

using namespace csp;

CSP_TESTFIXTURE(Quaternion) {

	template <class T>
	static double delta(T const& lhs, T const& rhs) {
		return (lhs - rhs).length();
	}

	CSP_TESTCASE(Basics) {
		Quat I(Quat::IDENTITY);
		CSP_EXPECT_EQ(I, I.inverse());
		CSP_EXPECT_EQ(1.0, I.length());
		CSP_EXPECT_EQ(1.0, I.length2());
		CSP_EXPECT_EQ(I, I.conj());
		CSP_EXPECT_EQ("type::Quat", I.typeString());
		CSP_EXPECT_EQ(0.0, Quat::ZERO.length());
		CSP_EXPECT_TRUE(I.isIdentity());
		CSP_EXPECT_FALSE(Quat::ZERO.isIdentity());
	}

	CSP_TESTCASE(Arithmetic) {
		Quat q(1, 2, -3, -4);
		CSP_EXPECT_DEQ(0, delta(Quat(2.5, 5, -7.5,  -10), 2.5 * q));
		CSP_EXPECT_DEQ(0, delta(Quat(2.5, 5, -7.5,  -10), q * 2.5));
		CSP_EXPECT_DEQ(0, delta(Quat(2.5, 5, -7.5, -10), q / 0.4));
		CSP_EXPECT_DEQ(0, delta(Quat(-1, -2, 3, 4), -q));
		CSP_EXPECT_DEQ(0, delta(Quat(3, 6, -9, -12), q + 2 * q));
		CSP_EXPECT_DEQ(0, (q - q).length());
		q += 1.5 * q;
		CSP_EXPECT_DEQ(0, delta(Quat(2.5, 5, -7.5,  -10), q));
		q /= 2.5;
		CSP_EXPECT_DEQ(0, delta(Quat(1, 2, -3, -4), q));
		q *= 2.5;
		CSP_EXPECT_DEQ(0, delta(Quat(2.5, 5, -7.5,  -10), q));
		q -= 0.6 * q;
		CSP_EXPECT_DEQ(0, delta(Quat(1, 2, -3, -4), q));
	}

	CSP_TESTCASE(Comparison) {
		Quat q(1, 2, -3, -4);
		CSP_EXPECT_TRUE(q == q);
		CSP_EXPECT_FALSE(q == -q);
		CSP_EXPECT_FALSE(q != q);
		CSP_EXPECT_TRUE(q != -q);
	}

	CSP_TESTCASE(Conjugation) {
		Quat q(1, 2, -3, -4);
		CSP_EXPECT_DEQ(0, delta(Quat(-1, -2, 3, -4), ~q));
		CSP_EXPECT_DEQ(0, delta(Quat(-1, -2, 3, -4), q.conj()));
	}

	CSP_TESTCASE(Components) {
		Quat q(1, 2, 3, 4);
		CSP_EXPECT_EQ(1, q.x());
		CSP_EXPECT_EQ(2, q.y());
		CSP_EXPECT_EQ(3, q.z());
		CSP_EXPECT_EQ(4, q.w());
		q.x() = -4;
		q.y() = -3;
		q.z() = -2;
		q.w() = -1;
		CSP_EXPECT_EQ(-4, q.x());
		CSP_EXPECT_EQ(-3, q.y());
		CSP_EXPECT_EQ(-2, q.z());
		CSP_EXPECT_EQ(-1, q.w());
		CSP_EXPECT_EQ(-4, q[0]);
		CSP_EXPECT_EQ(-3, q[1]);
		CSP_EXPECT_EQ(-2, q[2]);
		CSP_EXPECT_EQ(-1, q[3]);
		q[0] = 1;
		q[1] = 2;
		q[2] = 3;
		q[3] = 4;
		CSP_EXPECT_EQ(1, q.x());
		CSP_EXPECT_EQ(2, q.y());
		CSP_EXPECT_EQ(3, q.z());
		CSP_EXPECT_EQ(4, q.w());
	}

	CSP_TESTCASE(Assignment) {
		Quat q;
		q = Quat::IDENTITY;
		CSP_EXPECT_EQ(Quat::IDENTITY, q);
		q = Quat::ZERO;
		CSP_EXPECT_EQ(Quat::ZERO, q);
		q = Quat(1, 2, 3, 4);
		CSP_EXPECT_EQ(1, q.x());
		CSP_EXPECT_EQ(2, q.y());
		CSP_EXPECT_EQ(3, q.z());
		CSP_EXPECT_EQ(4, q.w());
	}

	CSP_TESTCASE(Set) {
		CSP_EXPECT_EQ(2, Quat(2, 0, 0, 0).x());
		CSP_EXPECT_EQ(2, Quat(0, 2, 0, 0).y());
		CSP_EXPECT_EQ(2, Quat(0, 0, 2, 0).z());
		CSP_EXPECT_EQ(2, Quat(0, 0, 0, 2).w());

		Quat q;
		CSP_EXPECT_EQ(0, q.x());
		CSP_EXPECT_EQ(0, q.y());
		CSP_EXPECT_EQ(0, q.z());
		CSP_EXPECT_EQ(0, q.w());

		q.set(0.5, 0.25, 0.125, 0.0625);
		CSP_EXPECT_EQ(0.5, q.x());
		CSP_EXPECT_EQ(0.25, q.y());
		CSP_EXPECT_EQ(0.125, q.z());
		CSP_EXPECT_EQ(0.0625, q.w());
	}

	CSP_TESTCASE(AsVector3) {
		Quat q(1, 2, 3, 4);
		CSP_EXPECT_EQ(1, q.asVector3().x());
		CSP_EXPECT_EQ(2, q.asVector3().y());
		CSP_EXPECT_EQ(3, q.asVector3().z());
	}

	CSP_TESTCASE(Conjugate) {
		Quat q(1, 2, 3, 4);
		CSP_EXPECT_EQ(-1, q.conj().x());
		CSP_EXPECT_EQ(-1, (~q).x());
		CSP_EXPECT_EQ(-2, q.conj().y());
		CSP_EXPECT_EQ(-2, (~q).y());
		CSP_EXPECT_EQ(-3, q.conj().z());
		CSP_EXPECT_EQ(-3, (~q).z());
		CSP_EXPECT_EQ(4, q.conj().w());
		CSP_EXPECT_EQ(4, (~q).w());
		CSP_EXPECT_EQ(q, q.conj().conj());
		CSP_EXPECT_EQ(q, ~~q);
	}

	CSP_TESTCASE(Length) {
		CSP_EXPECT_EQ(0, Quat::ZERO.length());
		CSP_EXPECT_EQ(0, Quat::ZERO.length2());
		CSP_EXPECT_EQ(1, Quat::IDENTITY.length());
		CSP_EXPECT_EQ(1, Quat::IDENTITY.length2());
		CSP_EXPECT_DEQ(sqrt(30.0), Quat(1, -2, 3, 4).length());
		CSP_EXPECT_DEQ(30, Quat(1, -2, 3, 4).length2());
	}

	CSP_TESTCASE(VecToVec) {
		Quat q;
		q.makeRotate(Vector3::XAXIS, Vector3::YAXIS);
		CSP_EXPECT_DEQ(0, delta(Vector3::YAXIS, q.rotate(Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0, delta(Vector3::ZAXIS, q.rotate(Vector3::ZAXIS)));

		Vector3 a(1, 2, 3);
		Vector3 b(-4, 5, 7);
		Vector3 c = a ^ b;
		q.makeRotate(a, b);
		CSP_EXPECT_DEQ(0, delta(b.normalized() * a.length(), q.rotate(a)));
		CSP_EXPECT_DEQ(0, delta(c, q.rotate(c)));
	}

	CSP_TESTCASE(AngleAxis) {
		CSP_EXPECT_DEQ(0, delta(-Quat::IDENTITY, Quat(toRadians(360), Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0, delta(-Quat::IDENTITY, Quat(toRadians(360), Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0, delta(-Quat::IDENTITY, Quat(toRadians(360), Vector3::ZAXIS)));
		CSP_EXPECT_DEQ(0, delta(Quat::IDENTITY, Quat(toRadians(720), Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0, delta(Quat::IDENTITY, Quat(toRadians(720), Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0, delta(Quat::IDENTITY, Quat(toRadians(720), Vector3::ZAXIS)));

		Vector3 axis(1, 2, -3);
		double angle = toRadians(138);
		Quat q(angle, axis);
		CSP_EXPECT_DEQ(cos(angle / 2), q.w());
		CSP_EXPECT_DEQ(sin(angle / 2) * axis.normalized().x(), q.x());
		CSP_EXPECT_DEQ(sin(angle / 2) * axis.normalized().y(), q.y());
		CSP_EXPECT_DEQ(sin(angle / 2) * axis.normalized().z(), q.z());

		Quat other;
		other.makeRotate(angle, axis);
		CSP_EXPECT_EQ(q, other);

		other.set(0, 0, 0, 0);
		other.makeRotate(angle, axis.x(), axis.y(), axis.z());
		CSP_EXPECT_EQ(q, other);

		double get_angle = 0;
		double get_x = 0;
		double get_y = 0;
		double get_z = 0;

		other.getRotate(get_angle, get_x, get_y, get_z);
		CSP_EXPECT_DEQ(angle, get_angle);
		CSP_EXPECT_DEQ(axis.normalized().x(), get_x);
		CSP_EXPECT_DEQ(axis.normalized().y(), get_y);
		CSP_EXPECT_DEQ(axis.normalized().z(), get_z);

		Vector3 get_axis;
		get_angle = 0;
		other.getRotate(get_angle, get_axis);
		CSP_EXPECT_DEQ(angle, get_angle);
		CSP_EXPECT_DEQ(0, delta(axis.normalized(), get_axis));
	}

	CSP_TESTCASE(Rotate) {
		Quat I(Quat::IDENTITY);
		CSP_EXPECT_EQ(Vector3::XAXIS, I.rotate(Vector3::XAXIS));
		CSP_EXPECT_EQ(Vector3::YAXIS, I.rotate(Vector3::YAXIS));
		CSP_EXPECT_EQ(Vector3::ZAXIS, I.rotate(Vector3::ZAXIS));

		Quat X(0.5 * PI, Vector3::XAXIS);
		CSP_EXPECT_DEQ(0.0, delta(Vector3::XAXIS, X.rotate(Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::ZAXIS, X.rotate(Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(-Vector3::YAXIS, X.rotate(Vector3::ZAXIS)));

		Quat Y(0.5 * PI, Vector3::YAXIS);
		CSP_EXPECT_DEQ(0.0, delta(-Vector3::ZAXIS, Y.rotate(Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::YAXIS, Y.rotate(Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::XAXIS, Y.rotate(Vector3::ZAXIS)));

		Quat Z(0.5 * PI, Vector3::ZAXIS);
		CSP_EXPECT_DEQ(0.0, delta(Vector3::YAXIS, Z.rotate(Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(-Vector3::XAXIS, Z.rotate(Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::ZAXIS, Z.rotate(Vector3::ZAXIS)));
	}

	CSP_TESTCASE(InvRotate) {
		Quat I(Quat::IDENTITY);
		CSP_EXPECT_EQ(Vector3::XAXIS, I.invrotate(Vector3::XAXIS));
		CSP_EXPECT_EQ(Vector3::YAXIS, I.invrotate(Vector3::YAXIS));
		CSP_EXPECT_EQ(Vector3::ZAXIS, I.invrotate(Vector3::ZAXIS));

		Quat X(0.5 * PI, Vector3::XAXIS);
		CSP_EXPECT_DEQ(0.0, delta(Vector3::XAXIS, X.invrotate(Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(-Vector3::ZAXIS, X.invrotate(Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::YAXIS, X.invrotate(Vector3::ZAXIS)));

		Quat Y(0.5 * PI, Vector3::YAXIS);
		CSP_EXPECT_DEQ(0.0, delta(Vector3::ZAXIS, Y.invrotate(Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::YAXIS, Y.invrotate(Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(-Vector3::XAXIS, Y.invrotate(Vector3::ZAXIS)));

		Quat Z(0.5 * PI, Vector3::ZAXIS);
		CSP_EXPECT_DEQ(0.0, delta(-Vector3::YAXIS, Z.invrotate(Vector3::XAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::XAXIS, Z.invrotate(Vector3::YAXIS)));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::ZAXIS, Z.invrotate(Vector3::ZAXIS)));

		Vector3 axis(-1.0, sqrt(2.0), 7.0);
		Quat R(0.74 * PI, axis);
		R.normalize();
		CSP_EXPECT_DEQ(1.0, R.length());
		CSP_EXPECT_DEQ(0.0, delta(Vector3::XAXIS, R.rotate(R.invrotate(Vector3::XAXIS))));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::YAXIS, R.rotate(R.invrotate(Vector3::YAXIS))));
		CSP_EXPECT_DEQ(0.0, delta(Vector3::ZAXIS, R.rotate(R.invrotate(Vector3::ZAXIS))));
		CSP_EXPECT_DEQ(0.0, delta(axis, R.rotate(axis)));
		CSP_EXPECT_DEQ(0.0, delta(axis, R.invrotate(axis)));
	}

	CSP_TESTCASE(EulerAngles) {
		Quat q;
		q.makeRotate(1, 0.5, 3.0);
		double roll = 0, pitch = 0, yaw = 0;
		q.getEulerAngles(roll, pitch, yaw);
		CSP_EXPECT_DEQ(1, roll);
		CSP_EXPECT_DEQ(0.5, pitch);
		CSP_EXPECT_DEQ(3, yaw);

		double pole = PI * 0.4999;
		q.makeRotate(1, pole, 3.0);
		q.getEulerAngles(roll, pitch, yaw);
		CSP_EXPECT_DEQ(0, roll);
		CSP_EXPECT_DEQ(pole, pitch);
		CSP_EXPECT_DEQ(3, yaw);

		double angle = toRadians(31);
		q.makeRotate(angle, 0, 0);
		CSP_EXPECT_DEQ(0, delta(q, Quat(angle, Vector3::XAXIS)));
		q.makeRotate(0, angle, 0);
		CSP_EXPECT_DEQ(0, delta(q, Quat(angle, Vector3::YAXIS)));
		q.makeRotate(0, 0, angle);
		CSP_EXPECT_DEQ(0, delta(q, Quat(angle, Vector3::ZAXIS)));

		q.makeRotate(0, toRadians(60), toRadians(225));
		CSP_EXPECT_DEQ(0, delta(q.rotate(Vector3::XAXIS), Vector3(-sqrt(0.125), -sqrt(0.125), -sqrt(0.75))));
		CSP_EXPECT_DEQ(0, delta(q.rotate(Vector3::YAXIS), Vector3(sqrt(0.5), -sqrt(0.5), 0.0)));
		q.getEulerAngles(roll, pitch, yaw);
		CSP_EXPECT_DEQ(0, roll);
		CSP_EXPECT_DEQ(toRadians(60), pitch);
		CSP_EXPECT_DEQ(toRadians(225 - 360), yaw);

		q.makeRotate(toRadians(60), 0, toRadians(225));
		CSP_EXPECT_DEQ(0, delta(q.rotate(Vector3::XAXIS), Vector3(-sqrt(0.5), -sqrt(0.5), 0.0)));
		CSP_EXPECT_DEQ(0, delta(q.rotate(Vector3::YAXIS), Vector3(sqrt(0.125), -sqrt(0.125), sqrt(0.75))));
		q.getEulerAngles(roll, pitch, yaw);
		CSP_EXPECT_DEQ(toRadians(60), roll);
		CSP_EXPECT_DEQ(0, pitch);
		CSP_EXPECT_DEQ(toRadians(225 - 360), yaw);

		q.makeRotate(toRadians(90), toRadians(30), 0);
		CSP_EXPECT_DEQ(0, delta(q.rotate(Vector3::XAXIS), Vector3(sqrt(0.75), 0, -sqrt(0.25))));
		CSP_EXPECT_DEQ(0, delta(q.rotate(Vector3::YAXIS), Vector3(sqrt(0.25), 0, sqrt(0.75))));
		q.getEulerAngles(roll, pitch, yaw);
		CSP_EXPECT_DEQ(toRadians(90), roll);
		CSP_EXPECT_DEQ(toRadians(30), pitch);
		CSP_EXPECT_DEQ(0, yaw);
	}

	CSP_TESTCASE(Matrix) {
		Quat convert;
		Quat q;
		Matrix3 m;
		q.makeRotate(Vector3::ZAXIS, Vector3::YAXIS);
		m.makeRotate(Vector3::ZAXIS, Vector3::YAXIS);
		convert.set(m);
		CSP_EXPECT_DEQ(0, delta(q, convert));

		q.makeRotate(toRadians(67), Vector3(1, -2, 3) / sqrt(14.0));
		m.makeRotate(toRadians(67), Vector3(1, -2, 3) / sqrt(14.0));
		convert.set(m);
		CSP_EXPECT_DEQ(0, delta(q, convert));

		Matrix3 m0 = m;
		m.invert();
		convert.set(m);
		CSP_EXPECT_DEQ(0, delta(q.inverse(), convert));

		m.makeRotate(q);
		convert.set(0, 0, 0, 0);
		convert.set(m);
		CSP_EXPECT_DEQ(0, delta(q, convert));
	}

	CSP_TESTCASE(Normalize) {
		Quat q(1, 2, 3, 4);
		CSP_EXPECT_LT(1.0, q.length());
		CSP_EXPECT_DEQ(1.0, q.normalized().length());
		Quat n = q.normalized();
		q.normalize();
		CSP_EXPECT_DEQ(0, delta(q, n));
		CSP_EXPECT_DEQ(1.0, q.length());
		CSP_EXPECT_DEQ(q.y(), 2 * q.x());
		CSP_EXPECT_DEQ(q.z(), 3 * q.x());
		CSP_EXPECT_DEQ(q.w(), 4 * q.x());
	}

	CSP_TESTCASE(Invert) {
		Quat q(1, 2, 3, 4);
		Quat i = q.inverse();
		CSP_EXPECT_DEQ(0, delta(q * i, Quat::IDENTITY));
		CSP_EXPECT_DEQ(0, delta(i * q, Quat::IDENTITY));
		CSP_EXPECT_DEQ(1.0 / q.length(), i.length());
		CSP_EXPECT_DEQ(0, delta(q, i.inverse()));
	}

	CSP_TESTCASE(Slerp) {
		Quat q, qy, qz;
		qy.makeRotate(Vector3::XAXIS, Vector3::YAXIS);
		qz.makeRotate(Vector3::XAXIS, Vector3::ZAXIS);
		q.slerp(0, qy, qz);
		CSP_EXPECT_DEQ(0, delta(qy, q));
		q.slerp(1, qy, qz);
		CSP_EXPECT_DEQ(0, delta(qz, q));
		q.slerp(0.5, qy, qz);
		CSP_EXPECT_DEQ(0, delta(Vector3(1, 2, 2).normalized(), q.rotate(Vector3::XAXIS)));

		q.slerp(0.4, Quat::IDENTITY, Quat(PI, Vector3::XAXIS));
		CSP_EXPECT_DEQ(0, delta(Quat(PI * 0.4, Vector3::XAXIS), q));
	}

	CSP_TESTCASE(Nlerp) {
		Quat q;
		q.nlerp(0.4, Quat::IDENTITY, Quat(PI, Vector3::XAXIS));
		CSP_EXPECT_DEQ(1, q.length());
		CSP_EXPECT_DEQ(0.4 / sqrt(0.52), q.x());
		CSP_EXPECT_EQ(0, q.y());
		CSP_EXPECT_EQ(0, q.z());
		CSP_EXPECT_DEQ(0.6 / sqrt(0.52), q.w());
	}

	CSP_TESTCASE(ParseXMLWith4Values) {
		// XML is using . as standard for decimals. Check if the parseXML
		// method is independent of locale.
		std::locale old = std::locale::global(std::locale("sv_SE"));
		Quat q;
		q.parseXML("0.2 0.5 0.7 0.8");
		CSP_EXPECT_EQ(0.2, q.x());
		CSP_EXPECT_EQ(0.5, q.y());
		CSP_EXPECT_EQ(0.7, q.z());
		CSP_EXPECT_EQ(0.8, q.w());
		std::locale::global(old);
	}

	CSP_TESTCASE(ParseXMLWith9Values) {
		// Arrange
		// XML is using . as standard for decimals. Check if the parseXML
		// method is independent of locale.
		std::locale old = std::locale::global(std::locale("sv_SE"));

		// Act
		Quat q;
		q.parseXML("0.2 0.5 0.7 0.8   0.9 1.1 1.2 1.3    1.4 ");

		// Assert
		Quat expectedResult;
		expectedResult.set(Matrix3(0.2, 0.5, 0.7, 0.8, 0.9, 1.1, 1.2, 1.3, 1.4));
		CSP_EXPECT_EQ(expectedResult.x(), q.x());
		CSP_EXPECT_EQ(expectedResult.y(), q.y());
		CSP_EXPECT_EQ(expectedResult.z(), q.z());
		CSP_EXPECT_EQ(expectedResult.w(), q.w());

		std::locale::global(old);
	}
};

