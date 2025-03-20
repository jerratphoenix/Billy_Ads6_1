SUMMARY = "Phoenix Technologies IPMI OEM Commands"
#LICENSE = "Apache-2.0"
#LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"
LICENSE = "CLOSED"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

#S = "${WORKDIR}/git"
#
#SRC_URI = "git://git@github.com/pteceng/phoenix-ipmi-oem-cmd.git;protocol=ssh;"
#SRCREV = "2efa27b3cee8a1d48210d1d26c2deb1e14e1f1c8"
S = "${WORKDIR}"
SRC_URI = " file://libphoenixoem.so.0.0.0 "

#inherit autotools pkgconfig
#inherit obmc-phosphor-ipmiprovider-symlink
#
#DEPENDS += "phosphor-ipmi-host"
#DEPENDS += "autoconf-archive-native"

FILES:${PN}:append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES:${PN}-dev:append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV} ${libdir}/ipmid-providers/*.la"

HOSTIPMI_PROVIDER_LIBRARY += "libphoenixoem.so"

# These values must match P2A config in kernel DTS/DTB
EXTRA_OECONF:append = " IMG_XFER_MEM_WINDOW_ADDRESS=0x9efe0000"
EXTRA_OECONF:append = " IMG_XFER_MEM_WINODW_LENGTH=0x10000"

do_install() {
    echo "Starting do_install for phoenix-ipmi-oem"

    install -d ${D}${libdir}/ipmid-providers
    install -m 755 libphoenixoem.so.0.0.0 ${D}${libdir}/ipmid-providers/

    # Create symlinks
    ln -sf libphoenixoem.so.0.0.0 ${D}${libdir}/ipmid-providers/libphoenixoem.so.0
    ln -sf libphoenixoem.so.0.0.0 ${D}${libdir}/ipmid-providers/libphoenixoem.so
}

INSANE_SKIP_${PN} += "installed-vs-shipped already-stripped"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"
