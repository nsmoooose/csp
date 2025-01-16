#pragma once
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

/**
 * @file Testing.h
 * @brief Classes and macros for creating unittests.
 *
 * Provides functionality that is similar to cppunit.  The convention used by
 * csplib is to place unittests in the same directory as the code they
 * exercise, using file names prefixed by "test_" (e.g., "test_Object.cpp").
 * Each test file must be included in the local SConscript file; see the
 * actual csplib SConscript files for examples.
 *
 * Tests are usually organized into test fixtures, declared like this:
 *
 * @code
 *   CSP_TESTFIXTURE(Vector3) {
 *
 *     // test operator=(Vector3 const&)
 *     CSP_TESTCASE(Assignment) {
 *       Vector3 test;
 *       test = Vector3(1, 2, 3);
 *       CSP_VERIFY_FLOATEQ(test.x(), 1);
 *       CSP_VERIFY_FLOATEQ(test.y(), 2);
 *       CSP_VERIFY_FLOATEQ(test.z(), 3);
 *       return true;
 *     }
 *
 *     // test x + y
 *     CSP_TESTCASE(Addition) {
 *       Vector3 lhs(1, 2, 3);
 *       Vector3 rhs(3, 4, 5);
 *       Vector3 sum = lhs + rhs;
 *       CSP_VERIFY_FLOATEQ(sum.x(), 4);
 *       CSP_VERIFY_FLOATEQ(sum.y(), 6);
 *       CSP_VERIFY_FLOATEQ(sum.z(), 8);
 *       return true;
 *     }
 *
 *   };
 * @endcode
 *
 * The CSP_TESTFIXTURE declaration resembles a class declaration (which it
 * is, essentially, under the covers).  Note that it must be terminated by
 * a semicolon.  The CSP_TESTCASE declarations are independent tests (which
 * are written like methods).  Each testcase must return true to indicate
 * success or false to indicate failure.  The CSP_VERIFY_* macros can also
 * be used to cause the test case to fail if a condition is not satisfied.
 * Similarly, CSP_VERIFY and CSP_ASSERT macros can be safely used by the
 * code that is being tested.  Vanilla assert() should be avoided since it
 * will abort the current test and all subsequent tests.  Tests within a
 * test fixture are executed in order of declaration.
 *
 * In addition to CSP_TESTCASE declarations, the test fixture can contain
 * member variables that are accessible to all test cases.  Such variables
 * may be shared between the various tests (the fixture is not destroyed
 * and recreated between test cases).  The following special methods may
 * be implemented to perform initialization and cleanup of the test fixture
 * and each test:
 *
 * @code
 *   // perform one-time initialization before any testcases run.
 *   virtual void setupFixture();
 *   // perform one-time initialization after all testcases run.
 *   virtual void teardownFixture();
 *   // perform initialization before each testcase
 *   virtual void setup();
     // perform cleanup after each testcase
 *   virtual void teardown();
 * @endcode
 *
 **/


// TODO add support for temporary and test data files.

#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Verify.h>

#include <cmath>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace csp {

namespace test {

CSP_EXCEPTION(PassTest)
CSP_EXCEPTION(FailTest)

/** Convenience macro for reporting a test case failure.  Can (and should) be followed
 *  by stream output to explain the failure (e.g., CSP_FAILTEST << "File not found";).
 */
#define CSP_FAILTEST CSPLOG(FATAL, TESTING)

/** Convenience macro for reporting a test case success.  An alternative is to
 *  return true from the test case method, but this macro has the advantage of
 *  working from within helper methods as well.
 */
#define CSP_PASSTEST throw csp::test::PassTest();

/// Single precision floating point comparison operations.  The nearness criterion for equality
/// tests is chosen to be roughly half the precision of the representation.  For comparison
/// against zero, differences up to 4*epsilon are allowed.
inline bool float_near_ratio(float x, float y) {
	return (x >= 0.0f) ? (y >= x * 0.99980f && y <= x * 1.00020f) :
	                     (y <= x * 0.99980f && y >= x * 1.00020f);
}
inline bool float_zero(float x) { return fabsf(x) <= 4.0f * std::numeric_limits<float>::epsilon(); }
inline bool float_eq(float x, float y) { return (x == 0.f) ? float_zero(y) : (y == 0.f) ? float_zero(x) : float_near_ratio(x, y); }
inline bool float_le(float x, float y) { return x <= y || float_eq(x, y); }
inline bool float_ge(float x, float y) { return x >= y || float_eq(x, y); }


/// Double precision floating point comparison operations.  The nearness criterion for equality
/// tests is chosen to be roughly half the precision of the representation.  For comparison
/// against zero, differences up to 4*epsilon are allowed.
inline bool double_near_ratio(double x, double y) {
	return (x >= 0.0) ? (y >= x * 0.999999990 && y <= x * 1.000000010) :
	                    (y <= x * 0.999999990 && y >= x * 1.000000010);
}
inline bool double_zero(double x) { return fabs(x) <= 4.0 * std::numeric_limits<double>::epsilon(); }
inline bool double_eq(double x, double y) { return (x == 0.0) ? double_zero(y) : (y == 0.0) ? double_zero(x) : double_near_ratio(x, y); }
inline bool double_le(double x, double y) { return x <= y || double_eq(x, y); }
inline bool double_ge(double x, double y) { return x >= y || double_eq(x, y); }


template <typename A, typename B>
struct equal_to { bool operator()(A const &a, B const &b) const { return a == b; } };
template <typename A, typename B>
struct less_than { bool operator()(A const &a, B const &b) const { return a < b; } };
template <typename A, typename B>
struct less_equal { bool operator()(A const &a, B const &b) const { return a <= b; } };
template <typename A, typename B>
struct not_equal_to { bool operator()(A const &a, B const &b) const { return !equal_to<A,B>()(a, b); } };
template <typename A, typename B>
struct greater_than { bool operator()(A const &a, B const &b) const { return !less_equal<A,B>()(a, b); } };
template <typename A, typename B>
struct greater_equal { bool operator()(A const &a, B const &b) const { return !less_than<A,B>()(a, b); } };
template <typename A >
struct float_equal { bool operator()(A const &a, A const &b) const { return float_eq(a, b); } };
template <typename A>
struct float_not_equal { bool operator()(A const &a, A const &b) const { return !float_eq(a, b); } };
template <typename A>
struct float_less_equal { bool operator()(A const &a, A const &b) const { return float_le(a, b); } };
template <typename A>
struct float_greater_equal { bool operator()(A const &a, A const &b) const { return float_ge(a, b); } };
template <typename A >
struct double_equal { bool operator()(A const &a, A const &b) const { return double_eq(a, b); } };
template <typename A>
struct double_not_equal { bool operator()(A const &a, A const &b) const { return !double_eq(a, b); } };
template <typename A>
struct double_less_equal { bool operator()(A const &a, A const &b) const { return double_le(a, b); } };
template <typename A>
struct double_greater_equal { bool operator()(A const &a, A const &b) const { return double_ge(a, b); } };

template <> struct equal_to<const char*, const char*> { bool operator()(const char *a, const char *b) const { return !strcmp(a, b); } };
template <> struct less_than<const char*, const char*> { bool operator()(const char *a, const char *b) const { return strcmp(a, b) < 0; } };
template <> struct less_equal<const char*, const char*> { bool operator()(const char *a, const char *b) const { return strcmp(a, b) <= 0; } };

#define CSP_OPERATOR(NAME, OP, REP, NREP) \
	struct NAME { \
		const char *rep() const { return REP; } \
		const char *nrep() const { return NREP; } \
		template <typename A, typename B> \
		bool operator()(const A& a, const B& b) const { return OP<A,B>()(a, b); } \
	};

#define CSP_TYPED_OPERATOR(NAME, TYPE, OP, REP, NREP) \
	struct NAME { \
		const char *rep() const { return REP; } \
		const char *nrep() const { return NREP; } \
		bool operator()(const TYPE& a, const TYPE& b) const { return OP<TYPE>()(a, b); } \
	};

CSP_OPERATOR(EqualTo, equal_to, "==", "!=")
CSP_OPERATOR(NotEqualTo, not_equal_to, "!=", "==")
CSP_OPERATOR(LessThan, less_than, "<", ">=")
CSP_OPERATOR(GreaterThan, greater_than, ">", "<=")
CSP_OPERATOR(LessEqual, less_equal, "<=", ">")
CSP_OPERATOR(GreaterEqual, greater_equal, ">=", "<")
CSP_TYPED_OPERATOR(FloatEqualTo, float, float_equal, "~=", "!=")
CSP_TYPED_OPERATOR(FloatNotEqualTo, float, float_not_equal, "!=", "~=")
CSP_TYPED_OPERATOR(FloatLessEqual, float, float_less_equal, "<~", ">")
CSP_TYPED_OPERATOR(FloatGreaterEqual, float, float_greater_equal, ">~", "<")
CSP_TYPED_OPERATOR(DoubleEqualTo, double, double_equal, "~=", "!=")
CSP_TYPED_OPERATOR(DoubleNotEqualTo, double, double_not_equal, "!=", "~=")
CSP_TYPED_OPERATOR(DoubleLessEqual, double, double_less_equal, "<~", ">")
CSP_TYPED_OPERATOR(DoubleGreaterEqual, double, double_greater_equal, ">~", "<")


class CSPLIB_EXPORT TestResult {
public:
	template <typename A, typename B, typename OP>
	TestResult(A const &a, B const &b, OP const &op, const char *astr, const char *bstr) {
		if (!op(a, b)) {
			std::stringstream ss;
			ss << "TEST FAILED (" << astr << op.rep() << bstr << ") [" << a << op.nrep() << b << "] ";
			_msg = ss.str();
		}
	}
	operator bool() const { return _msg.empty(); }
	std::string const &msg() { return _msg; }
private:
	std::string _msg;
};


class CSPLIB_EXPORT TestLogEntry {
public:
	typedef enum { NONE, PASS, FAIL, SOFTFAIL } Mode;
	TestLogEntry(const char *filename, int linenum, Mode mode): _filename(filename), _linenum(linenum), _mode(mode) {}
	~TestLogEntry();
	template <typename T> TestLogEntry & operator<<(T const& x) { _buffer << x; return *this; }
	template <typename T> TestLogEntry & operator<<(T& (*formatter)(T&)) { _buffer << formatter; return *this; }

	void check();
private:
	LogStream::LogEntry::BufferStream _buffer;
	const char *_filename;
	int _linenum;
	Mode _mode;
};

#define CSP_TEST__(A, B, OP, MODE) \
	if (csp::test::TestResult result = csp::test::TestResult(A, B, OP, #A, #B)); \
	else { \
		auto entry = csp::test::TestLogEntry(__FILE__, __LINE__, csp::test::TestLogEntry::MODE); \
		entry << result.msg() << " "; \
		entry.check(); \
	}

#define CSP_TEST_U__(A, MODE) \
	if (A); \
	else { \
		auto entry = csp::test::TestLogEntry(__FILE__, __LINE__, csp::test::TestLogEntry::MODE); \
		entry << "TEST FAILED (" #A ") "; \
		entry.check(); \
	}

#define CSP_ENSURE(A) CSP_TEST_U__(A, FAIL)
#define CSP_ENSURE_TRUE(A) CSP_TEST_U__(A, FAIL)
#define CSP_ENSURE_FALSE(A) CSP_TEST_U__(!(A), FAIL)
#define CSP_ENSURE_EQ(A, B)  CSP_TEST__(A, B, csp::test::EqualTo(), FAIL)
#define CSP_ENSURE_NE(A, B)  CSP_TEST__(A, B, csp::test::NotEqualTo(), FAIL)
#define CSP_ENSURE_LT(A, B)  CSP_TEST__(A, B, csp::test::LessThan(), FAIL)
#define CSP_ENSURE_GT(A, B)  CSP_TEST__(A, B, csp::test::GreaterThan(), FAIL)
#define CSP_ENSURE_LE(A, B)  CSP_TEST__(A, B, csp::test::LessEqual(), FAIL)
#define CSP_ENSURE_GE(A, B)  CSP_TEST__(A, B, csp::test::GreaterEqual(), FAIL)
#define CSP_ENSURE_NULL(A) CSP_TEST_U__((A) == (void*)(NULL), FAIL)
#define CSP_ENSURE_NOTNULL(A) CSP_TEST_U__((A) != (void*)(NULL), FAIL)
#define CSP_ENSURE_FEQ(A, B)  CSP_TEST__(A, B, csp::test::FloatEqualTo(), FAIL)
#define CSP_ENSURE_FNE(A, B)  CSP_TEST__(A, B, csp::test::FloatNotEqualTo(), FAIL)
#define CSP_ENSURE_FLE(A, B)  CSP_TEST__(A, B, csp::test::FloatLessEqual(), FAIL)
#define CSP_ENSURE_FGE(A, B)  CSP_TEST__(A, B, csp::test::FloatGreaterEqual(), FAIL)
#define CSP_ENSURE_DEQ(A, B)  CSP_TEST__(A, B, csp::test::DoubleEqualTo(), FAIL)
#define CSP_ENSURE_DNE(A, B)  CSP_TEST__(A, B, csp::test::DoubleNotEqualTo(), FAIL)
#define CSP_ENSURE_DLE(A, B)  CSP_TEST__(A, B, csp::test::DoubleLessEqual(), FAIL)
#define CSP_ENSURE_DGE(A, B)  CSP_TEST__(A, B, csp::test::DoubleGreaterEqual(), FAIL)

#define CSP_EXPECT(A) CSP_TEST_U__(A, SOFTFAIL)
#define CSP_EXPECT_TRUE(A) CSP_TEST_U__(A, SOFTFAIL)
#define CSP_EXPECT_FALSE(A) CSP_TEST_U__(!(A), SOFTFAIL)
#define CSP_EXPECT_EQ(A, B)  CSP_TEST__(A, B, csp::test::EqualTo(), SOFTFAIL)
#define CSP_EXPECT_NE(A, B)  CSP_TEST__(A, B, csp::test::NotEqualTo(), SOFTFAIL)
#define CSP_EXPECT_LT(A, B)  CSP_TEST__(A, B, csp::test::LessThan(), SOFTFAIL)
#define CSP_EXPECT_GT(A, B)  CSP_TEST__(A, B, csp::test::GreaterThan(), SOFTFAIL)
#define CSP_EXPECT_LE(A, B)  CSP_TEST__(A, B, csp::test::LessEqual(), SOFTFAIL)
#define CSP_EXPECT_GE(A, B)  CSP_TEST__(A, B, csp::test::GreaterEqual(), SOFTFAIL)
#define CSP_EXPECT_NULL(A) CSP_TEST_U__((A) == (void*)NULL, SOFTFAIL)
#define CSP_EXPECT_NOTNULL(A) CSP_TEST_U__((A) != (void*)NULL, SOFTFAIL)
#define CSP_EXPECT_FEQ(A, B)  CSP_TEST__(A, B, csp::test::FloatEqualTo(), SOFTFAIL)
#define CSP_EXPECT_FNE(A, B)  CSP_TEST__(A, B, csp::test::FloatNotEqualTo(), SOFTFAIL)
#define CSP_EXPECT_FLE(A, B)  CSP_TEST__(A, B, csp::test::FloatLessEqual(), SOFTFAIL)
#define CSP_EXPECT_FGE(A, B)  CSP_TEST__(A, B, csp::test::FloatGreaterEqual(), SOFTFAIL)
#define CSP_EXPECT_DEQ(A, B)  CSP_TEST__(A, B, csp::test::DoubleEqualTo(), SOFTFAIL)
#define CSP_EXPECT_DNE(A, B)  CSP_TEST__(A, B, csp::test::DoubleNotEqualTo(), SOFTFAIL)
#define CSP_EXPECT_DLE(A, B)  CSP_TEST__(A, B, csp::test::DoubleLessEqual(), SOFTFAIL)
#define CSP_EXPECT_DGE(A, B)  CSP_TEST__(A, B, csp::test::DoubleGreaterEqual(), SOFTFAIL)


typedef void (*test_cb)();

struct TestInstance {
	std::string name;
	test_cb test;
	test_cb setup;
	test_cb teardown;

	TestInstance(){}
	TestInstance(const std::string &name, test_cb test, test_cb setup=NULL, test_cb teardown=NULL);
};

/** A global (static) registry of test fixtures.  Provides a C++ and Python
 *  interface for running test fixtures.
 */
class CSPLIB_EXPORT TestRegistry: public NonConstructable {
public:
	static std::map<std::string, TestInstance> &tests();

	static void addTest(const TestInstance& test);
	static bool runTest(const TestInstance& test);

	static bool loadTestModule(const char *module);
	static bool runAll();
	static bool runOnePath(const char *path);
	static bool runOneTest(const char *test);
};

} // namespace test

} // namespace csp
