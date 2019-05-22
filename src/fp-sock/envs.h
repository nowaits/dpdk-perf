#ifndef __APP_ARGS__
#define __APP_ARGS__

#include <stdint.h>
#include <stdio.h>
// basename
#include <libgen.h>
// sleep
#include <unistd.h>
// PRIu64
#include <inttypes.h>

#include <ofp.h>

#define MAX_WORKERS		32

typedef struct {
    int core_count;
    char *cli_file;
    int perf_stat;
} appl_args_t;

#define IP4(a,b,c,d) ((uint32_t)(((a) & 0xff)) | \
    (((b) & 0xff) << 8) | \
    (((c) & 0xff) << 16)  | \
    ((d) & 0xff)  << 24)

#define ERROR(msg) OFP_ERR(#msg" (errno = %s)", ofp_strerror(ofp_errno));

#define TCP_PORT 5000
#define UDP_PORT 6000

#ifdef __GNUC__
    #define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
    #define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
    #define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
    #define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

//
int setup_exit(void);
int start_performance(odp_instance_t instance, int core_id);
int parse_args(int argc, char *argv[], appl_args_t *appl_args);
void print_info(char *progname, ofp_global_param_t *app_init_params);
void usage(char *progname);

int ofp_sigactions_set(void (*sig_func)(int));

//
int udp_main(void *args);
//
int tcp_main(void *args);

//
void setup_hook(ofp_global_param_t *app_init_params);
#endif /* __APP_ARGS__ */