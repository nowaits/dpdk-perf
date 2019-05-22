#!/bin/bash

ROOT_DIR=`pushd ${0%/*} >/dev/null && pwd && popd >/dev/null`

source $ROOT_DIR/../env.ini
sh $ROOT_DIR/../preload.sh

ARGS="-w $NIC21 -w $NIC22 --file-prefix=pktgen --create-uio-dev -l 1-3 -n 3  -- -P -m "2.0,3.1""

$PKGTEN_HOME/app/${RTE_TARGET}/pktgen ${ARGS} -f $ROOT_DIR/loop.conf