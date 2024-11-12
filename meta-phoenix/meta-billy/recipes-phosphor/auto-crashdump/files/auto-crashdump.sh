#!/bin/bash

echo "Start to monitor CATERR to trigger crashdump"

GPIO020=0x1e780020
while true
do
    # Judge GPIO H5 IERR = 0
    VAL=$(devmem ${GPIO020} 32)
    if [ $((${VAL} & 0x20000000)) -eq 0 ]; then
        echo "Trigger crashdump"
        busctl call com.intel.crashdump /com/intel/crashdump com.intel.crashdump.Stored GenerateStoredLog s "cpuError"
        sleep 30
    else
        sleep 1
    fi
done