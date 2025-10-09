SUMMARY = "host-state-monitor service"
DESCRIPTION = "Monitoring the Host state"


# Modify these as desired
LICENSE = "CLOSED"
LIC_FILES_CHKSUM=""
USERADD_PARAM_${PN} = "--groups messagebus"

SRC_URI += "file://host-state-monitor \
            file://ParserLog.sh \
    "
S = "${WORKDIR}/git"



FILES:${PN}= "/lib /lib/systemd/ /usr /usr/bin"
SYSTEMD_PACKAGES = "${PN}"
inherit meson pkgconfig systemd externalsrc

DEPENDS = "systemd \
    boost \
    sdbusplus \
    i2c-tools \
    nlohmann-json \
    "

SYSTEMD_SERVICE:${PN} = "xyz.openbmc_project.host-state-monitor.service"

EXTRA_OEMESON = "--buildtype=minsize \
    -Dtests=disabled \
    -Dyocto-deps=enabled \
    -Dinstall-service=enabled \
    "

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/host-state-monitor ${D}${bindir}/host-state-monitor
    
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/ParserLog.sh ${D}${bindir}/ParserLog.sh

    install -d ${D}${base_libdir}/systemd/system
    install -m 0644 ${S}/xyz.openbmc_project.host-state-monitor.service ${D}${base_libdir}/systemd/system
}

addtask do_copyfile after do_patch before do_configure
do_copyfile() {
    /usr/bin/cp ${WORKDIR}/host-state-monitor/* ${WORKDIR}/git -rf
}
