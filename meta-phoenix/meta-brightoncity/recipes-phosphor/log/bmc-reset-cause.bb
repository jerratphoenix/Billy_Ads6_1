SUMMARY = "BMC reset cause log"
DESCRIPTION = "Generate BMC reset cause log"

PR = "r1"
PV = "0.01"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

inherit systemd

SRC_URI = " file://bmc-reset-cause.sh \
            file://bmc-reset-cause.service \
          "

do_install() {
  install -d ${D}${sbindir}
  install -m 0755 ${WORKDIR}/bmc-reset-cause.sh ${D}${sbindir}
  install -d ${D}${systemd_unitdir}/system/
  install -m 0644 ${WORKDIR}/bmc-reset-cause.service ${D}${systemd_unitdir}/system
}

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} += "bmc-reset-cause.service"
