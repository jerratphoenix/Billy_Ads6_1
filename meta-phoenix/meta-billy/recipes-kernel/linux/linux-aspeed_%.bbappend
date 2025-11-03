# ============================================================
#  Phoenix Billy Platform – Kernel Upgrade to Linux 5.15 (Intel)
#  FIT structure: kernel + fdt + ramdisk-1 (non-bundle)
#  Hash: sha256, Compression: none (match zImage self-decompress)
# ============================================================

# --- Search Path for Files ---
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}/linux-aspeed:${THISDIR}/${PN}:"

# --- Enable FIT image flow ---
inherit kernel-fitimage
KERNEL_IMAGETYPE = "fitImage"
FIT_HASH_ALG = "sha256"
UBOOT_FIT_SIGNATURE = "0"

# ============================================================
#  [Key Fix]
#  The kernel 5.15 build produces self-decompressing zImage,
#  so outer FIT compression must be set to “none”.
# ============================================================
UBOOT_MKIMAGE_KERNEL_COMP = "none"
UBOOT_MKIMAGE_DTB_COMP = "none"
UBOOT_MKIMAGE_RAMDISK_COMP = "none"

# --- Add initramfs (non-bundle) so ramdisk-1 appears in FIT ---
INITRAMFS_IMAGE = "obmc-phosphor-initramfs"
INITRAMFS_IMAGE_BUNDLE = "0"

# --- Kernel Source & Version ---
LINUX_VERSION = "5.15.80"
KBRANCH = "dev-5.15-intel"
KSRC = "git://github.com/Intel-BMC/linux.git;protocol=https;branch=${KBRANCH}"
#SRCREV = "${AUTOREV}"
SRCREV = "7c1de25c06f31b04744beae891baf147af9ba0cb"

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

# --- Source Files ---
SRC_URI = "${KSRC} \
           file://aspeed-bmc-phoenix-billy.dts \
           file://billy.cfg \
           file://disable-drm.cfg \
"

# --- Copy Billy DTS into kernel tree ---
do_configure:append() {
    echo "[linux-aspeed.bbappend] Copying aspeed-bmc-phoenix-billy.dts into kernel source tree..."
    cp ${WORKDIR}/aspeed-bmc-phoenix-billy.dts ${S}/arch/arm/boot/dts/ || true
}

# --- Device Tree Target ---
KERNEL_DEVICETREE = "aspeed-bmc-phoenix-billy.dtb"

# --- Deployment adjustments ---
#   1) Search for initramfs FIT variant
#   2) Force image-kernel symlink → initramfs FIT
do_deploy:append() {
    echo "[linux-aspeed.bbappend] Ensuring image-kernel points to initramfs FIT..."
    initramfs_fit=""
    for f in ${DEPLOYDIR}/fitImage-obmc-phosphor-initramfs-*; do
        if [ -f "$f" ]; then
            initramfs_fit="$f"
        fi
    done
    if [ -n "$initramfs_fit" ]; then
        echo "  -> Found initramfs FIT: $initramfs_fit"
        ln -sf "$(basename "$initramfs_fit")" ${DEPLOYDIR}/image-kernel
    else
        echo "  !! No initramfs FIT found; keeping default fitImage"
    fi
}

do_kernel_configme[cleandirs] = "${S}/.config"
do_kernel_configme:append() {
    echo "[Billy Fix] Forcing JFFS2/SQUASHFS/TMPFS reapply"
    echo "CONFIG_JFFS2_FS=y" >> ${B}/.config
    echo "CONFIG_JFFS2_FS_WBUF_VERIFY=y" >> ${B}/.config
    echo "CONFIG_SQUASHFS=y" >> ${B}/.config
    echo "CONFIG_SQUASHFS_XZ=y" >> ${B}/.config
    echo "CONFIG_TMPFS=y" >> ${B}/.config
    echo "CONFIG_TMPFS_POSIX_ACL=y" >> ${B}/.config
    echo "CONFIG_TMPFS_XATTR=y" >> ${B}/.config
    echo "CONFIG_OVERLAY_FS=y" >> ${B}/.config
}

