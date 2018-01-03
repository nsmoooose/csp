===================
 CSP documentation
===================

Introduction
============

This document tries to capture knowledge of the inner workings on the CSP
project and the tools required for it.

Tools
=====

tools/data-compiler
-------------------

Data archive compiler command line utility. This tool lets you compile your xml structures
into a archive file readable by the csp framework.

Status: Working and in use.

tools/map2hid
-------------

Converts interface definitions (.map) to a condensed format used by the simulation (.hid).
This is needed to create bindings between keyboard and joystick events to specific actions
inside of the game.

This is a standalone utility to do this.

sim.py is invoking the compiler directly if .hid files are missing.

Status: Working and in use.

tools/pdot
----------

Generates a visual call graph from gprof profile data.

Status: Unknown

tools/layout
------------

This is the first effort of a layout editor. Kept as a reference
for layout2 tool.

Status: Broken and doesn't compile.

tools/layout2
-------------

A newer effort to create a layout editor for all the xml files.
It uses wxPython for user interface.

Status: Broken

tools/pyrun
-----------

Status: Unknown

tools/runtests.py
-----------------

Status: Unknown

tools/trc
---------

Compiler for converting tagged record definitions (.net files) into C++ classes.
Used by the build system.

.net files are communication records between the csp server and clients.

Status: In use and working.

tools/recorder
--------------

Status: Unknown

tools/python_signals
--------------------

Status: Unknown

tools/googlemaps
----------------

A tool to download satelite images for the terrain in use. It
uses google maps as a datasource.

Status: Broken since google has changed URLs for imaging.

tools/fcs
---------

Tool to generate xml files for the flight control system.

Status: Unknown

tools/canopy
------------

Generates a reflection map from the canopy of the cockpit of
the F16.

Status: Unknown

tools/chunkdemo_prototype
-------------------------

Status: Unknown

tools/build
-----------

All build extensions for the SCons build system.

Status: Working and in use.
