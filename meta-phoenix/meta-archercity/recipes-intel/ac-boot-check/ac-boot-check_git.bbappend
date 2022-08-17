FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://ac-boot-check.sh \
           "
S = "${WORKDIR}"

do_install:append() {
    install -m 0755 ${S}/ac-boot-check.sh ${D}/${bindir}/ac-boot-check.sh
}
