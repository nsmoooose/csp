from SCons.Script import *

NAME = 'Net'

def EmitNet(target, source, env):
    assert(len(source)==1)
    source = source[0]
    name = os.path.splitext(source.name)[0]
    dir = source.srcnode().dir
    target = [
        source.target_from_source('', env['CXXFILESUFFIX']),
        dir.File(name + '.h')
	]
    return (target, source)

def exists(env):
    return env.Detect(NAME)

def generate(env):
    _, cxx_file = SCons.Tool.createCFileBuilders(env)
    cxx_file.add_emitter('.net', EmitNet)
    cxx_file.add_action('.net', SCons.Action.Action('$TRC', '$TRCCOMSTR'))
    trc = env.File('#/tools/trc/trc.py')
    env['TRC'] = '%s --source=${TARGETS[0]} --header=${TARGETS[1]} $SOURCES' % trc
