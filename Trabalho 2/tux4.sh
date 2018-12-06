#!/bin/bash

workbench=$1

if [ "$#" -ne 1 ]; then
    echo "Wrong number of arguments ::tux4.sh <workbench>"
else
    /etc/init.d/networking restart
    ifconfig eth0 up
    ifconfig eth0 172.16.${workbench}0.1/24
    ifconfig eth1 up
    ifconfig eth1 172.16.${workbench}1.253/24
    ifconfig
    echo 1 > /proc/sys/net/ipv4/ip_forward
    echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
fi