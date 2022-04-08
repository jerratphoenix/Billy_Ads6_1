
# IPMI related
#
# Do not add here, add at the meta-wolfpass (or other board / platform) section
# this is not generic but board / platform specific.
# IMAGE_INSTALL:append = " default-fru"
#
IMAGE_INSTALL:append = " ipmitool"
IMAGE_INSTALL:append = " phosphor-sel-logger"

# Phoenix OEM command
# Before production, please remove recipe "phoenix-ipmi-oem" and use phoenix-ipmi-oem-XXXXXXXprebuild
IMAGE_INSTALL:append = " phoenix-ipmi-oem"
#IMAGE_INSTALL:append:archercity = " phoenix-ipmi-oem-ast2600prebuild"
#IMAGE_INSTALL:append:brightoncity = " phoenix-ipmi-oem-ast2500prebuild"

# Intel recipe 
IMAGE_INSTALL:append = " smbios-mdrv2"
IMAGE_INSTALL:append = " phosphor-node-manager-proxy"
IMAGE_INSTALL:append = " settings"

#meta-ptec-tp only
#IMAGE_INSTALL:append = " ac-boot-check"

# Firmware update related
IMAGE_INSTALL:append = " phosphor-ipmi-blobs"
IMAGE_INSTALL:append = " phosphor-ipmi-flash"

# UEFI FW related (Build fail need to fix in future)
#IMAGE_INSTALL:append = " uefi-import-export"

# PEF feature related
IMAGE_INSTALL:append = " phoenix-dbus-monitor"
IMAGE_INSTALL:append = " phoenix-ipmi-pef"

# Networking full version and Ethernet related
IMAGE_INSTALL:append = " ethtool"

#BIOS configuration
IMAGE_INSTALL:append = " biosconfig-manager"
