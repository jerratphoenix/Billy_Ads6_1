FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += "file://aspeed-bmc-phoenix-brightoncity.dts \
            file://aspeed-g5.dtsi \
            "

do_patch:append() {
    cp ${WORKDIR}/*.dts* \
        ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts
}
