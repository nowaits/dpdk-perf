#!/bin/bash

ROOT_DIR=`pushd ${0%/*} >/dev/null && pwd && popd >/dev/null`

source $ROOT_DIR/../env.ini
sh $ROOT_DIR/../preload.sh

export VCL_CONFIG=$ROOT_DIR/conf.conf

export LD_LIBRARY_PATH=$VPP_ROOT/vpp/lib:/host/lib64

BIN=$ROOT_DIR/../src/udp-echo/main

sh $ROOT_DIR/../src/udp-echo/build.sh

ps -ef|grep -w [b]in/vpp >/dev/null 2>&1
if [ $? != 0 ]; then
	echo -e "****** Please start VPP By: ${RED}sudo $ROOT_DIR/../vpp-l3/dut.sh${RES}"
	exit 1
fi

if [ x$1 = x"debug" ]; then
	gdb -ex "set confirm off" -ex "set args uri udp://0.0.0.0/5000 no-return" $BIN
else
	$BIN uri udp://0.0.0.0/5000 no-return
fi