FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
            file://0013-Add-UART-routing-logic-into-host-console-connection-.patch \
"

EXTRA_OEMESON:append = " \
    -Dhttp-body-limit=200 \
    "
