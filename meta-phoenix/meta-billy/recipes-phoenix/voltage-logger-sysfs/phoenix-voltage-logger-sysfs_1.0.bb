SUMMARY = "Voltage logger via sysfs (1-minute test, logs to /var/log)"
DESCRIPTION = "Lightweight C program reading 17 rails from /sys/class/hwmon and logging to /var/log/voltage_logger_1min.log."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://voltage_logger_sysfs.c \
           file://phoenix-voltage-logger-sysfs.service \
          "

inherit systemd

S = "${WORKDIR}"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} voltage_logger_sysfs.c -o voltage_logger_sysfs
}

do_install() {
    # binary
    install -d ${D}${bindir}
    install -m 0755 voltage_logger_sysfs ${D}${bindir}/voltage_logger_sysfs

    # systemd unit
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/phoenix-voltage-logger-sysfs.service ${D}${systemd_unitdir}/system/
	
}

SYSTEMD_SERVICE:${PN} = "phoenix-voltage-logger-sysfs.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

FILES:${PN} += "${bindir}/voltage_logger_sysfs \
                ${systemd_unitdir}/system/phoenix-voltage-logger-sysfs.service \
               "
