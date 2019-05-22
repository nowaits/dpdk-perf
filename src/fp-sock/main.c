#include "envs.h"

ofp_global_param_t app_init_params;

int main(int argc, char *argv[])
{
    odph_odpthread_t thread_tbl[MAX_WORKERS];
    appl_args_t params;
    int core_count, num_workers, ret_val;
    odp_cpumask_t cpumask;
    char cpumaskstr[64];
    odph_odpthread_params_t thr_params;
    odp_instance_t instance;

    if (parse_args(argc, argv, &params) != EXIT_SUCCESS ||
        setup_exit() != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if (odp_init_global(&instance, NULL, NULL)) {
        printf("Error: ODP global init failed.\n");
        return EXIT_FAILURE;
    }

    if (odp_init_local(instance, ODP_THREAD_CONTROL) != 0) {
        printf("Error: ODP local init failed.\n");
        odp_term_global(instance);
        return EXIT_FAILURE;
    }

    ofp_init_global_param(&app_init_params);

    setup_hook(&app_init_params);

    print_info(basename(argv[0]), &app_init_params);

    core_count = odp_cpu_count();

    if (params.core_count) {
        num_workers = params.core_count;
    } else {
        num_workers = core_count;
        if (core_count > 1) {
            num_workers--;
        }
    }

    if (num_workers > MAX_WORKERS) {
        num_workers = MAX_WORKERS;
    }

    num_workers = odp_cpumask_default_worker(&cpumask, num_workers);
    if (odp_cpumask_to_str(&cpumask, cpumaskstr, sizeof(cpumaskstr)) < 0) {
        printf("Error: Too small buffer provided to "
            "odp_cpumask_to_str\n");
        odp_term_local();
        odp_term_global(instance);
        return EXIT_FAILURE;
    }

    printf("Num worker threads: %i\n", num_workers);
    printf("first CPU:          %i\n", odp_cpumask_first(&cpumask));
    printf("cpu mask:           %s\n", cpumaskstr);

    if (app_init_params.pktin_mode != ODP_PKTIN_MODE_SCHED) {
        printf("Warning: Forcing scheduled pktin mode.\n");
        app_init_params.pktin_mode = ODP_PKTIN_MODE_SCHED;
    }

    switch (app_init_params.sched_sync) {
    case ODP_SCHED_SYNC_PARALLEL:
        printf("Warning: Packet order is not preserved with "
            "parallel RX queues\n");
        break;
    case ODP_SCHED_SYNC_ATOMIC:
        break;
    case ODP_SCHED_SYNC_ORDERED:
        if (app_init_params.pktout_mode != ODP_PKTOUT_MODE_QUEUE)
            printf("Warning: Packet order is not preserved with "
                "ordered RX queues and direct TX queues.\n");
        break;
    default:
        printf("Warning: Unknown scheduling synchronization mode. "
            "Forcing atomic mode.\n");
        app_init_params.sched_sync = ODP_SCHED_SYNC_ATOMIC;
        break;
    }

    odp_schedule_config(NULL);

    if (ofp_init_global(instance, &app_init_params) != 0) {
        printf("Error: OFP global init failed.\n");

        goto EXIT1;
    }

    if (ofp_init_local() != 0) {
        printf("Error: OFP local init failed.\n");

        goto EXIT0;
    }

    memset(thread_tbl, 0, sizeof(thread_tbl));
    thr_params.start = default_event_dispatcher;
    thr_params.arg = ofp_eth_vlan_processing;
    thr_params.thr_type = ODP_THREAD_WORKER;
    thr_params.instance = instance;
    ret_val = odph_odpthreads_create(thread_tbl,
            &cpumask,
            &thr_params);
    if (ret_val != num_workers) {
        OFP_ERR("Error: Failed to create worker threads, "
            "expected %d, got %d",
            num_workers, ret_val);
        ofp_stop_processing();
        odph_odpthreads_join(thread_tbl);

        goto EXIT0;
    }

    // init info level
    ofp_loglevel = OFP_LOG_INFO;

    if (ofp_start_cli_thread(instance, app_init_params.linux_core_id,
            params.cli_file) < 0) {
        OFP_ERR("Error: Failed to init CLI thread");
        ofp_stop_processing();
        odph_odpthreads_join(thread_tbl);

        goto EXIT0;
    }

    if (params.perf_stat) {
        if (start_performance(instance, app_init_params.linux_core_id) <= 0) {
            OFP_ERR("Error: Failed to init performance monitor");
            ofp_stop_processing();
            odph_odpthreads_join(thread_tbl);

            goto EXIT0;
        }
    }

    {
        static odph_odpthread_t server_pthread;
        odp_cpumask_t cpumask;
        odph_odpthread_params_t thr_params;

        odp_cpumask_zero(&cpumask);
        odp_cpumask_set(&cpumask, app_init_params.linux_core_id);
        thr_params.start =
#if 0
            tcp_main;
#else
            udp_main;
#endif
        thr_params.arg = NULL;
        thr_params.thr_type = ODP_THREAD_WORKER;
        thr_params.instance = instance;
        odph_odpthreads_create(&server_pthread, &cpumask, &thr_params);
    }


    odph_odpthreads_join(thread_tbl);

EXIT0:
    if (ofp_term_local() < 0) {
        printf("Error: ofp_term_local failed\n");
    }

EXIT1:
    if (ofp_term_global() < 0) {
        printf("Error: ofp_term_global failed\n");
    }

    if (odp_term_local() < 0) {
        printf("Error: odp_term_local failed\n");
    }

    if (odp_term_global(instance) < 0) {
        printf("Error: odp_term_global failed\n");
    }

    printf("FPM End Main()\n");

    return EXIT_SUCCESS;
}