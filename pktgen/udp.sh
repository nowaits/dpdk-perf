#!/bin/bash

ROOT_DIR=`realpath $(dirname $0)`

source $ROOT_DIR/../env.ini

sh $ROOT_DIR/../preload.sh

ARGS="-w $NIC11 -w $NIC21 --file-prefix=pktgen --create-uio-dev -l 1-3 -n 3  -- -P -m "2.1,3.0""

sed -e " \
s,{MAC12},$MAC12,g; \
s,{MAC22},$MAC22,g; \
s,{IP12},$IP12,g; \
s,{IP22},$IP22,g; \
" $ROOT_DIR/udp.conf > /tmp/udp.conf

$PKGTEN_HOME/app/${RTE_TARGET}/pktgen ${ARGS} -f /tmp/udp.conf