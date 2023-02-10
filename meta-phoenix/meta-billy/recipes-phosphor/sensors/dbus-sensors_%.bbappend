FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Shorten-the-name-to-keep-the-length-below-16-as-much.patch \
            file://0002-Adjust-the-ref-volt-to-2.5-for-ADC-sensors.patch \
            file://0003-Add-MUX-control-to-select-PECI-path.patch \
           "

