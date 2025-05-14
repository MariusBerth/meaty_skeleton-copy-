#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom veldos.iso\
       	-device rtl8139,netdev=n1,mac=ff:ff:ff:ff:ff:ff \
	-netdev socket,id=n1,listen=:1234 &

sleep 1s

qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom veldos.iso\
       	-device rtl8139,netdev=n2,mac=ff:ff:ff:ff:ff:ff \
	-netdev socket,id=n2,connect=127.0.0.1:1234

