SUMMARY = "Auto CPU Crashdump"
DESCRIPTION = "Auto CPU Crashdump trigger via GPIO"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

inherit systemd

SYSTEMD_SERVICE:${PN} = "auto.crashdump.service"

S = "${WORKDIR}"
SRC_URI += "file://auto-crashdump.sh \
            file://auto.crashdump.service \
           "
RDEPENDS:${PN} = "bash"

do_install:append() {
          install -d ${D}/${bindir}
          install -m 0755 ${S}/auto-crashdump.sh ${D}${bindir}/auto-crashdump.sh

          install -d ${D}${base_libdir}/systemd/system
          install -m 0644 ${S}/auto.crashdump.service ${D}${base_libdir}/systemd/system
}