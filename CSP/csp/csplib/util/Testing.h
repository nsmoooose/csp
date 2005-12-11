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

#ifndef __CSPLIB_UTIL_TESTING_H__
#define __CSPLIB_UTIL_TESTING_H__


#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/Verify.h>

#include <string>
#include <vector>
#include <map>

CSP_NAMESPACE

CSP_EXCEPTION(PassTest)
CSP_EXCEPTION(FailTest)


/** Begin the declaration of a test fixture.  The NAME argument is an arbitrary
 *  identifier for the test fixture.  The name is prefixed internally to avoid
 *  conflicts with existing symbols, but multiple test fixtures cannot share the
 *  same name (within the same namespace).  The macro should be followed by a
 *  open brace, test cases and other declarations, a closing brace, and a final
 *  semicolon.
 */
#define CSP_TESTFIXTURE(NAME) \
	class csp_testfixture_##NAME; \
	static CSP(TypedTestRunner)<csp_testfixture_##NAME> csp_runner_##NAME(#NAME, __FILE__); \
	class csp_testfixture_##NAME : public CSP(TypedTestFixture)<csp_testfixture_##NAME>

/** Begin the declaration of a testcase.  The NAME argument is an arbitrary
 *  identifier for the test case.  It must be unique within the enclosing
 *  test fixture.  The macro should be followed by an open brace, the test
 *  case code, and a final closing brace.  Code within the test case can
 *  reference member variables and methods of the enclosing test fixture.
 */
#define CSP_TESTCASE(NAME) \
	struct csp_testcase_##NAME: public CSP(TestCase)<TestFixtureClass> { \
		csp_testcase_##NAME(): CSP(TestCase)<TestFixtureClass>(#NAME, &TestFixtureClass::NAME) {} \
	} csp_testcase_##NAME##_member; \
	void NAME()

/** Convenience macro for reporting a test case failure.  Can (and should) be followed
 *  by stream output to explain the failure (e.g., CSP_FAILTEST << "File not found";).
 */
#define CSP_FAILTEST CSPLOG(FATAL, TESTING)

/** Convenience macro for reporting a test case success.  An alternative is to
 *  return true from the test case method, but this macro has the advantage of
 *  working from within helper methods as well.
 */
#define CSP_PASSTEST throw CSP(PassTest());


/*
LogStream& testlog();

#define CSPTESTLOG_ static_cast<CSP(LogStream&)>(CSP(testlog()))
#define CSPTESTLOG(P) \
	if CSP(testlog()).isNoteworthy(CSPLOG_PRIORITY(P), CSPLOG_CATEGORY(ALL)) \
	else CSP(LogStream::LogEntry)(TESTLOG_, CSPLOG_PRIORITY(P), __FILE__, __LINE__)
*/


/** Base class for all test fixtures.
 */
class CSPLIB_EXPORT TestFixture: public NonCopyable {
public:
	virtual ~TestFixture() {}
	virtual void setup() {}
	virtual void teardown() {}
	virtual void setupFixture() {}
	virtual void teardownFixture() {}
};


/** Base class for a test fixture that provides a typedef needed by the
 *  CSP_TESTCASE macro to identify the enclosing fixture class.
 */
template <class tCLASS>
class CSPLIB_EXPORT TypedTestFixture : public TestFixture {
protected:
	typedef tCLASS TestFixtureClass;
};


template <class tFIXTURE> class TestCase;

/** A helper class for automatically registering a test case method.  Instances
 *  of this class are declared as member variables within a test fixture class
 *  by the CSP_TESTCASE macro.
 */
template <class tFIXTURE>
class CSPLIB_EXPORT TestCase {
	std::string m_name;
	void (tFIXTURE::*m_test)();

public:
	/** Default constructor needed adding TestCase instances to stl containers.
	 */
	TestCase(): m_test(0) {}

	/** Bind to the test fixture method that implements the test.
	 */
	inline TestCase(std::string const &name, void (tFIXTURE::*test)());

	std::string const &name() const { return m_name; }

	void run(tFIXTURE *fixture) const {
		CSP_VERIFY(m_test);
		(fixture->*m_test)();
	}
};


/** An interface for classes that report test results.  A very simple, text-based
 *  reporting class is implemented in Testing.cpp and used by default.  More
 *  sophisticated reporters (e.g., GUI-based) could be implemented by implementing
 *  this interface if desired.
 */
class CSPLIB_EXPORT TestReporter: public NonCopyable {
public:
	virtual ~TestReporter() {}
	virtual void beginFixture(std::string const &name) = 0;
	virtual void beginTestCase(std::string const &name) = 0;
	virtual void endTestCase(bool success) = 0;
	virtual void endFixture() = 0;
};


/** A helper class for executing test cases within a single test fixture.
 */
class CSPLIB_EXPORT TestRunner: public NonCopyable {
public:
	bool runTests(TestReporter &reporter);
	std::string const &file() const { return m_file; }
	std::string const &name() const { return m_fixture_name; }
protected:
	TestRunner(const char* fixture_name, const char *file);
	virtual ~TestRunner() {}
	virtual bool _runTests(TestReporter &reporter) = 0;
private:
	std::string m_fixture_name;
	std::string m_file;
};


/** A global (static) registry of test fixtures.  Provides a C++ and Python
 *  interface for running test fixtures.
 */
class CSPLIB_EXPORT TestRegistry: public NonConstructable {
public:
	static CSPLIB_EXPORT bool loadTestModule(const char *module);
	static CSPLIB_EXPORT bool runAll();
	static CSPLIB_EXPORT bool runOnePath(const char *path);
	static CSPLIB_EXPORT bool runOneTest(const char *test);
	static CSPLIB_EXPORT void addTestRunner(TestRunner *runner);

private:
	static CSPLIB_EXPORT bool _runTests(std::vector<TestRunner*> const &tests);
	struct TestData;
	static TestData &data();
};


/** A specialized TestRunner for executing test cases within a specific test
 *  fixture class.
 */
template <class tFIXTURE>
class CSPLIB_EXPORT TypedTestRunner: public TestRunner {
	typedef std::map<std::string, TestCase<tFIXTURE> > TestCaseMap;
	static TestCaseMap *m_testcases;
public:
	TypedTestRunner(const char *fixture_name, const char *file): TestRunner(fixture_name, file) { }
	inline static void addTestCase(TestCase<tFIXTURE> &testcase);
protected:
	inline virtual bool _runTests(TestReporter &reporter);
};

template <class tFIXTURE>
typename TypedTestRunner<tFIXTURE>::TestCaseMap* TypedTestRunner<tFIXTURE>::m_testcases = 0;

template <class tFIXTURE>
void TypedTestRunner<tFIXTURE>::addTestCase(TestCase<tFIXTURE> &testcase) {
	if (!m_testcases) m_testcases = new TestCaseMap;
	(*m_testcases)[testcase.name()] = testcase;
}

template <class tFIXTURE>
bool TypedTestRunner<tFIXTURE>::_runTests(TestReporter &reporter) {
	tFIXTURE fixture;
	fixture.setupFixture();
	bool success = true;
	for (typename TestCaseMap::const_iterator iter = m_testcases->begin(); iter != m_testcases->end(); ++iter) {
		TestCase<tFIXTURE> const &testcase = iter->second;
		reporter.beginTestCase(testcase.name());
		fixture.setup();
		bool ok = false;
		try {
			testcase.run(&fixture);
			ok = true;
		} catch (FatalException const &e) {
			e.clear();  // already logged
		} catch (PassTest const &e) {
			ok = true;
		} catch (FailTest const &e) {
			ok = false;
		}
		fixture.teardown();
		reporter.endTestCase(ok);
		success = ok && success;
	}
	fixture.teardownFixture();
	return success;
}

template <class tFIXTURE>
TestCase<tFIXTURE>::TestCase(std::string const &name, void (tFIXTURE::*test)()): m_name(name), m_test(test) {
	TypedTestRunner<tFIXTURE>::addTestCase(*this);
}


CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_TESTING_H__




