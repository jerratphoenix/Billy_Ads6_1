FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append += " \
file://phoenix-billy.dts \
file://phoenix.cfg \
file://0001-Add-ast2600-intel-as-a-new-board.patch \
file://0002-AST2600-Enable-host-searial-port-clock-configuration.patch \
file://0003-ast2600-intel-layout-environment-addr.patch \
file://0004-AST2600-Adjust-default-GPIO-settings.patch \
file://0005-Ast2600-Enable-interrupt-in-u-boot.patch \
file://0006-SPI-Quad-IO-Mode.patch \
file://0007-ast2600-Override-OTP-strap-settings.patch \
file://0008-AST2600-Add-TPM-pulse-trigger.patch \
file://0009-AST2600-Disable-DMA-arbitration-options-on-MAC1-and-.patch \
file://0010-Fix-timer-support.patch \
file://0011-KCS-driver-support-in-uBoot.patch \
file://0012-IPMI-command-handler-implementation-in-uboot.patch \
file://0013-Add-a-workaround-to-cover-UART-interrupt-bug-in-AST2.patch \
file://0014-Add-a-workaround-to-cover-eSPI-OOB-free-bug-in-AST26.patch \
file://0015-net-phy-realtek-Change-LED-configuration.patch \
file://0016-Add-system-reset-status-support.patch \
file://0016-Add-LED-control-support.patch \
file://0017-Manufacturing-mode-physical-presence-detection.patch \
file://0018-Add-a-workaround-to-cover-VGA-memory-size-bug-in-A0.patch \
file://0019-Apply-WDT1-2-reset-mask-to-reset-needed-controller.patch \
file://0022-Reboot-into-UBOOT-on-Watchdog-Failures.patch \
file://0023-Add-WDT-to-u-boot-to-cover-booting-failures.patch \
file://0024-fix-SUS_WARN-handling-logic.patch \
file://0025-ast2600-PFR-platform-EXTRST-reset-mask-selection.patch \
file://0026-Enable-PCIe-L1-support.patch \
file://0027-ast2600-Add-Mailbox-init-function.patch \
file://0028-Improve-randomness-of-mac-address-generation.patch \
file://0029-Set-UART-routing-in-lowlevel_init.patch \
file://0030-Add-Aspeed-PWM-uclass-driver.patch \
file://0031-Add-a-workaround-to-fix-AST2600-A0-booting-issue.patch \
file://0032-Disable-eSPI-initialization-in-u-boot-for-normal-boo.patch \
file://0033-Disable-debug-interfaces.patch \
file://0034-Implement-the-IPMI-commands-in-FFUJ-mode-in-u-boot.patch \
file://0501-Add-phoenix-archercity-device-tree-to-Makefile.patch \
file://0502-Setup-Kernel-address.patch \
file://0503-Disable-Intel-FFUJ-abort-BMC-boot.patch \
file://0504-Get_Mac_Address_From_EEPROM.patch \
"

do_copyfile () {
    cp ${WORKDIR}/phoenix-billy.dts ${S}/arch/arm/dts
}

addtask copyfile after do_patch before do_configure


