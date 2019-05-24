#!/bin/bash

ROOT_DIR=`pushd ${0%/*} >/dev/null && pwd && popd >/dev/null`

source $ROOT_DIR/../env.ini
sh $ROOT_DIR/../preload.sh

set -e

for i in $ROOT_DIR/*; do
    if [ ! -d $i ]; then
        continue
    fi

	i=${i##*/}
	if [ -z $mode ]; then
		mode=$i
		continue
	fi
	mode="$mode|$i"
done

conf=$ROOT_DIR/$1
if [ ! -d "$conf" ]; then
	echo "usage: sh trex.sh ($mode)"
	exit 1
fi

export ODP_PKTIO_DPDK_PARAMS="-n 4 -w $NIC22 -w $NIC12 --file-prefix=sock"
export ODP_CONFIG_FILE=$conf/odp.conf
export OFP_CONF_FILE=$conf/ofp.conf
export ODP_SCHEDULER=scalable

rm -rf packets.txt

sed -e "s,{IP11},$IP11,g; \
s,{IP12},$IP12,g; \
s,{IP21},$IP21,g; \
s,{IP22},$IP22,g; \
" $conf/setting.cli > /tmp/setting.cli

cd $ROOT_DIR/../src/fp-sock && make && cd -

$ROOT_DIR/../src/fp-sock/sock -c 1 -f /tmp/setting.cli  -p 2>&1 | tee $ROOT_DIR/running.log