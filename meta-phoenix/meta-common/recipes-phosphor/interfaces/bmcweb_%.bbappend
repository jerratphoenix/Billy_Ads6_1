EXTRA_OEMESON += "-Dredfish-cpu-log=enabled \
                  -Dredfish-bmc-journal=enabled \
                  -Drest=enabled"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Merge-from-ptec-bmcweb.patch \
			file://0002-Add-ImgXfer-interface.patch \
			file://0005-ssl_key_handler.patch \
			file://0008-Fix-sensor-functional-flag.patch \
			file://0010-Add-CPU-PPIN-and-version.patch \
			file://0011-Add-fan-control-on-Redfish.patch \
			file://0012-Add-I2C-diagonstic-tool-on-Redfish.patch \
			file://0013-Add-GPIO-diagnostic-tool-on-redfish.patch \
			file://0014-Add-ADC-diagnostic-tool-on-redfish.patch \
			file://0015-Change-watchdog-severity-to-critical.patch \
			file://0016-Fixed-Get-Systems-system-cmd-fail-and-returns-500-In.patch \
			file://0017-Register-discrete-sensor-redfish-messages.patch \
           "

