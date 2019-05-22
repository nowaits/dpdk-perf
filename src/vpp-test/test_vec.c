#include <vppinfra/clib.h>
#include <vppinfra/longjmp.h>
#include <vppinfra/format.h>

int
main(int argc, char *argv[])
{
    int res;
    int i;
    u8 *vec = 0, *p;

    clib_mem_init(0, 64 << 20);
    vec_add2(vec, p, 1);
    *p = 0;

    for (i = 0; i < _vec_len(vec); i++) {
        if (i < 10) {
            vec_add2(vec, p, 1);
            *p = i;
        }

        printf("%i %d\n", i, vec[i]);
    }

    return res;
}