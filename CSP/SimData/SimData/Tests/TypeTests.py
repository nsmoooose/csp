import SimData
import os
from unittest import TestCase, TestSuite, makeSuite

def round8(x):
	return long((x*1e+8)+0.5)*1e-8;
def round5(x):
	return long((x*1e+5)+0.5)*1e-5;

class KeyTest(TestCase):
	"""Test functionality Key"""
	def testComparisons(self):
		"""Test Key comparisons"""
		key_foo = SimData.Key("Foo")
		key_bar = SimData.Key("Bar")
		self.assertEqual(key_foo, SimData.Key("Foo"))
		self.assertEqual(key_foo, "Foo")
		self.assertNotEqual(key_foo, key_bar)
		self.assertNotEqual(key_foo, "Bar")
	def testRepr(self):
		"""Test string and type representations"""
		key_foo = SimData.Key("Foo")
		self.assertEqual(str(key_foo)[:6], "Key<0x")
		self.assertEqual(key_foo.typeString(), "type::Key")

KeySuite = makeSuite(KeyTest, 'test')


class DateTest(TestCase):
	"""Test functionality SimDate and related classes"""
	def testRepr(self):
		"""Test string and type representations"""
		d = SimData.SimDate(2003, 10, 31, 22, 51, 7)
		self.assertEqual(str(d), "2003/10/31 22:51:07z")
		d = SimData.SimDate(1903, 1, 9, 5, 6, 7)
		self.assertEqual(str(d), "1903/01/09 05:06:07z")
		self.assertEqual(d.typeString(), "type::SimDate")
	def testErrors(self):
		def catch(code, err=1):
			caught = 0
			try:
				eval(code)
			except RuntimeError:
				caught = 1
			if caught != err:
				if err:
					msg = "invalid"
				else:
					msg = "valid"
				self.assertEqual(code, msg)
		catch("SimData.SimDate(2003, 0, 31, 22, 51, 7)")
		catch("SimData.SimDate(2003, 13, 31, 22, 51, 7)")
		catch("SimData.SimDate(2003, 2, 29, 22, 51, 7)")
		catch("SimData.SimDate(2100, 2, 29, 22, 51, 7)")
		catch("SimData.SimDate(2004, 2, 29, 22, 51, 7)",0)
		catch("SimData.SimDate(2000, 2, 29, 22, 51, 7)",0)
		catch("SimData.SimDate(2003, 2, 0, 22, 51, 7)")
		catch("SimData.SimDate(2003, 10, 32, 22, 51, 7)")
	def testRollover(self):
		sd = SimData.SimDate
		test = self.assertEqual
		test(sd(2003, 3, 1, -1, -1, -1), sd(2003, 2, 28, 22, 58, 59))
		test(sd(1999, 12, 31, 24, 0, 1), sd(2000, 1, 1, 0, 0, 1))
		test(sd(1999, 12, 31, 24, 0, 61), sd(2000, 1, 1, 0, 1, 1))
		
DateSuite = makeSuite(DateTest, 'test')


suites = [KeySuite, DateSuite]

