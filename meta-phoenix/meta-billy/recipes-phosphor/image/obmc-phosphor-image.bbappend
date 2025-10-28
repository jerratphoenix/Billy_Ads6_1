IMAGE_FEATURES:remove = "obmc-health-monitor"

IMAGE_INSTALL:append = " default-fru"
IMAGE_INSTALL:append = " phosphor-ipmi-ipmb"
IMAGE_INSTALL:append = " phoenix-ipmi-sdr"
IMAGE_INSTALL:append = " phoenix-dbus-sensors"
IMAGE_INSTALL:append = " default-datetime"
IMAGE_INSTALL:append = " ac-boot-check"
IMAGE_INSTALL:append = " systemd-analyze"
IMAGE_INSTALL:append = " rgmii-delay"
IMAGE_INSTALL:append = " libpeci"
IMAGE_INSTALL:append = " crashdump"
IMAGE_INSTALL:append = " auto-crash-dump"
IMAGE_INSTALL:append = " local-startup"
IMAGE_INSTALL:append = " vr-sensor-monitor"
IMAGE_INSTALL:append = " host-state-monitor"
IMAGE_INSTALL:append = " phoenix-voltage-logger-sysfs"


# ============================================================
#  Billy Platform override
#  Fix: fitImage too large when kernel upgraded to 5.15
# ============================================================
IMAGE_FITIMAGE_SIZE = "20971520"
FITIMAGE_SIZE_LIMIT = "20971520"
FITIMAGE_MAX_SIZE = "20971520"



