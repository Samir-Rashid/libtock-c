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
#include <lfs.h>
// #include <lfs_util.h>

/****** Filesystem Configuration ******/
// We port [LittleFS](https://github.com/littlefs-project/littlefs) as the filesystem to benchmark.
// This requires creating a shim layer to interface with Tock nonvolatile_storage capsule.

// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

int shim_read(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size) {
    int length_read = 0;
    int ret = libtocksync_nonvolatile_storage_read(block * c->block_size + off, size, buffer, size, &length_read);
    if (ret != RETURNCODE_SUCCESS) {
        printf("\tERROR calling read\n");
        return ret;
    }
    return length_read;
}

int shim_prog(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, const void *buffer, lfs_size_t size) {
    // TODO: does this have to clear the block?
    int length_written = 0;
    int ret = libtocksync_nonvolatile_storage_write(block * c->block_size + off, size, buffer, size, &length_written);
    if (ret != RETURNCODE_SUCCESS) {
        printf("\tERROR calling write\n");
        return ret;
    }
    return length_written;
}

int shim_erase(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, const void *buffer, lfs_size_t size) {
    uint8_t zero_buffer[size];
    memset(zero_buffer, 0, size);
    return shim_prog(c, block, off, buffer, size);
}

int shim_sync(const struct lfs_config *c) {
    return 0;
}

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    // TODO: these fn pointers need to match the following signatures <https://github.com/littlefs-project/littlefs/blob/d01280e64934a09ba16cac60cf9d3a37e228bb66/lfs.h#L157>
    .read  = shim_read,
    .prog  = shim_prog,
    .erase = shim_erase,
    .sync  = shim_sync,


    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = 128, // TODO: this must match the kernel assigned flash size
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};


/****** Benchmarking boilerplate macro ******/

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

/****** Filesystem ******/

void file_cache_size() {

}

void file_read_time() {

}

// TODO: can kind of do this one?
void remote_file_read_time() {

}

// TODO: cannot do this one
void contention() {

}

// Measure the cycles of a given `some_function`.
// Run all the tests
// NOTE: there is only one hardware counter, so BENCHMARKING a function
// with internal BENCHMARKS is incorrect.
int main(void) {
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    printf("boot_count: %ld\n", boot_count);    
}
