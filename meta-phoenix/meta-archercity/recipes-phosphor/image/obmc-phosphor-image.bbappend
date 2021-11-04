IMAGE_FEATURES:remove = "obmc-health-monitor"

IMAGE_INSTALL:append = " default-fru"
IMAGE_INSTALL:append = " phosphor-ipmi-ipmb"
IMAGE_INSTALL:append = " phoenix-ipmi-sdr"
IMAGE_INSTALL:append = " phoenix-dbus-sensors"
