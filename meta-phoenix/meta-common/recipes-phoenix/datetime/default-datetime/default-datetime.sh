#!/bin/sh
# Copyright (c) 2022 Phoenix Technologies Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

[ ! -x /sbin/hwclock ] && exit 1

find_rtc_dev() {
    for dir in $(find /sys/class/rtc/*); do
        if [ "$(grep -q "$1" "${dir}/name"; echo $?)" = "0" ];then
            source ${dir}/uevent
            echo "$DEVNAME"
            return
        fi
    done
}

ASTRTC_NAME=$(find_rtc_dev "aspeed")
ASTRTC_DEV=/dev/$ASTRTC_NAME

if [ -z "$ASTRTC_NAME" ] ;then
    echo "No find aspeed rtc"
    exit 0
fi

if [ ! -e "$ASTRTC_DEV" ] ;then
    echo "No aspeed rtc device node"
    exit 0
fi

if [ "$(hwclock -r -f $ASTRTC_DEV > /dev/null; echo $?)" = "0" ];then
    # Check if the time is in 1970
    if [ "$(hwclock -r -f $ASTRTC_DEV| cut -d" " -f 6)" != "1970" ]; then
        echo "ASPEED RTC($ASTRTC_DEV) already have default date and time."
        echo "Setting the System Clock using the ASPEED RTC($ASTRTC_DEV)"
        hwclock --hctosys -f "$ASTRTC_DEV"
        echo "System Clock is $(date +"%Y/%m/%d %H:%M:%S")."
        exit 0
    fi
fi

PCHRTC_NAME=$(find_rtc_dev "pch")
PCHRTC_DEV=/dev/$PCHRTC_NAME

if [ -z "$PCHRTC_NAME" ] ;then
    echo "No find pch rtc"
    exit 0
fi

if [ ! -e "$PCHRTC_DEV" ] ;then
    echo "No pch rtc device node"
    exit 0
fi

if [ "$(hwclock -r -f $PCHRTC_DEV > /dev/null; echo $?)" != "0" ];then
    echo "PCH RTC($PCHRTC_DEV) access failed."
    exit 0
fi

echo "Setting the System Clock using the PCH RTC($PCHRTC_DEV)"
hwclock --hctosys -f "$PCHRTC_DEV"
echo "System Clock is $(date +"%Y/%m/%d %H:%M:%S")."

echo "Setting the ASPEED RTC($ASTRTC_DEV) using the System Clock."
hwclock --systoh -f "$ASTRTC_DEV"
