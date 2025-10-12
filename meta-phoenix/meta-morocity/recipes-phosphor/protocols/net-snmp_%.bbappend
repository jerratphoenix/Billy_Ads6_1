FILESEXTRAPATHS:append := "${THISDIR}/${PN}:"

SRC_URI += "file://snmpd.conf"

EXTRA_OECONF += "--with-out-mib-modules=smux"

do_install:append() {
    install -m 644 ${WORKDIR}/snmpd.conf ${D}${sysconfdir}/snmp/
}
