IMAGE_INSTALL:append = " phosphor-ipmi-kcs"
IMAGE_INSTALL:append = " host-error-monitor"
IMAGE_INSTALL:append = " ac-boot-check"
IMAGE_INSTALL:append = " phosphor-ipmi-ipmb"

# Phoenix sensor module
IMAGE_INSTALL:append += " phoenix-ipmi-sdr"
IMAGE_INSTALL:append += " phoenix-dbus-sensors"

# Phoenix bmc update log
IMAGE_INSTALL:append += " bmc-update-log"
