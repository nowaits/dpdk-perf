#!/bin/sh

ROOT_DIR=`realpath $(dirname $0)`

source $ROOT_DIR/../../env.ini
export RTE_SDK=$DPDK

[ ! -z $RTE_OUTPUT ] || export RTE_OUTPUT=$ROOT_DIR/bin
[ ! -z $RTE_TARGET ] || export RTE_TARGET=x86_64-native-linuxapp-gcc
[ ! -z $APP ] || export APP=portid

set -e

usage () {
  echo "Usage :"
  echo "    sh ${0##*/}"
  echo '        -c [dpdk-driver] [pci id]'
  echo '        -t'
  exit
}

if [ "$(id -u)" != 0 ]; then
  echo 'Error: Please run as root (sudo etc.)'
  exit -1
fi

declare -a nics
declare -a dpdk_nics

DPDK_PORT_BIN=$RTE_OUTPUT/portid

function parse_dpdk_nics() {
	IFS=$'\n'
	for LINE in `$DPDK_PORT_BIN --log-level=critical`; do
		IFS=$' '
		local pci
		local mac
		local dri
		read pci mac dri<<<$LINE
		len=${#dpdk_nics[*]}
		dpdk_nics[$((len+1))]=$pci
		dpdk_nics[$((len+2))]=`tr [A-Z] [a-z] <<< $mac`
		dpdk_nics[$((len+3))]=$dri
	done
}

function mac() {
	mac=-
	ifs=-
	dpdk="false"

	# 1. try kernel
	for d in /sys/bus/pci/devices/$1/net/*; do
		if [ ! -d $d ]; then
			break;
		fi

		ifs=${d##*/}
		mac=`cat $d/address`
	done

	# 2. dpdk

	if [ $mac == "-" ]; then
		for ((i=1;i<=${#dpdk_nics[@]};i+=3)); do
			if [ ${dpdk_nics[$i]} == $1 ]; then
				mac=${dpdk_nics[$i+1]}
				ifs=${dpdk_nics[$i+2]}
				dpdk="true"
			fi
		done
	fi

	echo $mac $ifs $dpdk
}

function nics_info() {
	IFS=$' '
	for d in /sys/bus/pci/devices/*
	do
		local pci
		local dri
		local mod
		local mac
		local ifs
		local dpdk
		pci=${d##*/}

		read pci dri mod <<< `lspci -Dks $pci | awk -F '\n' '{
			split($1,a," " );
			if(NR==1){
				if (index($1, "Ethernet") != 0){pci=a[1]}
				else {exit}
			}
			if(NR==3)dri=a[length(a)]
			if(NR==4)mod=a[length(a)]
			}END{if(pci)print(pci, dri, mod)}'`
		if [ -z $pci ]; then
			continue;
		fi
		read mac ifs dpdk<<< `mac $pci`

		len=${#nics[*]}
		nics[$((len+1))]=$pci
		nics[$((len+2))]=$mac
		nics[$((len+3))]=$mod
		nics[$((len+4))]=$dri
		nics[$((len+5))]=$dpdk
		nics[$((len+6))]=$ifs
	done
}

function show_table() {
	printf "%-12s\t%-17s\t%-10s\t%-10s\t%-10s\t%-10s\n" PCI MAC MODULE DRIVER ISDPDK "IFS or DPDK DRIVER"
	printf "%100s\n" |tr " " "-"
	for ((i=1;i<=${#nics[@]};i+=6)); do
		printf "%-12s\t%-17s\t%-10s\t%-10s\t%-10s\t%-10s\n" \
			${nics[$(($i))]} ${nics[$(($i+1))]} ${nics[$(($i+2))]} \
			${nics[$(($i+3))]} ${nics[$(($i+4))]} ${nics[$(($i+5))]}
	done
}

function do_change_bind() {
	pci=$1
	dri=$2
	old_dri=$3

	if [ $dri == $old_dri ]; then
		echo no need to rebind!
		return
	fi

	if [ ! -d /sys/bus/pci/drivers/$old_dri/$pci -o ! -d /sys/bus/pci/drivers/$dri ]; then
		printf "\t$pci not in dir: /sys/bus/pci/drivers/$old_dri \nOR\n\tPath: /sys/bus/pci/drivers/$dri not exist!\n"
		exit 2
	fi

	echo $pci >> /sys/bus/pci/drivers/$old_dri/unbind

	if [ -f /sys/bus/pci/devices/$pci/driver_override ]; then
		echo $dri > /sys/bus/pci/devices/$pci/driver_override
	elif [ -f /sys/bus/pci/drivers/%pci/new_id ]; then
		echo "not support!"
		false
	fi

	echo $pci >> /sys/bus/pci/drivers/$dri/bind
}

function init_dpdk() {
	lsmod|grep -w uio >/dev/null 2>&1 || modprobe uio
	lsmod|grep -w igb_uio >/dev/null 2>&1 || insmod $RTE_SDK/$RTE_TARGET/kmod/igb_uio.ko

	[ -f $DPDK_PORT_BIN ] || make -C $ROOT_DIR/portid $APP
}

function do_loadinfo() {
	init_dpdk
	parse_dpdk_nics
	nics_info
}

#

case $1 in
-c)
hr=1
do_loadinfo
for ((i=1;i<=${#nics[@]};i+=6)); do
	if [ ${nics[$i]} == $2 ]; then
		do_change_bind $2 $3 ${nics[$(($i+3))]}
		echo "success"
		hr=0
	fi
done
exit $hr
;;
-t)
do_loadinfo
show_table;;
*) usage
esac
