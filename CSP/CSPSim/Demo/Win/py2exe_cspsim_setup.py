# py2exe_cspsim_setup.py
from distutils.core import setup
import py2exe, sys, os.path

sys.path.append('../../../SimData/SimData')
sys.path.append('../../Bin')

opts = { 
    "py2exe": { 
        #"includes": "cSimData,cCSP",
        "excludes": "CSP.SimData.Compile",
        "dist_dir": "Bin"
##data_files=[("bitmaps",
##                   ["bm/large.gif", "bm/small.gif"]),
##                  ("fonts",
##                   glob.glob("fonts\\*.fnt"))],

# This would create a subdirectory dist\bitmaps, containing the
# two .gif files, and a subdirectory dist\fonts, containing all the .fnt files.        
    } 
}  
setup(options=opts,
      windows=["CSPSim.py"],
      data_files=[(os.path.join('..','Data'),os.path.join('..','..','Data','*.*'))]
      )

