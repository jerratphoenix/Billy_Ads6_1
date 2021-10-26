FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " file://AC-Baseboard.json \
             file://AC-Chassis.json \
           "

do_install:append() {
    install -m 0444 ${WORKDIR}/AC-Baseboard.json ${D}/usr/share/entity-manager/configurations
    install -m 0444 ${WORKDIR}/AC-Chassis.json ${D}/usr/share/entity-manager/configurations
}
