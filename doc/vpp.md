### ![](https://wiki.fd.io/images/0/0e/Fdio_logo_resized.png)
- [wiki](https://wiki.fd.io/)
- [doc](https://fdio-vpp.readthedocs.io)
- [Continuous Performance Trending](https://docs.fd.io/csit/master/trending/index.html)
- [CSIT](https://wiki.fd.io/view/CSIT)
- [vpp文件](https://wiki.fd.io/view/Presentations)
- [邮件列表](https://lists.fd.io/g/main/join)

---

### [编译](https://wiki.fd.io/view/VPP/Pulling,_Building,_Running,_Hacking_and_Pushing_VPP_Code)
- git clone git@github.com:FDio/vpp.git
- ==首次安装系统依赖==：`build-root/vagrant/build.sh`
    - 从``下载dpdk，nasm等压缩包到`~/Downloads`目录下加快安装时间
- 编译：make build
- 运行：`make run`
- 调试模式运行：`make debug`

### 添加插件模板
- cd src/plugins
- `../../extras/emacs/make-plugin.sh`
    - plugin name: test
    - dual
- 调试：
    ```
    // 编译
    make build
    // 启用gdb调试
    make debug
    gdb# 
        b test_node_avx2
        r
    vppcli#
        // 
        create host-interface name ens33 hw-addr 00:0c:29:69:66:9a
        set interface state host-ens33 up
        set interface ip address host-ens33 192.168.22.200/24
        test http server uri tcp://0.0.0.0/80
        test enable-disable host-ens33
    // 验证
    wget http://192.168.22.200/sh/run
    ```

### 功能测试配置 src/scripts/vnet
---
## 基本处理
- vector基本处理过程
    
    ```
    // 向vlib_main_t::node_main::pending_frames添加处理
    void vlib_put_next_frame(vlib_main_t *vm,
        vlib_node_runtime_t *r,
        u32 next_index, u32 n_vectors_left) {
            ...
            vec_add2(nm->pending_frames, p, 1);
            ...
        }
    
    vlib_main_or_worker_loop() {
        ..
        while(1) {
            ...
            // 遍历各个数据源，生成vector，
            // 并添加到历nde_main->pending_frames中
            vec_foreach(n, nm->nodes_by_type[VLIB_NODE_TYPE_INPUT]) {
                cpu_time_now = dispatch_node(vm, n,
                            VLIB_NODE_TYPE_INPUT,
                            VLIB_NODE_STATE_POLLING,
                            /* frame */ 0,
                            cpu_time_now);
            }
            ...
            // 遍历nde_main->pending_frames
            // 实现vector在有向图中的流动
            for (i = 0; i < _vec_len(nm->pending_frames); i++) {
                    // 处理过程中，各个节点
                    // 可以通过vlib_put_next_frame
                    // 向nm->pending_frames动态添加
                    cpu_time_now = dispatch_pending_node(vm, i, cpu_time_now);
                }
            // 清空，有向图结束处理
            _vec_len(nm->pending_frames) = 0;
            ...
        }
    }
    ```

- 调试验证：
    ```
    make debug
    
    gdb#
    b vlib_put_next_frame
    b dispatch_pending_node
    display *((vec_header_t *)(vm->node_main.pending_frames)-1)
    
    Breakpoint 15, vlib_main_or_worker_loop (vm=0x7fffb523d3c0, is_main=0) at /home/dev/code/vpp/src/vlib/main.c:1756
    1756                cpu_time_now = dispatch_pending_node(vm, i, cpu_time_now);
    3: i = 0
    2: *((vec_header_t *)(vm->node_main.pending_frames)-1) = {len = 1, dlmalloc_header_offset = 0, 
      vector_data = 0x7fffb538dadc "\357\001"}
    (gdb) 
    Continuing.
    
    Breakpoint 15, vlib_main_or_worker_loop (vm=0x7fffb523d3c0, is_main=0) at /home/dev/code/vpp/src/vlib/main.c:1756
    1756                cpu_time_now = dispatch_pending_node(vm, i, cpu_time_now);
    3: i = 1
    2: *((vec_header_t *)(vm->node_main.pending_frames)-1) = {len = 4, dlmalloc_header_offset = 0, 
      vector_data = 0x7fffb538dadc "\357\001"}
    (gdb) 
    Continuing.
    
    Breakpoint 15, vlib_main_or_worker_loop (vm=0x7fffb523d3c0, is_main=0) at /home/dev/code/vpp/src/vlib/main.c:1756
    1756                cpu_time_now = dispatch_pending_node(vm, i, cpu_time_now);
    3: i = 2
    2: *((vec_header_t *)(vm->node_main.pending_frames)-1) = {len = 5, dlmalloc_header_offset = 0, 
      vector_data = 0x7fffb538dadc "\357\001"}
    (gdb) 
    Continuing.
    
    Breakpoint 15, vlib_main_or_worker_loop (vm=0x7fffb523d3c0, is_main=0) at /home/dev/code/vpp/src/vlib/main.c:1756
    1756                cpu_time_now = dispatch_pending_node(vm, i, cpu_time_now);
    3: i = 3
    2: *((vec_header_t *)(vm->node_main.pending_frames)-1) = {len = 6, dlmalloc_header_offset = 0, 
      vector_data = 0x7fffb538dadc "\357\001"}
    (gdb) 
    Continuing.
    ```

### doc
- [what's vpp](https://wiki.fd.io/view/VPP/What_is_VPP%3F)
- [vpp用户上传的资料文档文件](https://wiki.fd.io/view/Presentations)
    - https://wiki.fd.io/view/File:Fd.io_vpp_overview_29.03.17.pptx#file
    - https://fd.io/wp-content/uploads/sites/34/2017/07/FDioVPPwhitepaperJuly2017.pdf
    - https://fd.io/wp-content/uploads/sites/34/2018/02/performance_analysis_sw_data_planes_dec21_2017-1.pdf
    - https://fd.io/wp-content/uploads/sites/34/2018/07/fd-io-Datasheet_3.22.pdf
    - https://fd.io/wp-content/uploads/sites/34/2019/03/benchmarking_sw_data_planes_skx_bdx_mar07_2019.pdf
- [fd.io technology](https://fd.io/technology/)
- [fd.io about](https://fd.io/about/)
- [FD.io——助你创新更高效、更灵活的报文处理方案](https://www.sdnlab.com/23064.html)
- [Validating Cisco's NFV Infrastructure Pt. 1
](https://www.lightreading.com/nfv/nfv-tests-and-trials/validating-ciscos-nfv-infrastructure-pt-1/d/d-id/718684?page_number=8)

#### [Cisco Express Forwarding (CEF)](https://www.cisco.com/c/en/us/support/docs/routers/12000-series-routers/47321-ciscoef.html?dtid=osscdc000283)
- https://yq.aliyun.com/articles/472840
- https://en.wikipedia.org/wiki/Cisco_Express_Forwarding
- [Cisco Express Forwarding Overview](https://www.cisco.com/c/en/us/td/docs/ios/12_2/switch/configuration/guide/fswtch_c/xcfcef.html)
- [CISCO CEF(Cisco Express Forwarding,Cisco特快交换](https://yq.aliyun.com/articles/452086?spm=a2c4e.11153940.0.0.5d9057ceTuYKEV)

#### 其它
- [VPP/How To Optimize Performance (System Tuning)](https://wiki.fd.io/view/VPP/How_To_Optimize_Performance_(System_Tuning))
- [VPP/Command-line Interface (CLI) Guide](https://wiki.fd.io/view/VPP/Command-line_Interface_(CLI)_Guide)
- [VPP/Software Architecture](https://wiki.fd.io/view/VPP/Software_Architecture)
- [VPP/Using VPP In A Multi-thread Model](https://wiki.fd.io/view/VPP/Using_VPP_In_A_Multi-thread_Model)
- [VPP/NAT](https://wiki.fd.io/view/VPP/NAT)
- [VPP/Progressive VPP Tutorial](https://wiki.fd.io/view/VPP/Progressive_VPP_Tutorial)
- [vpp developers doc](https://fdio-vpp.readthedocs.io/en/latest/gettingstarted/developers/)
- [VPP/HostStack/SessionLayerArchitecture![](https://wiki.fd.io/images/thumb/e/e2/Session_API.png/1100px-Session_API.png)](https://wiki.fd.io/view/VPP/HostStack/SessionLayerArchitecture)
- [VPP/HostStack/TLS![](https://wiki.fd.io/images/6/68/TLS_Application_Architecture.png)](https://wiki.fd.io/view/VPP/HostStack/TLS)
- [vcl-ldpreload](https://github.com/FDio/vpp/tree/master/extras/vcl-ldpreload)
- [The VPP Object Model (VOM) library](https://wiki.fd.io/view/VPP/VOM)
