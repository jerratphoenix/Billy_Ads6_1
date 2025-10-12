FILESEXTRAPATHS:append := ":${THISDIR}/${PN}"
SRC_URI:append = " file://MC-Baseboard.json \
                 "

do_install:append() {
     install -d ${D}/usr/share/entity-manager/configurations
     install -m 0444 ${WORKDIR}/MC-Baseboard.json ${D}/usr/share/entity-manager/configurations
}
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Brightoncity-Fixed-BRIG-157-IPMI-Some-sensors-are-sh.patch"

