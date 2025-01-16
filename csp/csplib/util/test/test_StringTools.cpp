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
 * @file test_StringTools.h
 * @brief Test for functions in csplib/util/StringTools.h.
 */


#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/Testing.h>

using namespace csp;
using namespace csp::test;

static void TestConvertStringToUpper() {
	std::string s;

	s = "";
	ConvertStringToUpper(s);
	CSP_EXPECT_EQ(s, "");

	s = "hello WorLd";
	ConvertStringToUpper(s);
	CSP_EXPECT_EQ(s, "HELLO WORLD");
}

static void TestConvertStringToLower() {
	std::string s;

	s = "";
	ConvertStringToLower(s);
	CSP_EXPECT_EQ(s, "");

	s = "hello WorLd";
	ConvertStringToLower(s);
	CSP_EXPECT_EQ(s, "hello world");
}

static void TestTokenize() {
	std::vector<std::string> words;
	Tokenize("59 Temple Place - Suite 330, Boston.", words);
	CSP_EXPECT_EQ(words.size(), 7U);
	CSP_EXPECT_EQ(words[0], "59");
	CSP_EXPECT_EQ(words[1], "Temple");
	CSP_EXPECT_EQ(words[2], "Place");
	CSP_EXPECT_EQ(words[3], "-");
	CSP_EXPECT_EQ(words[4], "Suite");
	CSP_EXPECT_EQ(words[5], "330");
	CSP_EXPECT_EQ(words[6], "Boston.");

	words.clear();
	Tokenize("A B.C.D", words, ".");
	CSP_EXPECT_EQ(words.size(), 3U);
	CSP_EXPECT_EQ(words[0], "A B");
	CSP_EXPECT_EQ(words[1], "C");
	CSP_EXPECT_EQ(words[2], "D");

	words.clear();
	Tokenize("", words);
	CSP_EXPECT_EQ(words.size(), 0U);
}

static void TestTokenQueue() {
	TokenQueue tq("59 Temple Place - Suite 330, Boston.");
	CSP_EXPECT_EQ(tq.size(), 7U);
	CSP_EXPECT_EQ(tq[0], "59");
	CSP_EXPECT_EQ(tq[1], "Temple");
	CSP_EXPECT_EQ(tq[2], "Place");
	CSP_EXPECT_EQ(tq[3], "-");
	CSP_EXPECT_EQ(tq[4], "Suite");
	CSP_EXPECT_EQ(tq[5], "330");
	CSP_EXPECT_EQ(tq[6], "Boston.");
}

static void TestTrimString() {
	CSP_EXPECT_EQ(TrimString(" HELLO world  "), "HELLO world");
	CSP_EXPECT_EQ(TrimString(" HELLO world\n  "), "HELLO world");
	CSP_EXPECT_EQ(TrimString("\t\tHELLO\tworld\n\n"), "HELLO\tworld");
}

static void TestLeftTrimString() {
	CSP_EXPECT_EQ(LeftTrimString(" HELLO world  "), "HELLO world  ");
	CSP_EXPECT_EQ(LeftTrimString(" HELLO world\n  "), "HELLO world\n  ");
	CSP_EXPECT_EQ(LeftTrimString("\t\tHELLO\tworld\n\n"), "HELLO\tworld\n\n");
}

static void TestRightTrimString() {
	CSP_EXPECT_EQ(RightTrimString(" HELLO world  "), " HELLO world");
	CSP_EXPECT_EQ(RightTrimString(" HELLO world\n  "), " HELLO world");
	CSP_EXPECT_EQ(RightTrimString("\t\tHELLO\tworld\n\n"), "\t\tHELLO\tworld");
}

static void TestSkipWhitespace() {
	const char *s;
	s = "abcd";
	CSP_EXPECT(skipWhitespace(s) == s);
	s = "  abcd";
	CSP_EXPECT(skipWhitespace(s) == s + 2);
	s = "\t\n  abcd";
	CSP_EXPECT(skipWhitespace(s) == s + 4);
	s = "\t\n  ";
	CSP_EXPECT(skipWhitespace(s) == s + 4);
	s = "";
	CSP_EXPECT(skipWhitespace(s) == s);
}

static void TestParseInt32() {
	int32_t x_int32;
	CSP_EXPECT(parseInt("0", x_int32));
	CSP_EXPECT_EQ(x_int32, 0);
	CSP_EXPECT(parseInt("000", x_int32));
	CSP_EXPECT_EQ(x_int32, 0);
	CSP_EXPECT(parseInt("0x000", x_int32));
	CSP_EXPECT_EQ(x_int32, 0);

	CSP_EXPECT(parseInt("42", x_int32));
	CSP_EXPECT_EQ(x_int32, 42);
	CSP_EXPECT(parseInt("42\t", x_int32));
	CSP_EXPECT_EQ(x_int32, 42);
	CSP_EXPECT(parseInt("\t42\t", x_int32));
	CSP_EXPECT_EQ(x_int32, 42);
	CSP_EXPECT(parseInt("\t \r  42\t\n ", x_int32));
	CSP_EXPECT_EQ(x_int32, 42);
	CSP_EXPECT(!parseInt("w 42", x_int32));
	CSP_EXPECT(!parseInt("42 w", x_int32));

	CSP_EXPECT(parseInt("0Xfa18", x_int32));
	CSP_EXPECT_EQ(x_int32, 0xfa18);
	CSP_EXPECT(parseInt("0xfa18", x_int32));
	CSP_EXPECT_EQ(x_int32, 0xfa18);
	CSP_EXPECT(parseInt("0xfa18933", x_int32));
	CSP_EXPECT_EQ(x_int32, 0xfa18933);
	CSP_EXPECT(parseInt("0373324", x_int32));
	CSP_EXPECT_EQ(x_int32, 0373324);
	CSP_EXPECT(parseInt("-0373324", x_int32));
	CSP_EXPECT_EQ(x_int32, -0373324);
	CSP_EXPECT(parseInt("-99991", x_int32));
	CSP_EXPECT_EQ(x_int32, -99991);
	CSP_EXPECT(parseInt("-0x1000", x_int32));
	CSP_EXPECT_EQ(x_int32, -0x1000);
	CSP_EXPECT(parseInt("-0x80000000", x_int32));
	CSP_EXPECT_EQ(x_int32, (-1 - 0x7fffffff)); // avoid compiler warning (signed/unsigned compare)
	CSP_EXPECT(parseInt("0x7fffffff", x_int32));
	CSP_EXPECT_EQ(x_int32, 0x7fffffff);
	CSP_EXPECT(!parseInt("0x80000000", x_int32));
	CSP_EXPECT(!parseInt("fa18", x_int32));
	CSP_EXPECT(!parseInt("-fa18", x_int32));
	CSP_EXPECT(!parseInt("0393324", x_int32));
	CSP_EXPECT(!parseInt("-0393324", x_int32));
	CSP_EXPECT(!parseInt("", x_int32));
}

static void TestParseIntU32() {
	uint32_t x_uint32;
	CSP_EXPECT(parseInt("0", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0U);
	CSP_EXPECT(parseInt("000", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0U);
	CSP_EXPECT(parseInt("0x000", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0U);

	CSP_EXPECT(parseInt("42", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 42U);
	CSP_EXPECT(parseInt("42\t", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 42U);
	CSP_EXPECT(parseInt("\t42\t", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 42U);
	CSP_EXPECT(parseInt("\t \r  42\t\n ", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 42U);
	CSP_EXPECT(!parseInt("w 42", x_uint32));
	CSP_EXPECT(!parseInt("42 w", x_uint32));

	CSP_EXPECT(parseInt("0Xfa18", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0xfa18U);
	CSP_EXPECT(parseInt("0xfa18", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0xfa18U);
	CSP_EXPECT(parseInt("0xfa18933", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0xfa18933U);
	CSP_EXPECT(parseInt("0373324", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0373324U);
	CSP_EXPECT(!parseInt("-0373324", x_uint32));
	CSP_EXPECT(!parseInt("-99991", x_uint32));
	CSP_EXPECT(!parseInt("-0x1000", x_uint32));
	CSP_EXPECT(!parseInt("-0x80000000", x_uint32));
	CSP_EXPECT(parseInt("0x7fffffff", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0x7fffffffU);
	CSP_EXPECT(parseInt("0x80000000", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0x80000000U);
	CSP_EXPECT(parseInt("0xffffffff", x_uint32));
	CSP_EXPECT_EQ(x_uint32, 0xffffffffU);
	CSP_EXPECT(!parseInt("0x100000000", x_uint32));
	CSP_EXPECT(!parseInt("fa18", x_uint32));
	CSP_EXPECT(!parseInt("-fa18", x_uint32));
	CSP_EXPECT(!parseInt("0393324", x_uint32));
	CSP_EXPECT(!parseInt("-0393324", x_uint32));
	CSP_EXPECT(!parseInt("", x_uint32));
}

static void TestParseIntLimits() {
	int8_t x_int8;
	uint8_t x_uint8;
	int16_t x_int16;
	uint16_t x_uint16;
	int32_t x_int32;
	uint32_t x_uint32;
	int64_t x_int64;
	uint64_t x_uint64;

	CSP_EXPECT(!parseInt("-0x81", x_int8));
	CSP_EXPECT(parseInt("-0x80", x_int8));
	CSP_EXPECT(parseInt("0x7f", x_int8));
	CSP_EXPECT(!parseInt("0x80", x_int8));

	CSP_EXPECT(!parseInt("-0x1", x_uint8));
	CSP_EXPECT(parseInt("0x0", x_uint8));
	CSP_EXPECT(parseInt("0x80", x_uint8));
	CSP_EXPECT(parseInt("0xff", x_uint8));
	CSP_EXPECT(!parseInt("0x100", x_uint8));

	CSP_EXPECT(!parseInt("-0x8001", x_int16));
	CSP_EXPECT(parseInt("-0x8000", x_int16));
	CSP_EXPECT(parseInt("0x7fff", x_int16));
	CSP_EXPECT(!parseInt("0x8000", x_int16));

	CSP_EXPECT(!parseInt("-0x1", x_uint16));
	CSP_EXPECT(parseInt("0x0", x_uint16));
	CSP_EXPECT(parseInt("0x8000", x_uint16));
	CSP_EXPECT(parseInt("0xffff", x_uint16));
	CSP_EXPECT(!parseInt("0x10000", x_uint16));

	CSP_EXPECT(!parseInt("-0x80000001", x_int32));
	CSP_EXPECT(parseInt("-0x80000000", x_int32));
	CSP_EXPECT(parseInt("0x7fffffff", x_int32));
	CSP_EXPECT(!parseInt("0x80000000", x_int32));

	CSP_EXPECT(!parseInt("-0x1", x_uint32));
	CSP_EXPECT(parseInt("0x0", x_uint32));
	CSP_EXPECT(parseInt("0x80000000", x_uint32));
	CSP_EXPECT(parseInt("0xffffffff", x_uint32));
	CSP_EXPECT(!parseInt("0x100000000", x_uint32));

	CSP_EXPECT(!parseInt("-0x8000000000000001", x_int64));
	CSP_EXPECT(parseInt("-0x8000000000000000", x_int64));
	CSP_EXPECT(parseInt("0x7fffffffffffffff", x_int64));
	CSP_EXPECT(!parseInt("0x8000000000000000", x_int64));

	CSP_EXPECT(!parseInt("-0x1", x_uint64));
	CSP_EXPECT(parseInt("0x0", x_uint64));
	CSP_EXPECT(parseInt("0x8000000000000000", x_uint64));
	CSP_EXPECT(parseInt("0xffffffffffffffff", x_uint64));
	CSP_EXPECT(!parseInt("0x10000000000000000", x_uint64));
}

static void TestStringPrintf() {
	CSP_EXPECT_EQ(stringprintf("hello world"), "hello world");
	CSP_EXPECT_EQ(stringprintf("hello %d world", 42), "hello 42 world");
	CSP_EXPECT_EQ(stringprintf("hello %d w%crld", 42, 'O'), "hello 42 wOrld");
	CSP_EXPECT_EQ(stringprintf("h%s %d w%crld", "i there", 42, 'O'), "hi there 42 wOrld");
	CSP_EXPECT_EQ(stringprintf("h%s %%", std::string("i there")), "hi there %");
	CSP_EXPECT_EQ(stringprintf("%%%d%%%d%%3", 1, 2), "%1%2%3");
	CSP_EXPECT_EQ(stringprintf("..%04d..", 21), "..0021..");
	CSP_EXPECT_EQ(stringprintf("..%+04d..", 21), "..+021..");
	CSP_EXPECT_EQ(stringprintf("..%+04d..", -21), "..-021..");
	CSP_EXPECT_EQ(stringprintf("..%+4d..", 21), ".. +21..");
	CSP_EXPECT_EQ(stringprintf("..%+4d..", -21), ".. -21..");
	CSP_EXPECT_EQ(stringprintf("..%x..", 26), "..1a..");
	CSP_EXPECT_EQ(stringprintf("..%X..", 26), "..1A..");
	CSP_EXPECT_EQ(stringprintf("..0x%llx..", 0xabcdef0123456789ULL), "..0xabcdef0123456789..");
	CSP_EXPECT_EQ(stringprintf("..%llu..", 0xabcdef0123456789ULL), "..12379813738877118345..");
	CSP_EXPECT_EQ(stringprintf("..%llo..", 0xabcdef0123456789ULL), "..1257157360044321263611..");
	CSP_EXPECT_EQ(stringprintf("%0.3f %06.3f %.5e %g", PI, PI, PI, PI), "3.142 03.142 3.14159e+00 3.14159");
	CSP_EXPECT_EQ(stringprintf("%g %g %g", 0.000015, 1500000.0, 150.0), "1.5e-05 1.5e+06 150");

	// bad format strings and arguments
	CSP_EXPECT_EQ(stringprintf("%Z"), "");  // bad format "%Z"
	CSP_EXPECT_EQ(stringprintf("abc %z def"), "abc ");  // bad format "%z "
	CSP_EXPECT_EQ(stringprintf("%d"), "");  // too few arguments
	CSP_EXPECT_EQ(stringprintf("%d %d", 1), "1 ");  // too few arguments
	CSP_EXPECT_EQ(stringprintf("%10000d", 1), "");  // limit is 4096
}

static void TestParseDouble() {
	double x;
	CSP_EXPECT_EQ(parseDouble("abc", x), false);

	CSP_EXPECT_EQ(parseDouble("2", x), true);
	CSP_EXPECT_EQ(x, 2.0);

	CSP_EXPECT_EQ(parseDouble("3.3", x), true);
	CSP_EXPECT_EQ(x, 3.3);

	CSP_EXPECT_EQ(parseDouble("-3.3", x), true);
	CSP_EXPECT_EQ(x, -3.3);

	CSP_EXPECT_EQ(parseDouble("3,3", x), false);
}

static void TestSplitString() {
	CSP_EXPECT_EQ(SplitString("123 456").size(), std::vector<std::string>::size_type(2));
	CSP_EXPECT_EQ(SplitString("abc def").size(), std::vector<std::string>::size_type(2));
	CSP_EXPECT_EQ(SplitString("").size(), std::vector<std::string>::size_type(0));
	CSP_EXPECT_EQ(SplitString("abc def")[0], std::string("abc"));
	CSP_EXPECT_EQ(SplitString("123 456")[1], std::string("456"));
}

__attribute__((constructor)) static void RegisterTests() {
	TestRegistry::addTest(TestInstance{"StringTools_TestConvertStringToUpper", &TestConvertStringToUpper});
	TestRegistry::addTest(TestInstance{"StringTools_TestConvertStringToLower", &TestConvertStringToLower});
	TestRegistry::addTest(TestInstance{"StringTools_TestTokenize", &TestTokenize});
	TestRegistry::addTest(TestInstance{"StringTools_TestTokenQueue", &TestTokenQueue});
	TestRegistry::addTest(TestInstance{"StringTools_TestTrimString", &TestTrimString});
	TestRegistry::addTest(TestInstance{"StringTools_TestLeftTrimString", &TestLeftTrimString});
	TestRegistry::addTest(TestInstance{"StringTools_TestRightTrimString", &TestRightTrimString});
	TestRegistry::addTest(TestInstance{"StringTools_TestSkipWhitespace", &TestSkipWhitespace});
	TestRegistry::addTest(TestInstance{"StringTools_TestParseInt32", &TestParseInt32});
	TestRegistry::addTest(TestInstance{"StringTools_TestParseIntU32", &TestParseIntU32});
	TestRegistry::addTest(TestInstance{"StringTools_TestParseIntLimits", &TestParseIntLimits});
	TestRegistry::addTest(TestInstance{"StringTools_TestStringPrintf", &TestStringPrintf});
	TestRegistry::addTest(TestInstance{"StringTools_TestParseDouble", &TestParseDouble});
	TestRegistry::addTest(TestInstance{"StringTools_TestSplitString", &TestSplitString});
}
