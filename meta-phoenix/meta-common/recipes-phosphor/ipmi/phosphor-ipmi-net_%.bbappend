inherit useradd

# TODO: This should be removed, once up-stream bump up
# issue is resolved
SRC_URI += "git://github.com/openbmc/phosphor-net-ipmid"

USERADD_PACKAGES = "${PN}"
# add a group called ipmi
GROUPADD_PARAM:${PN} = "ipmi "

# Default rmcpp iface is eth0; channel 3

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " file://0006-Modify-dbus-namespace-of-chassis-control-for-guid.patch \
             file://0001-Remove-SOL-service-check.patch \
             file://0002-Add-dependency-of-phosphor-ipmi-net-service.patch \
           "
