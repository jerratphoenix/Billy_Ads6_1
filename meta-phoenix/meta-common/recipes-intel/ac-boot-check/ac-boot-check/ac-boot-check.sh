#!/bin/sh

if [[ `cat /proc/cmdline` =~ "resetreason=0x11" ]]
then
    busctl set-property xyz.openbmc_project.Intel_Settings /xyz/openbmc_project/control/host0/ac_boot xyz.openbmc_project.Common.ACBoot ACBoot s "True"
else
    if [[ `cat /proc/cmdline` =~ "resetreason=power" ]]
    then
        busctl set-property xyz.openbmc_project.Intel_Settings /xyz/openbmc_project/control/host0/ac_boot xyz.openbmc_project.Common.ACBoot ACBoot s "True"
    else
        busctl set-property xyz.openbmc_project.Intel_Settings /xyz/openbmc_project/control/host0/ac_boot xyz.openbmc_project.Common.ACBoot ACBoot s "False"
    fi
fi
