#!/bin/bash

staticIP=" "
line=" "
var=0
#while loop go through Static IP setting 2 times; ensuring IP is obtained
while [ $var != 2 ]
do
	ipmitool lan set 1 ipsrc static
	sleep 4
	ipmitool lan set 1 ipaddr 192.168.10.150
	sleep 4
	ipmitool lan set 1 netmask 255.255.255.0
	sleep 4
	ipmitool lan set 1 defgw ipaddr 192.168.10.1
	sleep 5
	line=$(ip a | wc -l)

	while [ $line != 0 ]
		do
#    		echo $(ip a | awk 'NR=='$line'{print $2}')
			staticIP=$(ip a | awk 'NR=='$line'{print $2}')
			if [[ $staticIP == *"192.168.10.150"* ]]; then
			echo "Static IP has been set and found"
			var=$((var+1))
			break
			fi
	
		line=$((line-1))
		#sleep 1
	done
done
