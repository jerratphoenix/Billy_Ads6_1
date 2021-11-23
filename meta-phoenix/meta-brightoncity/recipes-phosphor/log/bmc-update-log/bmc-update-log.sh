#!/bin/sh

if test -e /etc/bmc_update_complete
then
    echo "Generate BMC firmware update event log"
    busctl call xyz.openbmc_project.Logging.IPMI \
    /xyz/openbmc_project/Logging/IPMI \
    xyz.openbmc_project.Logging.IPMI \
    IpmiSelAdd \
    ssaybq \
    "BMC firmware update event log" \
    /xyz/openbmc_project/sensors/specific/BMC_FW_update \
    3 0xC1 0x01 0xFF \
    true \
    0x0020

    rm -f /etc/bmc_update_complete
fi
