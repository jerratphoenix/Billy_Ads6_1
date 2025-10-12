LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"
inherit cmake systemd

SRC_URI = "git://git@github.com/pteceng/ServerBMC-Gold-uefi-import-export.git;protocol=ssh"
SRCREV = "8aacd230079b4cea4781f25b21dc14f499dff725"

DEPENDS = "boost sdbusplus libgpiod"

PV = "0.1+git${SRCPV}"

S = "${WORKDIR}/git"

SYSTEMD_SERVICE:${PN} += "xyz.openbmc_project.uefi-import-export.service"

EXTRA_OECMAKE = "-DYOCTO=1"
