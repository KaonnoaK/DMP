#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "eviction.h"
#include "utils.h"

#define REPETITIONS 100
#define LATENCY 150

#define __volatile__ __attribute__((volatile))

int main(int argc, char** argv) {

    int S;
    sscanf(argv[1], "%d", &S);
    int test_idx=0;
    uint64_t* aop;

    enableCounters();
    uint64_t* aop_buf = (uint64_t*)aligned_alloc(4096, PAGE_SIZE);
    uint64_t* data_buf_addr = mmap(NULL, SIZE_DATA_ARRAY, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // Allocate memory for aop array (one page)
    uint64_t* aop_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // Allocate memory for reset array (one page)
    uint64_t* reset_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // Allocate memory for thrash array
    uint64_t *thrash_arr = mmap(0, SIZE_THRASH_ARRAY, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    printf("[+] data_buf_addr: %p\n", data_buf_addr);
    printf("[+] aop_addr: %p\n", aop_addr);
    assert(ADDR_CHECK((uintptr_t)data_buf_addr,(uintptr_t)aop_addr));
    // Fill data_buf with random values (these are just values, not pointers)
    for (int i = 0; i < SIZE_DATA_ARRAY / sizeof(uint64_t); i++) {
        data_buf_addr[i] = rand() & (MSB_MASK - 1);
    }

    // Fill aop array with the same valid pointer (points to data_buf_addr[0])
    for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++) {
        //printf("\n aop no %d \n",i);
        aop_addr[i] = (uint64_t)&data_buf_addr[0];  // All point to data_buf_addr[0]
    }

    uint64_t* dummy = NULL;
    uint64_t start, end , diff;
    dummy = aop_addr;
    uint64_t junk;

    start = read_timer();
    junk = thrash_array(thrash_arr, SIZE_THRASH_ARRAY, junk);
    junk = busy_wait(10000, junk);
    end = read_timer();
    printf("Time taken to thrash is %llu \n",end-start);
    int access=0,i=0;

    while (i < S) {
                //busy_wait(100000,junk);
                time_t current_time1 = time(NULL);
                volatile int junk1 = (int)current_time1;
                start = read_timer();
                isb();
                mem_access(dummy[i],junk1);
                isb();
                end = read_timer();
                diff = end - start ;
                printf("\n Train time = %llu \n",diff);
                i++;
    }

     junk = busy_wait(10000, junk);

    while (i<4096) {
        time_t current_time1 = time(NULL);
        volatile int junk1 = (int)current_time1;
        start = read_timer();
        isb();
        mem_access(&dummy[i],junk1);
        isb();
        end = read_timer();
        diff = end - start ;
        if (diff < 100) {
            access++;
        }
        i++;
    }

    printf("\n The number of successfull access % d \n",access);
    disableCounters();
    return 1;
}
