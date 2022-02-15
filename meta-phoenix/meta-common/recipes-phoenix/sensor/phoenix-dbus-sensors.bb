SUMMARY = "Phoenix Dbus Sensor Service"
DESCRIPTION = "Phoenix Dbus Sensor Service"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "git://git@github.com/pteceng/phoenix-dbus-sensors.git;protocol=ssh"
SRCREV = "df04612cd0f22f0f7db9e4ab4d650a9448a890cb"

S = "${WORKDIR}/git"
PV = "0.1+git${SRCPV}"

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

SYSTEMD_SERVICE:${PN} += "phoenix-dbus-sensors.service"

do_configure:prepend() {
    SENSOR_CONFIG_DIR=${STAGING_DATADIR_NATIVE}/phoenix-sensor-config

    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading.cpp ${S}
    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading.hpp ${S}
}

FILES:${PN}:append = " ${datadir}/phoenix-sensor-config/SDR.active"

do_install:prepend() {
  install -d ${D}/${datadir}/${PN}/

  install -d ${D}${bindir}
  install -m 0755 phoenix-dbus-sensors ${D}/${bindir}

  install -d ${D}/${datadir}/phoenix-sensor-config
  install -m 0644 ${STAGING_DATADIR_NATIVE}/phoenix-sensor-config/SDR.active ${D}/${datadir}/phoenix-sensor-config/SDR.active
}
