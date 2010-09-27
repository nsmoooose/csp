from SCons.Script import *

NAME = 'DataArchive'

def exists(env):
    return env.Detect(NAME)

def generate(env):
    bld = Builder(action = 'python tools/data-compiler --source-dir $DARXMLDIR --data-archive $TARGET --module $DARMODULE',
                            suffix = '.dar',
                  src_suffix = '.xml')
    env.Append(BUILDERS = { NAME : bld })
