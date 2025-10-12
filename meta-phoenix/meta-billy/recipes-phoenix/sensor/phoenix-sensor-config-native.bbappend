FILESEXTRAPATHS:append := "${THISDIR}/phoenix-sensor-config:"

SRC_URI:append += "\
    file://PtecSensorConfig.json \
    file://raw_reading.cpp \
"

do_install:append() {
    DEST=${D}/${datadir}/phoenix-sensor-config

    ${S}/gensdr \
        ${S}/PtecSensorConfig.json \
        ${DEST}/SDR.active

    ${S}/genraw \
        ${S}/PtecSensorConfig.json \
        ${DEST}/raw_reading_info.cpp

    install -Dm 0644 ${S}/raw_reading.cpp ${DEST}/raw_reading.cpp
}
