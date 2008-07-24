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

bool TestCase::_testcase_failed = false;
std::vector<std::string> TestCase::_testcase_messages;

class StandardReporter : public TestReporter {
public:
	StandardReporter(): m_successes(0), m_failures(0) {}

	virtual Log *getFixtureLog() { return &m_fixture_log; }

	virtual void beginFixture(std::string const &name) {
		CSPLOG(INFO, TESTING) << "Starting test fixture " << name;
		std::cout << "\nTESTING " << name << "\n";
		m_fixture_name = name;
		m_last_successes = 0;
		m_last_failures = 0;
		m_fixture_log.clear();
	}

	virtual void beginTestCase(std::string const &name) {
		m_test_name = name;
		CSPLOG(INFO, TESTING) << "Starting test " << getTestName();
		m_test_timer.start();
	}

	virtual void endTestCase(bool success) {
		m_test_timer.stop();
		const int time_ms = static_cast<int>(m_test_timer.elapsed() * 1000.0);
		CSPLOG(INFO, TESTING) << "Finished test " << getTestName() << ": " << (success ? "PASS" : "FAIL") << " [" << time_ms << " ms]";
		if (success) {
			std::cout << "        - " << m_test_name << "\n";
			m_successes++;
			m_last_successes++;
		} else {
			std::cout << " *FAIL* - " << m_test_name << "\n";
			m_failures++;
			m_last_failures++;
		}
	}

	virtual void end() {
		if (m_failures) {
			std::cout << "\nFAILED TESTCASES [" << m_failures << " of " << (m_successes + m_failures) << "]\n";
			for (int i = 0; i < m_log.size(); ++i) {
				const Log::TestCaseEntry *entry = m_log(i);
				if (!entry->success) {
					std::cout << "  - " << entry->fixture_name << "." << entry->testcase_name << "\n";
					for (unsigned j = 0; j < entry->log.size(); ++j) {
						std::cout << "     " << entry->log[j] << "\n";
					}
				}
			}
		} else {
			std::cout << "\nPASSED\n";
		}
	}

	virtual void endFixture() {
		/*
		if (m_last_failures) {
			CSPLOG(INFO, TESTING) << "Test fixture " << m_fixture_name << " had " << m_last_failures << " failures";
			std::cout << "\nFAILED TESTCASES [" << m_last_failures << " of " << (m_last_failures + m_last_successes) << "]\n";
		}
		*/
		m_log.add(m_fixture_log);
	}

protected:
	std::string getTestName() const {
		return m_fixture_name + "::" + m_test_name;
	}

private:
	Timer m_test_timer;
	std::string m_fixture_name;
	std::string m_test_name;
	int m_successes;
	int m_failures;
	int m_last_failures;
	int m_last_successes;
    Log m_fixture_log;
	Log m_log;
};


struct TestRegistry::TestData {
	std::vector<TestRunner*> tests;
	std::map<std::string, TestRunner*> tests_by_name;
	std::map<std::string, std::vector<TestRunner*> > tests_by_file;
};

TestRegistry::TestData& TestRegistry::data() {
	static TestData *data = 0;
	if (!data) data = new TestData;
	return *data;
}

bool TestRegistry::loadTestModule(const char *module) {
	try {
		new ost::DSO(module);  // no unload
	} catch (...) {
		return false;
	}
	return true;
}

bool TestRegistry::_runTests(std::vector<TestRunner*> const &tests) {
	const bool old_throw_on_fatal = log().getThrowOnFatal();
	log().setThrowOnFatal(true);
	StandardReporter reporter;
	bool success = true;
	reporter.begin();
	for (std::vector<TestRunner*>::const_iterator iter = tests.begin(); iter != tests.end(); ++iter) {
		TestRunner* runner = *iter;
		success = runner->runTests(reporter) && success;
	}
	reporter.end();
	log().setThrowOnFatal(old_throw_on_fatal);
	return success;
}

bool TestRegistry::runAll() {
	return _runTests(data().tests);
}

bool TestRegistry::runOnePath(const char *path) {
	std::map<std::string, std::vector<TestRunner*> >::const_iterator iter = data().tests_by_file.begin();
	std::map<std::string, std::vector<TestRunner*> >::const_iterator end = data().tests_by_file.end();
	std::vector<TestRunner*> tests;
	for (; iter != end; ++iter) {
		if (iter->first.find(path) == 0) {
			tests.insert(tests.end(), iter->second.begin(), iter->second.end());
		}
	}
	return _runTests(tests);
}

bool TestRegistry::runOneTest(const char *test) {
	std::map<std::string, TestRunner*>::const_iterator iter = data().tests_by_name.find(test);
	if (iter == data().tests_by_name.end()) {
		CSPLOG(ERROR, TESTING) << "Unknown test fixture '" << test << "'";
		return false;
	}
	std::vector<TestRunner*> tests;
	tests.push_back(iter->second);
	return _runTests(tests);
}

void TestRegistry::addTestRunner(TestRunner *runner) {
	data().tests.push_back(runner);

	// duplicates can occur for tests in different namespaces since the namespace isn't part
	// of the fixture name.  in that case its best to explicitly include the namespace in the
	// CSP_TESTFIXTURE declaration.
	bool duplicate = !data().tests_by_name.insert(std::make_pair(runner->name(), runner)).second;
	if (duplicate) {
		CSPLOG(ERROR, TESTING) << "Duplicate test fixture name " << runner->name();
	}

	std::string dirname = ospath::skiproot(ospath::dirname(runner->file()));
	data().tests_by_file[dirname].push_back(runner);
}

TestRunner::TestRunner(const char* fixture_name, const char *file): m_fixture_name(fixture_name), m_file(file) {
	TestRegistry::addTestRunner(this);
}

bool TestRunner::runTests(TestReporter &reporter) {
	reporter.beginFixture(m_fixture_name);
	bool success = _runTests(reporter);
	reporter.endFixture();
	return success;
}

// Test execution must be done inside Testing.cpp, rather than the header,
// so that FatalException is caught within the csplib library.  The VERIFY
// macros generate the exception via LogStream.cpp, and catching within the
// same library ensures that RTTI matching works properly.
bool TestRunner::_runTest(Test const &test, TestReporter::Log *log) {
	bool ok = false;
	std::string fatal_error;
	try {
		ok = test.run();
	} catch (FatalException const &e) {
		TestCase::AddMessage(e.getError());
		e.clear();  // already logged
	} catch (PassTest const &) {
		ok = true;
	} catch (FailTest const &) {
		ok = false;
	}
	if (log) log->add(name(), test.name(), ok);
	return ok;
}


TestLogEntry::~TestLogEntry() {
	LogStream::LogEntry(CSPLOG_, CSPLOG_PRIORITY(ERROR), _filename, _linenum) << _buffer.get();
	TestCase::AddMessage(stringprintf("%s:%d %s", _filename, _linenum, _buffer.get()));
	switch (_mode) {
		case PASS: throw PassTest();
		case FAIL: throw FailTest();
		case SOFTFAIL: TestCase::Fail();
		default: break;
	}
}

void TestReporter::Log::add(std::string const &fixture_name, std::string const &testcase_name, bool success) {
	TestCaseEntry::RefT entry = new TestCaseEntry;
	_log.push_back(entry);
	entry->fixture_name = fixture_name;
	entry->testcase_name = testcase_name;
	entry->success = success;
	TestCase::GetMessages(entry->log);
}

void TestReporter::Log::add(TestReporter::Log const &other) {
	_log.insert(_log.end(), other._log.begin(), other._log.end());
}

void TestReporter::Log::clear() {
	_log.clear();
}

} // namespace test

} // namespace csp

