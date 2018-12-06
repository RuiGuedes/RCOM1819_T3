#!/bin/bash

workbench=$1

if [ "$#" -ne 1 ]; then
    echo "Wrong number of arguments ::tux2.sh <workbench>"
else
    /etc/init.d/networking restart
    ifconfig eth0 up
    ifconfig eth0 172.16.${workbench}1.1/24
    ifconfig
    route add default gw 172.16.${workbench}1.254
    route add -net 172.16.${workbench}0.0/24 gw 172.16.${workbench}1.253
    route -n
    echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects
    echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects
fi