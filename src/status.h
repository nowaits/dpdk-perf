#ifndef __STATUS_H_
#define __STATUS_H_

#define STATUS_CALL 0
#define STATUS_PPS 1
#define STATUS_BYTES 2
#define STATUS_ALL 3

#ifndef NOW
    #define NOW clib_cpu_time_now()
#endif

#ifndef FEQ
    #define FEQ ({static u64 f = 0; if(f==0)f=os_cpu_clock_frequency();f;})
#endif

#ifndef TIDX
    #define TIDX vlib_get_thread_index()
#endif

#define RECORD(bytes, pps) ({ \
        static u64 status[20][2 * STATUS_ALL + 1] = {{0}}; \
        record(status[TIDX], bytes, pps); \
    })

static inline void record(u64 *status, u32 bytes, u32 pps) \
{
    status[2 * STATUS_BYTES + 1] += bytes;
    status[2 * STATUS_PPS + 1] += pps;
    status[2 * STATUS_CALL + 1] ++;

    if (NOW - status[2 * STATUS_ALL] < FEQ)
    {
        return;
    }

    status[2 * STATUS_BYTES] = status[2 * STATUS_BYTES + 1] * 8;
    status[2 * STATUS_PPS] = status[2 * STATUS_PPS + 1];
    status[2 * STATUS_CALL] = status[2 * STATUS_CALL + 1];

    status[2 * STATUS_BYTES + 1] = 0;
    status[2 * STATUS_PPS + 1] = 0;
    status[2 * STATUS_CALL + 1] = 0;
    status[2 * STATUS_ALL] = NOW;
#define _(data, tag1, tag2) \
    \
    if (data > 1024 * 1024 * 1024) { \
        fprintf(stderr, tag1"%.2lfG"tag2, data/(1024*1024*1024.0)); \
    } else if (data > 1024 * 1024) { \
        fprintf(stderr, tag1"%.2lfM"tag2, data/(1024*1024.0)); \
    }  else if (data > 1024) { \
        fprintf(stderr, tag1"%.2lfK"tag2, data/(1024.0)); \
    } else { \
        fprintf(stderr, tag1"%ld"tag2, data); \
    }

    fprintf(stderr, "---->> thread: %ld %s:%d ", TIDX, __FILE__, __LINE__);
    _(status[2 * STATUS_BYTES], "speed: ", "b/s - ")
    _(status[2 * STATUS_CALL], "calls: ", "/s - ")
    _(status[2 * STATUS_PPS], "pps: ", "/s\n")
#undef _
}

#endif // __STATUS_H_