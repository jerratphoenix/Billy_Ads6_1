SUMMARY = "Phoenix Sensor Config Handle"
DESCRIPTION = "Phoenix Sensor Config Handle"

PR = "r1"
PV = "0.01"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " file://gensdr.c \
            file://gensdr.h \
            file://genraw.c \
            file://Makefile \
            file://PtecSensorConfig.json \
          "

SRC_URI += " file://raw_reading.cpp \
          "

S = "${WORKDIR}"

inherit native

DEPENDS = "json-c-native"

do_install:append() {
    DEST=${D}/${datadir}/phoenix-sensor-config

    ${S}/gensdr \
        ${S}/PtecSensorConfig.json \
        ${DEST}/SDR.active

    ${S}/genraw \
        ${S}/PtecSensorConfig.json \
        ${DEST}/raw_reading.hpp

    install -Dm 0644 ${S}/raw_reading.cpp ${DEST}/raw_reading.cpp
}

