#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <libtock/kernel/ipc.h>
#include <libtock/tock.h>
#include <core_cm4.h>

// Benchmarking boilerplate macro
#define BENCHMARK(code_block) \
    { uint32_t start_cycles = get_cycle_count(); \
    code_block; \
    uint32_t end_cycles = get_cycle_count(); \
    uint32_t cycle_count = end_cycles - start_cycles; \
    printf("%s: Cycle count: %lu\n", __FUNCTION__, cycle_count); }

void start_cycle_counter(void) {
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0; 
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

inline uint32_t get_cycle_count(void) {
    return DWT->CYCCNT;
}

/****** Measurement overhead ******/

// overhead of reading time
void read_time() {
    BENCHMARK({});
}

// overhead of using a loop to measure many iterations of an operation
#define NUM_LOOPS 1000
void loop_overhead() {
    uint32_t cycle_counts[NUM_LOOPS] = {0};

    for (int i = 0; i < NUM_LOOPS; i++) {
        uint32_t start_cycles = get_cycle_count();
        
        // do nothing

        uint32_t end_cycles = get_cycle_count();
        uint32_t cycle_count = end_cycles - start_cycles;

        cycle_counts[i] = cycle_count;
    }

    // calculate average
    uint32_t total_cycles = 0;
    for (int i = 0; i < NUM_LOOPS; i++) {
        total_cycles += cycle_counts[i];
    }

    uint32_t average_cycles = total_cycles / NUM_LOOPS;
    printf("Average cycle count: %lu\n", average_cycles);
}


/****** Procedure call overhead ******/
// overhead of function calls with different number of arguments 0-7
void function_call_overhead() {
    // define functions with different number of arguments
    void func0() {}
    void func1(int arg1) {}
    void func2(int arg1, int arg2) {}
    void func3(int arg1, int arg2, int arg3) {}
    void func4(int arg1, int arg2, int arg3, int arg4) {}
    void func5(int arg1, int arg2, int arg3, int arg4, int arg5) {}
    void func6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6) {}
    void func7(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7) {}

    // benchmark function calls with different number of arguments
    BENCHMARK(func0());
    BENCHMARK(func1(1));
    BENCHMARK(func2(1, 2));
    BENCHMARK(func3(1, 2, 3));
    BENCHMARK(func4(1, 2, 3, 4));
    BENCHMARK(func5(1, 2, 3, 4, 5));
    BENCHMARK(func6(1, 2, 3, 4, 5, 6));
    BENCHMARK(func7(1, 2, 3, 4, 5, 6, 7));
}


/****** System call overhead ******/
// cost of a procedure call
void procedure_call_overhead() {
    void dummy() {}

    BENCHMARK(dummy());
}

// cost of a minimal system call
void system_call_overhead() {
    BENCHMARK(yield());
}


/****** Task creation time ******/
// time to create and run a process
void process_creation_overhead() {
    // fork and wait on child
    // int pid = fork();
    // if (pid == 0) {
    //     printf("Child process\n");
    //     exit(0);
    // } else {
    //     printf("Parent process\n");
        // wait(pid);
    // }
}

// time to create and run a kernel thread


/****** Context switch time ******/




// Measure the cycles of a given `some_function`.
// Run all the tests
int main(void) {
    start_cycle_counter();
    
    read_time();
    BENCHMARK(loop_overhead());
    BENCHMARK(function_call_overhead());

    BENCHMARK(procedure_call_overhead());
    BENCHMARK(system_call_overhead());

    BENCHMARK(process_creation_overhead());

}
