#include "buffer_pool.h"
#include "file_io.h"

#include <stdio.h>
#include <stdlib.h>

void init_buffer_pool(const char* filename, BufferPool* pool) {
    short i = 0;

    // open file 
    if (open_file(&(pool->file), filename) != FILE_IO_SUCCESS) {
        printf("init BufferPool error: file cannot open\n");
        pool->file.fp = NULL;
        return;
    }
    // initialize BufferPool
    for (i = 0; i < CACHE_PAGE; i++) {
        pool->pages[i] = EMPTY_PAGE;
        pool->addrs[i] = -1;
        // pool->age[i] = 1;
        pool->ref[i] = 0;
    }
}

void close_buffer_pool(BufferPool* pool) {
    if (pool->file.fp == NULL) {
        printf("close BufferPool error: file dose not exist\n");
        return;
    }

    short i = 0;

    // check if every page's ref = 0
    // for (i = 0; i < CACHE_PAGE; i++)
    //     if (pool->ref[i] != 0) {
    //         printf("close BufferPool error: existing page in use\n");
    //         return;
    //     }
    // apply `write_page` to all used pages in BufferPool
    for (i = 0; i < CACHE_PAGE; i++)
        if (pool->addrs[i] != -1)
            write_page(&(pool->pages[i]), &(pool->file), pool->addrs[i]);
    // close file
    close_file(&(pool->file));
}

Page* get_page(BufferPool* pool, off_t addr) {
    short i = 0, pos = -1;
    if (addr < 0 || addr > pool->file.length) {
        printf("get_page error: invalid address %ld\n", addr);
        return NULL;
    }

    // Try to find the page in the buffer pool
    for (i = 0; i < CACHE_PAGE; i++) {
        if (pool->ref[i] > 0 && pool->addrs[i] == addr) {
            pos = i;
            pool->ref[pos]++;
            return &(pool->pages[pos]);
        }
    }

    // Try to find an unused slot with the same address
    for (i = 0; i < CACHE_PAGE; i++) {
        if (pool->ref[i] == 0 && pool->addrs[i] == addr) {
            pos = i;
            pool->ref[pos] = 1;
            return &(pool->pages[pos]);
        }
    }

    // Find a slot to replace
    for (i = 0; i < CACHE_PAGE; i++) {
        if (pool->addrs[i] == -1 || pool->ref[i] == 0) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        printf("get page error: no more available space in the buffer pool to replace\n");
        return NULL;
    }

    if (pool->addrs[pos] != -1) {
        write_page(&(pool->pages[pos]), &(pool->file), pool->addrs[pos]);
    }

    if (addr == pool->file.length) {
        Page page = EMPTY_PAGE;
        write_page(&page, &(pool->file), addr);
    }

    read_page(&(pool->pages[pos]), &(pool->file), addr);
    pool->addrs[pos] = addr;
    pool->ref[pos] = 1;

    return &(pool->pages[pos]);
}

void release(BufferPool* pool, off_t addr) {
    for (short i = 0; i < CACHE_PAGE; i++) {
        if (pool->addrs[i] == addr) {
            if (pool->ref[i] > 0) {
                pool->ref[i]--;
            }
            break;
        }
    }
}


void print_buffer_pool(BufferPool *pool) {
    int i;
    printf("---------------CACHE---------------\n");
    for (i = 0; i < CACHE_PAGE; ++i) {
        if (pool->addrs[i] != -1) {
            printf("%d\t" FORMAT_OFF_T "\t" FORMAT_SIZE_T "\t" FORMAT_SIZE_T "\n", i, pool->addrs[i], pool->cnt[i], pool->ref[i]);
        }
    }
    printf("-----------------------------------\n");
}

void validate_buffer_pool(BufferPool *pool) {
    int i;
    for (i = 0; i < CACHE_PAGE; ++i) {
        if (pool->addrs[i] != -1 && pool->ref[i] != 0) {
            printf("warning: possible memory leaks\n");
        }
    }
}
