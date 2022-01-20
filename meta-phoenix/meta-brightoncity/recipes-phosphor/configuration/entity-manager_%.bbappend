FILESEXTRAPATHS:append := ":${THISDIR}/${PN}"
SRC_URI:append = " file://BC-Baseboard.json \
                   file://0001-Support-FRU-write-from-dbus.patch \
                 "

EXTRA_OEMESON += "-Dfru-device-resizefru=true"

do_install:append() {
     install -d ${D}/usr/share/entity-manager/configurations
     install -m 0444 ${WORKDIR}/BC-Baseboard.json ${D}/usr/share/entity-manager/configurations
}
