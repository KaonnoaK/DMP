#ifndef  EVICTION_H
#define EVICTION_H

//ifndef UTILS_H

#include <stdint.h>
#include "utils.h"

#define PAGE_SIZE  (16 * KB)
#define CACHE_SIZE 12 * 1024 * 1024    // 12MB L3 Cache
#define CACHE_LINE_SIZE 128            // 128 bytes per cache line
#define CACHE_WAY 16                   // 16-way associative
#define NUM_ELEMS (CACHE_SIZE / CACHE_LINE_SIZE)  // Total elements in the cache
#define L2_LINE_SIZE 128
#define L1_SIZE (128 * KB)
#define L2_SIZE (12 * MB)
#define THRASH_SIZE (12 * 8 * 1024 * 1024)

#define PNRG_a 75
#define PRNG_m 8388617
#define prng(x) ((PNRG_a * x) % PRNG_m)

#define SIZE_DATA_ARRAY (PRNG_m * L2_LINE_SIZE)
#define SIZE_THRASH_ARRAY ((L1_SIZE + L2_SIZE) * 8)

#define ADDR_CHECK(addr1, addr2) ((addr1 >> 32) == (addr2 >> 32))


// Cache configuration for M1 chip (we assume this from your specifications)
typedef struct Elem {
    uint64_t data[CACHE_LINE_SIZE / sizeof(uint64_t)];  // Each element takes a cache line
    struct Elem *next;
} Elem;

// Function declarations
int tests(Elem *ptr, char *victim, int rounds, int threshold, float ratio, int traverse);
int list_length(Elem *ptr);
Elem *list_pop(Elem **ptr);
void list_push(Elem **ptr, Elem *candidate);
void list_append(Elem **ptr, Elem *candidate);
void list_concat(Elem **can, Elem *ptr);
int naive_eviction_optimistic(Elem **ptr, Elem **can, char *victim,int cacheway);
uint64_t measure_latency(volatile uint64_t *addr);
Elem *create_list();
void free_list(Elem **list);
void thrash_cache(void);

#endif // UTILS_H
