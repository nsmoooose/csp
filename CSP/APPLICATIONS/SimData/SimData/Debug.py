# SimDataCSP: Data Infrastructure for Simulations
# Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
# 
# This file is part of SimDataCSP.
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


_WarningLevel = 0
_DebugLevel = 0
_MaxWarningLevel = 0
_WarningCount = 0

def DEBUG(level, msg):
	if level <= _DebugLevel:
		print "DEBUG:", msg.rstrip()

def WARN(level, msg):
	global _MaxWarningLevel 
	global _WarningCount
	if level <= _WarningLevel:
		print "WARNING:", msg.rstrip()
	_MaxWarningLevel = max(_MaxWarningLevel, level)
	_WarningCount = _WarningCount + 1

def setWarningLevel(n):
	global _WarningLevel
	_WarningLevel = n
	
def setDebugLevel(n):
	global _DebugLevel
	_DebugLevel = n
	
def getDebugLevel():
	return _DebugLevel
	
def getWarningLevel():
	return _WarningLevel

def getWarnings():
	return _WarningCount, _MaxWarningLevel
	
def resetWarnings():
	global _MaxWarningLevel 
	global _WarningCount
	_MaxWarningLevel = 0
	_WarningCount = 0

