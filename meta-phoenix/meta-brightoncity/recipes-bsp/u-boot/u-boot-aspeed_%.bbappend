FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append += " \
    file://fw_env.config \
    file://0000-Enable-Default-Restrictions.patch \
    file://0001-Setup-boot-command-for-kernel-address.patch \
    file://0005-enable-passthrough-in-uboot.patch \
    file://0006-feature-rich-power-on.patch \
    file://0007-Disable-PCIe-L1.patch \
    file://0008-Enable-kcs3-in-uboot.patch \
    file://0009-Add-resetreason-to-kernel-cmdline.patch \
    file://0010-Remove-the-redundancy-step-of-getting-BMC-reset-info.patch \
    "
