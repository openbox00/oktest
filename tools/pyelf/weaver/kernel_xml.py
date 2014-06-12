"""Collect data from the kernel element."""

from weaver import MergeError
from weaver import util
from weaver.ezxml import Element, long_attr, str_attr
from weaver.segments_xml import Segment_el, Patch_el, Heap_el

DEFAULT_KERNEL_HEAP_SIZE = 4 * 1024 * 1024
DEFAULT_KERNEL_MAX_THREADS = 1024

UseDevice_el = Element("use_device", name = (str_attr, "required"))

Option_el = Element("option",
                    key   = (str_attr, "required"),
                    value = (long_attr, "required"))

Config_el = Element("config", Option_el)

# Compound Elements
Kernel_el = Element("kernel", Segment_el, Patch_el, Heap_el,
                    Config_el, UseDevice_el,
                    file          = (str_attr, "optional"),
                    sdk           = (str_attr, "optional"),
                    configuration = (str_attr, "optional"),
                    platform      = (str_attr, "optional"),
                    linker        = (str_attr, "optional"),
                    kernel        = (str_attr, "optional"),
                    soc           = (str_attr, "optional"),
                    libs          = (str_attr, "optional"),
                    virtpool      = (str_attr, "required"),
                    physpool      = (str_attr, "required"),
                    seg_names     = (str_attr, "optional"))

Rootserver_el = Element("rootserver", Segment_el,
                        filename = (str_attr, "required"))

def get_symbol(elf, symbol, may_not_exist = False):
    """
    Return the address, and size in bytes, of a symbol from the ELF
    file.

    If may_not_exist is true then failing to find the symbol is not a
    fatal error.
    """
    try:
        return util.get_symbol(elf, symbol, may_not_exist)
    except MergeError:
        raise MergeError, \
              'Symbol "%s" not found in kernel ELF file.'  % \
              (symbol)
