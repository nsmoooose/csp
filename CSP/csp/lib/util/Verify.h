/* Combat Simulator Project
 * Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Verify.h
 * @brief VERIFY and ASSERT tests that log failures.
 *
 * Provides both ASSERT and VERIFY macros for runtime assertions.  The
 * former macros are evaluated only in debug mode, while the latter are
 * always evaluated.  Each type has specializations for comparing two
 * arguments, which log on failure both the literal comparison and the
 * values of the two parameters.  Example usage:
 *
 * @code
 * void set_age(char *name, int age) {
 *   CSP_ASSERT(name != NULL);
 *   CSP_VERIFY_LT(age, 100);
 *   ...
 * }
 *
 * // if called with age=120, the program would abort with the following
 * // log message:
 * //
 * // E (myfile.cpp:233) VERIFY_LT(age, 100) FAILED  [120 >= 100]
 * @endcode
 *
 */


#ifndef __CSPLIB_UTIL_VERIFY_H__
#define __CSPLIB_UTIL_VERIFY_H__


#include <csp/lib/util/Log.h>
#include <limits>


CSP_NAMESPACE


#ifdef __GNUC__
#	define CSP_NORETURN  __attribute__ ((__noreturn__))
#else
#	define CSP_NORETURN
#endif

template <typename A, typename B>
void __diagnose_op_failure(A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s,
                           const char *file, int line, const char *type, const char *op) CSP_NORETURN;

template <typename A, typename B>
void __diagnose_op_failure(A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s,
                           const char *file, int line, const char *type, const char *op) {
	LogStream::LogEntry(CSPLOG_, CSPLOG_PRIORITY(FATAL), file, line)
		<< type << '(' << lhs_s << ", " << rhs_s << ") FAILED  [" << lhs << op << rhs << ']';
	::abort();  // should never get here (log fatal above), but need to ensure __noreturn__.
}

// __diagnose_failure is declared as a template to avoid having
// to move the definition out of the header.  this is convenient
// during testing, but there isn't a good long term reason for
// doing so, since CSP_LOG_ requires that the full library
// be linked.
template <typename A>
void __diagnose_failure(const char *expr_s, const char *file, A line, const char *type) CSP_NORETURN;

template <typename A>
void __diagnose_failure(const char *expr_s, const char *file, A line, const char *type) {
	LogStream::LogEntry(CSPLOG_, CSPLOG_PRIORITY(FATAL), file, line) << type << '(' << expr_s << ") FAILED";
	::abort();  // should never get here (log fatal above), but need to ensure __noreturn__.
}

template <typename A>
inline void verify(const char *type, A const &expr, const char *expr_s, const char *file, int line) {
	if (!expr) __diagnose_failure(expr_s, file, line, type);
}

template <typename A, typename B>
inline void verify_eq(const char *type, A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s, const char *file, int line) {
	if (lhs != rhs) __diagnose_op_failure(lhs, rhs, lhs_s, rhs_s, file, line, type, " != ");
}

template <typename A, typename B>
inline void verify_ne(const char *type, A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s, const char *file, int line) {
	if (lhs == rhs) __diagnose_op_failure(lhs, rhs, lhs_s, rhs_s, file, line, type, " == ");
}

template <typename A, typename B>
inline void verify_gt(const char *type, A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s, const char *file, int line) {
	if (lhs <= rhs) __diagnose_op_failure(lhs, rhs, lhs_s, rhs_s, file, line, type, " <= ");
}

template <typename A, typename B>
inline void verify_lt(const char *type, A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s, const char *file, int line) {
	if (lhs >= rhs) __diagnose_op_failure(lhs, rhs, lhs_s, rhs_s, file, line, type, " >= ");
}

template <typename A, typename B>
inline void verify_ge(const char *type, A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s, const char *file, int line) {
	if (lhs < rhs) __diagnose_op_failure(lhs, rhs, lhs_s, rhs_s, file, line, type, " < ");
}

template <typename A, typename B>
inline void verify_le(const char *type, A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s, const char *file, int line) {
	if (lhs > rhs) __diagnose_op_failure(lhs, rhs, lhs_s, rhs_s, file, line, type, " > ");
}

template <typename A>
inline void verify_floateq(const char *type, A const &lhs, A const &rhs, const char *lhs_s, const char *rhs_s, const char *file, int line) {
	if (std::abs(lhs - rhs) > 2 * std::numeric_limits<A>::epsilon())  __diagnose_op_failure(lhs, rhs, lhs_s, rhs_s, file, line, type, " ~= ");
}

#define CSP_VERIFY(A) CSP(verify)("VERIFY", A, #A, __FILE__, __LINE__)
#define CSP_VERIFY_EQ(A, B) CSP(verify_eq)("VERIFY_EQ", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY equal
#define CSP_VERIFY_NE(A, B) CSP(verify_ne)("VERIFY_NE", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY not equal
#define CSP_VERIFY_GT(A, B) CSP(verify_gt)("VERIFY_GT", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY greater than
#define CSP_VERIFY_LT(A, B) CSP(verify_lt)("VERIFY_LT", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY less than
#define CSP_VERIFY_LE(A, B) CSP(verify_le)("VERIFY_LE", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY less than or equal
#define CSP_VERIFY_GE(A, B) CSP(verify_ge)("VERIFY_GE", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY greater than or equal
#define CSP_VERIFY_FLOATEQ(A, B) CSP(verify_floateq)("VERIFY_FLOATEQ", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY floating point values equal

#ifndef NDEBUG
#	define CSP_ASSERT(A) CSP(verify)("ASSERT", A, #A, __FILE__, __LINE__)
#	define CSP_ASSERT_EQ(A, B) CSP(verify_eq)("ASSERT_EQ", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT equal
#	define CSP_ASSERT_NE(A, B) CSP(verify_ne)("ASSERT_NE", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT not equal
#	define CSP_ASSERT_GT(A, B) CSP(verify_gt)("ASSERT_GT", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT greater than
#	define CSP_ASSERT_LT(A, B) CSP(verify_lt)("ASSERT_LT", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT less than
#	define CSP_ASSERT_LE(A, B) CSP(verify_le)("ASSERT_LE", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT less than or equal
#	define CSP_ASSERT_GE(A, B) CSP(verify_ge)("ASSERT_GE", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT greater than or equal
#	define CSP_ASSERT_FLOATEQ(A, B) CSP(verify_floateq)("VERIFY_FLOATEQ", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT floating point values equal
#else
#	define CSP_ASSERT(A)
#	define CSP_ASSERT_EQ(A, B)  //< ASSERT equal
#	define CSP_ASSERT_NE(A, B)  //< ASSERT not equal
#	define CSP_ASSERT_GT(A, B)  //< ASSERT greater than
#	define CSP_ASSERT_LT(A, B)  //< ASSERT less than
#	define CSP_ASSERT_LE(A, B)  //< ASSERT less than or equal
#	define CSP_ASSERT_GE(A, B)  //< ASSERT greater than or equal
#	define CSP_ASSERT_FLOATEQ(A, B)  //< ASSERT floating point values equal
#endif

CSP_NAMESPACE_END


#endif // __CSPLIB_UTIL_VERIFY_H__
