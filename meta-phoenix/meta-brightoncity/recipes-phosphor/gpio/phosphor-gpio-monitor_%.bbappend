FILESEXTRAPATHS:append := ":${THISDIR}/${PN}"
SRC_URI:append = " file://phosphor-multi-gpio-monitor.json \
                 "

do_install:append() {
        install -d ${D}/usr/share/phosphor-gpio-monitor
        install -m 0644 ${WORKDIR}/phosphor-multi-gpio-monitor.json \
            ${D}/usr/share/${PN}/
}

FILES:${PN}-monitor += "/usr/share/phosphor-gpio-monitor/phosphor-multi-gpio-monitor.json"
