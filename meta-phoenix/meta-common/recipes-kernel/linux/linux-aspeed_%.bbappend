FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"


SRC_URI += " \
        file://0001-Common-Add-header-file-to-enable-ASD-function.patch \
        file://0016-Add-ASPEED-SGPIO-driver.patch \
        "

