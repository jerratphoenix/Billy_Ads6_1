SUMMARY = "Phoenix Technologies IPMI OEM Commands prebuild for AST2500"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

S = "${WORKDIR}"

SRC_URI = " file://libphoenixoem.so.0.0.0"

do_compile[noexec] = "1"

inherit pkgconfig

DEPENDS += "phosphor-ipmi-host"

FILES:${PN}:append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES:${PN}-dev:append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV} ${libdir}/ipmid-providers/*.la"

do_install() {
  install -d ${D}${libdir}
  install -d ${D}${libdir}/ipmid-providers
  install -d ${D}${libdir}/host-ipmid

  install -m 0655 ${S}/libphoenixoem.so.0.0.0 ${D}/${libdir}/ipmid-providers/libphoenixoem.so.0.0.0

  lnr ${D}/${libdir}/ipmid-providers/libphoenixoem.so.0.0.0 ${D}/${libdir}/ipmid-providers/libphoenixoem.so
  lnr ${D}/${libdir}/ipmid-providers/libphoenixoem.so.0.0.0 ${D}/${libdir}/ipmid-providers/libphoenixoem.so.0
  lnr ${D}/${libdir}/ipmid-providers/libphoenixoem.so.0.0.0 ${D}/${libdir}/host-ipmid/libphoenixoem.so.0
}
