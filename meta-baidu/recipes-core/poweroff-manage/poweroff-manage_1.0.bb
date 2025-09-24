SUMMARY = "BMC Poweroff Manage service"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://log.h \
           file://srcp_msg.h \
           file://poweroff-manage.c \
           file://Makefile \
           file://poweroff-manage.service \
           file://poweroff-manage.conf"

S = "${WORKDIR}"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 poweroff-manage ${D}${bindir}

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/poweroff-manage.service ${D}${systemd_system_unitdir}

    install -d ${D}${sysconfdir}/logrotate.d
    install -m 0644 ${WORKDIR}/poweroff-manage.conf ${D}${sysconfdir}/logrotate.d
}

inherit systemd
SYSTEMD_SERVICE:${PN} = "poweroff-manage.service"
FILES:${PN} += "${systemd_system_unitdir}"
