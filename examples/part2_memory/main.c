#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <libtock/kernel/ipc.h>
#include <libtock/tock.h>
#include <core_cm4.h>
#include <libtock-sync/storage/nonvolatile_storage.h>

// Benchmarking boilerplate macro
// TODO: do I need to indicate the register as volatile?
//       do I need a memory barrier between the two samples?
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

/****** RAM access time ******/

// access entire array
void memory_array_latency(int* array, int array_size) {
    int num = 0;
    for (int i = 0; i < array_size; i++) {
        num = array[i];
    }
}

// NOTE: should only enable icache for this test to minimize random
// interference in other tests
void ram_access_time() {
    // allocate the memory in here and benchmark only the iterative accesses
    for (int i = 2; i <= (2 << 10); i << 1) {
        int array_size = i;
        int* array = (int*)malloc(array_size * sizeof(int));
        BENCHMARK(memory_array_latency(array, array_size));
        free(array);
    }
}

/****** RAM bandwidth ******/
// Reference: See "5.1. Memory bandwidth" of [lmbench](https://www.usenix.org/legacy/publications/library/proceedings/sd96/full_papers/mcvoy.pdf)

// NOTE: this must be loop unrolled to give accurate measurements
int memory_bandwidth_reading() {
    // sum and return a bunch of numbers
    int sum = 0;
    for (int i = 0; i < 2 << 10; i++) {
        sum += i;
    }
    return sum;
}

// NOTE: this must be loop unrolled to give accurate measurements
void write_to_array(int* array, int array_size) {
    // write and increment pointer `array_size` times
    for (int i = 0; i < array_size; i++) {
        *(array++) = i;
    }
}

void memory_bandwidth_writing() {
    int array_size = 2 << 10;
    int* array = (int*)malloc(array_size * sizeof(int));
    BENCHMARK(write_to_array(array, array_size));
    free(array);
}


/****** Page fault service time ******/
static int test_flash(uint8_t* readbuf, uint8_t* writebuf, size_t size, size_t offset, size_t len) {
  int ret;
  int length_written, length_read;

  printf("Test with size %d ...\n", size);

  for (size_t i = 0; i < len; i++) {
    writebuf[i] = i;
  }

  ret = libtocksync_nonvolatile_storage_write(offset, len, writebuf, size, &length_written);
  if (ret != RETURNCODE_SUCCESS) {
    printf("\tERROR calling write\n");
    return ret;
  }

  ret = libtocksync_nonvolatile_storage_read(offset, len, readbuf, size, &length_read);
  if (ret != RETURNCODE_SUCCESS) {
    printf("\tERROR calling read\n");
    return ret;
  }

  for (size_t i = 0; i < len; i++) {
    if (readbuf[i] != writebuf[i]) {
      printf("\tInconsistency between data written and read at index %u\n", i);
      return -1;
    }
  }

  return 0;
}

// measure reading/writing page to flash
void page_sized_flash_load() {
    // There is no virtual memory and thus no page size.
    // Instead, we measure a "page"-sized load of 4k.
    uint32_t PAGE_SIZE = 4096;
    uint8_t readbuf[PAGE_SIZE];
    uint8_t writebuf[PAGE_SIZE];
    BENCHMARK(test_flash(readbuf, writebuf, PAGE_SIZE, 0, PAGE_SIZE));
    

}

// Measure the cycles of a given `some_function`.
// Run all the tests
// NOTE: there is only one hardware counter, so BENCHMARKING a function
// with internal BENCHMARKS is incorrect.
int main(void) {
    start_cycle_counter();
    
    // BENCHMARK(ram_access_time());
    // BENCHMARK(memory_bandwidth_reading());
    // memory_bandwidth_writing();
}
