#!/bin/bash

interface=$1
workbench=$2
vlan=$3
subnet=$4

if [ "$#" -ne 3 ]; then
    echo "Wrong number of arguments :: configtux.sh <interface> <workbench> <vlan> <subnet>"
else
    /etc/init.d/networking restart
    ifconfig eth$interface up
    ifconfig eth$interface 172.16.${workbench}${vlan}.$subnet/24
    ifconfig
fi
