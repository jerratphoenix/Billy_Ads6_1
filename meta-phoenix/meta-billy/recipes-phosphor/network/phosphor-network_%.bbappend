FILESEXTRAPATHS:append := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://0001-Set-the-static-IP-for-eth0.patch \
    file://0002-Change-the-default-IPv4-IP-address.patch \
    "
