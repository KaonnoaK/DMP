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
    uint64_t* aop_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    uint64_t* reset_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // Allocate memory for thrash array
    uint64_t *thrash_arr = mmap(0, SIZE_THRASH_ARRAY, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    printf("[+] data_buf_addr: %p\n", data_buf_addr);
    printf("[+] aop_addr: %p\n", aop_addr);
    assert(ADDR_CHECK((uintptr_t)data_buf_addr,(uintptr_t)aop_addr));

    srand(time(NULL));

    for (int i = 0; i < SIZE_DATA_ARRAY / sizeof(uint64_t); i++) {
        data_buf_addr[i] = rand() & (MSB_MASK - 1);
    }

    for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++) {
        //printf("\n aop no %d \n",i);
        aop_addr[i] = (uint64_t)&data_buf_addr[0];  // All point to data_buf_addr[0]
    }

    aop = &aop_buf[0];

     for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++) {
        int index = (i * 8 + 4096) % (SIZE_DATA_ARRAY / sizeof(uint64_t));  // Ensures unique offset
        reset_addr[i] = (uint64_t)&data_buf_addr[index];  // Points to different elements in data_buf_addr
    }

   /*  for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++) {
          reset_addr[i] = rand() & (MSB_MASK - 1);
      }*/

    for (int i = 0; i < SIZE_THRASH_ARRAY / sizeof(uint64_t); i++) {
        thrash_arr[i] = rand() & (MSB_MASK - 1);
    }

    reset_addr = &reset_addr[16 * 8];
    printf("[+] reset_addr: %p\n", reset_addr);
    assert(ADDR_CHECK((uintptr_t)data_buf_addr, (uintptr_t)reset_addr));

    uint64_t latency_aop[REPETITIONS];
    uint64_t latency_ptr[REPETITIONS];
    float success[257] ;
    int suc=0;
    uint64_t junk;

   /*  Elem *cache_list = create_list();
    Elem *candidates = NULL;
    naive_eviction_optimistic(&cache_list, &candidates,(char *)&aop_addr[0],16);

    uint64_t start = read_timer();
    isb();
    mem_access(&aop_addr[0],junk);
    isb();
    uint64_t end = read_timer();


    printf("\n time taken and value %llu \t %llu \n", end-start ,aop_addr[0] );*/

    uint64_t* dummy = NULL;
    junk = thrash_array(thrash_arr, SIZE_THRASH_ARRAY, junk);
    junk = busy_wait(10000, junk);

    for (int i =1 ; i <=REPETITIONS  ; i++) {

        printf ("\n\n\t ----- ITERATION %d ----- \n",i);

        //evict aop...
        printf("\n-> Evicted aop at %p ",aop);
        isb();
        for (int l = 0; l < 1; l++) {
            //printf("\n aop no %d \n",l);
            Elem *cache_list = create_list();
            Elem *candidates = NULL;
            naive_eviction_optimistic(&cache_list, &candidates,(char *)aop,16);
            //free
            free_list(&candidates);
            }
        isb();


        //evict pointer
        isb();
        Elem *cache_list = create_list();
        Elem *candidates = NULL;
        naive_eviction_optimistic(&cache_list, &candidates,(char *)&aop,16);
        isb();
        //free
        free_list(&candidates);

        //reset
        printf ("\n\n Trying to reset the history filter with %d unique accesses ... ... ... \n",S);
        int j = 0;
        dummy = reset_addr;
        while (j < S ) {

            // access S unique pointers ....
                busy_wait(100000,junk);
                time_t current_time1 = time(NULL);
                volatile int junk1 = (int)current_time1;
                uint64_t start1 = read_timer();
                isb();
                mem_access(dummy[j],junk1);
                isb();
                uint64_t end1 = read_timer();
                uint64_t time1 = end1-start1;

                printf("\n-> Accessed pointer %p ",dummy[j]);

                /*time_t current_time2 = time(NULL);
                volatile int junk2 = (int)current_time2;
                uint64_t start2 = read_timer();
                isb();
                mem_access(dummy[j],junk2);
                isb();
                uint64_t end2 = read_timer();
                uint64_t time2 = end2-start2;

                if (time2<time1)*/
                    j++;
        }

            printf ("\n\n-----> Reaccessing the aop to test DMP re-activation ... ");
            busy_wait(100000,junk);
            //access the aop again
            uint64_t start1 = read_timer();
            time_t current_time1 = time(NULL);
            junk = (int)current_time1;
            isb();
            mem_access(&aop,junk);
            isb();
            uint64_t end1 = read_timer();

            busy_wait(1000,junk);
            //test the pointer again
            time_t current_time2 = time(NULL);
            junk = (int)current_time2;
            uint64_t start2 = read_timer();
            isb();
            mem_access(aop,junk);
            isb();
            uint64_t end2 = read_timer();

            latency_ptr[i] = end2 - start2;
            latency_aop[i] = end1 - start1;
            //printf ("\n latency during S = %d is .. %llu %llu ",i, latency_aop[i], latency_ptr[i]);
            if (latency_ptr[i]>latency_aop[i]){
                suc++;
            }
        }

    printf("\n\n\n Successful DMP re-activations : %d \n\n",suc);
    disableCounters();
    return 0;
}
