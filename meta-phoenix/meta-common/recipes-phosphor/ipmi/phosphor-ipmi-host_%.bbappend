FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://PtecLanConfig.json \
            file://phosphor-ipmi-host.service \
            file://0001-GetSelfTest_isImplemented_0x55.patch \
            file://0002-Add-BMC-Reset-Cause-for-cold-reset-command.patch \
            file://0003-Fix-DCMI-Get-Power-reading-command-always-report-zer.patch \
            file://0065-Add-Event-Only-SDR-Type.patch \
            file://0066-Fix-System-GUID.patch \
            file://0067-Fixed-Forced-Power-Off.patch \
            file://0069-Support-LAN-Configuration-Parameters.patch \
            file://0070-Fixed-ipmi-set-lan-config-parameter-fail.patch \
            file://0071-Before_Send_Set_SEL_Time_probe_NTP_Status.patch \
            "

do_install:append() {
  install -d ${D}${includedir}/phosphor-ipmi-host
  install -m 0644 -D ${S}/sensorhandler.hpp ${D}${includedir}/phosphor-ipmi-host
  install -m 0644 -D ${S}/selutility.hpp ${D}${includedir}/phosphor-ipmi-host
  install -Dm 0666 ${WORKDIR}/PtecLanConfig.json ${D}/${datadir}/${PN}/PtecLanConfig.json
}
