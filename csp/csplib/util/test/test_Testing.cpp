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
 * @file test_Testing.h
 * @brief Test for functions in csplib/util/Testing.h.
 */

#include <csp/csplib/util/Testing.h>

using namespace csp;
using namespace csp::test;

template <typename T> struct float_eq_t {};
template <> struct float_eq_t<float> { static bool call(float a, float b) { return test::float_eq(a, b); } };
template <> struct float_eq_t<double> { static bool call(double a, double b) { return test::double_eq(a, b); } };

// This currently only checks float_eq and double_eq, which are the operators
// used by CSP_{EXPECT,ENSURE}_{F,D}{EQ,LE,GE}.  We assume that CSP_EXPECT and
// the test infrastructure for reporting failures works correctly.  Since only
// CSP_EXPECT is used, often in helper methods, the error messages for failed
// tests are not very useful for determining what failed and why.  Some diagnostic
// data about the values being tested is explicitly logged to help debug failures.

template <typename T>
void checkFloatEqNearZero(T value, bool result) {
	CSPLOG(INFO, TESTING) << "value = " << value;
	const T zero = 0.0f;
	CSP_EXPECT(float_eq_t<T>::call(zero, value) == result);
	CSP_EXPECT(float_eq_t<T>::call(-zero, value) == result);
	CSP_EXPECT(float_eq_t<T>::call(value, zero) == result);
	CSP_EXPECT(float_eq_t<T>::call(value, -zero) == result);
	CSP_EXPECT(float_eq_t<T>::call(zero, -value) == result);
	CSP_EXPECT(float_eq_t<T>::call(-zero, -value) == result);
	CSP_EXPECT(float_eq_t<T>::call(-value, zero) == result);
	CSP_EXPECT(float_eq_t<T>::call(-value, -zero) == result);
}

template <typename T>
void checkSymmetricFloatEq(T a, T b, bool result) {
	CSPLOG(INFO, TESTING) << "a = " << a << ", b = " << b << ", result = " << result;
	CSP_EXPECT(float_eq_t<T>::call(a, b) == result);
	CSP_EXPECT(float_eq_t<T>::call(b, a) == result);
}

template <typename T>
void checkSignFloatEq(T value) {
	CSPLOG(INFO, TESTING) << "value = " << value;
	CSP_EXPECT(float_eq_t<T>::call(value, value));
	CSP_EXPECT(float_eq_t<T>::call(-value, -value));
}

template <typename T>
void checkFloatEqBasics() {
	const T epsilon = std::numeric_limits<T>::epsilon();
	const T inf = std::numeric_limits<T>::infinity();
	const T nan = std::numeric_limits<T>::quiet_NaN();
	const T fmin = std::numeric_limits<T>::min();
	const T fmax = std::numeric_limits<T>::max();
	const T zero = static_cast<T>(0);
	const T one = static_cast<T>(1);

	checkFloatEqNearZero<T>(zero, true);
	checkFloatEqNearZero<T>(fmin, true);
	checkFloatEqNearZero<T>(epsilon, true);
	checkFloatEqNearZero<T>(2*epsilon, true);
	checkFloatEqNearZero<T>(3*epsilon, true);
	checkFloatEqNearZero<T>(4*epsilon, true);
	checkFloatEqNearZero<T>(5*epsilon, false);
	checkFloatEqNearZero<T>(fmax, false);
	checkFloatEqNearZero<T>(inf, false);
	checkFloatEqNearZero<T>(nan, false);

	checkSignFloatEq<T>(fmin);
	checkSignFloatEq<T>(epsilon);
	checkSignFloatEq<T>(static_cast<T>(1));
	checkSignFloatEq<T>(static_cast<T>(53));
	checkSignFloatEq<T>(fmax);
	checkSignFloatEq<T>(inf);

	CSP_EXPECT(!float_eq_t<T>::call(nan, nan));

	checkSymmetricFloatEq<T>(one - epsilon, one, true);
	checkSymmetricFloatEq<T>(one + epsilon, one, true);

	checkSymmetricFloatEq<T>(inf, fmax, false);
	checkSymmetricFloatEq<T>(inf, -inf, false);

	checkSymmetricFloatEq<T>(inf, nan, false);
	checkSymmetricFloatEq<T>(fmax, nan, false);
	checkSymmetricFloatEq<T>(one, nan, false);
	checkSymmetricFloatEq<T>(epsilon, nan, false);
}

static void TestFloatEq() {
	checkFloatEqBasics<float>();

	checkSymmetricFloatEq<float>(2.1999f, 2.1994f, false);
	checkSymmetricFloatEq<float>(2.1999f, 2.1995f, true);
	checkSymmetricFloatEq<float>(2.1999f, 2.2003f, true);
	checkSymmetricFloatEq<float>(2.1999f, 2.2004f, false);

	checkSymmetricFloatEq<float>(-2.1999f, -2.1994f, false);
	checkSymmetricFloatEq<float>(-2.1999f, -2.1995f, true);
	checkSymmetricFloatEq<float>(-2.1999f, -2.2003f, true);
	checkSymmetricFloatEq<float>(-2.1999f, -2.2004f, false);

	checkSymmetricFloatEq<float>(2.1999e+30f, 2.1994e+30f, false);
	checkSymmetricFloatEq<float>(2.1999e+30f, 2.1995e+30f, true);
	checkSymmetricFloatEq<float>(2.1999e+30f, 2.2003e+30f, true);
	checkSymmetricFloatEq<float>(2.1999e+30f, 2.2004e+30f, false);

	checkSymmetricFloatEq<float>(2.1999e-30f, 2.1994e-30f, false);
	checkSymmetricFloatEq<float>(2.1999e-30f, 2.1995e-30f, true);
	checkSymmetricFloatEq<float>(2.1999e-30f, 2.2003e-30f, true);
	checkSymmetricFloatEq<float>(2.1999e-30f, 2.2004e-30f, false);

	checkSymmetricFloatEq<float>(3.1f - 2.9f, 0.2000f, true);
	checkSymmetricFloatEq<float>(3.1f - 2.9f, 0.2001f, false);
}

static void TestDoubleEq() {
	checkFloatEqBasics<double>();

	checkSymmetricFloatEq<double>(2.2, 2.19999997, false);
	checkSymmetricFloatEq<double>(2.2, 2.19999998, true);
	checkSymmetricFloatEq<double>(2.2, 2.20000002, true);
	checkSymmetricFloatEq<double>(2.2, 2.20000003, false);

	checkSymmetricFloatEq<double>(-2.2, -2.19999997, false);
	checkSymmetricFloatEq<double>(-2.2, -2.19999998, true);
	checkSymmetricFloatEq<double>(-2.2, -2.20000002, true);
	checkSymmetricFloatEq<double>(-2.2, -2.20000003, false);

	checkSymmetricFloatEq<double>(2.2e+100, 2.19999997e+100, false);
	checkSymmetricFloatEq<double>(2.2e+100, 2.19999998e+100, true);
	checkSymmetricFloatEq<double>(2.2e+100, 2.20000002e+100, true);
	checkSymmetricFloatEq<double>(2.2e+100, 2.20000003e+100, false);

	checkSymmetricFloatEq<double>(2.2e-100, 2.19999997e-100, false);
	checkSymmetricFloatEq<double>(2.2e-100, 2.19999998e-100, true);
	checkSymmetricFloatEq<double>(2.2e-100, 2.20000002e-100, true);
	checkSymmetricFloatEq<double>(2.2e-100, 2.20000003e-100, false);

	checkSymmetricFloatEq<double>(3.1 - 2.9, 0.20000000, true);
	checkSymmetricFloatEq<double>(3.1 - 2.9, 0.20000001, false);
}

__attribute__((constructor)) static void RegisterTests() {
	TestRegistry::addTest(TestInstance{"Testing_TestFloatEq", &TestFloatEq});
	TestRegistry::addTest(TestInstance{"Testing_TestDoubleEq", &TestDoubleEq});
}
