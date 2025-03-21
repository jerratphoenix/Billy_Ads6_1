SUMMARY = "Phoenix IPMI SDR Repository commands"
DESCRIPTION = "Phoenix IPMI SDR Repository commands"

PR = "r1"

#LICENSE = "Apache-2.0"
#LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"
LICENSE = "CLOSED"

#SRC_URI = "git://git@github.com/pteceng/phoenix-ipmi-sdr.git;protocol=ssh"
#SRCREV = "9fd9212f8dc28e2b653bc3793581c1ad1591ba0d"
#
#S = "${WORKDIR}/git"
#PV = "0.1+git${SRCPV}"

S = "${WORKDIR}"
SRC_URI = " file://libptecsdrcmds.so.0.1.0 "

#DEPENDS = "boost phosphor-ipmi-host"
#inherit pkgconfig cmake obmc-phosphor-ipmiprovider-symlink

LIBRARY_NAMES = "libptecsdrcmds.so"

HOSTIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"
NETIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"

FILES:${PN}:append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/net-ipmid/lib*${SOLIBS}"
FILES:${PN}-dev:append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV}"

do_install() {
    echo "Starting do_install for phoenix-ipmi-sdr"

    install -d ${D}${libdir}/ipmid-providers
    install -m 755 libptecsdrcmds.so.0.1.0 ${D}${libdir}/ipmid-providers/

    # Create symlinks
    ln -sf libptecsdrcmds.so.0.1.0 ${D}${libdir}/ipmid-providers/libptecsdrcmds.so.0
    ln -sf libptecsdrcmds.so.0 ${D}${libdir}/ipmid-providers/libptecsdrcmds.so
}

INSANE_SKIP_${PN} += "installed-vs-shipped already-stripped"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"
