import sys
sys.path = ['../Source'] + sys.path

sys.argv.append("--compile-data")
sys.argv.append("--rebuild")

import CSPSim

