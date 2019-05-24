#!/bin/bash

ROOT_DIR=`pushd ${0%/*} >/dev/null && pwd && popd >/dev/null`

source $ROOT_DIR/../env.ini
sh $ROOT_DIR/../preload.sh

ifs_conf=$ROOT_DIR/$1-ifs.yaml
if [ ! -f "$ifs_conf" ]; then
	echo "usage: sh trex.sh (l2|l3|loop)"
	exit 1
fi

sed -e "s|{PCAP}|$ROOT_DIR/udp-6000.pcap|;" $ROOT_DIR/cap.yaml > /tmp/cfg.yaml

sed -e "s,{NIC11},$NIC11,g; \
s,{NIC12},$NIC12,g; \
s,{NIC21},$NIC21,g; \
s,{NIC22},$NIC22,g; \
" $ifs_conf > /tmp/$1-ifs.yaml

pushd $TREX >/dev/null

./t-rex-64 -f /tmp/cfg.yaml -m 1  -c 1  -p --cfg /tmp/$1-ifs.yaml -l 1000

popd >/dev/null