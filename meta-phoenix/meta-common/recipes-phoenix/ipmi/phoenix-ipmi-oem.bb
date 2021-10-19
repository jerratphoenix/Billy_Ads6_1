SUMMARY = "Phoenix Technologies IPMI OEM Commands"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

S = "${WORKDIR}/git"

SRC_URI = "git://git@github.com/pteceng/ServerBMC-Gold-phoenix-ipmi-oem.git;protocol=ssh;branch=main"
SRCREV = "f8217180fd2e2f4f771a2d76329d3d631078ef4a"

inherit autotools pkgconfig
inherit obmc-phosphor-ipmiprovider-symlink

DEPENDS += "phosphor-ipmi-host"
DEPENDS += "autoconf-archive-native"

FILES:${PN}:append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES:${PN}-dev:append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV} ${libdir}/ipmid-providers/*.la"

HOSTIPMI_PROVIDER_LIBRARY += "libphoenixoem.so"

# These values must match P2A config in kernel DTS/DTB
EXTRA_OECONF:append = " IMG_XFER_MEM_WINDOW_ADDRESS=0x9efe0000"
EXTRA_OECONF:append = " IMG_XFER_MEM_WINODW_LENGTH=0x10000"

