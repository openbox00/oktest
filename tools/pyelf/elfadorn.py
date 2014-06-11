#!/usr/bin/env python

import sys
from traceback import print_exc

if "--traceback" in sys.argv:
    traceback = sys.argv.index("--traceback")
    del sys.argv[traceback]
else:
    traceback = 0

if sys.version_info[:3] == (2, 5, 0):
    print "Python version 2.5.0 is not supported due to a bug in the struct" \
          "module. Please upgrade to 2.5.1 or greater"
    sys.exit(1)

try:
    from adorn.main import main
    main(sys.argv)
except KeyboardInterrupt:
    pass
except SystemExit, exit_code:
    sys.exit(exit_code)
except:
    print "Elfadorn error:", sys.exc_info()[1]
    if traceback or True:
        print "Now printing a traceback."
        print
        print_exc(file=sys.stdout)
        print
    sys.exit(1)
