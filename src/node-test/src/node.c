

#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vppinfra/error.h>

/* for vlib_get_plugin_symbol() */
#include <vlib/unix/plugin.h>
#include <vnet/feature/feature.h>

#define EXAMPLE_POLL_PERIOD 1.0

always_inline uword
aaa_qs_inline(vlib_main_t *vm,
    vlib_node_runtime_t *node, vlib_frame_t *frame,
    int is_ip4, int is_trace)
{
    u32 n_left_from, *from;
    vlib_buffer_t *bufs[VLIB_FRAME_SIZE], **b;
    u16 nexts[VLIB_FRAME_SIZE], *next;

    from = vlib_frame_vector_args(frame);
    n_left_from = frame->n_vectors;

    vlib_get_buffers(vm, from, bufs, n_left_from);
    b = bufs;
    next = nexts;

    while (n_left_from >= 4) {
        /* Prefetch next iteration. */
        if (PREDICT_TRUE(n_left_from >= 8)) {
            vlib_prefetch_buffer_header(b[4], STORE);
            vlib_prefetch_buffer_header(b[5], STORE);
            vlib_prefetch_buffer_header(b[6], STORE);
            vlib_prefetch_buffer_header(b[7], STORE);
            CLIB_PREFETCH(b[4]->data, CLIB_CACHE_LINE_BYTES, STORE);
            CLIB_PREFETCH(b[5]->data, CLIB_CACHE_LINE_BYTES, STORE);
            CLIB_PREFETCH(b[6]->data, CLIB_CACHE_LINE_BYTES, STORE);
            CLIB_PREFETCH(b[7]->data, CLIB_CACHE_LINE_BYTES, STORE);
        }

        /* $$$$ process 4x pkts right here */
        next[0] = 0;
        next[1] = 0;
        next[2] = 0;
        next[3] = 0;

        if (is_trace) {
            if (b[0]->flags & VLIB_BUFFER_IS_TRACED) {
                aaa_qs_trace_t *t =
                    vlib_add_trace(vm, node, b[0], sizeof(*t));
                t->next_index = next[0];
                t->sw_if_index = vnet_buffer(b[0])->sw_if_index[VLIB_RX];
            }
            if (b[1]->flags & VLIB_BUFFER_IS_TRACED) {
                aaa_qs_trace_t *t =
                    vlib_add_trace(vm, node, b[1], sizeof(*t));
                t->next_index = next[1];
                t->sw_if_index = vnet_buffer(b[1])->sw_if_index[VLIB_RX];
            }
            if (b[2]->flags & VLIB_BUFFER_IS_TRACED) {
                aaa_qs_trace_t *t =
                    vlib_add_trace(vm, node, b[2], sizeof(*t));
                t->next_index = next[2];
                t->sw_if_index = vnet_buffer(b[2])->sw_if_index[VLIB_RX];
            }
            if (b[3]->flags & VLIB_BUFFER_IS_TRACED) {
                aaa_qs_trace_t *t =
                    vlib_add_trace(vm, node, b[3], sizeof(*t));
                t->next_index = next[3];
                t->sw_if_index = vnet_buffer(b[3])->sw_if_index[VLIB_RX];
            }
        }

        b += 4;
        next += 4;
        n_left_from -= 4;
    }

    while (n_left_from > 0) {

        /* $$$$ process 1 pkt right here */
        next[0] = 0;

        if (is_trace) {
            if (b[0]->flags & VLIB_BUFFER_IS_TRACED) {
                aaa_qs_trace_t *t =
                    vlib_add_trace(vm, node, b[0], sizeof(*t));
                t->next_index = next[0];
                t->sw_if_index = vnet_buffer(b[0])->sw_if_index[VLIB_RX];
            }
        }

        b += 1;
        next += 1;
        n_left_from -= 1;
    }

    vlib_buffer_enqueue_to_next(vm, node, from, nexts, frame->n_vectors);

    return frame->n_vectors;
}

VLIB_REGISTER_NODE(example_node, static) = {
    .function = example_process,
    .type = VLIB_NODE_TYPE_INTERNAL,
    .name = "l2-perf",

    .vector_size = sizeof(u32),

    .n_next_nodes = 1,
    .next_nodes = {
        [0] = "error-drop",
    },
};

VNET_FEATURE_INIT(example_node, static) = {
    .arc_name = "device-input",
    .node_name = "l2-perf",
    .runs_after = VNET_FEATURES("ethernet-input"),
};