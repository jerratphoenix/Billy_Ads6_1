FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://phosphor-ipmi-host.service \
            file://0001-GetSelfTest_isImplemented_0x55.patch \
            file://0002-Add-BMC-Reset-Cause-for-cold-reset-command.patch \
            file://0065-Add-Event-Only-SDR-Type.patch \
            file://0066-Fix-System-GUID.patch \
            file://0067-Fixed-Forced-Power-Off.patch \
            "

do_install:append() {
  install -d ${D}${includedir}/phosphor-ipmi-host
  install -m 0644 -D ${S}/sensorhandler.hpp ${D}${includedir}/phosphor-ipmi-host
  install -m 0644 -D ${S}/selutility.hpp ${D}${includedir}/phosphor-ipmi-host
}
