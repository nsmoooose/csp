FracBuild 1.0 - Fractal Heightmap Building Tools for CSP

usage: 

Mode 1: Iteration

Syntax:
FracBuild SOURCE_FILENAME.raw DEST_FILENAME.raw -iterate RANDOMFACTOR

Creates one iteration step. SOURCE_FILENAME specifies a heightmap in 16bit RAW format. "randomfactor" is a float value from 0.1 to 1.0 which controls the amount of displacement. start with small numbers, and continue with higher ones in higher levels. The iteration builds a new file which has four times the size of the source file.

Mode 2: Smoothing

Syntax:
FracBuild SOURCE_FILENAME DEST_FILENAME -gauss

Performs a gaussian filter on the whole terrain map, which gives it a smoother appearance.

Options:

-24

Generates a 24bit RAW output file, which can be read by the Demeter Terrain Engine and thus by the CSP Demo Flight Sim. the "-24" argument can be used in both modes.

See the "build.bat" for an exapmple of building a detailed heighemap from a low-detail source file.
