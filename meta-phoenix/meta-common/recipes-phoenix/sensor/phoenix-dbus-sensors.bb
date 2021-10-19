SUMMARY = "Phoenix Dbus Sensors"
DESCRIPTION = "Phoenix Dbus Sensor Services Configured from D-Bus"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " file://CMakeLists.txt \
            file://phoenix-dbus-sensors.hpp \
            file://phoenix-dbus-sensors.cpp  \
            file://phoenix-dbus-sensors.service \
            file://debug.hpp \
            file://SensorAPIs/sysfs.cpp \
            file://SensorAPIs/sysfs.hpp \
            file://SensorAPIs/adc.cpp \
            file://SensorAPIs/adc.hpp \
            file://SensorAPIs/gpio.cpp \
            file://SensorAPIs/gpio.hpp \
            file://SensorAPIs/sel.cpp \
            file://SensorAPIs/sel.hpp \
          "

S = "${WORKDIR}"

inherit obmc-phosphor-systemd
inherit cmake systemd

DEPENDS = "systemd boost sdbusplus \
           phosphor-dbus-interfaces \
           phoenix-sensor-config-native \
           libgpiod \
           i2c-tools \
           libpeci \
           libbsd \
          "

RDEPENDS:${PN} += "libsystemd bash libnl"

FILESEXTRAPATHS:append := "${THISDIR}/phoenix-dbus-sensors:"
SYSTEMD_SERVICE:${PN} += "phoenix-dbus-sensors.service"

DEBUG_BUILD = "1"

do_configure:prepend() {
    install -m 0644 ${STAGING_DATADIR_NATIVE}/phoenix-sensor-config/raw_reading.cpp ${S}

    ${STAGING_BINDIR_NATIVE}/genraw \
        ${STAGING_DATADIR_NATIVE}/phoenix-sensor-config/PtecSensorConfig.json \
        ${S}/raw_reading.hpp
}

do_install() {
  install -d ${D}/${datadir}/${PN}/

  install -d ${D}${bindir}
  install -m 0755 phoenix-dbus-sensors ${D}/${bindir}
}
