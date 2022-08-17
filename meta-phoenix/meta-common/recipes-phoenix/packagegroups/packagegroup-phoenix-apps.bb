SUMMARY = "OpenBMC for Phoenix - Applications"
PR = "r1"

inherit packagegroup

PROVIDES = "${PACKAGES}"
PACKAGES = " \
        ${PN}-chassis \
        ${PN}-fans \
        ${PN}-flash \
        ${PN}-system \
        "

PROVIDES += "virtual/obmc-chassis-mgmt"
PROVIDES += "virtual/obmc-fan-mgmt"
PROVIDES += "virtual/obmc-flash-mgmt"
PROVIDES += "virtual/obmc-system-mgmt"

RPROVIDES:${PN}-chassis += "virtual-obmc-chassis-mgmt"
RPROVIDES:${PN}-fans += "virtual-obmc-fan-mgmt"
RPROVIDES:${PN}-flash += "virtual-obmc-flash-mgmt"
RPROVIDES:${PN}-system += "virtual-obmc-system-mgmt"

SUMMARY:${PN}-chassis = "Phoenix Chassis"
RDEPENDS:${PN}-chassis = " \
        x86-power-control \
        obmc-host-failure-reboots \
        "

SUMMARY:${PN}-fans = "Phoenix Fans"
RDEPENDS:${PN}-fans = " \
        phosphor-pid-control \
        "

SUMMARY:${PN}-flash = "Phoenix Flash"
RDEPENDS:${PN}-flash = " \
        obmc-control-bmc \
        "

SUMMARY:${PN}-system = "Phoenix System"
RDEPENDS:${PN}-system = " \
        bmcweb \
        entity-manager \
        intel-ipmi-oem \
        dbus-sensors \
        webui-vue \
        "
