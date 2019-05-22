#include "envs.h"

static int init_sock(uint32_t ip, uint16_t port)
{
    struct ofp_sockaddr_in addr = {0};
    int optval = 1;
    int fd = -1;

    fd = ofp_socket(OFP_AF_INET, OFP_SOCK_STREAM, OFP_IPPROTO_TCP);
    if (fd == -1) {
        ERROR(Faild to create SEND socket);
        return -1;
    }

    optval = 1;
    ofp_setsockopt(fd, OFP_SOL_SOCKET, OFP_SO_REUSEADDR, &optval, sizeof(optval));
    ofp_setsockopt(fd, OFP_SOL_SOCKET, OFP_SO_REUSEPORT, &optval, sizeof(optval));

    addr.sin_len = sizeof(struct ofp_sockaddr_in);
    addr.sin_family = OFP_AF_INET;
    addr.sin_port = odp_cpu_to_be_16(port);
    addr.sin_addr.s_addr = ip;

    if (ofp_bind(fd, (const struct ofp_sockaddr *)&addr,
            sizeof(struct ofp_sockaddr_in)) == -1) {
        ERROR(Faild to bind socket);
        return -1;
    }

    if (ofp_listen(fd, 10) == -1) {
        ERROR(Faild to listen);
        return -1;
    }

    OFP_INFO("TCP SOCKET SETUP SUCCESS.");
    return fd;
}

static int accept_tcp(int fd, uint32_t *ip, uint16_t *port)
{
    struct ofp_sockaddr_in addr = {0};
    ofp_socklen_t addr_len = sizeof(addr);

    int cfd = ofp_accept(fd, (struct ofp_sockaddr *)&addr,
            &addr_len);

    if (cfd == -1) {
        ERROR(Faild to accept connection);
        return -1;
    }

    if (addr_len != sizeof(struct ofp_sockaddr_in)) {
        ERROR(Faild to accept: invalid address size);
        return -1;
    }

    *ip = addr.sin_addr.s_addr;
    *port = addr.sin_port;

    OFP_INFO("Address: %s, port: %d.",
        ofp_print_ip_addr(*ip),
        odp_be_to_cpu_16(*port));

    return cfd;
}

int tcp_main(void *UNUSED(args))
{
    int fd = 0;

    if (ofp_init_local()) {
        ERROR(Error: OFP local init failed.);
        return -1;
    }

    sleep(3);

    if ((fd = init_sock(IP4(0, 0, 0, 0), TCP_PORT)) <= 0) {
        ERROR(Faild to create socket);
        return -1;
    }

    while (1) {
        char buf[1024] = {0};
        uint32_t ip = 0;
        uint16_t port = 0;
        int cfd = accept_tcp(fd, &ip, &port);

        while (1) {
#if 0
            int len = ofp_recv(cfd, buf, sizeof(buf), 0);
            if (len == -1) {
                ERROR(FAILED to recv);
                break;
            }

            if (len == 0) {
                OFP_INFO("no message and disconnect!");
                break;
            }

            buf[len] = 0;
            OFP_INFO("\nMessage received: [%s] %d", buf, len);
#else
            sprintf(buf, "You address: %s:%d", ofp_print_ip_addr(ip),
                odp_be_to_cpu_16(port));

            if (ofp_send(cfd, buf, strlen(buf), 0) == -1) {
                ERROR(Faild to send);
                break;
            }

            break;
#endif
        }

        if (ofp_close(cfd) == -1) {
            ERROR(Faild to close accepted socket);
            return -1;
        }
    }

    if (ofp_close(fd) == -1) {
        ERROR(Faild to close accepted socket);
        return -1;
    }
}