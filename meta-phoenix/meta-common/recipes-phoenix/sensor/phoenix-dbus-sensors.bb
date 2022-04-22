SUMMARY = "Phoenix Dbus Sensor Service"
DESCRIPTION = "Phoenix Dbus Sensor Service"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "git://git@github.com/pteceng/phoenix-dbus-sensors.git;protocol=ssh"
SRCREV = "0d352f0d26a611e9e4ceca8c1972b46a3be3607d"

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
    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading_info.cpp ${S}
}

FILES:${PN}:append = " ${datadir}/phoenix-sensor-config/SDR.active"
FILES:${PN}:append = " ${libdir}/libptecsensorapi.so*"
FILES:${PN}:append = " ${libdir}/libptecsensorraw.so*"

do_install:prepend() {
  install -d ${D}/${datadir}/${PN}/

  install -d ${D}${bindir}
  install -m 0755 phoenix-dbus-sensors ${D}/${bindir}

  install -d ${D}${libdir}
  install -m 0655 libptecsensorapi.so.0.0.1 ${D}/${libdir}
  install -m 0655 libptecsensorraw.so.0.0.1 ${D}/${libdir}

  install -d ${D}/${datadir}/phoenix-sensor-config
  install -m 0644 ${STAGING_DATADIR_NATIVE}/phoenix-sensor-config/SDR.active ${D}/${datadir}/phoenix-sensor-config/SDR.active
}
