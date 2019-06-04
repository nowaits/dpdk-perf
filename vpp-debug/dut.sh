#!/bin/sh

ROOT_DIR=`realpath $(dirname $0)`

source $ROOT_DIR/../env.ini

for i in $ROOT_DIR/*.cli; do
	i=${i%.*}
	i=${i##*/}
	if [ -z $mode ]; then
		mode=$i
		continue
	fi
	mode="$mode|$i"
done

conf=$ROOT_DIR/$1.cli
if [ ! -f "$conf" ]; then
	echo "usage: sh trex.sh ($mode)"
	exit 1
fi

CLI=/tmp/vpp-cli.txt
CONF=/tmp/cli.conf

sed -e " \
s,{IP0},$IP0,g; \
s,{MAC0},$MAC0,g; \
s,{IP12},$IP12,g; \
s,{IP12},$IP12,g; \
s,{IP22},$IP22,g;" \
$conf > $CLI

sed -e " \
s,{START_CLI},$CLI,; \
s,{CERT_PATH},$ROOT_DIR/../ca-certificates.crt,; \
s,{NIC11},$NIC11,g; \
s,{NIC21},$NIC21,g; \
s,{NIC12},$NIC12,g; \
s,{NIC22},$NIC22,g; \
s/#.*//;" \
$ROOT_DIR/startup.conf > $CONF

if [ x$2 != "xgdb" ]; then
	$VPP_ROOT/vpp/bin/vpp -c $CONF
else
	gdb -ex "set args -c $CONF" -x $ROOT_DIR/gdb-cmds $VPP_ROOT/vpp/bin/vpp
fi
