/* Module source file 'module.c'. */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static void set_enable(int val)
{
    asm volatile("mcr p15, 0, %0, c9, c14, 0" :: "r"(val));
    asm volatile("mcr p15, 0, %0, C9, C14, 2" :: "r"(0x8000000f));

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

static int pum_user_enable(void)
{
    on_each_cpu(set_enable, 1, 1);
    printk(KERN_INFO "module load.\n");
    return 0;
}

static void end(void)
{
    on_each_cpu(set_enable, 0, 1);
    printk(KERN_INFO "module unloaded.\n");
}

module_init(pum_user_enable);
module_exit(end);

MODULE_LICENSE("GPL");