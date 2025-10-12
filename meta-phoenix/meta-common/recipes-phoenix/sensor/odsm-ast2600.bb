SUMMARY = "Phoenix odsm binary module"
DESCRIPTION = "Phoenix odsm binary module"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

S = "${WORKDIR}"
PV = "0.1"

inherit obmc-phosphor-systemd
inherit cmake systemd

DEPENDS = "systemd boost sdbusplus \
           phoenix-sensor-config-native \
           libgpiod \
           i2c-tools \
           libpeci \
           phosphor-logging \
          "

RDEPENDS:${PN} += "libsystemd bash"

SRC_URI = " file://odsm \
            file://libptecsensorapi.so \
            file://CMakeLists.txt \
            file://SensorAPIs/adc.hpp \ 
            file://SensorAPIs/bmc_factory_reset.hpp \ 
            file://SensorAPIs/bmc_state.hpp \ 
            file://SensorAPIs/bmc_update.hpp \ 
            file://SensorAPIs/chassis_state.hpp \ 
            file://SensorAPIs/dimm_presence.hpp \ 
            file://SensorAPIs/gpio.hpp \ 
            file://SensorAPIs/host_cpu.hpp \ 
            file://SensorAPIs/host_state.hpp \ 
            file://SensorAPIs/i2c.hpp \ 
            file://SensorAPIs/pef.hpp \ 
            file://SensorAPIs/sel.hpp \ 
            file://SensorAPIs/sensorapi.hpp \ 
            file://SensorAPIs/set_sel_time_event.hpp \ 
            file://SensorAPIs/smbus.hpp \ 
            file://SensorAPIs/sysfs.hpp \ 
            file://SensorAPIs/tmp75.hpp \ 
            file://SensorAPIs/watchdog2.hpp \ 
          "

SYSTEMD_SERVICE:${PN} += "odsm.service"

do_configure:prepend() {
    SENSOR_CONFIG_DIR=${STAGING_DATADIR_NATIVE}/phoenix-sensor-config

    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading.cpp ${S}
    install -m 0644 ${SENSOR_CONFIG_DIR}/raw_reading_info.cpp ${S}
}

FILES:${PN}:append = " ${datadir}/phoenix-sensor-config/SDR.active"
FILES:${PN}:append = " ${bindir}/odsm"
FILES:${PN}:append = " ${libdir}/libptecsensorapi.so*"
FILES:${PN}:append = " ${libdir}/libptecsensorraw.so*"

do_install:prepend() {

  install -d ${D}${bindir}
  install -m 0755 ${S}/odsm ${D}/${bindir}

  install -d ${D}${libdir}
  install -m 0655 ${S}/libptecsensorapi.so ${D}/${libdir}/libptecsensorapi.so.0.0.1
  lnr ${D}/${libdir}/libptecsensorapi.so.0.0.1 ${D}/${libdir}/libptecsensorapi.so

  install -m 0655 ${S}/build/libptecsensorraw.so ${D}/${libdir}/libptecsensorraw.so.0.0.1
  lnr ${D}/${libdir}/libptecsensorraw.so.0.0.1 ${D}/${libdir}/libptecsensorraw.so

  install -d ${D}/${datadir}/phoenix-sensor-config
  install -m 0644 ${STAGING_DATADIR_NATIVE}/phoenix-sensor-config/SDR.active ${D}/${datadir}/phoenix-sensor-config/SDR.active
}

