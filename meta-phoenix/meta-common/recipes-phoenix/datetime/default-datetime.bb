SUMMARY = "One time process the default date and time"
DESCRIPTION = "One time process the default date and time."

S = "${WORKDIR}"
SRC_URI = "file://default-datetime.sh \
           file://default-datetime.service \
          "

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

inherit systemd

RDEPENDS:${PN} += "bash"
FILES:${PN} += "${systemd_system_unitdir}/default-datetime.service"
SYSTEMD_SERVICE:${PN} += " default-datetime.service"

do_install() {
    # install service
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/default-datetime.service ${D}${systemd_system_unitdir}

    install -d ${D}${bindir}
    install -m 0755 ${S}/default-datetime.sh ${D}/${bindir}/default-datetime.sh
}
