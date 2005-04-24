# py2exe_cspsim_setup.py
from distutils.core import setup
import py2exe, sys

sys.path.append('../../SimData/SimData')

opts = { 
    "py2exe": { 
        #"includes": "cSimData,cCSP",
        "excludes": "CSP.SimData.Compile"
        #"dist_dir": "bin",
    } 
}  
setup(options=opts,
      windows=["CSPSim.py"])

