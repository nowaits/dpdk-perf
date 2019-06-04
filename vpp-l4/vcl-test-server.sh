#!/bin/bash

ROOT_DIR=`realpath $(dirname $0)`

source $ROOT_DIR/../env.ini
sh $ROOT_DIR/../preload.sh

export VCL_CONFIG=$ROOT_DIR/conf.conf

export LD_LIBRARY_PATH=$VPP_ROOT/vpp/lib:/host/lib64

BIN=$ROOT_DIR/../src/vcl-test-server/main
if [ ! -f $BIN ]; then
	sh $ROOT_DIR/../src/vcl-test-server/build.sh
fi

ps -ef|grep -w [b]in/vpp >/dev/null 2>&1
if [ $? != 0 ]; then
	echo -e "****** Please start VPP By: ${RED}sudo $ROOT_DIR/../vpp-l3/dut.sh${RES}"
	exit 1
fi

$BIN -D 5000