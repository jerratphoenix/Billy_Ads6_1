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

def find_target_tag(d, branch_tmp, machine_tmp):
    try:
        branch = d.getVar(branch_tmp, True)
        machine = d.getVar(machine_tmp, True)

        # search the correct tag name with branch name
        while True:
            tag_desc = run_git(d, 'describe --tags --abbrev=0')
            if tag_desc:
                if tag_desc in branch:
                    return True
                else:
                    # delete the local tag temporary to find the next tag
                    run_git(d, 'tag -d ' + tag_desc)
            else:
                break

        # sync remote tags
        run_git(d, 'fetch --tags')

        # if the HEAD is in other branches, use the machine name to choose the latest tag
        while True:
            tag_desc = run_git(d, 'describe --tags --abbrev=0')
            if tag_desc:
                if machine in tag_desc.lower():
                    return True
                else:
                    run_git(d, 'tag -d ' + tag_desc)
            else:
                break

        return False
    except Exception as e:
        bb.warn("Unexpected exception to find the target tag: %s" % e)
        pass

# DISTRO_VERSION can be overridden by a bbappend or config, so it must be a
# weak override.  But, when a variable is weakly overridden the definition
# and not the contents are used in the task-hash (for sstate reuse).  We need
# a strong variable in the vardeps chain for do_compile so that we get the
# contents of the 'git describe --dirty' call.  Create a strong/immediate
# indirection via PHOSPHOR_OS_RELEASE_DISTRO_VERSION.
BRANCHES := "${@run_git(d, 'branch')}"
TARGET_BRANCH = "${@''.join(d.getVar('BRANCHES').split('\n')[0])}"
FIND_TARGET_TAG := "${@find_target_tag(d, 'TARGET_BRANCH', 'MACHINE')}"

PHOSPHOR_OS_RELEASE_DISTRO_VERSION := "${@run_git(d, 'describe --tags --dirty')}"
DISTRO_VERSION ??= "${PHOSPHOR_OS_RELEASE_DISTRO_VERSION}"

VERSION_SPLIT := "${@strip_specific_str(d, 'VERSION_ID', '@')}"
VERSION = "0.23"
VERSION_ID = "0.23"

BUILD_ID_TAG := "${@run_git(d, 'describe --tags --abbrev=0')}"
BUILD_ID := "${@strip_specific_str(d, 'BUILD_ID_TAG', '@')}"
OPENBMC_TARGET_MACHINE = "${MACHINE}"

OS_RELEASE_FIELDS:append = " BUILD_ID OPENBMC_TARGET_MACHINE EXTENDED_VERSION"

# Ensure the git commands run every time bitbake is invoked.
BB_DONT_CACHE = "1"

# Make os-release available to other recipes.
SYSROOT_DIRS:append = " ${sysconfdir}"

# Restore to sync remote tags
SYNC_TAGS := "${@run_git(d, 'fetch --tags')}"
