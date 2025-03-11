SUMMARY = "Phoenix Dbus Sensor Service"
DESCRIPTION = "Phoenix Dbus Sensor Service"
PR = "r1"
LICENSE = "CLOSED"

S = "${WORKDIR}"
SRC_URI = " file://phoenix-dbus-sensors \
            file://libptecsensorapi.so.0.0.1 \
            file://libptecsensorraw.so.0.0.1 \
            file://SDR.active "

SYSTEMD_SERVICE:${PN} = "phoenix-dbus-sensors.service"

FILES:${PN}:append = " ${datadir}/phoenix-sensor-config/SDR.active"
FILES:${PN}:append = " ${libdir}/libptecsensorapi.so*"
FILES:${PN}:append = " ${libdir}/libptecsensorraw.so*"

do_install:prepend() {
    install -d ${D}${datadir}/phoenix-sensor-config
    install -m 0644 SDR.active ${D}${datadir}/phoenix-sensor-config/SDR.active
    echo "Installed SDR.active to ${D}${datadir}/phoenix-sensor-config/SDR.active"

    install -d ${D}${bindir}
    install -m 0755 phoenix-dbus-sensors ${D}${bindir}
    echo "Installed phoenix-dbus-sensors to ${D}${bindir}"

    install -d ${D}${libdir}
    install -m 0755 libptecsensorapi.so.0.0.1 ${D}${libdir}  # Should be 0755 for libraries
    install -m 0755 libptecsensorraw.so.0.0.1 ${D}${libdir}
    echo "Installed libraries to ${D}${libdir}"

    echo "do_install completed"
}

INSANE_SKIP_${PN} += "installed-vs-shipped already-stripped"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"
