SUMMARY = "vr-sensor-monitor service"
DESCRIPTION = "Monitoring the CPU VR sensors"


# Modify these as desired
LICENSE = "CLOSED"
LIC_FILES_CHKSUM=""
USERADD_PARAM_${PN} = "--groups messagebus"

SRC_URI += "file://vr-sensor-monitor"
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

SYSTEMD_SERVICE:${PN} = "xyz.openbmc_project.vr-sensor-monitor.service"

EXTRA_OEMESON = "--buildtype=minsize \
    -Dtests=disabled \
    -Dyocto-deps=enabled \
    -Dinstall-service=enabled \
    "

addtask do_copyfile after do_patch before do_configure
do_copyfile() {
    /usr/bin/cp ${WORKDIR}/vr-sensor-monitor/* ${WORKDIR}/git -rf
}
