#include "utils.h"

uint64_t busy_wait(uint64_t iter, uint64_t trash) {
    // multiplication loop
    for (uint64_t i=0; i<iter; i++) {
        trash = (trash + 1) & 0xffff;
        asm volatile(
            "mul %[trash], %[trash], %[trash]\n\t"
            : [trash] "+r" (trash)
            : :
        );
    }
    return trash;
}

uint64_t thrash_array(uint64_t* thrash_arr, uint32_t size_of_thrash_array, uint64_t trash) {
    for (uint32_t page_offset = 0; page_offset < 16384; page_offset += 64) {
        for (uint32_t page_idx = 0; page_idx < (size_of_thrash_array - 2 * 16384); page_idx += 16384) {
            trash += (thrash_arr[(page_idx + page_offset) / 8] ^ trash) & 0b1111;
            trash += (thrash_arr[(page_idx + page_offset + 16384) / 8] ^ trash) & 0b1111;
            trash += (thrash_arr[(page_idx + page_offset + 2 * 16384) / 8] ^ trash) & 0b1111;
        }
    }
    return trash;
}


void isb(){
    asm volatile("isb");
}

void enableCounters(){
    uint64_t val;
    read_system_register(PMCR0, val);
    val |= GENMASK(7, 0) | GENMASK(33, 32);
    set_system_register(PMCR0, val);
    isb();
}

void disableCounters(){
    uint64_t val;
    read_system_register(PMCR0, val);
    val &= ~(GENMASK(7, 0) | GENMASK(33, 32));
    set_system_register(PMCR0, val);
    isb();
}

uint64_t read_timer() {
    uint64_t val;
    read_system_register(PMC0, val);
    return val;
}
