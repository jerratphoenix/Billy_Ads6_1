FILESEXTRAPATHS:append := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://0001-Enable-UART-mux-setting-before-SOL-activation.patch \
    "
