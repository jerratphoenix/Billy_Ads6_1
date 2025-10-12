FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " file://0006-Modify-the-ID-of-software-image-updater-object-on-DB.patch \
	     file://0007-Enable-FW-Move.patch \
           "

PACKAGECONFIG += "flash_bios"

