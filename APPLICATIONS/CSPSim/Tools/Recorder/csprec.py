#!/usr/bin/env python

# Combat Simulator Project - CSPSim
# Copyright (C) 2003 The Combat Simulator Project
# http://csp.sourceforge.net
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


# this is an exceedingly trivial program for converting data recorder
# output from CSPSim to tab delimited text that can be imported into a
# spreadsheet. there is minimal error checking and lots of inelegant
# hacking, which can be cleaned up by anyone that has energy for it ;-)


import sys, time
from struct import pack, unpack, calcsize
from math import *

def readint(f):
	return unpack("I", f.read(4))[0]

def readfloat(f):
	return unpack("f", f.read(4))[0]

def readbyte(f):
	return unpack("B", f.read(1))[0]

def readdata(f):
	return unpack("If", f.read(8))


class RecorderConverter:

	def __init__(self):
		self.header = ""

	def convert(self, f):
		f.seek(-4, 2)
		f.seek(readint(f), 0)
		n = readint(f)
		self.channels = channels = []
		self.output = output = []
		for i in range(n):
			name = f.read(64)
			name = name[0:name.find("\0")]
			channels.append(name)
		n = len(channels)
		self.set = set = range(n)
		self.min_time = 1e+10
		self.max_time = -1e+10
		self.min_data = range(n)
		self.max_data = range(n)
		for i in range(n):
			set[i] = 0.0
			self.min_data[i] = 1e+10
			self.max_data[i] = -1e+10
		self.time = -1.0
		f.seek(0, 0)
		while 1:
			type, value = readdata(f)
			if type == 255: 
				output.append((self.time, set[:]))
				break
			if type == 250:
				output.append((self.time, set[:]))
				self.time = value
				self.min_time = min(self.min_time, self.time)
				self.max_time = max(self.max_time, self.time)
			if type < n:
				set[type] = value
				self.min_data[type] = min(self.min_data[type], value)
				self.max_data[type] = max(self.max_data[type], value)

	def dumpTab(self):
		print "time\t%s" % '\t'.join(self.channels)
		for timestamp, set in self.output:
			if timestamp > 0:
				set = map(lambda x: "%e" % x, set)
				print "%0.3f\t%s" % (timestamp, '\t'.join(set))


	def dumpJGraph(self):
		y_start = 8.5
		y_sep = 2.0
		y_limit = 0.5
		y = y_start

		x_min = self.min_time
		x_max = self.max_time
		dt = x_max - x_min
		if dt <= 0.0: return
		mag = log10(dt)
		scale = pow(10.0,floor(mag)-1)
		if mag - floor(mag) > 0.65:
			scale = scale * 5.0
		else:
			scale = scale * 2.0
		x_min = int(x_min/scale)*scale
		x_max = int(x_max/scale+0.9999)*scale

		if self.header != "":
			print "preamble : %s" % self.header

		print "X 8.5"
		print "Y 11"
		for i in range(len(self.channels)):
			channel = self.channels[i]
			y_min = self.min_data[i]
			y_max = self.max_data[i]
			dy = y_max - y_min
			scale = -1.0
			if dy > 0.0:
				mag = log10(dy)
				scale = pow(10.0, floor(mag))
				if mag - floor(mag) > 0.65:
					scale = scale * 5.0
				else:
					scale = scale * 2.0
				y_min = floor((y_min-dy*0.1)/scale)*scale
				y_max = floor((y_max+dy*0.1)/scale+0.9999)*scale
				dy = y_max - y_min
				hash = scale
				precision = max(0, 1-mag)
			else:
				y_min = int(y_min) - 1.0
				y_max = int(y_max) + 1.0
				dy = y_max - y_min
				scale = 1.0
				precision = 1
			print "newgraph border title y %f font Helvetica : %s" % (y_max+dy*0.15, channel)
			print "xaxis mgrid_lines grid_lines grid_gray 0.5 min %f max %f size 7.0" % (x_min, x_max)
			if (i == len(self.channels)-1) or y < y_limit + y_sep:
				print "xaxis label font Helvetica : Time (sec)"
				print "xaxis hash_labels font Helvetica"
			else:
				print "nodraw"
			hash = "%f" % scale
			while hash.endswith('0'): hash = hash[:-1]
			if hash.endswith('.'): hash = hash[:-1]
			print "yaxis grid_lines mgrid_lines grid_gray 0.5 min %f max %f size 1.2" % (y_min, y_max)
			print "yaxis hash_labels font Helvetica"
			mhash = int(5 * scale / dy)
			if mhash == 2: mhash = 1
			if mhash >= 3 and mhash < 7: mhash = 4

			print "precision %d hash %s mhash %d" % (precision, hash, mhash)
			print "y_translate", y
			print "newcurve marktype none linetype solid linethickness 2 pts"
			for timestamp, set in self.output:
				if timestamp > 0: print timestamp, set[i]	
			y = y - y_sep
			if y < y_limit:
				y = y_start
				print "newpage"

	def setHeader(self, header):
		self.header = header


def usage(msg=""):
	if msg != "":
		print >>sys.stderr, "Error: %s\n" % msg
	print >>sys.stderr, "Data recorder to tab-delimited text converter for use with CSP."
	print >>sys.stderr, "Usage: %s [--tab|--jgraph] infile" % sys.argv[0]
	sys.exit(1)


def parseHeader(filename, subst):
	if filename == "": return ""
	try:
		fh = open(filename, "rt")
		header = ' \\\n'.join(map(lambda x: x.strip(), fh.readlines()))
	except:
		print >>sys.stderr, "Unable to read header " % filename
		return ""
	for key in subst.keys():
		header = header.replace("$"+key, subst[key])
	return header

filename = ""
mode = 'tab'

header_subst = {}
header = ""

for arg in sys.argv[1:]:
	if arg.startswith('-'):
		if arg in ('--jgraph', '--tab'):
			mode = arg[2:]
		elif arg.startswith("--header="):
			header = arg[9:]
		elif arg.startswith("--set="):
			key, value = arg[6:].split(':')
			header_subst[key] = value
		else:
			usage("unrecognized option '%s'" % arg)
	else:
		filename = arg

if filename == "": usage()

file = open(filename, "rb")
			
header_subst["date"] = time.ctime()
header_subst["source"] = filename
header_subst["converter"] = "Generated by RecCSP, version 0.1"
header = parseHeader(header, header_subst)

r = RecorderConverter()
r.setHeader(header)
r.convert(file)
if mode == 'tab':
	r.dumpTab()
elif mode == 'jgraph':
	r.dumpJGraph()


