SUMMARY = "Default Fru"
DESCRIPTION = "Builds a default FRU file at runtime based on board ID"

inherit obmc-phosphor-systemd
inherit cmake

RDEPENDS:${PN} += "bash"

S = "${WORKDIR}"
SRC_URI += "file://checkFru.sh"
SRC_URI += "file://SetBaseboardFru.service"
SRC_URI += "file://mkfru.cpp"
SRC_URI += "file://CMakeLists.txt"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "\
    file://mkfru.cpp;beginline=2;endline=14;md5=c451359f18a13ee69602afce1588c01a \
    "

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/checkFru.sh ${D}/${bindir}/checkFru.sh
}

SYSTEMD_SERVICE:${PN} += "SetBaseboardFru.service"
