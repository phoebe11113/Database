#ifndef _HASH_MAP_H
#define _HASH_MAP_H

#include "file_io.h"
#include "buffer_pool.h"
#include "block.h"

/* static hash map for free space map */

typedef struct {
    off_t free_block_head;
    off_t n_directory_blocks;
    off_t max_size;
} HashMapControlBlock;

#define HASH_MAP_DIR_BLOCK_SIZE (PAGE_SIZE / sizeof(off_t))

typedef struct {
    off_t directory[HASH_MAP_DIR_BLOCK_SIZE];
} HashMapDirectoryBlock;

#define HASH_MAP_BLOCK_SIZE ((PAGE_SIZE - 2 * sizeof(off_t)) / sizeof(RID))

typedef struct {
    off_t next;
    off_t n_items;
    RID table[HASH_MAP_BLOCK_SIZE];
} HashMapBlock;

/* if hash table has already existed, it will be re-opened and n_directory_blocks will be ignored */
void hash_table_init(const char *filename, BufferPool *pool, off_t n_directory_blocks);

void hash_table_close(BufferPool *pool);

/* there should not be no duplicate rid */
void hash_table_insert(BufferPool *pool, short key, RID rid);

/* if there is no suitable block, return (-1, 0) */
RID hash_table_pop_lower_bound(BufferPool *pool, short key);

/* rid to be poped must exist */
void hash_table_pop(BufferPool *pool, short key, RID rid);

typedef int (*hash_table_ptr_row_cmp_t)(void *, size_t, RID);

/* if the row to be found does not exist, return (-1, 0) */
RID hash_table_search(BufferPool *pool, short key, void *ptr, size_t size, hash_table_ptr_row_cmp_t cmp);

void print_hash_table(BufferPool *pool);

#endif  /* _HASH_MAP_H */