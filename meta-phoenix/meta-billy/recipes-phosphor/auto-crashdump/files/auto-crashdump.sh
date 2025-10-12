#!/bin/bash

echo "Start to monitor CATERR to trigger crashdump"

GPIO020=0x1e780020
counter=0
while true
do
    # Judge GPIO H5 IERR = 0
    VAL=$(devmem ${GPIO020} 32)
	if [ $((${VAL} & 0x20000000)) -eq 0 ]; then
		counter=$(($counter+1))
		sleep 0.1
		VAL=$(devmem ${GPIO020} 32)
		if [ $counter -eq 20 ]; then
			echo "Trigger IERR crashdump"
			busctl call xyz.openbmc_project.PHX.HostState.Monitor /xyz/openbmc_project/phx/hoststate/monitor xyz.openbmc_project.PHX.HostState.Monitor CrashdumpTrigger
			busctl call com.intel.crashdump /com/intel/crashdump com.intel.crashdump.Stored GenerateStoredLog s "cpuError"
			break
		fi
	else
		sleep 0.5
	fi
done
