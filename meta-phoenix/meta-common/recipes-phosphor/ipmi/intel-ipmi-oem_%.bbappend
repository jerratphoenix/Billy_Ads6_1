FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

# Ignore patches for now since they refer to old version above
SRC_URI += "file://0001-Add-OEMDiscrete-Sensor-Type-And-SDR.patch \
            file://0002-Parse-PhoenixBMC-FirmwareRevision-GetDeviceID.patch \
            file://0003-Support-IPMI-Get-and-Add-SEL-Entry-commands.patch \
            file://0005-Add-Phoenix-OEM-Whitelist.patch \
	    file://0010-Enable-intel-ipmi-oem-command-and-add-to-whitelist.patch \
		file://0011-Add-blob-transfer-cmds-to-whitelist.patch \
		file://0012-Redfish-factory-reset-function-into-restore-default-IPMI-command-and-fix-SRVRBMC-1533.patch \
            file://0013-Add-DCMI-commands-to-whitelist.patch \
           "


EXTRA_OECMAKE += "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', 'validation-unsecure', '-DBMC_VALIDATION_UNSECURE_FEATURE=ON', '', d)}"
EXTRA_OECMAKE += "-DUSING_ENTITY_MANAGER_DECORATORS=OFF"

# Enable MDRv1 commands
EXTRA_OECMAKE += " -DMDR_V1_SUPPORT=ON"

