# Combat Simulator Project
# Copyright (C) 2002-2005 Mark Rose <mkrose@users.sf.net>
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
import sys
import os
import os.path
import time
from csp import csplib
from csp.base.signals import Signal
from csp.tools.data.parse import ObjectXMLArchive, XMLSyntax


class CompilerEvent:
    """This is the base event class for a compiler message.
    This object is emitted with a compiler signal to
    notify listeners about informational, warnings or error
    messages."""

    def __init__(self, type, message):
        self.type = type
        self.message = message

    def GetType(self):
        """The type of message corresponds to the constants
        defined in the compiler class. Information, Warning, Error."""
        return self.type

    def GetMessage(self):
        """The actual text message."""
        return self.message


class Compiler:
    """Class that is responsible for creating an archive file that is
    faster to load than an xml file."""

    Information = 1
    Warning = 2
    Error = 3

    def __init__(self):
        # A counter for the number of warnings that has occured.
        self.warnings = 0
        self.errors = 0

        # Define default signal functions.
        self.compilerSignal = Signal()
        self.compilerSignal.Connect(self.on_Signal)

    def on_Signal(self, event):
        if event.GetType() == Compiler.Warning:
            self.warnings += 1
        elif event.GetType() == Compiler.Error:
            self.errors += 1

    def GetCompilerSignal(self):
        """Returns a signal object that can be used to be notified when
        a compiler event occurs. Compiler events are information, warnings
        or errors."""
        return self.compilerSignal

    def CompileAll(self, sourceDirectory, archiveFile):
        """Compiles all files in the directory specified. All files will
        be placed in a single archive file."""

        self.compilerSignal.Emit(CompilerEvent(Compiler.Information, "Compiling '%s' from '%s'" % (archiveFile, sourceDirectory)))
        self.compilerSignal.Emit(CompilerEvent(Compiler.Information, "Opening input archive"))
        prefix = os.path.splitext(os.path.basename(archiveFile))[0]

        # Construct an xml archive and load every object found in it.
        xmlDirectoryArchive = ObjectXMLArchive(prefix, sourceDirectory)
        self.compilerSignal.Emit(CompilerEvent(Compiler.Information, "Loading all objects"))
        try:
            allObjects = xmlDirectoryArchive.loadAll()
        except Exception as e:
            self.compilerSignal.Emit(CompilerEvent(Compiler.Error, str(e)))
            return False
        self.compilerSignal.Emit(CompilerEvent(Compiler.Information, "XML parse completed."))

        # Verify that all paths are correct within the file.
        paths = xmlDirectoryArchive.getPaths()
        self.dumpBadPaths(allObjects, paths)

        # If there is any errors at this point we do not continue. We simply
        # return without no archive file beeing created.
        if self.errors > 0:
            return False

        # Now it is time to create the archive file and write every object
        # into it.
        self.compilerSignal.Emit(CompilerEvent(Compiler.Information, "Compiling all objects to archive file"))
        compiled = csplib.DataArchive(archiveFile, False)
        hash_string = csplib.hash_string
        for id in allObjects.keys():
            object = allObjects[id]
            self.compilerSignal.Emit(CompilerEvent(Compiler.Information, "Compiling object '%s' [%s] %s" % (id, hash_string(id), str(object))))
            compiled.addObject(object, id)
        self.compilerSignal.Emit(CompilerEvent(Compiler.Information, "Finished."))
        return True

    def dumpBadPaths(self, all, paths):
        badpaths = filter(lambda x, a=all: x not in a, paths.keys())
        if len(badpaths) > 0:
            self.compilerSignal.Emit(CompilerEvent(Compiler.Error, "Found %d broken path(s):" % len(badpaths)))
            idx = 0
            for path in badpaths:
                idx = idx + 1
                self.compilerSignal.Emit(CompilerEvent(Compiler.Error, "%03d: Path '%s'" % (idx, str(path))))
                objects = {}
                for obj in paths[path]:
                    objects[obj] = 1
                self.compilerSignal.Emit(CompilerEvent(Compiler.Error, "   : defined in '" + "'\n    defined in '".join(objects.keys()) + "'"))
