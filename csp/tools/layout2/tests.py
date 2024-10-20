#!/usr/bin/python
import os
import csp.csplib
import csp.cspsim
import unittest
from .scripts.tests import *

def loadModules():
    modules = ('chunklod', 'demeter')
    extension = {
        'posix' : '.so',
        'nt' : '.dll',
    }.get(os.name, '')

    prefix = {
        'posix' : 'lib',
        'nt' : '',
    }.get(os.name, '')

    for module in modules:
        module_path = os.path.join('..', '..', 'modules', module, '.bin', prefix + module) + extension
        # for windows demos, the modules are instead placed in the current directory.
        # TODO move them to ../modules/*.dll?
        if not os.path.exists(module_path):
            module_path = os.path.join(module) + extension
        if not csp.csplib.ModuleLoader.load(module_path):
            print(('Unable to load required extension module "%s"' % module))

if __name__ == "__main__":
    loadModules()
    csp.cspsim.registerAllObjectInterfaces()
    unittest.main()
