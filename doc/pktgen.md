### 编译

- 依赖：
    - yum install readline-devel -y
    - install lua
        - `curl -R -O http://www.lua.org/ftp/lua-5.3.5.tar.gz`
        - `tar zxf lua-5.3.5.tar.gz`
        - `cd lua-5.3.5 && make linux && make install`

- dpdk
    - export RTE_SDK=/home/dev/code/dpdk-19.02
    - export RTE_TARGET=x86_64-native-linuxapp-gcc

- make

### 启动

- dpdk env
    - `modprobe uio`
    - `ismod <DPDK-dir>/${RTE_TARGET}/kmod/igb_uio.ko`
    - `python <DPDK-dir>/usertools/dpdk-devbind.py -b igb_uio eth0`

- `cat conf`
    
    ```
    #
    set all proto icmp
    set all size 1024
    set all dport 5678
    set all src ip 192.168.101.200
    set all dst ip 192.168.101.1
    set all dst mac 00:50:56:c0:00:02
    
    #
    set all rate 1
    set all jitter 10
    set all pattern none
    
    range all proto udp
    range all size 64 64 1500 1
    range all src ip 192.168.101.10 192.168.101.10 192.168.101.254 0.0.0.1
    range all src port 1000 1000 60000 1
    range all dst ip 192.168.101.1 192.168.101.1 192.168.101.1 0.0.0.1
    range all dst mac 00:50:56:c0:00:02 00:50:56:c0:00:02 00:50:56:c0:00:02 00:00:00:00:00:00
    
    enable all random
    enable all range
    enable all latency
    
    start all
    page latency
    clr
    #off
    
    ```

- `<PKTGEN-dir>/app/${RTE_TARGET}/pkgten -b 02:02.0 -b 02:05.0 --file-prefix=pktgen --create-uio-dev -l 1-3 -n 3  -- -P -m "2.0,3.1" -f conf `

### 运行状态
- main: page main
- range: page range
- 时延: page latency
- 网卡队列： page stats
- xstats: page xstats