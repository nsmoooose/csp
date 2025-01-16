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
 * @file Testing.cpp
 * @brief Minimal interface to bind C++ unittests into a python module.
 **/

#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/util/Testing.h>
#include <csp/csplib/util/Timing.h>

#include <vector>
#include <cc++/file.h>
#include <csp/csplib/util/undef.h>

namespace csp {

namespace test {

void TestLogEntry::check() {
}

TestLogEntry::~TestLogEntry() {
}

TestInstance::TestInstance(const std::string &_name, test_cb _test, test_cb _setup, test_cb _teardown) :
	name(_name), test(_test), setup(_setup), teardown(_teardown) {
}

std::map<std::string, TestInstance> &TestRegistry::tests() {
	static std::map<std::string, TestInstance> tests;
	return tests;
}

void TestRegistry::addTest(const TestInstance& test) {
	tests()[test.name] = test;
}

bool TestRegistry::runTest(const TestInstance& test) {
	std::cout << "========================================" << std::endl;
	std::cout << "  " << test.name << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	bool result = true;
	try {
		if(test.setup) {
			test.setup();
		}
		test.test();
		if(test.teardown) {
			test.teardown();
		}
	}
	catch(const FatalException &e) {
		std::cout << e.getError() << std::endl;
		result = false;
	}
	catch(PassTest &e) {
		result = true;
	}
	catch(FailTest &e) {
		result = false;
	}
	catch(...) {
		result = false;
	}

	std::cout << "Finished test " << test.name << ":" << (result ? "PASS" : "FAIL") << std::endl;
	std::cout << "----------------------------------------" << std::endl << std::endl;
	return result;
}

bool TestRegistry::runAll() {
	std::cout << "Running all tests2" << std::endl;
	bool success = true;

	std::vector<std::string> all_results;
	int pass_count = 0;
	int fail_count = 0;

	for(auto &test : tests()) {
		bool atest = runTest(test.second);
		pass_count += atest ? 1 : 0;
		fail_count += atest ? 0 : 1;
		success = runTest(test.second) && success;;
	}

	std::cout << "Summary" << std::endl;
	std::cout << " " << pass_count << " tests are passing" << std::endl;
	std::cout << " " << fail_count << " tests are FAILING" << std::endl;
	std::cout << " " << pass_count + fail_count << " total number of tests" << std::endl;
	return success;
}

bool TestRegistry::loadTestModule(const char *module) {
	try {
		new ost::DSO(module);  // no unload
	} catch (...) {
		return false;
	}
	return true;
}

bool TestRegistry::runOnePath(const char *path) {
	return false;
}

bool TestRegistry::runOneTest(const char *test) {
	return false;
}

} // namespace test

} // namespace csp
