# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.
import _CSPEdit
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


class View(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, View, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, View, name)
    def __init__(self,*args):
        self.this = apply(_CSPEdit.new_View,args)
        self.thisown = 1
    def __del__(self, destroy= _CSPEdit.delete_View):
        try:
            if self.thisown: destroy(self)
        except: pass
    def init(*args): return apply(_CSPEdit.View_init,args)
    def run(*args): return apply(_CSPEdit.View_run,args)
    def quit(*args): return apply(_CSPEdit.View_quit,args)
    def lock(*args): return apply(_CSPEdit.View_lock,args)
    def unlock(*args): return apply(_CSPEdit.View_unlock,args)
    def add(*args): return apply(_CSPEdit.View_add,args)
    def remove(*args): return apply(_CSPEdit.View_remove,args)
    def setCamera(*args): return apply(_CSPEdit.View_setCamera,args)
    def getRoot(*args): return apply(_CSPEdit.View_getRoot,args)
    def getViewer(*args): return apply(_CSPEdit.View_getViewer,args)
    def __repr__(self):
        return "<C View instance at %s>" % (self.this,)

class ViewPtr(View):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = View
_CSPEdit.View_swigregister(ViewPtr)

class FeatureNodeVisitor(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FeatureNodeVisitor, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FeatureNodeVisitor, name)
    def apply(*args): return apply(_CSPEdit.FeatureNodeVisitor_apply,args)
    def __init__(self,*args):
        self.this = apply(_CSPEdit.new_FeatureNodeVisitor,args)
        self.thisown = 1
    def __del__(self, destroy= _CSPEdit.delete_FeatureNodeVisitor):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C FeatureNodeVisitor instance at %s>" % (self.this,)

class FeatureNodeVisitorPtr(FeatureNodeVisitor):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FeatureNodeVisitor
_CSPEdit.FeatureNodeVisitor_swigregister(FeatureNodeVisitorPtr)

class ClearSelectionVisitor(FeatureNodeVisitor):
    __swig_setmethods__ = {}
    for _s in [FeatureNodeVisitor]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ClearSelectionVisitor, name, value)
    __swig_getmethods__ = {}
    for _s in [FeatureNodeVisitor]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ClearSelectionVisitor, name)
    def apply(*args): return apply(_CSPEdit.ClearSelectionVisitor_apply,args)
    def __init__(self,*args):
        self.this = apply(_CSPEdit.new_ClearSelectionVisitor,args)
        self.thisown = 1
    def __del__(self, destroy= _CSPEdit.delete_ClearSelectionVisitor):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ClearSelectionVisitor instance at %s>" % (self.this,)

class ClearSelectionVisitorPtr(ClearSelectionVisitor):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ClearSelectionVisitor
_CSPEdit.ClearSelectionVisitor_swigregister(ClearSelectionVisitorPtr)

class FeatureNode(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FeatureNode, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FeatureNode, name)
    def __del__(self, destroy= _CSPEdit.delete_FeatureNode):
        try:
            if self.thisown: destroy(self)
        except: pass
    def reparent(*args): return apply(_CSPEdit.FeatureNode_reparent,args)
    def getGraph(*args): return apply(_CSPEdit.FeatureNode_getGraph,args)
    def getName(*args): return apply(_CSPEdit.FeatureNode_getName,args)
    def accept(*args): return apply(_CSPEdit.FeatureNode_accept,args)
    def setName(*args): return apply(_CSPEdit.FeatureNode_setName,args)
    def move(*args): return apply(_CSPEdit.FeatureNode_move,args)
    def moveBy(*args): return apply(_CSPEdit.FeatureNode_moveBy,args)
    def rotate(*args): return apply(_CSPEdit.FeatureNode_rotate,args)
    def rotateBy(*args): return apply(_CSPEdit.FeatureNode_rotateBy,args)
    def reset(*args): return apply(_CSPEdit.FeatureNode_reset,args)
    def clearAllSelections(*args): return apply(_CSPEdit.FeatureNode_clearAllSelections,args)
    def isSelected(*args): return apply(_CSPEdit.FeatureNode_isSelected,args)
    def setSelected(*args): return apply(_CSPEdit.FeatureNode_setSelected,args)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<C FeatureNode instance at %s>" % (self.this,)

class FeatureNodePtr(FeatureNode):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FeatureNode
_CSPEdit.FeatureNode_swigregister(FeatureNodePtr)

class NodeReference(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, NodeReference, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, NodeReference, name)
    def __init__(self,*args):
        self.this = apply(_CSPEdit.new_NodeReference,args)
        self.thisown = 1
    def getNode(*args): return apply(_CSPEdit.NodeReference_getNode,args)
    def __del__(self, destroy= _CSPEdit.delete_NodeReference):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C NodeReference instance at %s>" % (self.this,)

class NodeReferencePtr(NodeReference):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NodeReference
_CSPEdit.NodeReference_swigregister(NodeReferencePtr)

class FeatureModel(FeatureNode):
    __swig_setmethods__ = {}
    for _s in [FeatureNode]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, FeatureModel, name, value)
    __swig_getmethods__ = {}
    for _s in [FeatureNode]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, FeatureModel, name)
    def load(*args): return apply(_CSPEdit.FeatureModel_load,args)
    def accept(*args): return apply(_CSPEdit.FeatureModel_accept,args)
    def __init__(self,*args):
        self.this = apply(_CSPEdit.new_FeatureModel,args)
        self.thisown = 1
    def __del__(self, destroy= _CSPEdit.delete_FeatureModel):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C FeatureModel instance at %s>" % (self.this,)

class FeatureModelPtr(FeatureModel):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FeatureModel
_CSPEdit.FeatureModel_swigregister(FeatureModelPtr)

class FeatureGroup(FeatureNode):
    __swig_setmethods__ = {}
    for _s in [FeatureNode]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, FeatureGroup, name, value)
    __swig_getmethods__ = {}
    for _s in [FeatureNode]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, FeatureGroup, name)
    def __init__(self,*args):
        self.this = apply(_CSPEdit.new_FeatureGroup,args)
        self.thisown = 1
    def __del__(self, destroy= _CSPEdit.delete_FeatureGroup):
        try:
            if self.thisown: destroy(self)
        except: pass
    def traverse(*args): return apply(_CSPEdit.FeatureGroup_traverse,args)
    def accept(*args): return apply(_CSPEdit.FeatureGroup_accept,args)
    def addChild(*args): return apply(_CSPEdit.FeatureGroup_addChild,args)
    def removeChild(*args): return apply(_CSPEdit.FeatureGroup_removeChild,args)
    def __repr__(self):
        return "<C FeatureGroup instance at %s>" % (self.this,)

class FeatureGroupPtr(FeatureGroup):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FeatureGroup
_CSPEdit.FeatureGroup_swigregister(FeatureGroupPtr)


