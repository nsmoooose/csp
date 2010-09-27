from SCons.Script import *

NAME = 'VisualStudio'

def exists(env):
    return env.Detect(NAME)

def generate(env):
    def VisualStudioProject(env, target, source):
        if util.IsWindows(env) and hasattr(env, 'MSVSProject'):
            if isinstance(target, list): target = target[0]
            project_base = os.path.splitext(os.path.basename(target.name))[0]
            if project_base.startswith('_'): project_base = project_base[1:]
            dbg = env.MSVSProject(target=project_base+'-dbg.vcproj', srcs=source.sources, incs=source.headers, misc=source.misc, buildtarget=target, variant='Debug')
            rel = env.MSVSProject(target=project_base+'-rel.vcproj', srcs=source.sources, incs=source.headers, misc=source.misc, buildtarget=target, variant='Release')
            return [dbg, rel]
        else:
            return []
    env.Append(BUILDERS = {'VisualStudioProject': VisualStudioProject})
