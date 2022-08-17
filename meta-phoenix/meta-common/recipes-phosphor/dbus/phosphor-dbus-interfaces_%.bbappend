FILESEXTRAPATHS:append:= "${THISDIR}/${PN}:"

SRC_URI += " \
	file://0001-Sync-up-CPU-inventory-for-SMBIOS-Dbus-Redfish-and-BM.patch \
	file://0002-Add-the-pre-timeout-interrupt-defined-in-IPMI-spec-a.patch \
	"
