SUMMARY = "Local startup service"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "file://local-startup.service"
S = "${WORKDIR}"

inherit systemd

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "local-startup.service"

do_install() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/local-startup.service ${D}${systemd_system_unitdir}/local-startup.service
    install -d ${D}${localstatedir}/startup
}

FILES:${PN} += "${systemd_system_unitdir}/local-startup.service"
FILES:${PN} += "${localstatedir}/startup"