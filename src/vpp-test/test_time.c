#include <stdio.h>
#include <time.h>
#include <vppinfra/clib.h>
#include <vppinfra/time.h>

__attribute__((constructor)) static void _init_perfcounters()
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

static f64 estimate_clock_frequency(f64 sample_time)
{
    /* Round to nearest 100KHz. */
    const f64 round_to_units = 10;

    f64 time_now, time_start, time_limit, freq;
    u64 ifreq, t[2];

    time_start = time_now = unix_time_now();
    time_limit = time_now + sample_time;
    t[0] = clib_cpu_time_now();
    while (time_now < time_limit) {
        time_now = unix_time_now();
    }
    t[1] = clib_cpu_time_now();

    freq = (t[1] - t[0]) / (time_now - time_start);

    ifreq = flt_round_nearest(freq / round_to_units);
    freq = ifreq * round_to_units;

    return freq;
}

int main(int argc, char *argv[])
{
    f64 feq = 0;
    u64 start_c = 0;
    f64 start_u = 0;

    int times = 100;
    clib_mem_init(0, 1 << 20);
    feq = estimate_clock_frequency(1e-1);

    start_u = unix_time_now();
    start_c = clib_cpu_time_now();

    while (times-- > 0) {
        int c = feq * 10;

        while (c-- > 0);

        u64 n = clib_cpu_time_now();

        printf("feq: %lf; tick: %lu - time: %lf - unix: %lf\n", feq, n, (n - start_c) / feq,
            unix_time_now() - start_u);
    }

    return 0;
}