# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.
import _cSimData
def _swig_setattr(self,class_type,name,value):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    self.__dict__[name] = value

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


class Exception(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Exception, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Exception, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Exception,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_Exception):
        try:
            if self.thisown: destroy(self)
        except: pass
    def getMessage(*args): return apply(_cSimData.Exception_getMessage,args)
    def getType(*args): return apply(_cSimData.Exception_getType,args)
    def appendMessage(*args): return apply(_cSimData.Exception_appendMessage,args)
    def clear(*args): return apply(_cSimData.Exception_clear,args)
    def details(*args): return apply(_cSimData.Exception_details,args)
    def __repr__(self):
        return "<C Exception instance at %s>" % (self.this,)

class ExceptionPtr(Exception):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Exception
_cSimData.Exception_swigregister(ExceptionPtr)

class PythonException(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PythonException, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PythonException, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_PythonException,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_PythonException):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C PythonException instance at %s>" % (self.this,)

class PythonExceptionPtr(PythonException):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PythonException
_cSimData.PythonException_swigregister(PythonExceptionPtr)

class HashT(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, HashT, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, HashT, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_HashT,args)
        self.thisown = 1
    def __eq__(*args): return apply(_cSimData.HashT___eq__,args)
    def __ne__(*args): return apply(_cSimData.HashT___ne__,args)
    def __del__(self, destroy= _cSimData.delete_HashT):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C HashT instance at %s>" % (self.this,)

class HashTPtr(HashT):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HashT
_cSimData.HashT_swigregister(HashTPtr)

hash_string = _cSimData.hash_string

class ParseException(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ParseException, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ParseException, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_ParseException,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_ParseException):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ParseException instance at %s>" % (self.this,)

class ParseExceptionPtr(ParseException):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ParseException
_cSimData.ParseException_swigregister(ParseExceptionPtr)

class BaseType(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BaseType, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BaseType, name)
    def pack(*args): return apply(_cSimData.BaseType_pack,args)
    def unpack(*args): return apply(_cSimData.BaseType_unpack,args)
    def parseXML(*args): return apply(_cSimData.BaseType_parseXML,args)
    def convertXML(*args): return apply(_cSimData.BaseType_convertXML,args)
    def asString(*args): return apply(_cSimData.BaseType_asString,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_BaseType,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_BaseType):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C BaseType instance at %s>" % (self.this,)

class BaseTypePtr(BaseType):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BaseType
_cSimData.BaseType_swigregister(BaseTypePtr)

__lshift__ = _cSimData.__lshift__

class Object(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Object, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Object, name)
    def __del__(self, destroy= _cSimData.delete_Object):
        try:
            if self.thisown: destroy(self)
        except: pass
    def _new(*args): return apply(_cSimData.Object__new,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Object,args)
        self.thisown = 1
    __swig_getmethods__["_getClassName"] = lambda x: _cSimData.Object__getClassName
    if _newclass:_getClassName = staticmethod(_cSimData.Object__getClassName)
    def getClassName(*args): return apply(_cSimData.Object_getClassName,args)
    __swig_getmethods__["_getClassHash"] = lambda x: _cSimData.Object__getClassHash
    if _newclass:_getClassHash = staticmethod(_cSimData.Object__getClassHash)
    def getClassHash(*args): return apply(_cSimData.Object_getClassHash,args)
    __swig_getmethods__["_getClassVersion"] = lambda x: _cSimData.Object__getClassVersion
    if _newclass:_getClassVersion = staticmethod(_cSimData.Object__getClassVersion)
    def getClassVersion(*args): return apply(_cSimData.Object_getClassVersion,args)
    def pack(*args): return apply(_cSimData.Object_pack,args)
    def unpack(*args): return apply(_cSimData.Object_unpack,args)
    def setStatic(*args): return apply(_cSimData.Object_setStatic,args)
    def isStatic(*args): return apply(_cSimData.Object_isStatic,args)
    def ref(*args): return apply(_cSimData.Object_ref,args)
    def deref(*args): return apply(_cSimData.Object_deref,args)
    def getCount(*args): return apply(_cSimData.Object_getCount,args)
    def __repr__(*args): return apply(_cSimData.Object___repr__,args)
    def getPath(*args): return apply(_cSimData.Object_getPath,args)

class ObjectPtr(Object):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Object
_cSimData.Object_swigregister(ObjectPtr)
Object__getClassName = _cSimData.Object__getClassName

Object__getClassHash = _cSimData.Object__getClassHash

Object__getClassVersion = _cSimData.Object__getClassVersion


class Packer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Packer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Packer, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Packer,args)
        self.thisown = 1
    def resetCount(*args): return apply(_cSimData.Packer_resetCount,args)
    def getCount(*args): return apply(_cSimData.Packer_getCount,args)
    def pack(*args): return apply(_cSimData.Packer_pack,args)
    def pack_double(*args): return apply(_cSimData.Packer_pack_double,args)
    def pack_float(*args): return apply(_cSimData.Packer_pack_float,args)
    def pack_int(*args): return apply(_cSimData.Packer_pack_int,args)
    def pack_hasht(*args): return apply(_cSimData.Packer_pack_hasht,args)
    def pack_string(*args): return apply(_cSimData.Packer_pack_string,args)
    def pack_basetype(*args): return apply(_cSimData.Packer_pack_basetype,args)
    def __del__(self, destroy= _cSimData.delete_Packer):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C Packer instance at %s>" % (self.this,)

class PackerPtr(Packer):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Packer
_cSimData.Packer_swigregister(PackerPtr)

class UnPacker(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, UnPacker, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, UnPacker, name)
    def _getArchive(*args): return apply(_cSimData.UnPacker__getArchive,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_UnPacker,args)
        self.thisown = 1
    def isComplete(*args): return apply(_cSimData.UnPacker_isComplete,args)
    def unpack(*args): return apply(_cSimData.UnPacker_unpack,args)
    def unpack_packable(*args): return apply(_cSimData.UnPacker_unpack_packable,args)
    def unpack_double(*args): return apply(_cSimData.UnPacker_unpack_double,args)
    def unpack_float(*args): return apply(_cSimData.UnPacker_unpack_float,args)
    def unpack_int(*args): return apply(_cSimData.UnPacker_unpack_int,args)
    def unpack_hasht(*args): return apply(_cSimData.UnPacker_unpack_hasht,args)
    def unpack_string(*args): return apply(_cSimData.UnPacker_unpack_string,args)
    def unpack_basetype(*args): return apply(_cSimData.UnPacker_unpack_basetype,args)
    def __del__(self, destroy= _cSimData.delete_UnPacker):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C UnPacker instance at %s>" % (self.this,)

class UnPackerPtr(UnPacker):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = UnPacker
_cSimData.UnPacker_swigregister(UnPackerPtr)

class ObjectTypeMismatch(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ObjectTypeMismatch, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ObjectTypeMismatch, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_ObjectTypeMismatch,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_ObjectTypeMismatch):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ObjectTypeMismatch instance at %s>" % (self.this,)

class ObjectTypeMismatchPtr(ObjectTypeMismatch):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ObjectTypeMismatch
_cSimData.ObjectTypeMismatch_swigregister(ObjectTypeMismatchPtr)

class Path(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Path, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Path, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Path,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_Path):
        try:
            if self.thisown: destroy(self)
        except: pass
    def setPath(*args): return apply(_cSimData.Path_setPath,args)
    def setNone(*args): return apply(_cSimData.Path_setNone,args)
    def getPath(*args): return apply(_cSimData.Path_getPath,args)
    def pack(*args): return apply(_cSimData.Path_pack,args)
    def unpack(*args): return apply(_cSimData.Path_unpack,args)
    def isNone(*args): return apply(_cSimData.Path_isNone,args)
    def __eq__(*args): return apply(_cSimData.Path___eq__,args)
    def __repr__(*args): return apply(_cSimData.Path___repr__,args)

class PathPtr(Path):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Path
_cSimData.Path_swigregister(PathPtr)

class PointerBase(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PointerBase, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PointerBase, name)
    def __ne__(*args): return apply(_cSimData.PointerBase___ne__,args)
    def __del__(self, destroy= _cSimData.delete_PointerBase):
        try:
            if self.thisown: destroy(self)
        except: pass
    def unique(*args): return apply(_cSimData.PointerBase_unique,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_PointerBase,args)
        self.thisown = 1
    def isNull(*args): return apply(_cSimData.PointerBase_isNull,args)
    def __not__(*args): return apply(_cSimData.PointerBase___not__,args)
    def valid(*args): return apply(_cSimData.PointerBase_valid,args)
    def __eq__(*args): return apply(_cSimData.PointerBase___eq__,args)
    def __get__(*args): return apply(_cSimData.PointerBase___get__,args)
    def __repr__(self):
        return "<C PointerBase instance at %s>" % (self.this,)

class PointerBasePtr(PointerBase):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PointerBase
_cSimData.PointerBase_swigregister(PointerBasePtr)

class PathPointerBase(Path,PointerBase):
    __swig_setmethods__ = {}
    for _s in [Path,PointerBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, PathPointerBase, name, value)
    __swig_getmethods__ = {}
    for _s in [Path,PointerBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, PathPointerBase, name)
    def __ne__(*args): return apply(_cSimData.PathPointerBase___ne__,args)
    def __del__(self, destroy= _cSimData.delete_PathPointerBase):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __init__(self,*args):
        self.this = apply(_cSimData.new_PathPointerBase,args)
        self.thisown = 1
    def unpack(*args): return apply(_cSimData.PathPointerBase_unpack,args)
    def __repr__(*args): return apply(_cSimData.PathPointerBase___repr__,args)
    def __eq__(*args): return apply(_cSimData.PathPointerBase___eq__,args)
    def setNone(*args): return apply(_cSimData.PathPointerBase_setNone,args)
    def setNull(*args): return apply(_cSimData.PathPointerBase_setNull,args)

class PathPointerBasePtr(PathPointerBase):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PathPointerBase
_cSimData.PathPointerBase_swigregister(PathPointerBasePtr)

tstart = _cSimData.tstart

tend = _cSimData.tend

tval = _cSimData.tval

get_realtime = _cSimData.get_realtime

F1p0_60p0 = _cSimData.F1p0_60p0
F1p0_24p0 = _cSimData.F1p0_24p0
F1p0_36525p0 = _cSimData.F1p0_36525p0
F1p0_86000p0 = _cSimData.F1p0_86000p0
class InvalidDate(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, InvalidDate, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, InvalidDate, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_InvalidDate,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_InvalidDate):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C InvalidDate instance at %s>" % (self.this,)

class InvalidDatePtr(InvalidDate):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InvalidDate
_cSimData.InvalidDate_swigregister(InvalidDatePtr)

class Date(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Date, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Date, name)
    MONDAY = _cSimData.Date_MONDAY
    TUESDAY = _cSimData.Date_TUESDAY
    WEDNESDAY = _cSimData.Date_WEDNESDAY
    THURSDAY = _cSimData.Date_THURSDAY
    FRIDAY = _cSimData.Date_FRIDAY
    SATURDAY = _cSimData.Date_SATURDAY
    SUNDAY = _cSimData.Date_SUNDAY
    JANUARY = _cSimData.Date_JANUARY
    FEBRUARY = _cSimData.Date_FEBRUARY
    MARCH = _cSimData.Date_MARCH
    APRIL = _cSimData.Date_APRIL
    MAY = _cSimData.Date_MAY
    JUNE = _cSimData.Date_JUNE
    JULY = _cSimData.Date_JULY
    AUGUST = _cSimData.Date_AUGUST
    SEPTEMBER = _cSimData.Date_SEPTEMBER
    OCTOBER = _cSimData.Date_OCTOBER
    NOVEMBER = _cSimData.Date_NOVEMBER
    DECEMBER = _cSimData.Date_DECEMBER
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Date,args)
        self.thisown = 1
    __swig_getmethods__["validYMD"] = lambda x: _cSimData.Date_validYMD
    if _newclass:validYMD = staticmethod(_cSimData.Date_validYMD)
    __swig_getmethods__["isLeap"] = lambda x: _cSimData.Date_isLeap
    if _newclass:isLeap = staticmethod(_cSimData.Date_isLeap)
    def getWeekday(*args): return apply(_cSimData.Date_getWeekday,args)
    def getMonth(*args): return apply(_cSimData.Date_getMonth,args)
    def getDay(*args): return apply(_cSimData.Date_getDay,args)
    def getYear(*args): return apply(_cSimData.Date_getYear,args)
    def getJulian(*args): return apply(_cSimData.Date_getJulian,args)
    def getMonthName(*args): return apply(_cSimData.Date_getMonthName,args)
    def getMonthShortName(*args): return apply(_cSimData.Date_getMonthShortName,args)
    def getWeekdayName(*args): return apply(_cSimData.Date_getWeekdayName,args)
    def getWeekdayShortName(*args): return apply(_cSimData.Date_getWeekdayShortName,args)
    def setJulian(*args): return apply(_cSimData.Date_setJulian,args)
    def getDayOfYear(*args): return apply(_cSimData.Date_getDayOfYear,args)
    def getMondayWeekOfYear(*args): return apply(_cSimData.Date_getMondayWeekOfYear,args)
    def getSundayWeekOfYear(*args): return apply(_cSimData.Date_getSundayWeekOfYear,args)
    def isFirstOfMonth(*args): return apply(_cSimData.Date_isFirstOfMonth,args)
    def isLastOfMonth(*args): return apply(_cSimData.Date_isLastOfMonth,args)
    def addDays(*args): return apply(_cSimData.Date_addDays,args)
    def subtractDays(*args): return apply(_cSimData.Date_subtractDays,args)
    def addMonths(*args): return apply(_cSimData.Date_addMonths,args)
    def subtractMonths(*args): return apply(_cSimData.Date_subtractMonths,args)
    def addYears(*args): return apply(_cSimData.Date_addYears,args)
    def subtractYears(*args): return apply(_cSimData.Date_subtractYears,args)
    __swig_getmethods__["getDaysInMonth"] = lambda x: _cSimData.Date_getDaysInMonth
    if _newclass:getDaysInMonth = staticmethod(_cSimData.Date_getDaysInMonth)
    __swig_getmethods__["getWeeksInYear"] = lambda x: _cSimData.Date_getWeeksInYear
    if _newclass:getWeeksInYear = staticmethod(_cSimData.Date_getWeeksInYear)
    __swig_getmethods__["getSundayWeeksInYear"] = lambda x: _cSimData.Date_getSundayWeeksInYear
    if _newclass:getSundayWeeksInYear = staticmethod(_cSimData.Date_getSundayWeeksInYear)
    __swig_getmethods__["compare"] = lambda x: _cSimData.Date_compare
    if _newclass:compare = staticmethod(_cSimData.Date_compare)
    def convert(*args): return apply(_cSimData.Date_convert,args)
    def formatString(*args): return apply(_cSimData.Date_formatString,args)
    def __repr__(*args): return apply(_cSimData.Date___repr__,args)
    def __del__(self, destroy= _cSimData.delete_Date):
        try:
            if self.thisown: destroy(self)
        except: pass

class DatePtr(Date):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Date
_cSimData.Date_swigregister(DatePtr)
Date_validYMD = _cSimData.Date_validYMD

Date_isLeap = _cSimData.Date_isLeap

Date_getDaysInMonth = _cSimData.Date_getDaysInMonth

Date_getWeeksInYear = _cSimData.Date_getWeeksInYear

Date_getSundayWeeksInYear = _cSimData.Date_getSundayWeeksInYear

Date_compare = _cSimData.Date_compare


class Zulu(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Zulu, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Zulu, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Zulu,args)
        self.thisown = 1
    def setTZ(*args): return apply(_cSimData.Zulu_setTZ,args)
    def getTZ(*args): return apply(_cSimData.Zulu_getTZ,args)
    def reduce(*args): return apply(_cSimData.Zulu_reduce,args)
    def getTime(*args): return apply(_cSimData.Zulu_getTime,args)
    def overflow(*args): return apply(_cSimData.Zulu_overflow,args)
    def getHour(*args): return apply(_cSimData.Zulu_getHour,args)
    def getMinute(*args): return apply(_cSimData.Zulu_getMinute,args)
    def getSecond(*args): return apply(_cSimData.Zulu_getSecond,args)
    def addTime(*args): return apply(_cSimData.Zulu_addTime,args)
    def setTime(*args): return apply(_cSimData.Zulu_setTime,args)
    def convert(*args): return apply(_cSimData.Zulu_convert,args)
    def formatString(*args): return apply(_cSimData.Zulu_formatString,args)
    def __repr__(*args): return apply(_cSimData.Zulu___repr__,args)
    def __del__(self, destroy= _cSimData.delete_Zulu):
        try:
            if self.thisown: destroy(self)
        except: pass

class ZuluPtr(Zulu):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Zulu
_cSimData.Zulu_swigregister(ZuluPtr)

class DateZulu(Date,Zulu):
    __swig_setmethods__ = {}
    for _s in [Date,Zulu]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, DateZulu, name, value)
    __swig_getmethods__ = {}
    for _s in [Date,Zulu]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, DateZulu, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_DateZulu,args)
        self.thisown = 1
    def convert(*args): return apply(_cSimData.DateZulu_convert,args)
    def formatString(*args): return apply(_cSimData.DateZulu_formatString,args)
    def __repr__(*args): return apply(_cSimData.DateZulu___repr__,args)
    def addTime(*args): return apply(_cSimData.DateZulu_addTime,args)
    def setTime(*args): return apply(_cSimData.DateZulu_setTime,args)
    def getJulianDate(*args): return apply(_cSimData.DateZulu_getJulianDate,args)
    def getAccurateMST(*args): return apply(_cSimData.DateZulu_getAccurateMST,args)
    def getMST(*args): return apply(_cSimData.DateZulu_getMST,args)
    def __del__(self, destroy= _cSimData.delete_DateZulu):
        try:
            if self.thisown: destroy(self)
        except: pass

class DateZuluPtr(DateZulu):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DateZulu
_cSimData.DateZulu_swigregister(DateZuluPtr)

class SimDate(DateZulu,BaseType):
    __swig_setmethods__ = {}
    for _s in [DateZulu,BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, SimDate, name, value)
    __swig_getmethods__ = {}
    for _s in [DateZulu,BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, SimDate, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_SimDate,args)
        self.thisown = 1
    def __repr__(*args): return apply(_cSimData.SimDate___repr__,args)
    __swig_getmethods__["interval"] = lambda x: _cSimData.SimDate_interval
    if _newclass:interval = staticmethod(_cSimData.SimDate_interval)
    __swig_getmethods__["getSystemTime"] = lambda x: _cSimData.SimDate_getSystemTime
    if _newclass:getSystemTime = staticmethod(_cSimData.SimDate_getSystemTime)
    def update(*args): return apply(_cSimData.SimDate_update,args)
    def setReferenceTime(*args): return apply(_cSimData.SimDate_setReferenceTime,args)
    def pause(*args): return apply(_cSimData.SimDate_pause,args)
    def unpause(*args): return apply(_cSimData.SimDate_unpause,args)
    def isPaused(*args): return apply(_cSimData.SimDate_isPaused,args)
    def pack(*args): return apply(_cSimData.SimDate_pack,args)
    def unpack(*args): return apply(_cSimData.SimDate_unpack,args)
    def parseXML(*args): return apply(_cSimData.SimDate_parseXML,args)
    def __del__(self, destroy= _cSimData.delete_SimDate):
        try:
            if self.thisown: destroy(self)
        except: pass

class SimDatePtr(SimDate):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SimDate
_cSimData.SimDate_swigregister(SimDatePtr)
SimDate_interval = _cSimData.SimDate_interval

SimDate_getSystemTime = _cSimData.SimDate_getSystemTime


class Vector3(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Vector3, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Vector3, name)
    __swig_setmethods__["x"] = _cSimData.Vector3_x_set
    __swig_getmethods__["x"] = _cSimData.Vector3_x_get
    if _newclass:x = property(_cSimData.Vector3_x_get,_cSimData.Vector3_x_set)
    __swig_setmethods__["y"] = _cSimData.Vector3_y_set
    __swig_getmethods__["y"] = _cSimData.Vector3_y_get
    if _newclass:y = property(_cSimData.Vector3_y_get,_cSimData.Vector3_y_set)
    __swig_setmethods__["z"] = _cSimData.Vector3_z_set
    __swig_getmethods__["z"] = _cSimData.Vector3_z_get
    if _newclass:z = property(_cSimData.Vector3_z_get,_cSimData.Vector3_z_set)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Vector3,args)
        self.thisown = 1
    def __sub__(*args): return apply(_cSimData.Vector3___sub__,args)
    def __add__(*args): return apply(_cSimData.Vector3___add__,args)
    def __neg__(*args): return apply(_cSimData.Vector3___neg__,args)
    def Set(*args): return apply(_cSimData.Vector3_Set,args)
    def Length(*args): return apply(_cSimData.Vector3_Length,args)
    def LengthSquared(*args): return apply(_cSimData.Vector3_LengthSquared,args)
    def IsNull(*args): return apply(_cSimData.Vector3_IsNull,args)
    def Normalize(*args): return apply(_cSimData.Vector3_Normalize,args)
    def Unitize(*args): return apply(_cSimData.Vector3_Unitize,args)
    def Cross(*args): return apply(_cSimData.Vector3_Cross,args)
    def Dot(*args): return apply(_cSimData.Vector3_Dot,args)
    def GetElements(*args): return apply(_cSimData.Vector3_GetElements,args)
    def StarMatrix(*args): return apply(_cSimData.Vector3_StarMatrix,args)
    def Print(*args): return apply(_cSimData.Vector3_Print,args)
    ZERO = _cSimData.cvar.Vector3_ZERO
    XAXIS = _cSimData.cvar.Vector3_XAXIS
    YAXIS = _cSimData.cvar.Vector3_YAXIS
    ZAXIS = _cSimData.cvar.Vector3_ZAXIS
    def __repr__(*args): return apply(_cSimData.Vector3___repr__,args)
    def __eq__(*args): return apply(_cSimData.Vector3___eq__,args)
    def __ne__(*args): return apply(_cSimData.Vector3___ne__,args)
    def __mul__(*args): return apply(_cSimData.Vector3___mul__,args)

    def pack(*args): return apply(_cSimData.Vector3_pack,args)
    def unpack(*args): return apply(_cSimData.Vector3_unpack,args)
    def parseXML(*args): return apply(_cSimData.Vector3_parseXML,args)
    def __del__(self, destroy= _cSimData.delete_Vector3):
        try:
            if self.thisown: destroy(self)
        except: pass

class Vector3Ptr(Vector3):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Vector3
_cSimData.Vector3_swigregister(Vector3Ptr)
cvar = _cSimData.cvar

class External(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, External, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, External, name)
    def __del__(self, destroy= _cSimData.delete_External):
        try:
            if self.thisown: destroy(self)
        except: pass
    def setSource(*args): return apply(_cSimData.External_setSource,args)
    def getSource(*args): return apply(_cSimData.External_getSource,args)
    def pack(*args): return apply(_cSimData.External_pack,args)
    def unpack(*args): return apply(_cSimData.External_unpack,args)
    def __repr__(*args): return apply(_cSimData.External___repr__,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_External,args)
        self.thisown = 1

class ExternalPtr(External):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = External
_cSimData.External_swigregister(ExternalPtr)

class Spread(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Spread, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Spread, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Spread,args)
        self.thisown = 1
    def set(*args): return apply(_cSimData.Spread_set,args)
    def regen(*args): return apply(_cSimData.Spread_regen,args)
    def getMean(*args): return apply(_cSimData.Spread_getMean,args)
    def getSigma(*args): return apply(_cSimData.Spread_getSigma,args)
    def getValue(*args): return apply(_cSimData.Spread_getValue,args)
    def __repr__(*args): return apply(_cSimData.Spread___repr__,args)
    def __neg__(*args): return apply(_cSimData.Spread___neg__,args)
    def __pos__(*args): return apply(_cSimData.Spread___pos__,args)
    def __abs__(*args): return apply(_cSimData.Spread___abs__,args)
    def __nonzero__(*args): return apply(_cSimData.Spread___nonzero__,args)
    def __float__(*args): return apply(_cSimData.Spread___float__,args)
    def __int__(*args): return apply(_cSimData.Spread___int__,args)
    def __long__(*args): return apply(_cSimData.Spread___long__,args)
    def __add__(*args): return apply(_cSimData.Spread___add__,args)
    def __radd__(*args): return apply(_cSimData.Spread___radd__,args)
    def __sub__(*args): return apply(_cSimData.Spread___sub__,args)
    def __rsub__(*args): return apply(_cSimData.Spread___rsub__,args)
    def __mul__(*args): return apply(_cSimData.Spread___mul__,args)
    def __rmul__(*args): return apply(_cSimData.Spread___rmul__,args)
    def __div__(*args): return apply(_cSimData.Spread___div__,args)
    def __rdiv__(*args): return apply(_cSimData.Spread___rdiv__,args)
    def __rpow__(*args): return apply(_cSimData.Spread___rpow__,args)
    def __pow__(*args): return apply(_cSimData.Spread___pow__,args)
    def pack(*args): return apply(_cSimData.Spread_pack,args)
    def unpack(*args): return apply(_cSimData.Spread_unpack,args)
    def parseXML(*args): return apply(_cSimData.Spread_parseXML,args)
    def __del__(self, destroy= _cSimData.delete_Spread):
        try:
            if self.thisown: destroy(self)
        except: pass

class SpreadPtr(Spread):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Spread
_cSimData.Spread_swigregister(SpreadPtr)

class swig_string_bug_workaround(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, swig_string_bug_workaround, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, swig_string_bug_workaround, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_swig_string_bug_workaround,args)
        self.thisown = 1
    __swig_setmethods__["get_as_swig_wrapped_pointer"] = _cSimData.swig_string_bug_workaround_get_as_swig_wrapped_pointer_set
    __swig_getmethods__["get_as_swig_wrapped_pointer"] = _cSimData.swig_string_bug_workaround_get_as_swig_wrapped_pointer_get
    if _newclass:get_as_swig_wrapped_pointer = property(_cSimData.swig_string_bug_workaround_get_as_swig_wrapped_pointer_get,_cSimData.swig_string_bug_workaround_get_as_swig_wrapped_pointer_set)
    def __del__(self, destroy= _cSimData.delete_swig_string_bug_workaround):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C swig_string_bug_workaround instance at %s>" % (self.this,)

class swig_string_bug_workaroundPtr(swig_string_bug_workaround):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = swig_string_bug_workaround
_cSimData.swig_string_bug_workaround_swigregister(swig_string_bug_workaroundPtr)

class vEnum(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vEnum, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vEnum, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vEnum,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vEnum___len__,args)
    def clear(*args): return apply(_cSimData.vEnum_clear,args)
    def append(*args): return apply(_cSimData.vEnum_append,args)
    def __nonzero__(*args): return apply(_cSimData.vEnum___nonzero__,args)
    def pop(*args): return apply(_cSimData.vEnum_pop,args)
    def __getitem__(*args): return apply(_cSimData.vEnum___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vEnum___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vEnum___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vEnum___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vEnum___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vEnum___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vEnum):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vEnum instance at %s>" % (self.this,)

class vEnumPtr(vEnum):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vEnum
_cSimData.vEnum_swigregister(vEnumPtr)

class vString(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vString, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vString, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vString,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vString___len__,args)
    def __nonzero__(*args): return apply(_cSimData.vString___nonzero__,args)
    def clear(*args): return apply(_cSimData.vString_clear,args)
    def append(*args): return apply(_cSimData.vString_append,args)
    def pop(*args): return apply(_cSimData.vString_pop,args)
    def __getitem__(*args): return apply(_cSimData.vString___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vString___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vString___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vString___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vString___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vString___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vString):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vString instance at %s>" % (self.this,)

class vStringPtr(vString):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vString
_cSimData.vString_swigregister(vStringPtr)

class EnumError(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EnumError, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EnumError, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_EnumError,args)
        self.thisown = 1
    def disarm(*args): return apply(_cSimData.EnumError_disarm,args)
    def __del__(self, destroy= _cSimData.delete_EnumError):
        try:
            if self.thisown: destroy(self)
        except: pass
    def getError(*args): return apply(_cSimData.EnumError_getError,args)
    def __repr__(self):
        return "<C EnumError instance at %s>" % (self.this,)

class EnumErrorPtr(EnumError):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EnumError
_cSimData.EnumError_swigregister(EnumErrorPtr)

class EnumIndexError(EnumError):
    __swig_setmethods__ = {}
    for _s in [EnumError]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, EnumIndexError, name, value)
    __swig_getmethods__ = {}
    for _s in [EnumError]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, EnumIndexError, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_EnumIndexError,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_EnumIndexError):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C EnumIndexError instance at %s>" % (self.this,)

class EnumIndexErrorPtr(EnumIndexError):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EnumIndexError
_cSimData.EnumIndexError_swigregister(EnumIndexErrorPtr)

class EnumTypeError(EnumError):
    __swig_setmethods__ = {}
    for _s in [EnumError]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, EnumTypeError, name, value)
    __swig_getmethods__ = {}
    for _s in [EnumError]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, EnumTypeError, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_EnumTypeError,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_EnumTypeError):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C EnumTypeError instance at %s>" % (self.this,)

class EnumTypeErrorPtr(EnumTypeError):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EnumTypeError
_cSimData.EnumTypeError_swigregister(EnumTypeErrorPtr)

class Enum(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Enum, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Enum, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Enum,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_Enum):
        try:
            if self.thisown: destroy(self)
        except: pass
    def asInt(*args): return apply(_cSimData.Enum_asInt,args)
    def cycle(*args): return apply(_cSimData.Enum_cycle,args)
    def cycleBack(*args): return apply(_cSimData.Enum_cycleBack,args)
    def pack(*args): return apply(_cSimData.Enum_pack,args)
    def unpack(*args): return apply(_cSimData.Enum_unpack,args)
    def parseXML(*args): return apply(_cSimData.Enum_parseXML,args)
    def __repr__(*args): return apply(_cSimData.Enum___repr__,args)
    def __eq__(*args): return apply(_cSimData.Enum___eq__,args)
    def __ne__(*args): return apply(_cSimData.Enum___ne__,args)
    def __le__(*args): return apply(_cSimData.Enum___le__,args)
    def __ge__(*args): return apply(_cSimData.Enum___ge__,args)
    def __lt__(*args): return apply(_cSimData.Enum___lt__,args)
    def __gt__(*args): return apply(_cSimData.Enum___gt__,args)
    def fromString(*args): return apply(_cSimData.Enum_fromString,args)
    def fromEnum(*args): return apply(_cSimData.Enum_fromEnum,args)
    def __copy__(*args): return apply(_cSimData.Enum___copy__,args)

class EnumPtr(Enum):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Enum
_cSimData.Enum_swigregister(EnumPtr)

class Enumeration(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Enumeration, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Enumeration, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Enumeration,args)
        self.thisown = 1
    def each(*args): return apply(_cSimData.Enumeration_each,args)
    def eachString(*args): return apply(_cSimData.Enumeration_eachString,args)
    def getLabels(*args): return apply(_cSimData.Enumeration_getLabels,args)
    def size(*args): return apply(_cSimData.Enumeration_size,args)
    def _getID(*args): return apply(_cSimData.Enumeration__getID,args)
    def _getName(*args): return apply(_cSimData.Enumeration__getName,args)
    def irange(*args): return apply(_cSimData.Enumeration_irange,args)
    def contains(*args): return apply(_cSimData.Enumeration_contains,args)
    def __getslice__(*args): return apply(_cSimData.Enumeration___getslice__,args)
    def __len__(*args): return apply(_cSimData.Enumeration___len__,args)
    def __contains__(*args): return apply(_cSimData.Enumeration___contains__,args)
    def __getitem__(*args): return apply(_cSimData.Enumeration___getitem__,args)
    def __getattr_c__(*args): return apply(_cSimData.Enumeration___getattr_c__,args)
    def __repr__(*args): return apply(_cSimData.Enumeration___repr__,args)
    def __getattr__(*args): 
    	self, name = args
    	if name in ["this", "thisown", "__str__"]: 
    		return _swig_getattr(self, Enumeration, name)
    	return apply(_cSimData.Enumeration___getattr_c__,args)


    def __del__(self, destroy= _cSimData.delete_Enumeration):
        try:
            if self.thisown: destroy(self)
        except: pass

class EnumerationPtr(Enumeration):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Enumeration
_cSimData.Enumeration_swigregister(EnumerationPtr)

class vector_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vector_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vector_t, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vector_t,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vector_t___len__,args)
    def __nonzero__(*args): return apply(_cSimData.vector_t___nonzero__,args)
    def clear(*args): return apply(_cSimData.vector_t_clear,args)
    def append(*args): return apply(_cSimData.vector_t_append,args)
    def pop(*args): return apply(_cSimData.vector_t_pop,args)
    def __getitem__(*args): return apply(_cSimData.vector_t___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vector_t___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vector_t___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vector_t___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vector_t___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vector_t___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vector_t):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vector_t instance at %s>" % (self.this,)

class vector_tPtr(vector_t):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vector_t
_cSimData.vector_t_swigregister(vector_tPtr)

class InterpolatedData(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, InterpolatedData, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, InterpolatedData, name)
    Method = _cSimData.cvar.InterpolatedData_Method
    __swig_setmethods__["method"] = _cSimData.InterpolatedData_method_set
    __swig_getmethods__["method"] = _cSimData.InterpolatedData_method_get
    if _newclass:method = property(_cSimData.InterpolatedData_method_get,_cSimData.InterpolatedData_method_set)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_InterpolatedData,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_InterpolatedData):
        try:
            if self.thisown: destroy(self)
        except: pass
    def pack(*args): return apply(_cSimData.InterpolatedData_pack,args)
    def unpack(*args): return apply(_cSimData.InterpolatedData_unpack,args)
    def getValue(*args): return apply(_cSimData.InterpolatedData_getValue,args)
    def getPrecise(*args): return apply(_cSimData.InterpolatedData_getPrecise,args)
    def compute_second_derivatives(*args): return apply(_cSimData.InterpolatedData_compute_second_derivatives,args)
    def find(*args): return apply(_cSimData.InterpolatedData_find,args)
    def __repr__(self):
        return "<C InterpolatedData instance at %s>" % (self.this,)

class InterpolatedDataPtr(InterpolatedData):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InterpolatedData
_cSimData.InterpolatedData_swigregister(InterpolatedDataPtr)

class Curve(InterpolatedData):
    __swig_setmethods__ = {}
    for _s in [InterpolatedData]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Curve, name, value)
    __swig_getmethods__ = {}
    for _s in [InterpolatedData]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Curve, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Curve,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_Curve):
        try:
            if self.thisown: destroy(self)
        except: pass
    def pack(*args): return apply(_cSimData.Curve_pack,args)
    def unpack(*args): return apply(_cSimData.Curve_unpack,args)
    def getBreaks(*args): return apply(_cSimData.Curve_getBreaks,args)
    def setBreaks(*args): return apply(_cSimData.Curve_setBreaks,args)
    def setData(*args): return apply(_cSimData.Curve_setData,args)
    def interpolate(*args): return apply(_cSimData.Curve_interpolate,args)
    def _compute_second_derivatives(*args): return apply(_cSimData.Curve__compute_second_derivatives,args)
    def getPrecise(*args): return apply(_cSimData.Curve_getPrecise,args)
    def getValue(*args): return apply(_cSimData.Curve_getValue,args)
    def dumpCurve(*args): return apply(_cSimData.Curve_dumpCurve,args)
    def __repr__(*args): return apply(_cSimData.Curve___repr__,args)

class CurvePtr(Curve):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Curve
_cSimData.Curve_swigregister(CurvePtr)

class Table(InterpolatedData):
    __swig_setmethods__ = {}
    for _s in [InterpolatedData]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Table, name, value)
    __swig_getmethods__ = {}
    for _s in [InterpolatedData]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Table, name)
    __swig_setmethods__["_x_label"] = _cSimData.Table__x_label_set
    __swig_getmethods__["_x_label"] = _cSimData.Table__x_label_get
    if _newclass:_x_label = property(_cSimData.Table__x_label_get,_cSimData.Table__x_label_set)
    __swig_setmethods__["_y_label"] = _cSimData.Table__y_label_set
    __swig_getmethods__["_y_label"] = _cSimData.Table__y_label_get
    if _newclass:_y_label = property(_cSimData.Table__y_label_get,_cSimData.Table__y_label_set)
    __swig_setmethods__["_x_breaks"] = _cSimData.Table__x_breaks_set
    __swig_getmethods__["_x_breaks"] = _cSimData.Table__x_breaks_get
    if _newclass:_x_breaks = property(_cSimData.Table__x_breaks_get,_cSimData.Table__x_breaks_set)
    __swig_setmethods__["_y_breaks"] = _cSimData.Table__y_breaks_set
    __swig_getmethods__["_y_breaks"] = _cSimData.Table__y_breaks_get
    if _newclass:_y_breaks = property(_cSimData.Table__y_breaks_get,_cSimData.Table__y_breaks_set)
    __swig_setmethods__["_data"] = _cSimData.Table__data_set
    __swig_getmethods__["_data"] = _cSimData.Table__data_get
    if _newclass:_data = property(_cSimData.Table__data_get,_cSimData.Table__data_set)
    __swig_setmethods__["_table"] = _cSimData.Table__table_set
    __swig_getmethods__["_table"] = _cSimData.Table__table_get
    if _newclass:_table = property(_cSimData.Table__table_get,_cSimData.Table__table_set)
    __swig_setmethods__["_dcols"] = _cSimData.Table__dcols_set
    __swig_getmethods__["_dcols"] = _cSimData.Table__dcols_get
    if _newclass:_dcols = property(_cSimData.Table__dcols_get,_cSimData.Table__dcols_set)
    __swig_setmethods__["_drows"] = _cSimData.Table__drows_set
    __swig_getmethods__["_drows"] = _cSimData.Table__drows_get
    if _newclass:_drows = property(_cSimData.Table__drows_get,_cSimData.Table__drows_set)
    __swig_setmethods__["_x_spacing"] = _cSimData.Table__x_spacing_set
    __swig_getmethods__["_x_spacing"] = _cSimData.Table__x_spacing_get
    if _newclass:_x_spacing = property(_cSimData.Table__x_spacing_get,_cSimData.Table__x_spacing_set)
    __swig_setmethods__["_y_spacing"] = _cSimData.Table__y_spacing_set
    __swig_getmethods__["_y_spacing"] = _cSimData.Table__y_spacing_get
    if _newclass:_y_spacing = property(_cSimData.Table__y_spacing_get,_cSimData.Table__y_spacing_set)
    __swig_setmethods__["_x_min"] = _cSimData.Table__x_min_set
    __swig_getmethods__["_x_min"] = _cSimData.Table__x_min_get
    if _newclass:_x_min = property(_cSimData.Table__x_min_get,_cSimData.Table__x_min_set)
    __swig_setmethods__["_y_min"] = _cSimData.Table__y_min_set
    __swig_getmethods__["_y_min"] = _cSimData.Table__y_min_get
    if _newclass:_y_min = property(_cSimData.Table__y_min_get,_cSimData.Table__y_min_set)
    __swig_setmethods__["_x_range"] = _cSimData.Table__x_range_set
    __swig_getmethods__["_x_range"] = _cSimData.Table__x_range_get
    if _newclass:_x_range = property(_cSimData.Table__x_range_get,_cSimData.Table__x_range_set)
    __swig_setmethods__["_y_range"] = _cSimData.Table__y_range_set
    __swig_getmethods__["_y_range"] = _cSimData.Table__y_range_get
    if _newclass:_y_range = property(_cSimData.Table__y_range_get,_cSimData.Table__y_range_set)
    __swig_setmethods__["_x_in"] = _cSimData.Table__x_in_set
    __swig_getmethods__["_x_in"] = _cSimData.Table__x_in_get
    if _newclass:_x_in = property(_cSimData.Table__x_in_get,_cSimData.Table__x_in_set)
    __swig_setmethods__["_y_in"] = _cSimData.Table__y_in_set
    __swig_getmethods__["_y_in"] = _cSimData.Table__y_in_get
    if _newclass:_y_in = property(_cSimData.Table__y_in_get,_cSimData.Table__y_in_set)
    __swig_setmethods__["_x_n"] = _cSimData.Table__x_n_set
    __swig_getmethods__["_x_n"] = _cSimData.Table__x_n_get
    if _newclass:_x_n = property(_cSimData.Table__x_n_get,_cSimData.Table__x_n_set)
    __swig_setmethods__["_y_n"] = _cSimData.Table__y_n_set
    __swig_getmethods__["_y_n"] = _cSimData.Table__y_n_get
    if _newclass:_y_n = property(_cSimData.Table__y_n_get,_cSimData.Table__y_n_set)
    __swig_setmethods__["_valid"] = _cSimData.Table__valid_set
    __swig_getmethods__["_valid"] = _cSimData.Table__valid_get
    if _newclass:_valid = property(_cSimData.Table__valid_get,_cSimData.Table__valid_set)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Table,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_Table):
        try:
            if self.thisown: destroy(self)
        except: pass
    def invalidate(*args): return apply(_cSimData.Table_invalidate,args)
    def isValid(*args): return apply(_cSimData.Table_isValid,args)
    def pack(*args): return apply(_cSimData.Table_pack,args)
    def unpack(*args): return apply(_cSimData.Table_unpack,args)
    def getXBreaks(*args): return apply(_cSimData.Table_getXBreaks,args)
    def getYBreaks(*args): return apply(_cSimData.Table_getYBreaks,args)
    def setXBreaks(*args): return apply(_cSimData.Table_setXBreaks,args)
    def setYBreaks(*args): return apply(_cSimData.Table_setYBreaks,args)
    def setBreaks(*args): return apply(_cSimData.Table_setBreaks,args)
    def setXSpacing(*args): return apply(_cSimData.Table_setXSpacing,args)
    def setYSpacing(*args): return apply(_cSimData.Table_setYSpacing,args)
    def setSpacing(*args): return apply(_cSimData.Table_setSpacing,args)
    def getXSpacing(*args): return apply(_cSimData.Table_getXSpacing,args)
    def getYSpacing(*args): return apply(_cSimData.Table_getYSpacing,args)
    def setData(*args): return apply(_cSimData.Table_setData,args)
    def interpolate(*args): return apply(_cSimData.Table_interpolate,args)
    def getPrecise(*args): return apply(_cSimData.Table_getPrecise,args)
    def getValue(*args): return apply(_cSimData.Table_getValue,args)
    def _compute_second_derivatives(*args): return apply(_cSimData.Table__compute_second_derivatives,args)
    def toPGM(*args): return apply(_cSimData.Table_toPGM,args)
    def dumpTable(*args): return apply(_cSimData.Table_dumpTable,args)
    def dumpDRows(*args): return apply(_cSimData.Table_dumpDRows,args)
    def dumpDCols(*args): return apply(_cSimData.Table_dumpDCols,args)
    def __repr__(*args): return apply(_cSimData.Table___repr__,args)

class TablePtr(Table):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Table
_cSimData.Table_swigregister(TablePtr)

class ListBase(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ListBase, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ListBase, name)
    def __del__(self, destroy= _cSimData.delete_ListBase):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __init__(self,*args):
        self.this = apply(_cSimData.new_ListBase,args)
        self.thisown = 1
    def __repr__(self):
        return "<C ListBase instance at %s>" % (self.this,)

class ListBasePtr(ListBase):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListBase
_cSimData.ListBase_swigregister(ListBasePtr)

class vInt(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vInt, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vInt, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vInt,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vInt___len__,args)
    def __nonzero__(*args): return apply(_cSimData.vInt___nonzero__,args)
    def clear(*args): return apply(_cSimData.vInt_clear,args)
    def append(*args): return apply(_cSimData.vInt_append,args)
    def pop(*args): return apply(_cSimData.vInt_pop,args)
    def __getitem__(*args): return apply(_cSimData.vInt___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vInt___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vInt___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vInt___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vInt___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vInt___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vInt):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vInt instance at %s>" % (self.this,)

class vIntPtr(vInt):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vInt
_cSimData.vInt_swigregister(vIntPtr)

class lInt(ListBase,vInt):
    __swig_setmethods__ = {}
    for _s in [ListBase,vInt]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, lInt, name, value)
    __swig_getmethods__ = {}
    for _s in [ListBase,vInt]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, lInt, name)
    def __del__(self, destroy= _cSimData.delete_lInt):
        try:
            if self.thisown: destroy(self)
        except: pass
    def extend(*args): return apply(_cSimData.lInt_extend,args)
    def pack(*args): return apply(_cSimData.lInt_pack,args)
    def unpack(*args): return apply(_cSimData.lInt_unpack,args)
    def __repr__(*args): return apply(_cSimData.lInt___repr__,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_lInt,args)
        self.thisown = 1

class lIntPtr(lInt):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = lInt
_cSimData.lInt_swigregister(lIntPtr)

class vSpread(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vSpread, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vSpread, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vSpread,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vSpread___len__,args)
    def clear(*args): return apply(_cSimData.vSpread_clear,args)
    def append(*args): return apply(_cSimData.vSpread_append,args)
    def __nonzero__(*args): return apply(_cSimData.vSpread___nonzero__,args)
    def pop(*args): return apply(_cSimData.vSpread_pop,args)
    def __getitem__(*args): return apply(_cSimData.vSpread___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vSpread___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vSpread___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vSpread___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vSpread___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vSpread___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vSpread):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vSpread instance at %s>" % (self.this,)

class vSpreadPtr(vSpread):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vSpread
_cSimData.vSpread_swigregister(vSpreadPtr)

class lSpread(ListBase,vSpread):
    __swig_setmethods__ = {}
    for _s in [ListBase,vSpread]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, lSpread, name, value)
    __swig_getmethods__ = {}
    for _s in [ListBase,vSpread]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, lSpread, name)
    def __del__(self, destroy= _cSimData.delete_lSpread):
        try:
            if self.thisown: destroy(self)
        except: pass
    def extend(*args): return apply(_cSimData.lSpread_extend,args)
    def pack(*args): return apply(_cSimData.lSpread_pack,args)
    def unpack(*args): return apply(_cSimData.lSpread_unpack,args)
    def __repr__(*args): return apply(_cSimData.lSpread___repr__,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_lSpread,args)
        self.thisown = 1

class lSpreadPtr(lSpread):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = lSpread
_cSimData.lSpread_swigregister(lSpreadPtr)

class vPath(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vPath, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vPath, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vPath,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vPath___len__,args)
    def clear(*args): return apply(_cSimData.vPath_clear,args)
    def append(*args): return apply(_cSimData.vPath_append,args)
    def __nonzero__(*args): return apply(_cSimData.vPath___nonzero__,args)
    def pop(*args): return apply(_cSimData.vPath_pop,args)
    def __getitem__(*args): return apply(_cSimData.vPath___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vPath___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vPath___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vPath___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vPath___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vPath___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vPath):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vPath instance at %s>" % (self.this,)

class vPathPtr(vPath):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vPath
_cSimData.vPath_swigregister(vPathPtr)

class lPath(ListBase,vPath):
    __swig_setmethods__ = {}
    for _s in [ListBase,vPath]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, lPath, name, value)
    __swig_getmethods__ = {}
    for _s in [ListBase,vPath]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, lPath, name)
    def __del__(self, destroy= _cSimData.delete_lPath):
        try:
            if self.thisown: destroy(self)
        except: pass
    def extend(*args): return apply(_cSimData.lPath_extend,args)
    def pack(*args): return apply(_cSimData.lPath_pack,args)
    def unpack(*args): return apply(_cSimData.lPath_unpack,args)
    def __repr__(*args): return apply(_cSimData.lPath___repr__,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_lPath,args)
        self.thisown = 1

class lPathPtr(lPath):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = lPath
_cSimData.lPath_swigregister(lPathPtr)

class vCurve(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vCurve, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vCurve, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vCurve,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vCurve___len__,args)
    def clear(*args): return apply(_cSimData.vCurve_clear,args)
    def append(*args): return apply(_cSimData.vCurve_append,args)
    def __nonzero__(*args): return apply(_cSimData.vCurve___nonzero__,args)
    def pop(*args): return apply(_cSimData.vCurve_pop,args)
    def __getitem__(*args): return apply(_cSimData.vCurve___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vCurve___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vCurve___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vCurve___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vCurve___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vCurve___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vCurve):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vCurve instance at %s>" % (self.this,)

class vCurvePtr(vCurve):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vCurve
_cSimData.vCurve_swigregister(vCurvePtr)

class lCurve(ListBase,vCurve):
    __swig_setmethods__ = {}
    for _s in [ListBase,vCurve]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, lCurve, name, value)
    __swig_getmethods__ = {}
    for _s in [ListBase,vCurve]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, lCurve, name)
    def __del__(self, destroy= _cSimData.delete_lCurve):
        try:
            if self.thisown: destroy(self)
        except: pass
    def extend(*args): return apply(_cSimData.lCurve_extend,args)
    def pack(*args): return apply(_cSimData.lCurve_pack,args)
    def unpack(*args): return apply(_cSimData.lCurve_unpack,args)
    def __repr__(*args): return apply(_cSimData.lCurve___repr__,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_lCurve,args)
        self.thisown = 1

class lCurvePtr(lCurve):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = lCurve
_cSimData.lCurve_swigregister(lCurvePtr)

class vTable(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vTable, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vTable, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vTable,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vTable___len__,args)
    def clear(*args): return apply(_cSimData.vTable_clear,args)
    def append(*args): return apply(_cSimData.vTable_append,args)
    def __nonzero__(*args): return apply(_cSimData.vTable___nonzero__,args)
    def pop(*args): return apply(_cSimData.vTable_pop,args)
    def __getitem__(*args): return apply(_cSimData.vTable___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vTable___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vTable___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vTable___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vTable___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vTable___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vTable):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vTable instance at %s>" % (self.this,)

class vTablePtr(vTable):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vTable
_cSimData.vTable_swigregister(vTablePtr)

class lTable(ListBase,vTable):
    __swig_setmethods__ = {}
    for _s in [ListBase,vTable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, lTable, name, value)
    __swig_getmethods__ = {}
    for _s in [ListBase,vTable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, lTable, name)
    def __del__(self, destroy= _cSimData.delete_lTable):
        try:
            if self.thisown: destroy(self)
        except: pass
    def extend(*args): return apply(_cSimData.lTable_extend,args)
    def pack(*args): return apply(_cSimData.lTable_pack,args)
    def unpack(*args): return apply(_cSimData.lTable_unpack,args)
    def __repr__(*args): return apply(_cSimData.lTable___repr__,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_lTable,args)
        self.thisown = 1

class lTablePtr(lTable):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = lTable
_cSimData.lTable_swigregister(lTablePtr)

class vExternal(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, vExternal, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, vExternal, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_vExternal,args)
        self.thisown = 1
    def __len__(*args): return apply(_cSimData.vExternal___len__,args)
    def clear(*args): return apply(_cSimData.vExternal_clear,args)
    def append(*args): return apply(_cSimData.vExternal_append,args)
    def __nonzero__(*args): return apply(_cSimData.vExternal___nonzero__,args)
    def pop(*args): return apply(_cSimData.vExternal_pop,args)
    def __getitem__(*args): return apply(_cSimData.vExternal___getitem__,args)
    def __getslice__(*args): return apply(_cSimData.vExternal___getslice__,args)
    def __setitem__(*args): return apply(_cSimData.vExternal___setitem__,args)
    def __setslice__(*args): return apply(_cSimData.vExternal___setslice__,args)
    def __delitem__(*args): return apply(_cSimData.vExternal___delitem__,args)
    def __delslice__(*args): return apply(_cSimData.vExternal___delslice__,args)
    def __del__(self, destroy= _cSimData.delete_vExternal):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C vExternal instance at %s>" % (self.this,)

class vExternalPtr(vExternal):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = vExternal
_cSimData.vExternal_swigregister(vExternalPtr)

class lExternal(ListBase,vExternal):
    __swig_setmethods__ = {}
    for _s in [ListBase,vExternal]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, lExternal, name, value)
    __swig_getmethods__ = {}
    for _s in [ListBase,vExternal]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, lExternal, name)
    def __del__(self, destroy= _cSimData.delete_lExternal):
        try:
            if self.thisown: destroy(self)
        except: pass
    def extend(*args): return apply(_cSimData.lExternal_extend,args)
    def pack(*args): return apply(_cSimData.lExternal_pack,args)
    def unpack(*args): return apply(_cSimData.lExternal_unpack,args)
    def __repr__(*args): return apply(_cSimData.lExternal___repr__,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_lExternal,args)
        self.thisown = 1

class lExternalPtr(lExternal):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = lExternal
_cSimData.lExternal_swigregister(lExternalPtr)

class Matrix3(BaseType):
    __swig_setmethods__ = {}
    for _s in [BaseType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Matrix3, name, value)
    __swig_getmethods__ = {}
    for _s in [BaseType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Matrix3, name)
    __swig_setmethods__["rowcol"] = _cSimData.Matrix3_rowcol_set
    __swig_getmethods__["rowcol"] = _cSimData.Matrix3_rowcol_get
    if _newclass:rowcol = property(_cSimData.Matrix3_rowcol_get,_cSimData.Matrix3_rowcol_set)
    def pack(*args): return apply(_cSimData.Matrix3_pack,args)
    def unpack(*args): return apply(_cSimData.Matrix3_unpack,args)
    def parseXML(*args): return apply(_cSimData.Matrix3_parseXML,args)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_Matrix3,args)
        self.thisown = 1
    def SetColumn(*args): return apply(_cSimData.Matrix3_SetColumn,args)
    def GetColumn(*args): return apply(_cSimData.Matrix3_GetColumn,args)
    def GetRow(*args): return apply(_cSimData.Matrix3_GetRow,args)
    def GetElements(*args): return apply(_cSimData.Matrix3_GetElements,args)
    def __eq__(*args): return apply(_cSimData.Matrix3___eq__,args)
    def __ne__(*args): return apply(_cSimData.Matrix3___ne__,args)
    def __add__(*args): return apply(_cSimData.Matrix3___add__,args)
    def __sub__(*args): return apply(_cSimData.Matrix3___sub__,args)
    def __neg__(*args): return apply(_cSimData.Matrix3___neg__,args)
    def __mul__(*args): return apply(_cSimData.Matrix3___mul__,args)
    def Transpose(*args): return apply(_cSimData.Matrix3_Transpose,args)
    def Inverse(*args): return apply(_cSimData.Matrix3_Inverse,args)
    def Determinant(*args): return apply(_cSimData.Matrix3_Determinant,args)
    def ToAxisAngle(*args): return apply(_cSimData.Matrix3_ToAxisAngle,args)
    def FromAxisAngle(*args): return apply(_cSimData.Matrix3_FromAxisAngle,args)
    def ToEulerAnglesXYZ(*args): return apply(_cSimData.Matrix3_ToEulerAnglesXYZ,args)
    def ToEulerAnglesXZY(*args): return apply(_cSimData.Matrix3_ToEulerAnglesXZY,args)
    def ToEulerAnglesYXZ(*args): return apply(_cSimData.Matrix3_ToEulerAnglesYXZ,args)
    def ToEulerAnglesYZX(*args): return apply(_cSimData.Matrix3_ToEulerAnglesYZX,args)
    def ToEulerAnglesZXY(*args): return apply(_cSimData.Matrix3_ToEulerAnglesZXY,args)
    def ToEulerAnglesZYX(*args): return apply(_cSimData.Matrix3_ToEulerAnglesZYX,args)
    def FromEulerAnglesXYZ(*args): return apply(_cSimData.Matrix3_FromEulerAnglesXYZ,args)
    def FromEulerAnglesXZY(*args): return apply(_cSimData.Matrix3_FromEulerAnglesXZY,args)
    def FromEulerAnglesYXZ(*args): return apply(_cSimData.Matrix3_FromEulerAnglesYXZ,args)
    def FromEulerAnglesYZX(*args): return apply(_cSimData.Matrix3_FromEulerAnglesYZX,args)
    def FromEulerAnglesZXY(*args): return apply(_cSimData.Matrix3_FromEulerAnglesZXY,args)
    def FromEulerAnglesZYX(*args): return apply(_cSimData.Matrix3_FromEulerAnglesZYX,args)
    __swig_getmethods__["TensorProduct"] = lambda x: _cSimData.Matrix3_TensorProduct
    if _newclass:TensorProduct = staticmethod(_cSimData.Matrix3_TensorProduct)
    EPSILON = _cSimData.cvar.Matrix3_EPSILON
    ZERO = _cSimData.cvar.Matrix3_ZERO
    IDENTITY = _cSimData.cvar.Matrix3_IDENTITY
    def Print(*args): return apply(_cSimData.Matrix3_Print,args)
    def __repr__(*args): return apply(_cSimData.Matrix3___repr__,args)

    def __del__(self, destroy= _cSimData.delete_Matrix3):
        try:
            if self.thisown: destroy(self)
        except: pass

class Matrix3Ptr(Matrix3):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Matrix3
_cSimData.Matrix3_swigregister(Matrix3Ptr)
Matrix3_TensorProduct = _cSimData.Matrix3_TensorProduct


class BadMagic(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BadMagic, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BadMagic, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_BadMagic,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_BadMagic):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C BadMagic instance at %s>" % (self.this,)

class BadMagicPtr(BadMagic):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BadMagic
_cSimData.BadMagic_swigregister(BadMagicPtr)

class BadByteOrder(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BadByteOrder, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BadByteOrder, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_BadByteOrder,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_BadByteOrder):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C BadByteOrder instance at %s>" % (self.this,)

class BadByteOrderPtr(BadByteOrder):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BadByteOrder
_cSimData.BadByteOrder_swigregister(BadByteOrderPtr)

class CorruptArchive(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, CorruptArchive, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, CorruptArchive, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_CorruptArchive,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_CorruptArchive):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C CorruptArchive instance at %s>" % (self.this,)

class CorruptArchivePtr(CorruptArchive):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CorruptArchive
_cSimData.CorruptArchive_swigregister(CorruptArchivePtr)

class IndexError(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, IndexError, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, IndexError, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_IndexError,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_IndexError):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C IndexError instance at %s>" % (self.this,)

class IndexErrorPtr(IndexError):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IndexError
_cSimData.IndexError_swigregister(IndexErrorPtr)

class ObjectMismatch(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ObjectMismatch, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ObjectMismatch, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_ObjectMismatch,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_ObjectMismatch):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ObjectMismatch instance at %s>" % (self.this,)

class ObjectMismatchPtr(ObjectMismatch):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ObjectMismatch
_cSimData.ObjectMismatch_swigregister(ObjectMismatchPtr)

class IOError(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, IOError, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, IOError, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_IOError,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_IOError):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C IOError instance at %s>" % (self.this,)

class IOErrorPtr(IOError):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IOError
_cSimData.IOError_swigregister(IOErrorPtr)

class TableEntry(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TableEntry, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TableEntry, name)
    __swig_setmethods__["pathhash"] = _cSimData.TableEntry_pathhash_set
    __swig_getmethods__["pathhash"] = _cSimData.TableEntry_pathhash_get
    if _newclass:pathhash = property(_cSimData.TableEntry_pathhash_get,_cSimData.TableEntry_pathhash_set)
    __swig_setmethods__["classhash"] = _cSimData.TableEntry_classhash_set
    __swig_getmethods__["classhash"] = _cSimData.TableEntry_classhash_get
    if _newclass:classhash = property(_cSimData.TableEntry_classhash_get,_cSimData.TableEntry_classhash_set)
    __swig_setmethods__["offset"] = _cSimData.TableEntry_offset_set
    __swig_getmethods__["offset"] = _cSimData.TableEntry_offset_get
    if _newclass:offset = property(_cSimData.TableEntry_offset_get,_cSimData.TableEntry_offset_set)
    __swig_setmethods__["length"] = _cSimData.TableEntry_length_set
    __swig_getmethods__["length"] = _cSimData.TableEntry_length_get
    if _newclass:length = property(_cSimData.TableEntry_length_get,_cSimData.TableEntry_length_set)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_TableEntry,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_TableEntry):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C TableEntry instance at %s>" % (self.this,)

class TableEntryPtr(TableEntry):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TableEntry
_cSimData.TableEntry_swigregister(TableEntryPtr)

class FP(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FP, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FP, name)
    __swig_setmethods__["f"] = _cSimData.FP_f_set
    __swig_getmethods__["f"] = _cSimData.FP_f_get
    if _newclass:f = property(_cSimData.FP_f_get,_cSimData.FP_f_set)
    __swig_setmethods__["name"] = _cSimData.FP_name_set
    __swig_getmethods__["name"] = _cSimData.FP_name_get
    if _newclass:name = property(_cSimData.FP_name_get,_cSimData.FP_name_set)
    __swig_setmethods__["mode"] = _cSimData.FP_mode_set
    __swig_getmethods__["mode"] = _cSimData.FP_mode_get
    if _newclass:mode = property(_cSimData.FP_mode_get,_cSimData.FP_mode_set)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_FP,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_FP):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C FP instance at %s>" % (self.this,)

class FPPtr(FP):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FP
_cSimData.FP_swigregister(FPPtr)

class DataArchive(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, DataArchive, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, DataArchive, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_DataArchive,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_DataArchive):
        try:
            if self.thisown: destroy(self)
        except: pass
    def _addEntry(*args): return apply(_cSimData.DataArchive__addEntry,args)
    def addObject(*args): return apply(_cSimData.DataArchive_addObject,args)
    def close(*args): return apply(_cSimData.DataArchive_close,args)
    def isClosed(*args): return apply(_cSimData.DataArchive_isClosed,args)
    def isWrite(*args): return apply(_cSimData.DataArchive_isWrite,args)
    def setDefault(*args): return apply(_cSimData.DataArchive_setDefault,args)
    def getObject(*args): return apply(_cSimData.DataArchive_getObject,args)
    def _getOffset(*args): return apply(_cSimData.DataArchive__getOffset,args)
    def _filePointer(*args): return apply(_cSimData.DataArchive__filePointer,args)
    def _lookupPath(*args): return apply(_cSimData.DataArchive__lookupPath,args)
    def __repr__(self):
        return "<C DataArchive instance at %s>" % (self.this,)

class DataArchivePtr(DataArchive):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DataArchive
_cSimData.DataArchive_swigregister(DataArchivePtr)

class InterfaceProxy(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, InterfaceProxy, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, InterfaceProxy, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_InterfaceProxy,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_InterfaceProxy):
        try:
            if self.thisown: destroy(self)
        except: pass
    def createObject(*args): return apply(_cSimData.InterfaceProxy_createObject,args)
    def set_enum(*args): return apply(_cSimData.InterfaceProxy_set_enum,args)
    def clear(*args): return apply(_cSimData.InterfaceProxy_clear,args)
    def variableExists(*args): return apply(_cSimData.InterfaceProxy_variableExists,args)
    def variableRequired(*args): return apply(_cSimData.InterfaceProxy_variableRequired,args)
    def pack(*args): return apply(_cSimData.InterfaceProxy_pack,args)
    def unpack(*args): return apply(_cSimData.InterfaceProxy_unpack,args)
    def getClassHash(*args): return apply(_cSimData.InterfaceProxy_getClassHash,args)
    def getClassName(*args): return apply(_cSimData.InterfaceProxy_getClassName,args)
    def getVariableNames(*args): return apply(_cSimData.InterfaceProxy_getVariableNames,args)
    def getRequiredNames(*args): return apply(_cSimData.InterfaceProxy_getRequiredNames,args)
    def set(*args): return apply(_cSimData.InterfaceProxy_set,args)
    def push_back(*args): return apply(_cSimData.InterfaceProxy_push_back,args)
    def __repr__(self):
        return "<C InterfaceProxy instance at %s>" % (self.this,)

class InterfaceProxyPtr(InterfaceProxy):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InterfaceProxy
_cSimData.InterfaceProxy_swigregister(InterfaceProxyPtr)

class InterfaceRegistry(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, InterfaceRegistry, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, InterfaceRegistry, name)
    def __init__(self,*args):
        self.this = apply(_cSimData.new_InterfaceRegistry,args)
        self.thisown = 1
    def __del__(self, destroy= _cSimData.delete_InterfaceRegistry):
        try:
            if self.thisown: destroy(self)
        except: pass
    def getInterface(*args): return apply(_cSimData.InterfaceRegistry_getInterface,args)
    def hasInterface(*args): return apply(_cSimData.InterfaceRegistry_hasInterface,args)
    def getInterfaceNames(*args): return apply(_cSimData.InterfaceRegistry_getInterfaceNames,args)
    def getInterfaces(*args): return apply(_cSimData.InterfaceRegistry_getInterfaces,args)
    def addInterface(*args): return apply(_cSimData.InterfaceRegistry_addInterface,args)
    def __repr__(self):
        return "<C InterfaceRegistry instance at %s>" % (self.this,)

class InterfaceRegistryPtr(InterfaceRegistry):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InterfaceRegistry
_cSimData.InterfaceRegistry_swigregister(InterfaceRegistryPtr)


