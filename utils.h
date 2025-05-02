#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>

#define MSB_MASK    0x8000000000000000ULL
#define KB 1024ULL
#define MB (1024*KB)
#define RND_INIT 8

#define INST_SYNC asm volatile("ISB")
#define DATA_SYNC asm volatile("DSB SY")

#define MEM_BARRIER \
	DATA_SYNC;      \
	INST_SYNC

#define mem_access(addr, val) ({ \
    asm volatile( \
        "ldr %x[value], [%[address]]\n\t" \
        : [value] "=r" (val) \
        : [address] "r" (addr) \
        : "memory"); \
})



#define set_system_register(SR, V) __set_system_register(SR, V)
#define __set_system_register(SR, V) asm volatile("msr " #SR ", %0;" : : "r"(V))

#define read_system_register(SR, V) __read_system_register(SR, V)
#define __read_system_register(SR, V) asm volatile("mrs %0, " #SR : "=r"(V))

#define GENMASK(h, l) (((~0UL) - (1UL << (l)) + 1) & (~0UL >> (64 - 1 - (h))))
#define BIT(nr) (1UL << (nr))
#define CPU_AFFINITY 2

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

void isb();
void enableCounters();
void disableCounters();
uint64_t read_timer();
uint64_t busy_wait(uint64_t, uint64_t);
uint64_t thrash_array(uint64_t* , uint32_t , uint64_t );
