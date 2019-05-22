#include <stdio.h>

#include <time.h>

#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <string.h>
#include <errno.h>
#include <asm/unistd.h>

typedef double f64;
typedef unsigned int u32;
typedef unsigned long long u64;

static inline  u64 cpucycles(void)
{
    u32 tsc;
    asm volatile("mrc p15, 0, %0, c9, c13, 0":"=r"(tsc));
    return (u64) tsc;
}

#if 0
__attribute__((constructor)) static void arm_pmu_init()
{

#define ARMV7_PMNC_E        (1 << 0) /* Enable all counters */
#define ARMV7_PMNC_P        (1 << 1) /* Reset all counters */
#define ARMV7_PMNC_C        (1 << 2) /* Cycle counter reset */
#define ARMV7_PMNC_D        (1 << 3) /* CCNT counts every 64th cpu cycle */
#define ARMV7_PMNC_X        (1 << 4) /* Export to ETM */

    u32 value = ARMV7_PMNC_E | ARMV7_PMNC_P | ARMV7_PMNC_C | ARMV7_PMNC_D | ARMV7_PMNC_X;

    // program the performance-counter control-register:
    asm volatile("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));

    // enable all counters:
    asm volatile("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));

    // clear overflows:
    asm volatile("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}
#endif

#include <sys/syscall.h>

static inline f64 unix_time_now(void)
{
    struct timespec ts;
    syscall(SYS_clock_gettime, CLOCK_REALTIME, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

static inline f64 estimate_clock_frequency(f64 sample_time)
{
    u64 t = cpucycles();

    f64 start = unix_time_now();
    f64 now = start;

    while (now < start + sample_time) {
        now = unix_time_now();
    }

    return (cpucycles() - t) / (now - start);
}

int main(int argc, char *argv[])
{
    int c = 10;
    f64 feq = estimate_clock_frequency(1e-3);
    int last_time = 0;
    u64 last_clock = 0;

    printf("cpu feq:  %.2lfM\n", feq / (1 << 20));
    last_clock = cpucycles();

    while (c > 0) {
        u64 n = cpucycles();
        if (last_time == (int)(n / feq)) {
            continue;
        }

        last_time = (int)(n / feq);
        c --;
        printf("time: %.2lf time:%.2lf clock: %lld diff: %lld\n", n / feq, unix_time_now(), n,
            n - last_clock);
        last_clock = n;
    }
}