#!/usr/bin/env python3

import os
import csp
import csp.csplib
import csp.cspsim

from csp.tools.layout2 import layout_module
from csp.tools.layout2.scripts.ui.LayoutApplication import LayoutApplication

def loadModules():
    modules = ('chunklod', 'demeter')
    extension = {
        'posix': '.so',
        'nt': '.dll',
    }.get(os.name, '')

    prefix = {
        'posix': 'lib',
        'nt': '',
    }.get(os.name, '')

    for module in modules:
        module_path = os.path.join(csp.dir, 'modules', module, '.bin', prefix + module) + extension

        # for windows demos, the modules are instead placed in the current directory.
        # TODO move them to ../modules/*.dll?
        if not os.path.exists(module_path):
            module_path = prefix + os.path.join(module) + extension
        if not csp.csplib.ModuleLoader.load(module_path):
            print('Unable to load required extension module "%s"' % module)

loadModules()

# Make all objects available for introspection and serialization
csp.cspsim.registerAllObjectInterfaces()

# Create the application object and run the main loop.
app = LayoutApplication(0)
app.MainLoop()
