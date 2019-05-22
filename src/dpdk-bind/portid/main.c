
#include <stdio.h>
#include <stdint.h>
#include <rte_eal.h>
#include <rte_ethdev.h>

int32_t main(int32_t argc, char *argv[])
{
    uint8_t port;
    rte_eal_init(argc, argv);

    RTE_ETH_FOREACH_DEV(port) {
        struct ether_addr mac_addr;
        char mac_str[ETHER_ADDR_FMT_SIZE];
        struct rte_eth_dev_info dev_info;
        rte_eth_dev_info_get(port, &dev_info);
        rte_eth_macaddr_get(port, &mac_addr);

        ether_format_addr(mac_str, sizeof mac_str, &mac_addr);

        printf("%s %s %s\n", dev_info.device->name, mac_str, dev_info.driver_name);
    }

    return 0;
}
