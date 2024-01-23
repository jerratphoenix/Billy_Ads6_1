SUMMARY = "RGMII-delay"
DESCRIPTION = "RGMII-delay"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

inherit systemd

SYSTEMD_SERVICE:${PN} = "rgmii-delay.service"

S = "${WORKDIR}"
SRC_URI = "file://rgmii-delay.sh \
           file://rgmii-delay.service \
           "

RDEPENDS:${PN} = "bash"

do_install:append() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/rgmii-delay.sh ${D}/${bindir}/rgmii-delay.sh

    install -d ${D}${base_libdir}/systemd/system
    install -m 0644 ${S}/rgmii-delay.service ${D}${base_libdir}/systemd/system
}
