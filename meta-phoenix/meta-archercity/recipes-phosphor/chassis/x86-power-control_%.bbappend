FILESEXTRAPATHS:append := "${THISDIR}/${PN}:"

SRC_URI += " \
        file://0001-Ignore-SIO-Power-Good-assert-from-Off.patch \
        "
