from machines import Machine, Region
from simulators import skyeye_sim
from toolchains import gnu_arm_toolchain

############################################################################
# arm based machines
############################################################################
class arm(Machine):
    page_sizes = [0x1000, 0x10000, 0x100000]
    preferred_alignment = 0x100000L
    wordsize = 32
    arch = "arm"
    endian = "little"
    run_methods = {
        'skyeye': skyeye_sim
    }
    default_method = 'skyeye'
    default_toolchain = gnu_arm_toolchain
    rvct_c_flags = []
    ads_c_flags = []
    arch_version_no_kidding = None

# ARM CPU Classes
class armv5(arm):
    memory = arm.memory.copy()
    base_vaddr = 0x80000000
    memory['virtual'] = [Region(0x80000000, 0xd0000000)] # Trap NULL pointer derefs.
    cpp_defines = [("ARM_PID_RELOC", 1), ("ARM_SHARED_DOMAINS", 1)]
    pid_reloc = True
    shared_domains = True

class armv5tej(armv5):
    c_flags = armv5.c_flags + ["-march=armv5te"]
    cpp_defines = armv5.cpp_defines + [("__ARMv__", 5), "__ARMv5TEJ__"]
    arch_version = 5

class arm926ejs(armv5tej):
    # This breaks EABI builds because gcc 4.2.4 doesn't like this flag
    #c_flags = armv5tej.c_flags + ["-mtune=arm926ejs"]
    rvct_c_flags = armv5tej.rvct_c_flags + ["--cpu", "arm926ej-s"]
    ads_c_flags = armv5tej.ads_c_flags + ["-cpu", "arm926ej-s"]
    ads_as_flags = armv5tej.ads_c_flags + ["-cpu", "arm926ej-s"]
    cpu = "arm926ejs"

