#!/bin/bash

workbench=$1

if [ "$#" -ne 1 ]; then
    echo "Wrong number of arguments ::tux1.sh <workbench>"
else
    /etc/init.d/networking restart
    ifconfig eth0 up
    ifconfig eth0 172.16.${workbench}0.1/24
    ifconfig
    route add default gw 172.16.${workbench}0.254
    route -n
fi