EXTRA_OEMESON += "-Dredfish-cpu-log=enabled \
                  -Dredfish-bmc-journal=enabled \
                  -Drest=disabled"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Merge-from-ptec-bmcweb.patch"

