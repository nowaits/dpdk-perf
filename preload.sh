ROOT_DIR=`realpath $(dirname $0)`
source $ROOT_DIR/env.ini

if [ "$(id -u)" != 0 ]; then
  echo -e "${RED}Error${RES}: Please run as root (sudo etc.)"
  exit -1
fi

# dpdk
(
lsmod|grep -w uio >/dev/null 2>&1 || modprobe uio
lsmod|grep -w igb_uio >/dev/null 2>&1 || insmod $DPDK/$RTE_TARGET/kmod/igb_uio.ko
for pci in $NIC11 $NIC12 $NIC21 $NIC22; do
	ifs=`ls /sys/bus/pci/devices/0000:$pci/net/`
	if [ ! -z $ifs ]; then
		ifconfig $ifs down
	fi
	$DPDK/usertools/dpdk-devbind.py -b igb_uio $pci;
done
)>/dev/null 2>&1
