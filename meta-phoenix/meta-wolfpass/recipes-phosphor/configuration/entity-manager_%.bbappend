FILESEXTRAPATHS:append := ":${THISDIR}/${PN}"
SRC_URI:append = " file://PtecWolfpass.json \
                 "

RDEPENDS:${PN} += " default-fru "

do_install:append() {
     rm -f ${D}/usr/share/entity-manager/configurations/*.json
     install -d ${D}/usr/share/entity-manager/configurations
     install -m 0444 ${WORKDIR}/PtecWolfpass.json ${D}/usr/share/entity-manager/configurations
}
