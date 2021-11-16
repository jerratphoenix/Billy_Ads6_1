FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Expand-snmp-trap-to-follow-IPMI-SPEC.patch \
            file://0002-Change-phoenix-snmp-send-function-to-reture-bool-typ.patch \
            "

DEPENDS += "nlohmann-json"
