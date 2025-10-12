FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += " file://dev_id.json \
             file://channel_access.json \
             file://channel_config.json \
             file://master_write_read_white_list.json \
             file://dcmi_sensors.json \
             file://dcmi_cap.json \
             file://power_reading.json \
           "

FILES:${PN} += " \
    ${datadir}/ipmi-providers/channel_access.json \
    ${datadir}/ipmi-providers/channel_config.json \
    ${datadir}/ipmi-providers/master_write_read_white_list.json \
    ${datadir}/ipmi-providers/dcmi_sensors.json \
    ${datadir}/ipmi-providers/dcmi_cap.json \
    ${datadir}/ipmi-providers/power_reading.json \
    "

do_install:append() {
    install -m 0644 -D ${WORKDIR}/channel_access.json \
        ${D}${datadir}/ipmi-providers/channel_access.json
    install -m 0644 -D ${WORKDIR}/channel_config.json \
        ${D}${datadir}/ipmi-providers/channel_config.json
    install -m 0644 -D ${WORKDIR}/master_write_read_white_list.json \
        ${D}${datadir}/ipmi-providers/master_write_read_white_list.json
    install -m 0644 -D ${WORKDIR}/dcmi_sensors.json \
        ${D}${datadir}/ipmi-providers/dcmi_sensors.json
    install -m 0644 -D ${WORKDIR}/dcmi_cap.json \
        ${D}${datadir}/ipmi-providers/dcmi_cap.json
    install -m 0644 -D ${WORKDIR}/power_reading.json \
        ${D}${datadir}/ipmi-providers/power_reading.json
}
