// include/api/ofp_hook.h
// include/api/ofp_types.h

#include "envs.h"

#include <ofp_portconf.h>

static enum ofp_return_code fastpath_local_hook(odp_packet_t pkt, void *arg)
{
    int protocol = *(int *)arg;
    (void) pkt;
    (void) protocol;
    struct ofp_ip *ip = (struct ofp_ip *)odp_packet_l3_ptr(pkt, NULL);

    ofp_print_packet(__FUNCTION__, pkt);
    if (ip) {
        OFP_DBG("%s, Packet: %s => %s",
            __FUNCTION__,
            ofp_print_ip_addr(ip->ip_src.s_addr),
            ofp_print_ip_addr(ip->ip_dst.s_addr));
    }

    return OFP_PKT_CONTINUE;
}

static enum ofp_return_code fastpath_fwd_hook(odp_packet_t pkt, void *UNUSED(arg))
{
    struct ofp_ip *ip = (struct ofp_ip *)odp_packet_l3_ptr(pkt, NULL);
    //return OFP_PKT_DROP;

    ofp_print_packet(__FUNCTION__, pkt);
    if (ip) {
        OFP_DBG("%s: Packet: %s => %s",
            __FUNCTION__,
            ofp_print_ip_addr(ip->ip_src.s_addr),
            ofp_print_ip_addr(ip->ip_dst.s_addr));
    }

    return OFP_PKT_CONTINUE;
}

static enum ofp_return_code fastpath_out_hook(odp_packet_t pkt, void *UNUSED(arg))
{
    struct ofp_ip *ip = (struct ofp_ip *)odp_packet_l3_ptr(pkt, NULL);

    ofp_print_packet(__FUNCTION__, pkt);
    if (ip) {
        OFP_DBG("%s: Packet: %s => %s",
            __FUNCTION__,
            ofp_print_ip_addr(ip->ip_src.s_addr),
            ofp_print_ip_addr(ip->ip_dst.s_addr));
    }
    return OFP_PKT_CONTINUE;
}

void setup_hook(ofp_global_param_t *app_init_params)
{
#if 0
    app_init_params->pkt_hook[OFP_HOOK_LOCAL] = fastpath_local_hook;
    app_init_params->pkt_hook[OFP_HOOK_FWD_IPv4] = fastpath_fwd_hook;
    app_init_params->pkt_hook[OFP_HOOK_OUT_IPv4] = fastpath_out_hook;
#endif
}