SUMMARY = "Phoenix DBus Monitor"
DESCRIPTION = "Phoenix DBus Monitor is a general purpose DBus application \
that watches DBus traffic for events and takes actions based on those events."
PR = "r1.1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " file://bootstrap.sh \
            file://configure.ac \ 
            file://Makefile.am \ 
            file://phoenix-dbus-monitor.service \
            file://config/PtecPEFConfig.json \
            file://config/PtecEmailConfig.json \
            file://src/Makefile.am \
            file://src/main.cpp \
            file://src/phoenix_discrete_monitor.cpp \
            file://src/phoenix_threshold_monitor.cpp \
            file://src/json_parser.cpp \
            file://src/phoenix_util.cpp \
            file://src/sendmail.cpp \
            file://include/phoenix_discrete_monitor.hpp \
            file://include/phoenix_threshold_monitor.hpp \
            file://include/json_parser.hpp \
            file://include/phoenix_action.hpp \
            file://include/phoenix_util.hpp \
            file://include/sendmail.hpp \
            file://include/sensorutils.hpp \
            file://include/type.hpp \
            file://include/debug.hpp \
          "

S = "${WORKDIR}"

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

