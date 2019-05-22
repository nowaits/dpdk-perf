#include <vppinfra/clib.h>
#include <vppinfra/longjmp.h>
#include <vppinfra/format.h>
#include <vppinfra/time.h>
#include <vppinfra/mem.h>

typedef void(handle_t)(uword);

typedef struct {
    u8 id;

    clib_longjmp_t return_longjmp;
#define VLIB_PROCESS_RETURN_LONGJMP_RETURN (3)
#define VLIB_PROCESS_RETURN_LONGJMP_SUSPEND (4)

    clib_longjmp_t resume_longjmp;
#define VLIB_PROCESS_RESUME_LONGJMP_SUSPEND 0
#define VLIB_PROCESS_RESUME_LONGJMP_RESUME  1

#define STACK_SIZE (1<<20)
    u8 *stack;

    handle_t *handle;
    uword n_suspends;
    u64 last_time_stamp;
} process_t;

f64 vlib_process_wait_for_event_or_clock(process_t *p)
{
    uword r;

    u64 wakeup_time = clib_cpu_time_now();

    r = clib_setjmp(&p->resume_longjmp, VLIB_PROCESS_RESUME_LONGJMP_SUSPEND);
    if (r == VLIB_PROCESS_RESUME_LONGJMP_SUSPEND) {
        clib_longjmp(&p->return_longjmp, VLIB_PROCESS_RETURN_LONGJMP_SUSPEND);
    }

    return wakeup_time - p->last_time_stamp;
}

uword vlib_process_resume(process_t *p)
{
    int r = clib_setjmp(&p->return_longjmp, VLIB_PROCESS_RETURN_LONGJMP_RETURN);
    if (r == VLIB_PROCESS_RETURN_LONGJMP_RETURN) {
        clib_longjmp(&p->resume_longjmp, VLIB_PROCESS_RESUME_LONGJMP_RESUME);
    } else if (r == VLIB_PROCESS_RETURN_LONGJMP_SUSPEND) {
        p->n_suspends += 1;
    }

    return r;
}

uword vlib_process_bootstrap(uword arg)
{
    uword n = 1000;
    process_t *p = uword_to_pointer(arg, process_t *);

    p->handle(arg);

    clib_longjmp(&p->return_longjmp, n);
    return n;
}

u64 start_process(process_t *p)
{
    uword r;

    r = clib_setjmp(&p->return_longjmp, VLIB_PROCESS_RETURN_LONGJMP_RETURN);
    if (r == VLIB_PROCESS_RETURN_LONGJMP_RETURN) {
        r = clib_calljmp(vlib_process_bootstrap, pointer_to_uword(p),
                p->stack + STACK_SIZE);
    } else if (r == VLIB_PROCESS_RETURN_LONGJMP_SUSPEND) {
        p->n_suspends += 1;
    }

    return p->last_time_stamp;
}

#define MAX_PROCESS 10

u32 new_id()
{
    static u32 id = 0;
    return id++;
}

void demo_1(uword arg)
{
    process_t *p = uword_to_pointer(arg, process_t *);
    while (1) {
        u32 wait_id = new_id();

        clib_warning("Process:%d Wait ID:%d suspend: %ld Wait Event!", p->id, wait_id, p->n_suspends);
        vlib_process_wait_for_event_or_clock(p);
        clib_warning("Process:%d Wait ID:%d suspend: %ld Get Event!", p->id, wait_id, p->n_suspends);
    }
}

void demo_2(uword arg)
{
    process_t *p = uword_to_pointer(arg, process_t *);
    while (1) {
        u32 wait_id = new_id();

        clib_warning("Process:%d Wait ID:%d suspend: %ld Wait Event!", p->id, wait_id, p->n_suspends);
        vlib_process_wait_for_event_or_clock(p);
        clib_warning("Process:%d Wait ID:%d suspend: %ld Get Event!", p->id, wait_id, p->n_suspends);
    }
}

int main(int argc, char *argv[])
{
    process_t *ps = 0, *p = 0;
    uword i = 0;

    clib_mem_init(0, 64 << 20);
    vec_resize(ps, MAX_PROCESS);

    vec_foreach(p, ps) {
        p->id = p - ps;
        p->handle = p->id & 1 ? demo_1 : demo_2;
        p->stack = clib_mem_alloc_aligned(STACK_SIZE, STACK_SIZE);
        if (mprotect(p->stack, clib_mem_get_page_size(), PROT_READ) < 0) {
            clib_unix_warning("process stack protect error!");
        }
    }

    // start
    vec_foreach(p, ps) {
        p->last_time_stamp = clib_cpu_time_now();
        start_process(p);
    }

    // dispatch
    int start = time(0);
    while (time(0) - start < 5) {
        u64 tick = clib_cpu_time_now();
        u8 random = tick % (MAX_PROCESS * 2);

        if (random < MAX_PROCESS) {
            process_t *p = ps + random;
            p->last_time_stamp = clib_cpu_time_now();
            clib_warning("\E[1;32mProcess:%d Signaled!\E[0m", random);
            vlib_process_resume(p);
        }

        usleep(100000);
    }

    // clean
    vec_free(ps);
    clib_warning("exit");

    return 0;
}