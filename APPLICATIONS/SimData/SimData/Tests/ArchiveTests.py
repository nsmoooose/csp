import SimData
import os
from unittest import TestCase, TestSuite, makeSuite

def round8(x):
	return long((x*1e+8)+0.5)*1e-8;
def round5(x):
	return long((x*1e+5)+0.5)*1e-5;

class ArchiveTest(TestCase):
	def setUp(self):
		self.f = SimData.PackFile('__test__.dar', 'wb') #open('__test__.dar', 'wb')
		self.archive = SimData.Packer(self.f)
	def setRead(self):
		self.size = self.archive.getCount()
		self.archive = None
		self.f.close()
		self.f = open('__test__.dar', 'rb')
		self.data = self.f.read(self.size)
		self.f.close()
		os.unlink('__test__.dar')
		self.archive = SimData.UnPacker(self.data)
	def tearDown(self):
		self.failUnless(self.archive.isComplete())

class TypeArchiveTest(ArchiveTest):
	def testSimDate(self):
		"""Test storage and retrieval of SimDate"""
		date0 = SimData.SimDate(2003, 2, 28, 01, 14, 59.6)
		date0str = str(date0)
		self.assertEqual(date0str, '2003/02/28 01:14:59z')
		self.archive._basetype(date0)
		self.assertEqual(str(date0), date0str)
		self.setRead()
		self.assertEqual(self.size, 8)
		date1 = self.archive._SimDate()
		# test times to 1 part in 10^8  (approx 1 ms)
		self.assertEqual(round8(date0.getJulianDate()), round8(date1.getJulianDate()))

	def testVector3(self):
		"""Test storage and retrieval of Vector3"""
		x0 = SimData.Vector3(1.0, -2.0, 3.3)
		y0 = SimData.Vector3(1.0, -2.0, 3.3)
		self.assertEqual(x0, y0)
		self.assertEqual(x0.x,  1.0)
		self.assertEqual(x0.y, -2.0)
		self.assertEqual(x0.z,  3.3)
		self.archive._basetype(x0)
		self.assertEqual(x0, y0)
		self.setRead()
		self.assertEqual(self.size, 24)
		x1 = self.archive._Vector3()
		self.assertEqual(x0, x1)

	def testMatrix3(self):
		"""Test storage and retrieval of Matrix3"""
		x0 = SimData.Matrix3(1.0, -2.0, 3.3, -1.0, -3.0, -5.0, 9.1, 8.4, 2.3)
		y0 = SimData.Matrix3(1.0, -2.0, 3.3, -1.0, -3.0, -5.0, 9.1, 8.4, 2.3)
		self.assertEqual(x0, y0)
		self.assertEqual(x0.getElement(0,0),  1.0)
		self.assertEqual(x0.getElement(1,0), -1.0)
		self.assertEqual(x0.getElement(2,2),  2.3)
		self.archive._basetype(x0)
		self.assertEqual(x0, y0)
		self.setRead()
		self.assertEqual(self.size, 72)
		x1 = self.archive._Matrix3()
		self.assertEqual(x0, x1)

	def testKey(self):
		"""Test storage and retrieval of Key"""
		x0 = SimData.Key("KeyTest")
		y0 = SimData.Key("KeyTest")
		self.archive._basetype(x0)
		self.assertEqual(x0, y0)
		self.setRead()
		self.assertEqual(self.size, 4)
		x1 = self.archive._Key()
		self.assertEqual(x0, x1)

	def testReal(self):
		"""Test storage and retrieval of Real"""
		x0 = SimData.Real(10.0, 1.0)
		y0 = SimData.Real(10.0, 1.0)
		self.archive._basetype(x0)
		self.assertEqual(x0.getMean(), y0.getMean())
		self.assertEqual(x0.getSigma(), y0.getSigma())
		self.setRead()
		self.assertEqual(self.size, 8)
		x1 = self.archive._Real()
		self.assertEqual(x0.getMean(), x1.getMean())
		self.assertEqual(x0.getSigma(), x1.getSigma())
		v = x1.getValue()
		# 5 sigma test ;-)
		self.failIf(v == 10.0 or v < 5.0 or v > 15.0)

	def testExternal(self):
		"""Test storage and retrieval of External"""
		path = "foo/bar/baz.txt"
		x0 = SimData.External()
		x0.setSource(path)
		y0 = SimData.External()
		y0.setSource(path)
		self.archive._basetype(x0)
		self.assertEqual(x0, y0)
		self.setRead()
		self.assertEqual(self.size, len(path)+4)
		x1 = self.archive._External()
		self.assertEqual(x0, x1)

	def testQuat(self):
		"""Test storage and retrieval of Quat"""
		x0 = SimData.Quat(1.0, -2.0, 3.3, -4.0)
		y0 = SimData.Quat(1.0, -2.0, 3.3, -4.0)
		self.assertEqual(x0, y0)
		self.assertEqual(x0.x,  1.0)
		self.assertEqual(x0.y, -2.0)
		self.assertEqual(x0.z,  3.3)
		self.assertEqual(x0.w, -4.0)
		self.archive._basetype(x0)
		self.assertEqual(x0, y0)
		self.setRead()
		self.assertEqual(self.size, 32)
		x1 = self.archive._Quat()
		self.assertEqual(x0, x1)

	def testPath(self):
		"""Test storage and retrieval of Path"""
		path="x:a.b.c"
		x0 = SimData.Path(path)
		y0 = SimData.Path(path)
		self.assertEqual(x0, y0)
		self.archive._basetype(x0)
		self.assertEqual(x0, y0)
		self.setRead()
		self.assertEqual(self.size, 8)
		x1 = self.archive._Path()
		self.assertEqual(x0, x1)

	def testLLA(self):
		"""Test storage and retrieval of LLA"""
		x0 = SimData.LLA(0.2, -1.1, 100.1)
		y0 = SimData.LLA(0.2, -1.1, 100.1)
		self.archive._basetype(x0)
		self.assertEqual(x0.latitude(), y0.latitude())
		self.assertEqual(x0.longitude(), y0.longitude())
		self.assertEqual(x0.altitude(), y0.altitude())
		self.setRead()
		self.assertEqual(self.size, 24)
		x1 = self.archive._LLA()
		self.assertEqual(x0.latitude(), x1.latitude())
		self.assertEqual(x0.longitude(), x1.longitude())
		self.assertEqual(x0.altitude(), x1.altitude())

	def testUTM(self):
		"""Test storage and retrieval of UTM"""
		lla = SimData.LLA(0.2, -1.1, 100.1)
		x0 = SimData.UTM(lla)
		y0 = SimData.UTM(lla)
		self.archive._basetype(x0)
		self.assertEqual(x0.northing(), y0.northing())
		self.assertEqual(x0.easting(), y0.easting())
		self.assertEqual(x0.zone(), y0.zone())
		self.assertEqual(x0.designator(), y0.designator())
		self.assertEqual(x0.altitude(), y0.altitude())
		self.setRead()
		self.assertEqual(self.size, 26)
		x1 = self.archive._UTM()
		self.assertEqual(x0.northing(), x1.northing())
		self.assertEqual(x0.easting(), x1.easting())
		self.assertEqual(x0.zone(), x1.zone())
		self.assertEqual(x0.designator(), x1.designator())
		self.assertEqual(x0.altitude(), x1.altitude())

	def testECEF(self):
		"""Test storage and retrieval of ECEF"""
		lla = SimData.LLA(0.2, -1.1, 100.1)
		x0 = SimData.ECEF(lla)
		y0 = SimData.ECEF(lla)
		self.assertEqual(x0.x, y0.x)
		self.assertEqual(x0.y, y0.y)
		self.assertEqual(x0.z, y0.z)
		self.archive._basetype(x0)
		self.setRead()
		self.assertEqual(self.size, 24)
		x1 = self.archive._ECEF()
		self.assertEqual(x0.x, x1.x)
		self.assertEqual(x0.y, x1.y)
		self.assertEqual(x0.z, x1.z)

	def testString(self):
		"""Test storage and retrieval of String"""
		x0 = "hello world"
		y0 = "hello world"
		self.archive._string(x0)
		self.setRead()
		self.assertEqual(self.size, 15)
		x1 = self.archive._string()
		self.assertEqual(x0, x1)

	def testInt(self):
		"""Test storage and retrieval of Int"""
		x0 = 42
		self.archive._int(x0)
		self.setRead()
		self.assertEqual(self.size, 4)
		x1 = self.archive._int()
		self.assertEqual(x0, x1)

	def testDouble(self):
		"""Test storage and retrieval of Double"""
		x0 = 42.6
		self.archive._double(x0)
		self.setRead()
		self.assertEqual(self.size, 8)
		x1 = self.archive._double()
		self.assertEqual(x0, x1)

	def testFloat(self):
		"""Test storage and retrieval of Float"""
		x0 = 42.6
		self.archive._float(x0)
		self.setRead()
		self.assertEqual(self.size, 4)
		x1 = self.archive._float()
		self.assertEqual(round5(x0), round5(x1))

	def testBool(self):
		"""Test storage and retrieval of Bool"""
		x0 = 1
		self.archive._bool(x0)
		self.setRead()
		self.assertEqual(self.size, 1)
		x1 = self.archive._bool()
		self.assertEqual(x0, x1)

TypeArchiveSuite = makeSuite(TypeArchiveTest, 'test')

suites = [TypeArchiveSuite]

