SUMMARY = "Phoenix Dbus Sensors"
DESCRIPTION = "Phoenix Dbus Sensor Services Configured from D-Bus"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " \
    file://CMakeLists.txt \
    file://phoenix-dbus-sensors.hpp \
    file://phoenix-dbus-sensors.cpp  \
    file://phoenix-dbus-sensors.service \
    file://debug.hpp \
    file://SensorAPIs/sensorapi.hpp \
    file://SensorAPIs/sysfs.cpp \
    file://SensorAPIs/sysfs.hpp \
    file://SensorAPIs/adc.cpp \
    file://SensorAPIs/adc.hpp \
    file://SensorAPIs/gpio.cpp \
    file://SensorAPIs/gpio.hpp \
    file://SensorAPIs/sel.cpp \
    file://SensorAPIs/sel.hpp \
    file://SensorAPIs/bmc_state.cpp \
    file://SensorAPIs/bmc_state.hpp \
    file://SensorAPIs/chassis_state.cpp \
    file://SensorAPIs/chassis_state.hpp \
    file://SensorAPIs/host_cpu.cpp \
    file://SensorAPIs/host_cpu.hpp \
    file://SensorAPIs/bmc_update.cpp \
    file://SensorAPIs/bmc_update.hpp \
    file://SensorAPIs/bmc_factory_reset.cpp \
    file://SensorAPIs/bmc_factory_reset.hpp \
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
    SENSOR_CONFIG_DIR=${STAGING_DATADIR_NATIVE}/phoenix-sensor-config

    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading.cpp ${S}
    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading.hpp ${S}
}

FILES:${PN}:append = " ${datadir}/phoenix-sensor-config/SDR.active"

do_install() {
  install -d ${D}/${datadir}/${PN}/

  install -d ${D}${bindir}
  install -m 0755 phoenix-dbus-sensors ${D}/${bindir}

  install -d ${D}/${datadir}/phoenix-sensor-config
  install -m 0644 ${STAGING_DATADIR_NATIVE}/phoenix-sensor-config/SDR.active ${D}/${datadir}/phoenix-sensor-config/SDR.active
}
