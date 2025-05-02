#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#define set_system_register(SR, V) __set_system_register(SR, V)
#define __set_system_register(SR, V) asm volatile("msr " #SR ", %0;" : : "r"(V))
#define read_system_register(SR, V) __read_system_register(SR, V)
#define __read_system_register(SR, V) asm volatile("mrs %0, " #SR : "=r"(V))

#define PMCR0  s3_1_c15_c0_0
#define PMCR1  s3_1_c15_c1_0
#define PMESR0 s3_1_c15_c5_0
#define PMESR1 s3_1_c15_c6_0
#define PMC0   s3_2_c15_c0_0
#define PMC1   s3_2_c15_c1_0
#define PMC2   s3_2_c15_c2_0
#define PMC3   s3_2_c15_c3_0
#define PMC4   s3_2_c15_c4_0
#define PMC5   s3_2_c15_c5_0
#define PMC6   s3_2_c15_c6_0
#define PMC7   s3_2_c15_c7_0
#define PMC8   s3_2_c15_c9_0
#define PMC9   s3_2_c15_c10_0


static void enable_pmu(void *info){
    u64 val;
    val = BIT(30);
    val |= BIT(23);
    val |= BIT(8);
    val |= BIT(22);
    val |= BIT(22);
    set_system_register(PMCR0, val);

    val = GENMASK(41, 40) | GENMASK(15, 8);
    val |= GENMASK(49, 48) | GENMASK(23, 16);
    set_system_register(PMCR1, val);

    asm volatile("isb");
}

static void disable_pmu(void *info){
    u64 zero = 0;
    set_system_register(PMCR0, zero);
    set_system_register(PMCR1, zero);

    set_system_register(PMESR0, zero);
    set_system_register(PMESR1, zero);

    set_system_register(PMC0, zero);
    set_system_register(PMC1, zero);
    set_system_register(PMC2, zero);
    set_system_register(PMC3, zero);
    set_system_register(PMC4, zero);
    set_system_register(PMC5, zero);
    set_system_register(PMC6, zero);
    set_system_register(PMC7, zero);
    set_system_register(PMC8, zero);
    set_system_register(PMC9, zero);

    asm volatile("isb");
}

static int __init load(void){
    on_each_cpu(enable_pmu, NULL, 0);
    printk("\n Loading enable PMC ... \n");
    return 0;
}

static void __exit unload(void){
    on_each_cpu(disable_pmu, NULL, 0);
    printk("\n Loading enable PMC ... \n");
}

MODULE_LICENSE("GPL");
module_init (load);
module_exit (unload);

