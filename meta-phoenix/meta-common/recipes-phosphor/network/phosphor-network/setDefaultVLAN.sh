#!/bin/bash

function createEth1Vlan(){
    busctl call xyz.openbmc_project.Network /xyz/openbmc_project/network \
        xyz.openbmc_project.Network.VLAN.Create VLAN su "eth1" 10
}


function setEth1VlanIPAddr(){
    busctl call  xyz.openbmc_project.Network /xyz/openbmc_project/network/eth1_10 \
        xyz.openbmc_project.Network.IP.Create IP ssys \
        "xyz.openbmc_project.Network.IP.Protocol.IPv4" "192.168.10.150" 24 "192.168.10.0"
}

createEth1Vlan
setEth1VlanIPAddr
exit 0;
