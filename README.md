## 目录说明
- doc: 
  - 编译/配置相关说明
- dpdk: 
  - dpdk测试例子
- fp: 
  - ofp的测试例子
- trex: 
  - trex l2/3/loop 测试配置脚本
- pktgen:
  - pkgten l3/loop测试脚本
- vpp
  - l2/l3待测模式
  - pg发包模式
- vpp-l4: 
  - 测试VPP extern APP模式监听UDP数据包例子
- vpp-debug: 
  - sudo vpp-debug/dut.sh (tls|ikev2)
  - vpp以ap-packet模式运行，ikev2/ipsec/tls等例子
  - 可以从`<VPP-DIR>/src/scripts/vnet`目录下，复制相关例子到该目录，并修改后缀为`.cli`，运行测试
- src/armv71-pmu:
  - xilinx pmu模块测试例子
- src/dpdk-bind:
  - sudo src/dpdk-bind/dpdk-bind/sh
  - 通过shell脚本实现dpdk网卡，查看/绑定功能
- src/vpp-test
  - 编译： `src/vpp-test/build.sh all`
  - 运行：`src/vpp-test/build.sh test (longjmp|process|...)`
  - vpp相关的测试例子

## 性能测试
### 时延测试
> 测试工具可以使用trex或者pktgen<br/>
> 待测程序有：vpp, ofp, dpdk l2fwd
- 测试拓朴结构
    1. `nic11,nic12`在同一个wlan上;`nic21,nic22`在另一个vlan上;链路情况如拓朴图所示
    2. 四张网卡IP/MAC/PCI配置如：`evn.ini`文件中所示，使用时可以根据具体环境做相应改动
    ```
                 +---------+               
        +--------|   DUT   |--------+      
        |        +---------+        |      
    +---------+                +---------+
    |  NIC12  |                |  NIC22  |
    +---------+                +---------+
        |                            |      
    +---------+                +---------+
    |  NIC11  |                |  NIC21  |
    +---------+                +---------+
        |        +--------+        |
        +--------|  Trex  |--------+      
                 +--------+
    ```
#### 测试l2fwd
- sudo dpdk/l2fwd.sh
- sudo trex/trex.sh l2 
#### 测试VPP
- l2：
    - 启动VPP: sudo vpp/dut.sh l2
    - 启动trex: sudo trex/trex.sh l2
- l3:
    - 启动VPP: sudo vpp/dut.sh l3
    - 启动trex: sudo trex/trex.sh l3
