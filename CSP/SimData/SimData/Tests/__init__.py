import unittest

suites = []

def addTestSuites(module):
	mod = __import__(module)
	suites.extend(mod.suites)

addTestSuites("ArchiveTests")
addTestSuites("TypeTests")

runner = unittest.TextTestRunner()
runner.run(unittest.TestSuite(suites))

