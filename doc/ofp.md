### 准备
- ubuntu
    - apt install libconfig++-dev autoconf libtool libssl-dev libtool-bin
- centos 7
    - yum install libatomic libconfig-devel

### 编译
- odp
```
# dpdk
export RTE_SDK=$PWD
export RTE_TARGET=x86_64-native-linuxapp-gcc
make config T=${RTE_TARGET} O=${RTE_TARGET}
pushd ${RTE_TARGET}

#To use I/O without DPDK supported NIC's enable pcap pmd:
sed -ri 's,(CONFIG_RTE_LIBRTE_PMD_PCAP=).*,\1y,' .config
popd

#Build DPDK
make -j build O=${RTE_TARGET} EXTRA_CFLAGS="-fPIC"
make -j install O=${RTE_TARGET} DESTDIR=${RTE_TARGET}

# load ko
modprobe uio
insmod ${RTE_TARGET}/kmod/igb_uio.ko

# bind nic
./usertools/dpdk-devbind.py -s
./usertools/dpdk-devbind.py -b igb_uio 0000:01:00.0
./usertools/dpdk-devbind.py -b igb_uio 0000:02:00.0
		
# odp

./configure --enable-debug --enable-debug-print \
    --enable-helper-debug-print --prefix=/var/opt/ofp \
    --enable-pcapng-support \
    --enable-dpdk --enable-dpdk-zero-copy \
    --with-dpdk-path=/home/dev/code/dpdk/${RTE_TARGET}/usr/local/

make && sudo make install

# test
sudo ./test/performance/odp_l2fwd -i 0,1 -c 2 -m 0
```
- ofp
```
./configure --prefix=/var/opt/ofp \
    --with-odp=/var/opt/ofp \
    --enable-sp  --enable-debug --enable-cli

// ofp_init_global()之前添加odp_schedule_config(NULL);
make

./example/fpm/fpm -i ens33

# cat a.cli
# debug 0
# loglevel set debug
# ifconfig fp0 192.168.22.134/24

sudo ./example/fpm/fpm -i 0,1 -c 2 -f a.cli
```

### docker中运行
- lib
```
docker run -it --rm -v /var/opt/ofp:/host/ofp -v /usr/lib64:/host/lib64 $CONTAINERNAME /bin/bash
```
- 添加共享内存： `-v /dev/shm:/dev/shm`
- 运行库设置 `LD_LIBRARY_PATH=/host/ofp/lib:/host/lib64`