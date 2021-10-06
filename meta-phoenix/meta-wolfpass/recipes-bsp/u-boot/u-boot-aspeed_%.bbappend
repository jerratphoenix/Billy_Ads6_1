FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append += "file://0001-Setup-boot-command-for-kernel-address.patch"
