#include "envs.h"

static int init_sock(uint32_t ip, uint16_t port)
{
    int fd = 0;
    fd = ofp_socket(OFP_AF_INET, OFP_SOCK_DGRAM, 0);
    if (fd == -1) {
        ERROR(Faild to create socket);
        return -1;
    }

    struct ofp_sockaddr_in addr = {0};

    addr.sin_len = sizeof(struct ofp_sockaddr_in);
    addr.sin_family = OFP_AF_INET;
    addr.sin_port = odp_cpu_to_be_16(port);
    addr.sin_addr.s_addr = ip;

    if (ofp_bind(fd, (const struct ofp_sockaddr *)&addr,
            sizeof(struct ofp_sockaddr_in)) == -1) {
        ERROR(Faild to bind socket);
        return -1;
    }

    OFP_INFO("UDP SOCKET SETUP SUCCESS.");
    return fd;
}

static int send_data(int fd, uint32_t ip, uint16_t port, const char *buf)
{
    struct ofp_sockaddr_in dest_addr = {0};

    dest_addr.sin_len = sizeof(struct ofp_sockaddr_in);
    dest_addr.sin_family = OFP_AF_INET;
    dest_addr.sin_port = port;
    dest_addr.sin_addr.s_addr = ip;

    if (ofp_sendto(fd, buf, strlen(buf), 0,
            (struct ofp_sockaddr *)&dest_addr,
            sizeof(dest_addr)) == -1) {
        ERROR(Faild to send data);
        return -1;
    }

    return 0;
}

static int recvfrom_udp(int fd, uint32_t *ip, uint16_t *port, char *buf, size_t blen)
{
    struct ofp_sockaddr_in addr = {0};
    ofp_socklen_t addr_len = 0;

    int len = ofp_recvfrom(fd, buf, blen, 0,
            (struct ofp_sockaddr *)&addr, &addr_len);

    if (len == -1) {
        ERROR(Faild to rcv data);
        return -1;
    }

    if (addr_len != sizeof(addr)) {
        OFP_ERR("Faild to rcv source address: %d (errno = %d)\n",
            addr_len, ofp_errno);
        return -1;
    }

    *ip = addr.sin_addr.s_addr;
    *port = addr.sin_port;

    buf[len] = 0;
    OFP_INFO("%s:%d Data (len = %d) was received.\n",
        ofp_print_ip_addr(*ip),
        odp_be_to_cpu_16(*port),
        len);
    {
        int i = 0;
        static int c = 0;
        if (c++ % 1000000 == 0) {
            for (i; i < len; i ++) {
                printf("%c", buf[i]);
            }
            printf("\n---------------------------\n");
        }
    }

    return 0;
}

static void notify(union ofp_sigval sv)
{
    struct ofp_sock_sigval *ss = sv.sival_ptr;
    int s = ss->sockfd;
    int event = ss->event;
    odp_packet_t pkt = ss->pkt;
    int n;
    struct ofp_sockaddr_in addr;
    ofp_socklen_t addr_len = sizeof(addr);

    /*
     * Only receive events are accepted.
     */
    if (event != OFP_EVENT_RECV) {
        return;
    }

    /*
     * L2, L3, and L4 pointers are as they were when the packet was
     * received. L2 and L3 areas may have ancillary data written
     * over original headers. Only L4 pointer and data after that is valid.
     * Note that short packets may have padding. Thus odp_packet_length()
     * may give wrong results. Sender information is over L2 area.
     * It is best to use function ofp_udp_packet_parse() to
     * retrieve the information. It also sets the packet's data pointer
     * to payload and removes padding from the end.
     */
    uint8_t *p = ofp_udp_packet_parse(pkt, &n,
            (struct ofp_sockaddr *)&addr,
            &addr_len);
    /* Pointer and length are not used here. */
    (void)p;
    (void)n;
    {
#if 0
        OFP_INFO("%s:%d Data (len = %d) was received.\n",
            ofp_print_ip_addr(*ip),
            odp_be_to_cpu_16(*port),
            len);
#endif
        {
            int i = 0;
            static int c = 0;
            if (c++ % 1000000 == 0) {
                for (i; i < n; i ++) {
                    printf("%c", p[i]);
                }
                printf("\n---------------------------\n");
            }
        }
    }

    /*
     * There are two alternatives to send a respond.
     */
#if 1
    /*
     * Reuse received packet.
     * Here we want to send the same payload back prepended with "ECHO:".
     */
    odp_packet_push_head(pkt, 5);
    memcpy(odp_packet_data(pkt), "ECHO:", 5);
    ofp_udp_pkt_sendto(s, pkt, (struct ofp_sockaddr *)&addr, sizeof(addr));
#else
    /*
     * Send using usual sendto(). Remember to free the packet.
     */
    ofp_sendto(s, p, r, 0,
        (struct ofp_sockaddr *)&addr, sizeof(addr));
    odp_packet_free(pkt);
#endif
    /*
     * Mark ss->pkt invalid to indicate it was released or reused by us.
     */
    ss->pkt = ODP_PACKET_INVALID;
}

int udp_main(void *UNUSED(args))
{
    int fd = 0;

    if (ofp_init_local()) {
        ERROR(Error: OFP local init failed.);
        return -1;
    }

    sleep(3);

    if ((fd = init_sock(IP4(0, 0, 0, 0), UDP_PORT)) <= 0) {
        ERROR(Faild to create socket);
        return -1;
    }

#if 1
    {
        struct ofp_sock_sigval ss;
        struct ofp_sigevent ev;
        ss.sockfd = fd;
        ss.event = OFP_EVENT_INVALID;
        ss.pkt = ODP_PACKET_INVALID;
        ev.ofp_sigev_notify = OFP_SIGEV_HOOK;
        ev.ofp_sigev_notify_function = notify;
        ev.ofp_sigev_value.sival_ptr = &ss;
        if (ofp_socket_sigevent(&ev) == -1) {
            OFP_ERR("Error: Failed configure socket callback: errno = %s\n",
                ofp_strerror(ofp_errno));
        } else {
            sleep(-1);
        }
    }
#else
    while (1) {
        char buf[51024] = {0};
        uint32_t ip = 0;
        uint16_t port = 0;
        uint32_t ip1 = IP4(192, 168, 100, 129);
        uint32_t ip2 = IP4(192, 168, 101, 129);

        bzero(buf, sizeof(buf));
        if (recvfrom_udp(fd, &ip, &port, buf, sizeof(buf)) != 0) {
            continue;
        }

        //continue;
        memset(buf, '*', 1024);

        if (!send_data(fd, ip1, port, buf)) {
            OFP_INFO("Send buf ok!");
        }
    }
#endif
    if (ofp_close(fd) == -1) {
        ERROR(Faild to close accepted socket);
        return -1;
    }
}