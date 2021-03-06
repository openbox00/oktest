"""Implements a command for displaying an ELF file in a format very
similar to readelf. It also is able to print out l4 data structures
such as the Kip and Bootinfo."""

from optparse import OptionParser
from elf.core import PreparedElfFile
from weaver.kernel_micro_elf import find_kernel_config, find_init_script, \
        find_elfweaver_info
from weaver.kernel_nano_elf import find_nano_heap
from weaver.kernel_nano import NANO_KERNEL_API_VERSION
from weaver.kernel import check_api_versions
from weaver.cells.iguana.bootinfo_elf import find_bootinfo
import sys

def print_pheaders(elf, summary=True, fout=sys.stdout):
    """Print out the program headers of a given elf file. If summary is
    true, then provide a summary of the Elf file."""
    segments = elf.segments
    if summary:
        if not elf.has_segments():
            print >> fout
            print >> fout,  "There are no program headers in this file."
        else:
            print >> fout
            print >> fout,  "Elf file type is %s" % elf.elf_type
            print >> fout,  "Entry point 0x%x" % elf.entry_point
            print >> fout,  "There are %d program headers, starting at offset %s" % \
                  (len(segments), elf._ph_offset)
    if not elf.has_segments():
        return
    print >> fout
    print >> fout,  "Program Headers:"
    if elf.wordsize == 64:
        print >> fout,  "  Type           Offset             VirtAddr           PhysAddr\n" +  \
        "                 FileSiz            MemSiz              Flags  Align"
    else:
        print >> fout,  "  Type           Offset   VirtAddr   PhysAddr   " \
              "FileSiz MemSiz  Flg Align"

    for segment in segments:
        segment.get_program_header(elf.endianess, elf.wordsize).output(fout)
    if elf.sections:
        print >> fout
        print >> fout,  " Section to Segment mapping:"
        print >> fout,  "  Segment Sections..."
        for i, segment in enumerate(segments):
            print >> fout,  "   %02d    " % i,
            if segment.has_sections():
                for sect in segment.sections:
                    if sect.get_size():
                        print >> fout,  sect.name,
            print >> fout,  ""


def print_sheaders(elf, summary=True, fout=sys.stdout):
    """Print out the section headers of a given elf file. If summary is
    true, then provide a summary of the Elf file."""
    sections = elf.sections
    if summary:
        if sections is None or sections == []:
            print >> fout
            print >> fout, "There are no sections in this file."
        else:
            print >> fout, "There are %d section headers, starting at offset 0x%x:" % \
                  (len(sections), elf._sh_offset)
    if sections is None or sections == []:
        return
    hdrs = elf.get_section_headers()
    print >> fout
    print >> fout, "Section Headers:"
    if elf.wordsize == 64:
        print >> fout, "  [Nr] Name              Type             Address           Offset\n" \
                      "       Size              EntSize          Flags  Link  Info  Align"
    else:
        print >> fout, "  [Nr] Name              Type            Addr" \
              "     Off    Size   ES Flg Lk Inf Al"
    for sect in hdrs:
        sect.output(fout)

    print >> fout, "Key to Flags:"
    print >> fout, "  W (write), A (alloc), X (execute), M (merge), S (strings)"
    print >> fout, "  I (info), L (link order), G (group), x (unknown)"
    print >> fout, "  O (extra OS processing required) o (OS specific), "\
          "p (processor specific)"

def print_symbol_table(elf, fout=sys.stdout, wide=False):
    print >> fout, "   Num:    Value  Size Type    Bind   Vis      Ndx Name"

    for (idx, sym) in enumerate(elf.get_symbol_table()):
        sym.output(idx, fout, wide)

def print_relocations(elf, fout=sys.stdout, wide=False):
    found = False
    for sec in elf.sections:
        if hasattr(sec, "relocs"):
            for reloc in sec.relocs:
                reloc.output(fout, wide)
                found = True
    if not found:
        print >> fout
        print >> fout, "There are no relocations in this file."

def print_cmd(args):
    """Display an ELF file. args contains command line arguments passed
    from sys.argv. These are parsed using option parser."""

    parser = OptionParser("%prog print [options] file", add_help_option=0)
    parser.add_option("-H", "--help", action="help")
    parser.add_option("-a", "--all", action="store_true", dest="all",
                      help="Print all information")
    parser.add_option("-h", "--header", action="store_true", dest="header",
                      help="Print ELF header")
    parser.add_option("-l", "--pheaders", action="store_true", dest="pheaders",
                      help="Print ELF sections headers")
    parser.add_option("-S", "--sheaders", action="store_true", dest="sheaders",
                      help="Print ELF sections headers")
    parser.add_option("-k", "--kconfig", action="store_true", dest="kconfig",
                      help="Print L4 kernel config data structure (Default)")
    parser.add_option("-B", "--bootinfo", action="store_true", dest="bootinfo",
                      help="Print L4 Bootinfo")
    parser.add_option("-m", "--segnames", action="store_true", dest="segnames",
                      help="Print segment names")
    parser.add_option("-s", "--syms", action="store_true", dest="symbols",
                      help="Print the symbol table")
    parser.add_option("-r", "--relocs", action="store_true", dest="relocs",
                      help="Display the relocations (if present)")
    parser.add_option("-W", "--wide", action="store_true",
                      dest="wide", default=False,
                      help="Allow output width to exceed 80 characters")
    parser.add_option("-e", "--elfweaver-info", action="store_true",
                      dest="elfweaverinfo", help="Print elfweaver info section")

    (options, args) = parser.parse_args(args)

    if len(args) != 1:
        parser.error("incorrect number of arguments")

    if options.all:
        options.header = options.pheaders = options.sheaders = \
                             options.kconfig = options.bootinfo = \
                             options.elfweaverinfo = options.segnames = True

    elf = PreparedElfFile(filename=args[0])

    if options.header:
        elf.get_elf_header().output(sys.stdout)

    if options.sheaders:
        print_sheaders(elf, not options.header, sys.stdout)

    if options.pheaders:
        print_pheaders(elf, not options.header, sys.stdout)

    if options.symbols:
        print_symbol_table(elf, sys.stdout, options.wide)

    if options.relocs:
        print_relocations(elf, sys.stdout, options.wide)

    if options.kconfig:
        kern_ver = check_api_versions(elf)
        if kern_ver == NANO_KERNEL_API_VERSION:
            kconfig = find_nano_heap(elf)
        else: # kern_ver == MICRO_KERNEL_API_VERSION:
            kconfig = find_kernel_config(elf)

        print
        if kconfig:
            kconfig.output(sys.stdout)
        else:
            print "There is no kernel configuration in this file."

        if kern_ver != NANO_KERNEL_API_VERSION:
            print
            initscript = find_init_script(elf)
            if initscript:
                initscript.output(sys.stdout)
            else:
                print "There is no initialisation script in this file."

    if options.elfweaverinfo:
        print
        elfweaverinfo = find_elfweaver_info(elf)
        if elfweaverinfo:
            elfweaverinfo.output(sys.stdout)
        else:
            print "There is no elfweaver info section in this file."

    if options.bootinfo:
        print
        bootinfo = find_bootinfo(elf)
        if bootinfo:
            bootinfo.output(sys.stdout)
        else:
            print "There is no Bootinfo section in this file."

    if options.segnames:
        segcount = 0
        print
        segnames = get_segnames(elf)

        if segnames:
            print "Segment    Name"

            for segname in segnames.strings[1:]:
                idx = segname.index('\x00')
                segname = segname[:idx]
                segname = segname.strip()

                if segname != "":
                    print "  %02d       %s" % (segcount, segname)

                segcount += 1
        else:
            # TODO - use the first section in each segment
            # as the segment name and print that.
            print "There is no .segnames section in this file"

    return 0


def get_segnames(elf):
    """Find the segnames data structructure in an ELF. Return none if it
    can't be found."""
    return elf.find_section_named(".segment_names")
