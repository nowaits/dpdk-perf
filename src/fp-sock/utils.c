#include <signal.h>

#include "envs.h"

static int linux_sigaction(int signum, void (*sig_func)(int))
{
    sigset_t sigmask_all;
    struct sigaction sigact = {
        .sa_handler = sig_func,
        .sa_mask = { { 0 } },
        .sa_flags = 0,
    };

    if (sigfillset(&sigmask_all)) {
        printf("Error: sigfillset failed.\n");
        return -1;
    }
    sigact.sa_mask = sigmask_all;

    if (sigaction(signum, &sigact, NULL)) {
        printf("Error: sigaction failed.\n");
        return -1;
    }

    return 0;
}

int ofp_sigactions_set(void (*sig_func)(int))
{
    if (linux_sigaction(SIGINT, sig_func)) {
        printf("Error: ODP sighandler setup failed: SIGINT.\n");
        return -1;
    }

    if (linux_sigaction(SIGQUIT, sig_func)) {
        printf("Error: ODP sighandler setup failed: SIGQUIT.\n");
        return -1;
    }

    if (linux_sigaction(SIGTERM, sig_func)) {
        printf("Error: ODP sighandler setup failed: SIGQUIT.\n");
        return -1;
    }

    return 0;
}

static void ofp_sig_func_stop(int signum)
{
    printf("Signal handler (signum = %d) ... exiting.\n", signum);

    ofp_stop_processing();
}

int setup_exit(void)
{
    if (ofp_sigactions_set(ofp_sig_func_stop)) {
        printf("Error: failed to set signal actions.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int perf_client(void *arg)
{
    (void) arg;

    if (ofp_init_local()) {
        OFP_ERR("Error: OFP local init failed.\n");
        return -1;
    }

    ofp_set_stat_flags(OFP_STAT_COMPUTE_PERF);

    while (1) {
        struct ofp_perf_stat *ps = ofp_get_perf_statistics();
        // odp_shm_print_all();
        if (ps->rx_fp_pps > 0) {
            printf("Mpps:%4.3f\n", ((float)ps->rx_fp_pps));
        }
        usleep(1000000UL);
    }

    return 0;
}

int start_performance(odp_instance_t instance, int core_id)
{
    static odph_odpthread_t cli_linux_pthread;
    odp_cpumask_t cpumask;
    odph_odpthread_params_t thr_params;

    odp_cpumask_zero(&cpumask);
    odp_cpumask_set(&cpumask, core_id);

    thr_params.start = perf_client;
    thr_params.arg = NULL;
    thr_params.thr_type = ODP_THREAD_CONTROL;
    thr_params.instance = instance;
    return odph_odpthreads_create(&cli_linux_pthread,
            &cpumask, &thr_params);

}

int parse_args(int argc, char *argv[], appl_args_t *appl_args)
{
    int opt;
    int long_index;
    size_t len;

    static struct option longopts[] = {
        {"count", required_argument, NULL, 'c'},
        {"performance", no_argument, NULL, 'p'},	/* return 'p' */
        {"help", no_argument, NULL, 'h'},		/* return 'h' */
        {
            "cli-file", required_argument,
            NULL, 'f'
        },/* return 'f' */
        {NULL, 0, NULL, 0}
    };

    memset(appl_args, 0, sizeof(*appl_args));

    while (1) {
        opt = getopt_long(argc, argv, "+c:hpf:",
                longopts, &long_index);

        if (opt == -1) {
            break;    /* No more options */
        }

        switch (opt) {
        case 'c':
            appl_args->core_count = atoi(optarg);
            break;

        case 'h':
            usage(argv[0]);
            return EXIT_FAILURE;

        case 'p':
            appl_args->perf_stat = 1;
            break;

        case 'f':
            len = strlen(optarg);
            if (len == 0) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            len += 1;	/* add room for '\0' */

            appl_args->cli_file = malloc(len);
            if (appl_args->cli_file == NULL) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }

            strcpy(appl_args->cli_file, optarg);
            break;

        default:
            break;
        }
    }

    optind = 1;		/* reset 'extern optind' from the getopt lib */

    return EXIT_SUCCESS;
}

void print_info(char *progname, ofp_global_param_t *app_init_params)
{
    int i;

    odp_sys_info_print();

    printf("Running ODP appl: \"%s\"\n"
        "-----------------\n"
        "IF-count:        %i\n"
        "Using IFs:      ",
        progname, app_init_params->if_count);
    for (i = 0; i < app_init_params->if_count; ++i) {
        printf(" %s", app_init_params->if_names[i]);
    }
    printf("\n\n");
    fflush(NULL);
}

void usage(char *progname)
{
    printf("\n"
        "Usage: %s OPTIONS\n"
        "  E.g. %s -i eth1,eth2,eth3\n"
        "\n"
        "ODPFastpath application.\n"
        "\n"
        "Mandatory OPTIONS:\n"
        "  -i, --interface Eth interfaces (comma-separated, no spaces)\n"
        "\n"
        "Optional OPTIONS\n"
        "  -c, --count <number> Core count.\n"
        "  -p, --performance    Performance Statistics\n"
        "  -h, --help           Display help and exit.\n"
        "\n", basename(progname), basename(progname)
    );
}