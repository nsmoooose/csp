/* SimData: Data Infrastructure for Simulations
 * Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
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
 *   SIMDATA_ASSERT(name != NULL);
 *   SIMDATA_VERIFY_LT(age, 100);
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


#ifndef __SIMDATA_VERIFY_H__
#define __SIMDATA_VERIFY_H__


#include <cstdlib>
#include <SimData/Log.h>


NAMESPACE_SIMDATA


#ifdef __GNUC__
#	define SIMDATA_NORETURN  __attribute__ ((__noreturn__))
#else
#	define SIMDATA_NORETURN
#endif

template <typename A, typename B>
void __diagnose_op_failure(A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s,
                           const char *file, int line, const char *type, const char *op) SIMDATA_NORETURN;

template <typename A, typename B>
void __diagnose_op_failure(A const &lhs, B const &rhs, const char *lhs_s, const char *rhs_s,
                           const char *file, int line, const char *type, const char *op)
{
	SIMDATA_LOG_.entry(simdata::LOG_ERROR, simdata::LOG_ALL, file, line)
		<< type << '(' << lhs_s << ", " << rhs_s << ") FAILED  ["
		<< lhs << op << rhs << ']' << std::endl;
	std::abort();
}

// __diagnose_failure is declared as a template to avoid having
// to move the definition out of the header.  this is convenient
// during testing, but there isn't a good long term reason for
// doing so, since SIMDATA_LOG_ requires that the full library
// be linked.
template <typename A>
void __diagnose_failure(const char *expr_s, const char *file, A line, const char *type) SIMDATA_NORETURN;

template <typename A>
void __diagnose_failure(const char *expr_s, const char *file, A line, const char *type)
{
	SIMDATA_LOG_.entry(simdata::LOG_ERROR, simdata::LOG_ALL, file, line)
		<< type << '(' << expr_s << ") FAILED" << std::endl;
	std::abort();
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

#define SIMDATA_VERIFY(A) SIMDATA(verify)("VERIFY", A, #A, __FILE__, __LINE__)
#define SIMDATA_VERIFY_EQ(A, B) SIMDATA(verify_eq)("VERIFY_EQ", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY equal
#define SIMDATA_VERIFY_NE(A, B) SIMDATA(verify_ne)("VERIFY_NE", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY not equal
#define SIMDATA_VERIFY_GT(A, B) SIMDATA(verify_gt)("VERIFY_GT", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY greater than
#define SIMDATA_VERIFY_LT(A, B) SIMDATA(verify_lt)("VERIFY_LT", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY less than
#define SIMDATA_VERIFY_LE(A, B) SIMDATA(verify_le)("VERIFY_LE", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY less than or equal
#define SIMDATA_VERIFY_GE(A, B) SIMDATA(verify_ge)("VERIFY_GE", A, B, #A, #B, __FILE__, __LINE__)  //< VERIFY greater than or equal

#ifndef NDEBUG
#	define SIMDATA_ASSERT(A) SIMDATA(verify)("ASSERT", A, #A, __FILE__, __LINE__)
#	define SIMDATA_ASSERT_EQ(A, B) SIMDATA(verify_eq)("ASSERT_EQ", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT equal
#	define SIMDATA_ASSERT_NE(A, B) SIMDATA(verify_ne)("ASSERT_NE", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT not equal
#	define SIMDATA_ASSERT_GT(A, B) SIMDATA(verify_gt)("ASSERT_GT", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT greater than
#	define SIMDATA_ASSERT_LT(A, B) SIMDATA(verify_lt)("ASSERT_LT", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT less than
#	define SIMDATA_ASSERT_LE(A, B) SIMDATA(verify_le)("ASSERT_LE", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT less than or equal
#	define SIMDATA_ASSERT_GE(A, B) SIMDATA(verify_ge)("ASSERT_GE", A, B, #A, #B, __FILE__, __LINE__)  //< ASSERT greater than or equal
#else
#	define SIMDATA_ASSERT(A)
#	define SIMDATA_ASSERT_EQ(A, B)  //< ASSERT equal
#	define SIMDATA_ASSERT_NE(A, B)  //< ASSERT not equal
#	define SIMDATA_ASSERT_GT(A, B)  //< ASSERT greater than
#	define SIMDATA_ASSERT_LT(A, B)  //< ASSERT less than
#	define SIMDATA_ASSERT_LE(A, B)  //< ASSERT less than or equal
#	define SIMDATA_ASSERT_GE(A, B)  //< ASSERT greater than or equal
#endif

NAMESPACE_SIMDATA_END


#endif // __SIMDATA_VERIFY_H__
