import SimData
import os
from unittest import TestCase, TestSuite, makeSuite

def round8(x):
	return long((x*1e+8)+0.5)*1e-8;
def round5(x):
	return long((x*1e+5)+0.5)*1e-5;

class KeyTest(TestCase):
	"""Test functionality Key"""
	def setUp(self):
		pass
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

suites = [KeySuite]

