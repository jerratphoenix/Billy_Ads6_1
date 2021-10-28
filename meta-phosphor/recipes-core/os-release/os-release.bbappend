# WARNING!
#
# These modifications to os-release disable the bitbake parse
# cache (for the os-release recipe only).  Before copying
# and pasting into another recipe ensure it is understood
# what that means!

OS_RELEASE_ROOTPATH ?= "${COREBASE}"

def run_git(d, cmd):
    try:
        oeroot = d.getVar('OS_RELEASE_ROOTPATH', True)
        return bb.process.run(("export PSEUDO_DISABLED=1; " +
                               "git --work-tree %s --git-dir %s/.git %s")
            % (oeroot, oeroot, cmd))[0].strip('\n')
    except Exception as e:
        bb.warn("Unexpected exception from 'git' call: %s" % e)
        pass

def strip_specific_str(d, target, key):
    try:
        tag = d.getVar(target, True)
        split_str = tag.split(key)  # strip the specific tag string
        if split_str:
            if len(split_str) == 1:
                # for committee and master branches
                return split_str[0]
            else:
                # for CRB and customization branches
                return split_str[1]
        return None
    except Exception as e:
        bb.warn("Unexpected exception to strip specific string: %s" % e)
        pass

# DISTRO_VERSION can be overridden by a bbappend or config, so it must be a
# weak override.  But, when a variable is weakly overridden the definition
# and not the contents are used in the task-hash (for sstate reuse).  We need
# a strong variable in the vardeps chain for do_compile so that we get the
# contents of the 'git describe --dirty' call.  Create a strong/immediate
# indirection via PHOSPHOR_OS_RELEASE_DISTRO_VERSION.
PHOSPHOR_OS_RELEASE_DISTRO_VERSION := "${@run_git(d, 'describe --dirty')}"
DISTRO_VERSION ??= "${PHOSPHOR_OS_RELEASE_DISTRO_VERSION}"

VERSION_SPLIT := "${@strip_specific_str(d, 'VERSION_ID', '@')}"
VERSION = "${@'-'.join(d.getVar('VERSION_SPLIT').split('-')[0:2])}"
VERSION_ID = "${VERSION}"

BUILD_ID_TAG := "${@run_git(d, 'describe --abbrev=0')}"
BUILD_ID := "${@strip_specific_str(d, 'BUILD_ID_TAG', '@')}"
OPENBMC_TARGET_MACHINE = "${MACHINE}"

OS_RELEASE_FIELDS:append = " BUILD_ID OPENBMC_TARGET_MACHINE EXTENDED_VERSION"

# Ensure the git commands run every time bitbake is invoked.
BB_DONT_CACHE = "1"

# Make os-release available to other recipes.
SYSROOT_DIRS:append = " ${sysconfdir}"
