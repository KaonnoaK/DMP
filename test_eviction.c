#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "eviction.h"
#include "utils.h"

int main() {
    enableCounters();
    srand(time(NULL));

    // Create a list of elements representing the cache lines
    Elem *cache_list = create_list();
    Elem *candidates = NULL;

    // Select a victim address (in the cache list)
    Elem *victim_elem = cache_list;
    char *victim = (char *)victim_elem;

    // Measure latency before eviction
    unsigned long pre_latency = measure_latency((volatile uint64_t *)victim);
    printf("Pre-eviction latency: %llu cycles\n", pre_latency);

    // Perform cache eviction (naive eviction)
    naive_eviction_optimistic(&cache_list, &candidates, victim,16);

    // Measure latency after eviction
    unsigned long post_latency = measure_latency((volatile uint64_t *)victim);
    printf("Post-eviction latency: %llu cycles\n", post_latency);

    // Output the latency difference
    printf("Latency difference: %llu cycles\n", post_latency - pre_latency);

    disableCounters();
    return 0;
}
