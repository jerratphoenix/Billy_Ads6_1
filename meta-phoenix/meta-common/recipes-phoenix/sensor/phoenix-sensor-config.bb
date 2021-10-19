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

BBCLASSEXTEND = "native nativesdk"

DEPENDS = "json-c-native"

do_install:append() {
    install -Dm 0755 ${S}/gensdr ${D}/${bindir}/gensdr
    install -Dm 0644 ${S}/PtecSensorConfig.json ${D}/${datadir}/phoenix-sensor-config/PtecSensorConfig.json

    install -Dm 0755 ${S}/genraw ${D}/${bindir}/genraw

#    install -Dm 0644 ${S}/raw_reading.hpp ${D}/${datadir}/phoenix-sensor-config/raw_reading.hpp
    install -Dm 0644 ${S}/raw_reading.cpp ${D}/${datadir}/phoenix-sensor-config/raw_reading.cpp
}

