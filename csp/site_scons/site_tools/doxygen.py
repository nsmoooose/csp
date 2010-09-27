from SCons.Script import *

NAME = 'Doxygen'

def exists(env):
    return env.Detect(NAME)

def generate(env):
    env['DOXYGEN'] = 'doxygen'
    env['DOXYGENCOM'] = 'cd $SOURCE.dir && $DOXYGEN $SOURCES.file 1>.dox.log 2>.dox.err'
    env.Append(BUILDERS = {'Doxygen': SCons.Builder.Builder(action=SCons.Action.Action('$DOXYGENCOM', '$DOXYGENCOMSTR'))})
