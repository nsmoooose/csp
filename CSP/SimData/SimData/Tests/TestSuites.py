import sys
import os.path

root = os.path.join(os.path.dirname(__file__), '..', '..')
sys.path.insert(0, os.path.abspath(root))

import unittest

suites = []

def AddSuite(suite):
  global suites
  suites.append(suite)

def Run():
  runner = unittest.TextTestRunner(descriptions=1, verbosity=2)
  runner.run(unittest.TestSuite(suites))
