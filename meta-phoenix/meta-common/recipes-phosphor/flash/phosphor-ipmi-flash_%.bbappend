FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

SRC_URI += " file://fw_prepare.sh \
	     file://fw_verify.sh \
	     file://fw_update.sh \
	     file://fw_prepare.service \
	     file://fw_verify.service \
	     file://fw_update.service \
	     file://0000-Allow-Blob-Cleanup-On-Open.patch \
		"

# static-bmc enables static-layout
PACKAGECONFIG:append = " static-bmc"
#PACKAGECONFIG:append = " reboot-update"
#PACKAGECONFIG:append = " net-bridge"
PACKAGECONFIG:append = " aspeed-p2a"
IPMI_FLASH_BMC_ADDRESS = "0x9efe0000"

SYSTEMD_SERVICE:${PN} += " fw_prepare.service fw_verify.service fw_update.service"

EXTRA_OECONF += " STATIC_HANDLER_STAGED_NAME=/tmp/image-update.tar "
#EXTRA_OECONF += " STATIC_HANDLER_STAGED_NAME=/tmp/image-update-test.tar "
EXTRA_OECONF += " PREPARATION_DBUS_SERVICE=fw_prepare.service "
EXTRA_OECONF += " VERIFY_DBUS_SERVICE=fw_verify.service "
EXTRA_OECONF += " UPDATE_DBUS_SERVICE=fw_update.service "

do_install:append() {
        install -d ${D}${bindir}
        install -m 0755 ${WORKDIR}/fw_prepare.sh ${D}${bindir}
        install -m 0755 ${WORKDIR}/fw_verify.sh ${D}${bindir}
        install -m 0755 ${WORKDIR}/fw_update.sh ${D}${bindir}

        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${WORKDIR}/fw_prepare.service ${D}${systemd_unitdir}/system
        install -m 0644 ${WORKDIR}/fw_verify.service ${D}${systemd_unitdir}/system
        install -m 0644 ${WORKDIR}/fw_update.service ${D}${systemd_unitdir}/system
}

