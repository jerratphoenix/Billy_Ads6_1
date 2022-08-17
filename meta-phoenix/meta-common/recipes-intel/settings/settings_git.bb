SUMMARY = "Settings"

SRC_URI = "git://github.com/Intel-BMC/settings.git;protocol=ssh"
SRCREV = "5d2150656d23412c59acd4da7b9f155902f089d9"

S = "${WORKDIR}/git"

PV = "0.1+git${SRCPV}"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SYSTEMD_SERVICE:${PN} = "xyz.openbmc_project.Intel_Settings.service"

SRC_URI += "file://0000-Avoid-Conflict-Service.patch \
	    file://0001-Remove-Duplicate-Boot-Option-Members.patch \
	    file://0002-Remove-Duplicate-Restore-Policy.patch \
            file://0003-Remove-duplicate-time-synchronization-members.patch \
	"

DEPENDS = "boost \
           nlohmann-json \
           sdbusplus"


inherit cmake systemd

EXTRA_OECMAKE = "-DYOCTO=1"
