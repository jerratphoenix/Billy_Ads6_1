SUMMARY = "Phoenix GPIO monitor application"
DESCRIPTION = "Application to monitor gpio assertions"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " \
    file://meson.build \
    file://phoenix-gpio-monitor.service \
    file://gpioMonMain.cpp \
    file://gpioMon.cpp \
    file://gpioMon.hpp \
"

S = "${WORKDIR}"

inherit meson pkgconfig
inherit obmc-phosphor-dbus-service

DEPENDS = " \
    sdbusplus \
    phosphor-logging \
    systemd \
    boost \
    libgpiod \
    cli11 \
    nlohmann-json \
"

RDEPENDS:${PN} += "libsystemd"

SYSTEMD_SERVICE:${PN} += "phoenix-gpio-monitor.service"

FILES:${PN} += "${bindir}/phoenix-gpio-monitor"

do_install() {
        install -d ${D}${bindir}
        install -m 0755 phoenix-gpio-monitor ${D}/${bindir}
}
