SUMMARY = "Phoenix IPMI PEF commands"
DESCRIPTION = "Phoenix IPMI PEF commands"

PR = "r1"

LICENSE = "CLOSED"

S = "${WORKDIR}"
SRC_URI = " file://libptecpefcmds.so.0.1.0 "

LIBRARY_NAMES = "libptecpefcmds.so"

HOSTIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"
NETIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"

FILES:${PN}:append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/net-ipmid/lib*${SOLIBS}"
FILES:${PN}-dev:append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV}"

do_install() {
    install -d ${D}${libdir}/ipmid-providers
    install -m 755 libptecpefcmds.so.0.1.0 ${D}${libdir}/ipmid-providers/
    
    # Create symlinks
    ln -sf libptecpefcmds.so.0.1.0 ${D}${libdir}/ipmid-providers/libptecpefcmds.so.0
    ln -sf libptecpefcmds.so.0 ${D}${libdir}/ipmid-providers/libptecpefcmds.so
}

INSANE_SKIP_${PN} += "installed-vs-shipped already-stripped"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

