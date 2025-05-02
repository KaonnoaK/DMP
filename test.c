#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include "utils.h"

// Cache configuration for M1 chip (we assume this from your specifications)
#define CACHE_SIZE 12 * 1024 * 1024    // 12MB L3 Cache
#define CACHE_LINE_SIZE 128            // 128 bytes per cache line
#define CACHE_WAY 16                   // 16-way associative
#define NUM_ELEMS (CACHE_SIZE / CACHE_LINE_SIZE)  // Total elements in the cache

typedef struct Elem {
    uint64_t data[CACHE_LINE_SIZE / sizeof(uint64_t)];  // Each element takes a cache line
    struct Elem *next;
} Elem;

// Global variables (cache-related configurations)
int cache_way = CACHE_WAY;
int cache_size = NUM_ELEMS;

// Eviction test configuration (tweaks could be made here)
struct {
    float ratio;
    int rounds;
    int threshold;
    int flags;
    int traverse;
} conf = {1.0, 100, 10, 0, 1};  // Default config: ratio=1.0, rounds=100

// Mocked function to test cache line status (dummy)
int tests(Elem *ptr, char *victim, int rounds, int threshold, float ratio, int traverse) {
    // Simulate a "test" that checks the victim's presence in the cache
    // In reality, we would check cache line hits/misses, but for this demonstration
    // we’ll simulate the result randomly.
    return rand() % 2;  // Random test success/failure
}

// List manipulation functions (naive eviction logic)
int list_length(Elem *ptr) {
    int len = 0;
    while (ptr) {
        len++;
        ptr = ptr->next;
    }
    return len;
}

Elem *list_pop(Elem **ptr) {
    if (*ptr == NULL) return NULL;
    Elem *temp = *ptr;
    *ptr = (*ptr)->next;
    temp->next = NULL;
    return temp;
}

void list_push(Elem **ptr, Elem *candidate) {
    candidate->next = *ptr;
    *ptr = candidate;
}

void list_append(Elem **ptr, Elem *candidate) {
    if (*ptr == NULL) {
        *ptr = candidate;
        return;
    }
    Elem *temp = *ptr;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = candidate;
}

void list_concat(Elem **can, Elem *ptr) {
    if (*can == NULL) {
        *can = ptr;
        return;
    }
    Elem *temp = *can;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = ptr;
}

// Cache eviction function (using naive eviction strategy)
int naive_eviction_optimistic(Elem **ptr, Elem **can, char *victim) {
    Elem *candidate = NULL;
    int len = 0, elen = 0, ret = 0;

    len = list_length(*ptr);

    while (elen < cache_way && len > cache_way) {
        candidate = list_pop(ptr);

        ret = tests(*ptr, victim, conf.rounds, conf.threshold, conf.ratio, conf.traverse);
        if (ret) {
            // List still is an eviction set of victim, discard candidate
            list_push(can, candidate);
        } else {
            // Candidate is congruent, keep it
            elen++;
            list_append(ptr, candidate);
        }

        len = list_length(*ptr);
    }

    list_concat(can, *ptr);

    if (elen < cache_way) {
        *ptr = NULL;
        return 1;
    } else {
        *ptr = candidate;
    }

    ret = tests(*ptr, victim, conf.rounds, conf.threshold, conf.ratio, conf.traverse);
    return !ret;
}

// Access latency measurement function
uint64_t measure_latency(volatile uint64_t *addr) {
    uint64_t start, end;

    // Read the address to ensure it’s in the cache
    start = read_timer();  // Get CPU timestamp (time in cycles)
    volatile uint64_t tmp = *addr;
    end = read_timer();    // Get CPU timestamp after access

    printf("\n \t %llu \t %llu \t %llu \n",start, end , end-start);
    return end - start;  // Latency in cycles
}

// Function to populate the list with cache line-sized elements
Elem *create_list() {
    Elem *head = NULL;
    for (int i = 0; i < NUM_ELEMS; i++) {
        Elem *new_elem = (Elem *)malloc(sizeof(Elem));
        if (!new_elem) {
            printf("Memory allocation failed!\n");
            exit(1);
        }
        new_elem->next = head;
        head = new_elem;
    }
    return head;
}

// Main function to demonstrate cache eviction and latency
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
    naive_eviction_optimistic(&cache_list, &candidates, victim);

    // Measure latency after eviction
    unsigned long post_latency = measure_latency((volatile uint64_t *)victim);
    printf("Post-eviction latency: %llu cycles\n", post_latency);

    // Output the latency difference
    printf("Latency difference: %llu cycles\n", post_latency - pre_latency);
	disableCounters();
    return 0;
}

