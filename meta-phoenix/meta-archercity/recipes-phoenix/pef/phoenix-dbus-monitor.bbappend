FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://PtecPEFConfig.json \
            file://PtecEmailConfig.json \
           "

do_install:append() {
    install -Dm 0666 ${WORKDIR}/PtecPEFConfig.json ${D}/${datadir}/${PN}/PtecPEFConfig.json
    install -Dm 0666 ${WORKDIR}/PtecEmailConfig.json ${D}/${datadir}/${PN}/PtecEmailConfig.json
}
