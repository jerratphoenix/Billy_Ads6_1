FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Restart-netipmid-service-if-VLAN-change.patch \
            "

RDEPENDS_${PN} = "bash"
#SYSTEMD_SERVICE_${PN} += "StaticDefault.service.in"

#do_install:append() {
#    install -d ${D}/usr/sbin
#    install -m 0755 ${WORKDIR}/setDefaultVLAN.sh ${D}/${sbindir}/
#}
