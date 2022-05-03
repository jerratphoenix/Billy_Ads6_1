FILESEXTRAPATHS:append := ":${THISDIR}/${PN}"
SRC_URI:append = " file://phoenix-gpio-monitor.json \
                 "

do_install:append() {
        install -d ${D}/usr/share/${PN}
        install -m 0644 ${WORKDIR}/phoenix-gpio-monitor.json \
            ${D}/usr/share/${PN}/
}

FILES:${PN} += "/usr/share/${PN}/phoenix-gpio-monitor.json"
