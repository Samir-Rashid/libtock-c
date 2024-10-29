#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <libtock/kernel/ipc.h>
#include <libtock/tock.h>
#include <core_cm4.h>
// trace_syscalls kernel cargo toml
// ficr registers
// disable Mpu in the context switch kernel loop function

/*
int main(void) {
  // TODO: mark as volatile
  // DwtRegisters base address
  uint32_t BASE = 0xE0001000;
  // DcbRegisters base
  uint32_t DCB_BASE = 0xE000EDF0;
  uint32_t DCB_DEMCR = DCB_BASE + 0xC;


}
*/

void start_cycle_counter(void) {
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0; 
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t get_cycle_count(void) {
    return DWT->CYCCNT;
}

void some_function() {
    // Your function code
    for (int i = 0; i < 100; i++) {
        printf("Hello World\n");
    }
}

int main(void) {
    start_cycle_counter();
    uint32_t start_cycles = get_cycle_count();
    
    // some_function();  // Code to measure

    uint32_t end_cycles = get_cycle_count();
    uint32_t cycle_count = end_cycles - start_cycles;

    // Print or log cycle_count as needed
    printf("Cycle count: %lu\n", cycle_count);
}