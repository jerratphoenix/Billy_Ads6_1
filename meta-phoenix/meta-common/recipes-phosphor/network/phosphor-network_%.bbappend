FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Restart-netipmid-service-if-VLAN-change.patch \
	    file://0002-add-static-ip.patch \
	    file://vlandDefault.service.in \
	    file://setDefaultVLAN.sh \
            "
RDEPENDS_${PN} = "bash"
SYSTEMD_SERVICE_${PN} += "vlanDefault.service"

do_install:append() {
    install -d ${D}/usr/sbin
    install -m 0755 ${WORKDIR}/setDefaultVLAN.sh ${D}/${sbindir}/
}
