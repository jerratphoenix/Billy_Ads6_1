FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += "file://aspeed-bmc-phoenix-wolfpass.dts \
            file://aspeed-g5.dtsi \
            "
do_patch:append() {
    if [ -r "${WORKDIR}/aspeed-g5.dtsi" ]; then
        cp ${WORKDIR}/aspeed-g5.dtsi \
            ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts
    fi
    if [ -r "${WORKDIR}/aspeed-bmc-phoenix-wolfpass.dts" ]; then
        cp ${WORKDIR}/aspeed-bmc-phoenix-wolfpass.dts \
            ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts
    fi
    if [ -r "${DEVTOOL_TEMPDIR}/oe-local-files/aspeed-g5.dtsi" ]; then
        cp ${DEVTOOL_TEMPDIR}/oe-local-files/aspeed-g5.dtsi \
            ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts
    fi
    if [ -r "${DEVTOOL_TEMPDIR}/oe-local-files/aspeed-bmc-phoenix-wolfpass.dts" ]; then
        cp ${DEVTOOL_TEMPDIR}/oe-local-files/aspeed-bmc-phoenix-wolfpass.dts \
            ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts
    fi
}
