SUMMARY = "Phoenix IPMI SDR Repository commands"
DESCRIPTION = "Phoenix IPMI SDR Repository commands"

PR = "r1"
PV = "0.01"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " file://CMakeLists.txt \
            file://src/sdrcommands.cpp \
            file://src/storagecommands.cpp \
            file://include/sdrcommands.hpp \
            file://include/storagecommands.hpp \
            file://include/sdrutils.hpp \
            file://include/types.hpp \
           "

S = "${WORKDIR}"

DEPENDS = "boost phosphor-ipmi-host"
inherit pkgconfig cmake obmc-phosphor-ipmiprovider-symlink

LIBRARY_NAMES = "libptecsdrcmds.so"

HOSTIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"
NETIPMI_PROVIDER_LIBRARY += "${LIBRARY_NAMES}"

FILES:${PN}:append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES:${PN}:append = " ${libdir}/net-ipmid/lib*${SOLIBS}"
FILES:${PN}-dev:append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV}"
