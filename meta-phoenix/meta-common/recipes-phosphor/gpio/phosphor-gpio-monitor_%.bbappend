FILESEXTRAPATHS:append := ":${THISDIR}/${PN}"
SRC_URI:append = " file://phosphor-multi-gpio-monitor.service \
                   file://0001-Add-dbus-service-for-multi-GPIO-monitor.patch \
                 "

do_install:append() {
        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${WORKDIR}/phosphor-multi-gpio-monitor.service \
            ${D}${systemd_system_unitdir}/
}
