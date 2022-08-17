IMAGE_FEATURES:remove = "obmc-health-monitor"

IMAGE_INSTALL:append = " default-fru"
IMAGE_INSTALL:append = " phosphor-ipmi-ipmb"
IMAGE_INSTALL:append = " phoenix-ipmi-sdr"
#IMAGE_INSTALL:append = " odsm"
IMAGE_INSTALL:append = " odsm-ast2600"
IMAGE_INSTALL:append = " default-datetime"
IMAGE_INSTALL:append = " ac-boot-check"
