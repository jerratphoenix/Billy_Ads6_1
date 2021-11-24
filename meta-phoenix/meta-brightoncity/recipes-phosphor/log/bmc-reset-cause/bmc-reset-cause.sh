#!/bin/sh

BMC_RESET_CAUSE_FILE="/var/log/bmc_reset_cause"

DBUS_ADD_BMC_RESET_CAUSE_SEL="xyz.openbmc_project.Logging.IPMI \
/xyz/openbmc_project/Logging/IPMI \
xyz.openbmc_project.Logging.IPMI \
IpmiSelAdd ssaybq \"BmcResetCauseLog\" \
/xyz/openbmc_project/sensors/specific/BMC_Reset \
3"

handle_reset_unspecified()
{
    EVENT_DATA="0x82 0x00 0xFF"
    busctl call $DBUS_ADD_BMC_RESET_CAUSE_SEL $EVENT_DATA true 0x0020

    # TODO: add redfish log here
}

handle_reset_by_ipmi_cold_reset_cmd()
{
    EVENT_DATA="0x82 0x01 0xFF"
    busctl call $DBUS_ADD_BMC_RESET_CAUSE_SEL $EVENT_DATA true 0x0020

    # TODO: add redfish log here
}

if [ ! -f "$BMC_RESET_CAUSE_FILE" ]; then
    echo "$BMC_RESET_CAUSE_FILE doesn't exist"
    value="unspecified"
else
    value=`cat $BMC_RESET_CAUSE_FILE`
fi

echo "Generate BMC Reset Cause event log"

case $value in
    1)
        handle_reset_by_ipmi_cold_reset_cmd
        ;;
    *)
        handle_reset_unspecified
        ;;
esac

rm -f $BMC_RESET_CAUSE_FILE

exit 0

