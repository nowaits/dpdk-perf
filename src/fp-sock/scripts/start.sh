#!/bin/sh

ROOT_DIR=`pushd ${0%/*} >/dev/null && pwd && popd >/dev/null`

export ODP_CONFIG_FILE=$ROOT_DIR/odp.conf
export OFP_CONF_FILE=$ROOT_DIR/ofp.conf
export ODP_SCHEDULER=scalable

rm -rf $ROOT_DIR/../packets.txt
$ROOT_DIR/../sock -c 1 -f $ROOT_DIR/setting.cli  -p 2>&1 | tee $ROOT_DIR/../running.log

