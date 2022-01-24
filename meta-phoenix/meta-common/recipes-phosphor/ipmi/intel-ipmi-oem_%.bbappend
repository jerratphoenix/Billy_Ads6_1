FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

# Ignore patches for now since they refer to old version above
SRC_URI += "file://ptec-ipmi-whitelist.conf \
            file://ptec-oemcommands.cpp \
            file://ptec-oemcommands.hpp \
            file://0001-Add-OEMDiscrete-Sensor-Type-And-SDR.patch \
            file://0002-Parse-PhoenixBMC-FirmwareRevision-GetDeviceID.patch \
            file://0003-Support-IPMI-Get-and-Add-SEL-Entry-commands.patch \
            file://0005-Add-Phoenix-OEM-Whitelist.patch \
            file://0010-Enable-intel-ipmi-oem-command-and-add-to-whitelist.patch \
            file://0011-Add-blob-transfer-cmds-to-whitelist.patch \
            file://0012-Redfish-factory-reset-function-into-restore-default-IPMI-command-and-fix-SRVRBMC-1533.patch \
            file://0013-Add-DCMI-commands-to-whitelist.patch \
            file://0014-Disable_Set_Sel_Time_Cmd_for_intel-ipmi-oem.patch \
            file://0015-Add-dbus-Clear-method-to-ClearSEL-command.patch \
            file://0016-Support-IPMI-FRU-write.patch \
            file://0017-Fix-the-mapping-issue-of-FruDevice-object-and-IPMI-FRU-ID.patch \
           "


EXTRA_OECMAKE += "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', 'validation-unsecure', '-DBMC_VALIDATION_UNSECURE_FEATURE=ON', '', d)}"
EXTRA_OECMAKE += "-DUSING_ENTITY_MANAGER_DECORATORS=OFF"

# Enable MDRv1 commands
EXTRA_OECMAKE += " -DMDR_V1_SUPPORT=ON"

do_patch:append() {
    bb.build.exec_func('do_override_whitelist', d)
}

do_override_whitelist () {
    if [ -r "${WORKDIR}/ptec-ipmi-whitelist.conf" ]; then
        cp ${WORKDIR}/ptec-ipmi-whitelist.conf \
            ${S}/ipmi-whitelist.conf
    fi
    if [ -r "${DEVTOOL_TEMPDIR}/oe-local-files/ptec-ipmi-whitelist.conf" ]; then
        cp ${DEVTOOL_TEMPDIR}/oe-local-files/ptec-ipmi-whitelist.conf \
            ${S}/ipmi-whitelist.conf
    fi
    if [ -r "${DEVTOOL_TEMPDIR}/oe-local-files/ptec-oemcommands.cpp" ]; then
        cp ${DEVTOOL_TEMPDIR}/oe-local-files/ptec-oemcommands.cpp \
            ${S}/src/oemcommands.cpp
    fi
    if [ -r "${DEVTOOL_TEMPDIR}/oe-local-files/ptec-oemcommands.hpp" ]; then
        cp ${DEVTOOL_TEMPDIR}/oe-local-files/ptec-oemcommands.hpp \
            ${S}/include/oemcommands.hpp
    fi
}
