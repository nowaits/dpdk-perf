#/bin/bash

ROOT_DIR=`realpath $(dirname $0)`

source $ROOT_DIR/../env.ini
sh $ROOT_DIR/../preload.sh

$DPDK/examples/l2fwd/x86_64-native-linuxapp-gcc/l2fwd -c 0x60 -n 4 \
	-w $NIC22 -w $NIC12 --file-prefix=l2fwd -- -q 1 -p 0x3 --no-mac-updating -T 1