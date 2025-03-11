SUMMARY = "Phoenix DBus Monitor"
DESCRIPTION = "Phoenix DBus Monitor is a general purpose DBus application \
that watches DBus traffic for events and takes actions based on those events."
PR = "r1.1"
LICENSE = "CLOSED"

SRC_URI = "file://phoenix-dbus-monitor.service \
          "

S = "${WORKDIR}"

FILESEXTRAPATHS:append := "${THISDIR}/${PN}:"
SYSTEMD_SERVICE:${PN} += "phoenix-dbus-monitor.service"

do_install:append() {
    install -Dm 0666 ${S}/PtecPEFConfig.json ${D}/${datadir}/${PN}/PtecPEFConfig.json
    install -Dm 0666 ${S}/PtecEmailConfig.json ${D}/${datadir}/${PN}/PtecEmailConfig.json
}
