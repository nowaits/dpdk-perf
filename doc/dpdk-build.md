### 准备
- [Cross compile DPDK for ARM64](https://doc.dpdk.org/guides/linux_gsg/cross_build_dpdk_for_arm64.html)

- 升级gcc 4.7
    - apt-get install gcc-4.7
    - https://codeload.github.com/ninja-build/ninja/zip/v1.9.0
    -  apt-get install linux-kernel-headers kernel-package

### 编译：
```
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

make -j examples T=${RTE_TARGET}

mkdir -p /mnt/huge
mount -t hugetlbfs nodev /mnt/huge
echo 20148 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages

# 释放空间
rm -rf /dev/hugepages/0
rm -rf /dev/shm/0
```
### test
- testpmd
    ```
    ${RTE_TARGET}/app/testpmd -c7 -n3 --vdev=net_pcap0,iface=ens32 --vdev=net_pcap1,iface=ens33 -- -i --nb-cores=2 --nb-ports=2 --total-num-mbufs=2048
    > show port stats all
    > start tx_first
    > stop
    ```
- [l2fwd](http://dpdk.org/doc/guides/linux_gsg/build_dpdk.html)
    ```
    modprobe uio
    insmod ${RTE_TARGET}/kmod/igb_uio.ko
    ```
    - DPDK 17.05
        ```
        ./usertools/dpdk-devbind.py -s
        ./usertools/dpdk-devbind.py -b igb_uio 0000:01:00.0
        ./usertools/dpdk-devbind.py -b igb_uio 0000:02:00.0
        
        ./examples/l2fwd/${RTE_TARGET}/l2fwd -c 0x3 -n 4 -b 0b:00.0 -b 04:00.0 -b 13:00.0 -b 1b:00.0 --file-prefix=l2fwd -- -q 1 -p 3 --no-mac-updating
        ```
	- DPDK 1.7.1
        ```
        ./tools/dpdk_nic_bind.py --status
        ./tools/dpdk_nic_bind.py -b igb_uio 0000:02:01.0
        
        ./examples/l2fwd/${RTE_TARGET}/l2fwd -c 0xf -n 4 -- -q 4 -p ffff
        ```

### FAQ

- EAL: No free hugepages reported in hugepages-1048576kB
    ```
    cat /proc/meminfo | grep Huge
    umount /mnt/huge
    rm -rf /mnt/huge
    mkdir -p /mnt/huge
    mount -t hugetlbfs nodev /mnt/huge
    
    echo 2048 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
    cat /proc/meminfo | grep Huge
    ```
- 	EAL: Error reading from file descriptor 21: Input/output error；
    ```
    diff --git a/lib/librte_eal/linuxapp/igb_uio/igb_uio.c b/lib/librte_eal/linuxapp/igb_uio/igb_uio.c
    index d1ca26e..c46a00f 100644
    --- a/lib/librte_eal/linuxapp/igb_uio/igb_uio.c
    +++ b/lib/librte_eal/linuxapp/igb_uio/igb_uio.c
    @@ -505,14 +505,11 @@  igbuio_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
     		}
     		/* fall back to INTX */
     	case RTE_INTR_MODE_LEGACY:
    -		if (pci_intx_mask_supported(dev)) {
    -			dev_dbg(&dev->dev, "using INTX");
    -			udev->info.irq_flags = IRQF_SHARED;
    -			udev->info.irq = dev->irq;
    -			udev->mode = RTE_INTR_MODE_LEGACY;
    -			break;
    -		}
    -		dev_notice(&dev->dev, "PCI INTX mask not supported\n");
    +		dev_dbg(&dev->dev, "using INTX");
    +		udev->info.irq_flags = IRQF_SHARED;
    +		udev->info.irq = dev->irq;
    +		udev->mode = RTE_INTR_MODE_LEGACY;
    +		break;
     		/* fall back to no IRQ */
     	case RTE_INTR_MODE_NONE:
     		udev->mode = RTE_INTR_MODE_NONE;
    ```
