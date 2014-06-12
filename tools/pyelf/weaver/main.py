"""
The ElfWeaver script takes multiple files and merges
them into one elf file.
"""
import sys
from weaver.merge import merge_cmd
from weaver.display import print_cmd
from weaver.modify import modify_cmd
from weaver.memstats import memstats_cmd
from weaver.link import link_cmd
from weaver.hacky_link import hacky_link_cmd
import weaver.cells
import weaver.extensions

import weaver.cells.iguana_cell
import weaver.cells.okl4_cell
import weaver.cells.linux_cell

def print_basic_usage():
    """Print the basic usage message"""
    print "Elfweaver -- a tool for manipulating ELF files."
    print
    print "Basic commands:"
    print
    print "   elfweaver print           Print display an ELF."
    print "   elfweaver merge           Merge a set of files into one ELF."
    print "   elfweaver modify          Modify attributes of an ELF."
    print "   elfweaver memstats        Print memory statistics of an ELF."
    print "   elfweaver link            Link the kernel to SoC."
    print
    print "   elfweaver <cmd> -H        Obtain help on a specific command."

__commands__ = {
    "print" : print_cmd,
    "merge" : merge_cmd,
    "modify" : modify_cmd,
    "memstats" : memstats_cmd,
    "link" : link_cmd,
    "hacky_link" : hacky_link_cmd
    }

def main(args):
    """Main program entry point."""
    if len(args) < 2:
        print_basic_usage()
    elif args[1] not in __commands__:
        print >> sys.stderr, "%s: Unknown command %s" % \
              (args[0], args[1])
        sys.exit(3)
    else:
        __commands__[args[1]](args[2:])
