
# Disable the shell to remove the usage of readline lib
# to fix the license conflict issue. More information:
# https://lists.ozlabs.org/pipermail/openbmc/2019-November/019678.html
SRCREV = "4d25a93f4922885c3d557bf4f9a83968ad2edee9"
EXTRA_OECONF:append = " --disable-ipmishell"
