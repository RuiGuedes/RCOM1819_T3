#!/bin/bash

workbench=$1

if [ "$#" -ne 1 ]; then
    echo "Wrong number of arguments ::tux2.sh <workbench>"
else
    /etc/init.d/networking restart
    ifconfig eth0 up
    ifconfig eth0 172.16.${workbench}1.1/24
    ifconfig
    route add -net 172.16.${workbench}0.0/24 gw 172.16.${workbench}1.253
    route -n
fi