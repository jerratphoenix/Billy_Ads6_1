FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " file://AC-Baseboard.json \
           "

do_install:append() {
     install -m 0444 ${WORKDIR}/AC-Baseboard.json ${D}/usr/share/entity-manager/configurations
}
