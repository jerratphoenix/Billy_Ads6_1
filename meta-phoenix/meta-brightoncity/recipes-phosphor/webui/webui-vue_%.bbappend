FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " file://0001-Fix-the-path-of-Redfish.patch \
                 "
