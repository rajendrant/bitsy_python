import importlib
import sys

if len(sys.argv)==2:
    m = importlib.import_module(sys.argv[1])
    m.main()
