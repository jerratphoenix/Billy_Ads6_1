SUMMARY = "Phoenix DBus Monitor"
DESCRIPTION = "Phoenix DBus Monitor is a general purpose DBus application \
that watches DBus traffic for events and takes actions based on those events."
PR = "r1.1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "git://git@github.com/pteceng/phoenix-dbus-monitor.git;protocol=ssh \
           file://phoenix-dbus-monitor.service \
          "
SRCREV = "a247b458adc24a38d648a194ca67e6674c8ac86e"

S = "${WORKDIR}/git"
PV = "0.1+git${SRCPV}"

inherit autotools \
        pkgconfig \
        python3native \
        obmc-phosphor-systemd

FILESEXTRAPATHS:append := "${THISDIR}/${PN}:"
SYSTEMD_SERVICE:${PN} += "phoenix-dbus-monitor.service"

DEPENDS += " \
        phosphor-logging \
        autoconf-archive-native \
        ${PYTHON_PN}-sdbus++-native \
        sdeventplus \
        sdbusplus \
        phosphor-snmp \
        systemd \
        boost \
        nlohmann-json \
        libesmtp \
        "

do_install:append() {
    install -Dm 0666 ${S}/config/PtecPEFConfig.json ${D}/${datadir}/${PN}/PtecPEFConfig.json
    install -Dm 0666 ${S}/config/PtecEmailConfig.json ${D}/${datadir}/${PN}/PtecEmailConfig.json
}

