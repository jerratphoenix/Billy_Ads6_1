SUMMARY = "Phoenix IPMI PEF commands"
DESCRIPTION = "Phoenix IPMI PEF commands"

PR = "r1"
PV = "0.01"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " file://CMakeLists.txt \
            file://src/pefcommands.cpp \
            file://include/pefcommands.hpp \
           "

S = "${WORKDIR}"

DEPENDS = "boost phosphor-ipmi-host nlohmann-json"
inherit cmake obmc-phosphor-ipmiprovider-symlink

LIBRARY_NAMES = "libptecpefcmds.so"

HOSTIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"
NETIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"

FILES:${PN}:append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/net-ipmid/lib*${SOLIBS}"
FILES:${PN}-dev:append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV}"

export PSEUDO_DISABLED = "1"
