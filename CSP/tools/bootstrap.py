import sys
import os.path

if not getattr(sys, 'CSP', 0):

  dn = os.path.dirname
  root = dn(dn(dn(__file__)))
  sys.path.insert(0, root)

  try:
    import CSP
  except:
    print 'Unable to import the main CSP module.  Check that you have'
    print 'a complete working copy.'
    sys.exit(1)

  sys.CSP = root
