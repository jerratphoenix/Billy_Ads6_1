IMAGE_INSTALL:append = " phosphor-ipmi-kcs"
IMAGE_INSTALL:append = " ac-boot-check"
IMAGE_INSTALL:append = " phosphor-ipmi-ipmb"
#Post code related recipe
IMAGE_INSTALL:append += " phosphor-post-code-manager"
IMAGE_INSTALL:append += " phosphor-host-postd"

# Phoenix sensor module
IMAGE_INSTALL:append += " phoenix-ipmi-sdr"
IMAGE_INSTALL:append += " phoenix-dbus-sensors"

# Phoenix gpio module
IMAGE_INSTALL:append += " phoenix-gpio-monitor"

