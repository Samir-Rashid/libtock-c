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
    int ret = libtocksync_nonvolatile_storage_read((block * (c->block_size)) + off, size, buffer, size, &length_read);
    if (ret != RETURNCODE_SUCCESS) {
        printf("\tERROR calling read\n");
        return ret;
    }
    // printf("read %d of %ld\n", length_read, size);
    // printf("block %ld, off %ld\n", block, off);
    // return length_read;
    return 0;
}

int shim_prog(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, const void *buffer, lfs_size_t size) {
    // TODO: does this have to clear the block?
    /*
    uint8_t zero_buffer[c->block_size];
    memset(zero_buffer, 0, c->block_size);
    libtocksync_nonvolatile_storage_write(block * c->block_size, size, buffer, c->block_size, NULL);
    */

    int length_written = 0;
    int ret = libtocksync_nonvolatile_storage_write((block * c->block_size) + off, size, buffer, size, &length_written);
    if (ret != RETURNCODE_SUCCESS) {
        printf("\tERROR calling write\n");
        return ret;
    }
    // printf("write %d of %ld\n", length_written, size);
    // return length_written;
    return 0;
}

int shim_erase(const struct lfs_config *c, lfs_block_t block) {
    // printf("erasing %ld\n", block);
    uint8_t zero_buffer[c->block_size];
    memset(zero_buffer, 0, c->block_size);
    return shim_prog(c, block, 0, zero_buffer, c->block_size);
}

int shim_sync(const struct lfs_config *c) {
    // printf("sync\n");
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
    .block_size = 512,
    .block_count = 64, // NOTE: this must match the kernel assigned flash size
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
// sizes of the files to be created (in bytes)
int file_sizes[] = {1, 10, 100, 500, 510, 520, 600, 1000, 2000, 5000};

// make files of various sizes to test with. Only needs to be run once
void create_files() {
    printf("Recreating all the test files. Remember to disable me!\n");
    for (int i = 0; i < (sizeof(file_sizes) / sizeof(file_sizes[0])); i++) {
        // create the files
        char filename[20];
        sprintf(filename, "file_%d.txt", file_sizes[i]);
        lfs_file_open(&lfs, &file, filename, LFS_O_WRONLY | LFS_O_CREAT);

        // write nonzero data so the fs doesn't do any optimizations
        uint8_t data[file_sizes[i]];
        memset(data, 'A', file_sizes[i]);
        lfs_file_write(&lfs, &file, data, file_sizes[i]);

        lfs_file_close(&lfs, &file); // need to write it out to disk!
    }
}

// take average read time for each `file_sizes`
void file_cache_size() {
    // printf("MEASURING CACHE SIZE\n");
    for (int i = 0; i < (sizeof(file_sizes) / sizeof(file_sizes[0])); i++) {
        // reconstruct filenames
        char filename[20];
        sprintf(filename, "file_%d.txt", file_sizes[i]);
        lfs_file_open(&lfs, &file, filename, LFS_O_RDONLY);

        uint8_t data[file_sizes[i]];
        lfs_file_read(&lfs, &file, data, file_sizes[i]);
        lfs_file_close(&lfs, &file);

        // calculate the average read time
        uint32_t total_cycles = 0;
        int it = 10; // num iterations
        for (int j = 0; j < it; j++) {
            lfs_file_open(&lfs, &file, filename, LFS_O_RDONLY);
            uint32_t start = get_cycle_count();
            lfs_file_read(&lfs, &file, data, file_sizes[i]); // measure only the reading
            total_cycles += get_cycle_count() - start;
            printf("print a random byte so stuff does not get compiled out %d\n", data[0]);
            lfs_file_close(&lfs, &file);
        }
        uint32_t average_cycles = total_cycles / it;

        printf("File size: %d bytes, Average read time (n= %d): %lu cycles\n", file_sizes[i], it, average_cycles);
    }
}

// sequential and random access as a function of file size (average per-block read time)
void file_read_time() {
    for (int i = 0; i < (sizeof(file_sizes) / sizeof(file_sizes[0])); i++) {
        int file_size = file_sizes[i];
        // reconstruct filenames
        char filename[20];
        sprintf(filename, "file_%d.txt", file_size);
        lfs_file_open(&lfs, &file, filename, LFS_O_RDONLY);

        uint8_t data[file_size];
        lfs_file_read(&lfs, &file, data, file_size);
        lfs_file_close(&lfs, &file);

        // calculate the average per-block read time
        uint32_t total_cycles = 0;
        for (int j = 0; j < file_size; j += cfg.block_size) {
        // for (int j = 0; j < file_size; j += cfg.block_size) {
            start_cycle_counter();
            lfs_file_open(&lfs, &file, filename, LFS_O_RDONLY);
            lfs_file_seek(&lfs, &file, file_size - j, LFS_SEEK_SET);
            lfs_file_read(&lfs, &file, data, cfg.block_size);
            lfs_file_close(&lfs, &file);
            total_cycles += get_cycle_count();
        }
        uint32_t average_cycles = total_cycles / (file_size / cfg.block_size); // cycles per block

        printf("File size: %d bytes, Average per-block read time: %lu cycles\n", file_sizes[i], average_cycles);
    }
}

// TODO: can kind of do this one?
void remote_file_read_time() {
    printf("do this on ieng6 machine\n");
}

// Each process reads a distinct file and reports average time to read per block
void contention() {
    printf("contention needs multiple apps\n");
    int MY_UNIQUE_ID = 0; // change this for each process
    
    int NUM_BLOCKS = 10;
    int file_size = NUM_BLOCKS * cfg.block_size;
    // arbitrary file data
    uint8_t data[file_size];
    memset(data, 'A', file_size);

    // Define the file name - THIS NEEDS TO BE UNIQUE PER PROCESS!
    char filename[20];
    sprintf(filename, "contention_%d.txt", MY_UNIQUE_ID);

    // SETUP: Write the data to the file
    lfs_file_open(&lfs, &file, filename, LFS_O_WRONLY | LFS_O_CREAT);
    lfs_file_write(&lfs, &file, data, file_size);
    lfs_file_close(&lfs, &file);

    // Read the data from the file n times and calculate average read time per block
    int n = 10; // Number of times to read the data
    uint32_t total_cycles = 0;
    for (int i = 0; i < n; i++) {
        lfs_file_open(&lfs, &file, filename, LFS_O_RDONLY);
        uint32_t start = get_cycle_count();
        lfs_file_read(&lfs, &file, data, file_size);
        total_cycles += get_cycle_count() - start;
        lfs_file_close(&lfs, &file);
    }
    uint32_t average_cycles = total_cycles / n / NUM_BLOCKS;

    printf("Average read time per block: %lu cycles\n", average_cycles);
}

void filesystem_initialization() {
    // mount the filesystem
    printf("mounting fs\n");
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        printf("reformat fs\n");
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }
}

void boot_count() {
    // read current count
    printf("read count\n");
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    printf("write count\n");
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    printf("close fs\n");
    lfs_file_close(&lfs, &file);

    // print the boot count
    printf("boot_count: %ld\n", boot_count);    
}

void filesystem_cleanup() {
    // release any resources we were using
    lfs_unmount(&lfs);
}

// Measure the cycles of a given `some_function`.
// Run all the tests
// NOTE: there is only one hardware counter, so BENCHMARKING a function
// with internal BENCHMARKS is incorrect.
int main(void) {
    filesystem_initialization();
    if (false) 
        create_files();
    boot_count();
    filesystem_cleanup();


    filesystem_initialization();
    start_cycle_counter(); // TODO: watch out for overflow on slow ops!

    // file_cache_size();
    // file_read_time();
    // remote_file_read_time();
    contention();
    filesystem_cleanup();

    return 0;
}
