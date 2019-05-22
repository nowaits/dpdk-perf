

#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vppinfra/error.h>

/* for vlib_get_plugin_symbol() */
#include <vlib/unix/plugin.h>

#define EXAMPLE_POLL_PERIOD 1.0

static uword example_process(vlib_main_t *vm, vlib_node_runtime_t *rt,
    vlib_frame_t *f)
{
    f64 poll_time_remaining;
    uword event_type, *event_data = 0;

    poll_time_remaining = EXAMPLE_POLL_PERIOD;

    while (1) {
        poll_time_remaining =
            vlib_process_wait_for_event_or_clock(vm, poll_time_remaining);

        event_type = vlib_process_get_events(vm, &event_data);
        switch (event_type) {
        case ~0:		// no events => timeout
            break;

        default:
            // This should never happen...
            clib_warning("BUG: unhandled event type %d",
                event_type);
            break;
        }
        vec_reset_length(event_data);

        if (vlib_process_suspend_time_is_zero(poll_time_remaining)) {
            poll_time_remaining = EXAMPLE_POLL_PERIOD;
        }
    }

    return 0;
}

VLIB_REGISTER_NODE(example_node, static) = {
    .function = example_process,
    .type = VLIB_NODE_TYPE_PROCESS,
    .name = "process-test",
};