FILESEXTRAPATHS:append := "${THISDIR}/intel-led-manager-config:"

SRC_URI:append = " file://led.yaml \
                 "
S = "${WORKDIR}"

# Overwrite the example led layout yaml file prior
# to building the phosphor-led-manager package
do_install:append() {
    SRC=${S}
    DEST=${D}${datadir}/phosphor-led-manager
    install -D ${SRC}/led.yaml ${DEST}/led.yaml
}
