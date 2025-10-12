FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " file://Billy-Baseboard.json \
             file://Billy-Chassis.json \
           "

do_install:append() {
    install -m 0444 ${WORKDIR}/Billy-Baseboard.json ${D}/usr/share/entity-manager/configurations
    install -m 0444 ${WORKDIR}/Billy-Chassis.json ${D}/usr/share/entity-manager/configurations
}
