FILESEXTRAPATHS:append := ":${THISDIR}/${PN}"
SRC_URI:append = " file://0001-Support-FRU-write-from-dbus.patch \
                 "

EXTRA_OEMESON += "-Dfru-device-resizefru=true"
