SUMMARY = "set static ip service"
DESCRIPTION = "Script for setting static IP after bootup"

FILESEXTRAPATHS:prepend := "${THISDIR}:"

LICENSE = "CLOSED"

inherit systemd
SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE:${PN} = "set-static-ip.service"

SRC_URI += " \
        file://set-static-ip.sh \
        file://set-static-ip.service \
        "
RDEPENDS:${PN} += "bash"
S="${WORKDIR}"


do_install() {
  install -d ${D}/${sbindir}/set-static-ip
  install -m 0755 ${S}/set-static-ip.sh ${D}/${sbindir}/set-static-ip

  install -d ${D}${systemd_unitdir}/system
  install -c -m 0644 ${WORKDIR}/set-static-ip.service ${D}/${systemd_unitdir}/system
}

FILES:${PN} += "${systemd_unitdir}/system/set-static-ip.service"

