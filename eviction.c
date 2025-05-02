#include "eviction.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"

// Mocked function to test cache line status (dummy)
int tests(Elem *ptr, char *victim, int rounds, int threshold, float ratio, int traverse) {
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
int naive_eviction_optimistic(Elem **ptr, Elem **can, char *victim,int cacheway) {
    Elem *candidate = NULL;
    int len = 0, elen = 0, ret = 0;

    len = list_length(*ptr);

    while (elen < cacheway && len > cacheway) {
        candidate = list_pop(ptr);
        //candidate->next = NULL;

        ret = tests(*ptr, victim, 100, 10, 1.0, 1);  // Example config
        if (ret) {
            list_push(can, candidate);
        } else {
            elen++;
            list_append(ptr, candidate);
        }

        len = list_length(*ptr);
    }

    list_concat(can, *ptr);

    if (elen < cacheway) {
        *ptr = NULL;
        return 1;
    } else {
        *ptr = candidate;
    }

    ret = tests(*ptr, victim, 100, 10, 1.0, 1);
    return !ret;
}

// Access latency measurement function
uint64_t measure_latency(volatile uint64_t *addr) {
    uint64_t start, end;
    start = read_timer();  // Get CPU timestamp (time in cycles)
    volatile uint64_t tmp = *addr;
    end = read_timer();    // Get CPU timestamp after access

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

void free_list(Elem **list) {
    Elem *current = *list;
    while (current) {
        Elem *next = current->next;
        free(current);
        current = next;
    }
    *list = NULL;
}

void thrash_cache() {
    uint64_t *thrash_array = mmap(NULL, THRASH_SIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (thrash_array == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    //printf("[+] Thrashing %d MB of memory to flush LLC\n", THRASH_SIZE / (1024 * 1024));

    size_t num_lines = THRASH_SIZE / CACHE_LINE_SIZE;

    volatile uint64_t sum = 0;
    for (size_t i = 0; i < num_lines; i++) {
        size_t offset = (i * CACHE_LINE_SIZE) / sizeof(uint64_t);
        sum += thrash_array[offset];
    }

    // Use sum to prevent compiler from optimizing the loop away
    if (sum == 123456789) {
        //printf("Unlikely sum matched (just to use it): %llu\n", sum);
    }

    // Optional cleanup
    munmap(thrash_array, THRASH_SIZE);
}
