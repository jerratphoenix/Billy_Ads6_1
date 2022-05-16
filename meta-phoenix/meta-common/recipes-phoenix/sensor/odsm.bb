SUMMARY = "Phoenix ODSM"
DESCRIPTION = "Phoenix ODSM"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "git://git@github.com/pteceng/odsm.git;protocol=ssh"
SRCREV = "acca84a418920bf98add22d022c53b08e6a69d3e"

S = "${WORKDIR}/git"
PV = "0.1"

inherit obmc-phosphor-systemd
inherit cmake systemd

DEPENDS = "systemd boost sdbusplus \
           phosphor-dbus-interfaces \
           phoenix-sensor-config-native \
           libgpiod \
           i2c-tools \
           libpeci \
           libbsd \
           phosphor-logging \
          "

RDEPENDS:${PN} += "libsystemd bash libnl"

SYSTEMD_SERVICE:${PN} += "odsm.service"

do_configure:prepend() {
    SENSOR_CONFIG_DIR=${STAGING_DATADIR_NATIVE}/phoenix-sensor-config

    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading.cpp ${S}
    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading_info.cpp ${S}
}

FILES:${PN}:append = " ${datadir}/phoenix-sensor-config/SDR.active"
FILES:${PN}:append = " ${libdir}/libptecsensorapi.so*"
FILES:${PN}:append = " ${libdir}/libptecsensorraw.so*"

do_install:prepend() {
  install -d ${D}/${datadir}/${PN}/

  install -d ${D}${bindir}
  install -m 0755 odsm ${D}/${bindir}

  install -d ${D}${libdir}
  install -m 0655 libptecsensorapi.so.0.0.1 ${D}/${libdir}
  install -m 0655 libptecsensorraw.so.0.0.1 ${D}/${libdir}

  install -d ${D}/${datadir}/phoenix-sensor-config
  install -m 0644 ${STAGING_DATADIR_NATIVE}/phoenix-sensor-config/SDR.active ${D}/${datadir}/phoenix-sensor-config/SDR.active
}
