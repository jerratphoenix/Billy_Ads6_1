# ============================================================
#  Phoenix Billy Platform – Kernel Upgrade to Linux 5.15 (Intel)
#  Source: github.com/Intel-BMC/linux.git  branch=dev-5.15-intel
# ============================================================

# --- Search Path for Files ---
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}/linux-aspeed:${THISDIR}/${PN}:"

# --- Kernel Source & Version ---
LINUX_VERSION = "5.15.80"
KBRANCH = "dev-5.15-intel"
KSRC = "git://github.com/Intel-BMC/linux.git;protocol=https;branch=${KBRANCH}"
SRCREV = "${AUTOREV}"

# --- Kernel Config ---
KMETA = ""
KERNEL_FEATURES = ""
LINUX_KERNEL_TYPE = ""
KBUILD_DEFCONFIG = "multi_v7_defconfig"
KERNEL_CONFIG_FRAGMENTS += "billy.cfg disable-drm.cfg"

# --- Skip sanity checks ---
KERNEL_VERSION_SANITY_SKIP = "1"
do_kernel_configcheck[noexec] = "1"

# --- Compiler flags ---
do_compile:prepend() {
    export DTC_FLAGS=-@
}

# --- Source Files (custom DTS and configs) ---
SRC_URI = "${KSRC} \
           file://aspeed-bmc-phoenix-billy.dts \
           file://billy.cfg \
           file://disable-drm.cfg \
"

# --- Copy Billy DTS into kernel source tree ---
do_configure:append() {
    echo "[linux-aspeed.bbappend] Copying aspeed-bmc-phoenix-billy.dts into kernel source tree..."
    cp ${WORKDIR}/aspeed-bmc-phoenix-billy.dts ${S}/arch/arm/boot/dts/ || true
}

# --- Device Tree Target ---
KERNEL_DEVICETREE = "aspeed-bmc-phoenix-billy.dtb"
