#include "hash_map.h"

#include <stdio.h>

void hash_table_init(const char* filename, BufferPool* pool, off_t n_directory_blocks) {
    short i = 0, j = 0;
    init_buffer_pool(filename, pool);
    
    if(pool->file.length != 0){
        release(pool, 0);
        return;
    }
        

    HashMapControlBlock* ctl = (HashMapControlBlock*)get_page(pool, 0);
    if (ctl == NULL) {
        printf("Error: Failed to get control block\n");
        return;
    }
    
    ctl->free_block_head = 0;
    ctl->n_directory_blocks = n_directory_blocks;
    ctl->max_size = n_directory_blocks * HASH_MAP_DIR_BLOCK_SIZE;
    // ctl->n_directory_blocks = PAGE_SIZE / HASH_MAP_DIR_BLOCK_SIZE;

    for (i = 0; i < ctl->n_directory_blocks; i++) {
        HashMapDirectoryBlock* dir = (HashMapDirectoryBlock*)get_page(pool, (i + 1) * PAGE_SIZE);
        if (dir == NULL) {
            printf("Error: Failed to get directory block\n");
            release(pool, 0); 
            return;
        }
        for (j = 0; j < HASH_MAP_DIR_BLOCK_SIZE; j++)
            dir->directory[j] = 0;
        release(pool, (i + 1) * PAGE_SIZE);
    }

    release(pool, 0);
    print_buffer_pool(pool);
    return;
}

void hash_table_close(BufferPool* pool) {
    close_buffer_pool(pool);
}


void hash_table_insert(BufferPool *pool, short key, RID rid) {
    short i = 0;
    off_t addr = get_rid_block_addr(rid);
    short idx = get_rid_idx(rid);
    short size = key;
    short beg_page = size / HASH_MAP_DIR_BLOCK_SIZE;
    short beg_page_item = size % HASH_MAP_DIR_BLOCK_SIZE;
    // printf("insert beg_page_item:%d\n",beg_page_item);
    HashMapControlBlock* ctl = (HashMapControlBlock*)get_page(pool, 0);
    if (ctl == NULL) {
        printf("Error: Failed to get control block\n");
        return;
    }
    HashMapDirectoryBlock* dir = (HashMapDirectoryBlock*)get_page(pool, (beg_page + 1) * PAGE_SIZE);
    if (dir == NULL) {
        printf("Error: Failed to get directory block\n");
        release(pool, 0); 
        return;
    }
    
    off_t block_addr = 0, block_addr_nxt = 0;
    HashMapBlock* block = NULL, *block_nxt = NULL;

    if (dir->directory[beg_page_item] == 0) {
        if (ctl->free_block_head == 0) {
            block_addr = pool->file.length;
            block = (HashMapBlock*)get_page(pool, block_addr);
            if (block == NULL) {
                printf("FFFailed to get page at address: %lld\n", block_addr);
                release(pool, (beg_page + 1) * PAGE_SIZE);
                release(pool, 0);
                return;
            }
            block->next = 0;
            block->n_items = 1;
            get_rid_block_addr(block->table[0]) = addr;
            get_rid_idx(block->table[0]) = idx;
            dir->directory[beg_page_item] = block_addr;
            release(pool, block_addr);
        } else {
            block_addr = ctl->free_block_head;
            block = (HashMapBlock*)get_page(pool, block_addr);
            if (block == NULL) {
                printf("free_block_head:%lld\n",ctl->free_block_head);
                printf("CFailed to get page at address: %lld\n", block_addr);
                release(pool, (beg_page + 1) * PAGE_SIZE);
                release(pool, 0);
                return;
            }
            ctl->free_block_head = block->next;
            block->next = 0;
            block->n_items = 1;
            get_rid_block_addr(block->table[0]) = addr;
            get_rid_idx(block->table[0]) = idx;
            dir->directory[beg_page_item] = block_addr;
            release(pool, block_addr);
        }
    } else {
        block_addr = dir->directory[beg_page_item];
        block = (HashMapBlock*)get_page(pool, block_addr);
        if (block == NULL) {
                printf("Failed to get page at address: %lld\n", block_addr);
                release(pool, (beg_page + 1) * PAGE_SIZE);
                release(pool, 0);
                return;
            }

        if (block->n_items == HASH_MAP_BLOCK_SIZE) {
            if (ctl->free_block_head == 0) {
                block_addr_nxt = pool->file.length;
                block_nxt = (HashMapBlock*)get_page(pool, block_addr_nxt);
                if (block_nxt == NULL) {
                printf("FFFailed to get page at address: %lld\n", block_addr_nxt);
                release(pool, (beg_page + 1) * PAGE_SIZE);
                release(pool, 0);
                return;
            }
                block_nxt->next = block_addr;
                block_nxt->n_items = 1;
                get_rid_block_addr(block_nxt->table[0]) = addr;
                get_rid_idx(block_nxt->table[0]) = idx;
                dir->directory[beg_page_item] = block_addr_nxt;
                release(pool, block_addr_nxt);
                release(pool, block_addr);
                
            } else {
                block_addr_nxt = ctl->free_block_head;
                block_nxt = (HashMapBlock*)get_page(pool, block_addr_nxt);
                if (block_nxt == NULL) {
                    printf("CFailed to get page at address: %lld\n", block_addr_nxt);
                    release(pool, (beg_page + 1) * PAGE_SIZE);
                    release(pool, 0);
                    return;
                }
                ctl->free_block_head = block_nxt->next;
                block_nxt->next = block_addr;
                block_nxt->n_items = 1;
                get_rid_block_addr(block_nxt->table[0]) = addr;
                get_rid_idx(block_nxt->table[0]) = idx;
                dir->directory[beg_page_item] = block_addr_nxt;
                release(pool, block_addr_nxt);
                release(pool, block_addr);
                
            }
        } else {
            get_rid_block_addr(block->table[block->n_items]) = addr;
            get_rid_idx(block->table[block->n_items]) = idx;
            block->n_items++;
            release(pool, block_addr);
        }
    }
    // printf("INSERT IDX:%d\n",get_rid_idx(rid));
    release(pool, (beg_page + 1) * PAGE_SIZE);
    release(pool, 0);
    // print_buffer_pool(pool);
}


RID hash_table_pop_lower_bound(BufferPool *pool, short key) {
    short i = 0, j = 0, hit = 0, pos_page = 0, pos_item = 0;
    short size = key;
    RID invalid_rid = {{0}};
    *(off_t*)invalid_rid.data = -1;
    *(short*)(invalid_rid.data + sizeof(off_t)) = 0;
    HashMapControlBlock* ctl = (HashMapControlBlock*)get_page(pool, 0);
    if (ctl == NULL) {
        printf("Error: Failed to get control block\n");
        return invalid_rid;
    }

    short beg_page = size / HASH_MAP_DIR_BLOCK_SIZE;
    short beg_page_item = size % HASH_MAP_DIR_BLOCK_SIZE;
    HashMapDirectoryBlock* dir = NULL;

    for (i = beg_page; i < ctl->n_directory_blocks; i++) {
        dir = (HashMapDirectoryBlock*)get_page(pool, (i + 1) * PAGE_SIZE);
        if (dir == NULL) {
            printf("Error: Failed to get directory block\n");
            release(pool, 0); 
            return invalid_rid;
        }

        for (j = (i == beg_page) ? beg_page_item : 0; j < HASH_MAP_DIR_BLOCK_SIZE; j++) {
            if (dir->directory[j] != 0) {
                hit = 1;
                pos_page = i;
                pos_item = j;
                break;
            }
        }

        if (hit) break;
        release(pool, (i + 1) * PAGE_SIZE);
    }

    if (!hit) {
        if (dir != NULL) {
            release(pool, (pos_page + 1) * PAGE_SIZE); 
        }
        release(pool, 0);
        return invalid_rid;
    }

    RID rid;
    off_t addr = dir->directory[pos_item];
    HashMapBlock* block = (HashMapBlock*)get_page(pool, addr);
    if (block == NULL) {
        printf("Error: Failed to get block\n");
        release(pool, (pos_page + 1) * PAGE_SIZE);
        release(pool, 0);
        return invalid_rid;
    }

    if (block->n_items == 0) {
        release(pool, addr);
        release(pool, (pos_page + 1) * PAGE_SIZE);
        release(pool, 0);
        return invalid_rid;
    }

    block->n_items--;
    rid = block->table[block->n_items];
    get_rid_block_addr(block->table[block->n_items]) = 0;

    if (block->n_items == 0) {
        dir->directory[pos_item] = block->next;
        block->next = ctl->free_block_head;
        ctl->free_block_head = addr;
    }

    release(pool, addr);
    release(pool, (pos_page + 1) * PAGE_SIZE);
    release(pool, 0);
    // print_buffer_pool(pool);

    return rid;
}


void hash_table_pop(BufferPool *pool, short key, RID rid) {
    short i = 0, j = 0, hit = 0;
    off_t addr = get_rid_block_addr(rid);
    short idx = get_rid_idx(rid);
    short size = key;
    short beg_page = size / HASH_MAP_DIR_BLOCK_SIZE;
    short beg_page_item = size % HASH_MAP_DIR_BLOCK_SIZE;

    HashMapControlBlock* ctl = (HashMapControlBlock*)get_page(pool, 0);
    HashMapDirectoryBlock* dir = (HashMapDirectoryBlock*)get_page(pool, (beg_page + 1) * PAGE_SIZE);

    off_t block_addr = 0, block_addr_nxt = 0;
    HashMapBlock* block = NULL, *block_nxt = NULL;

    if (dir->directory[beg_page_item] == 0) {
        release(pool, (beg_page + 1) * PAGE_SIZE);
        release(pool, 0);
        return; // Invalid, so return early after releasing resources
    }

    block_addr = dir->directory[beg_page_item];
    block = (HashMapBlock*)get_page(pool, block_addr);

    // Step 1: Find the head node block to retrieve the last RID
    HashMapBlock* head_block = block;  // Initially, the block at the beginning
    off_t head_block_addr = block_addr;
    head_block = (HashMapBlock*)get_page(pool, head_block_addr);

    // Step 2: Find the element to delete and replace it with the last element of the head block
    for (i = 0; i < block->n_items; i++) {
        if (get_rid_block_addr(block->table[i]) == addr && get_rid_idx(block->table[i])==idx) {
            hit = 1;

            // Step 3: Replace the current position with the last element in the head block
            block->table[i] = head_block->table[head_block->n_items - 1];
            get_rid_block_addr(head_block->table[head_block->n_items - 1]) = 0;
            // get_rid_idx(head_block->table[head_block->n_items - 1]) = -1;
            head_block->n_items--; // Decrement the head block's item count

            // Check if the head block is empty
            if (head_block->n_items == 0) {
                // If the head block is empty, free it
                dir->directory[beg_page_item] = head_block->next;
                head_block->next = ctl->free_block_head;
                ctl->free_block_head = head_block_addr;
            }
            release(pool, head_block_addr);
            release(pool, block_addr);
            release(pool, (beg_page + 1) * PAGE_SIZE);
            release(pool, 0);
            return; // Return early after releasing resources
        }
    }

    // If not found in the first block, check the next blocks
    if (!hit) {
        while (block->next != 0) {
            block_addr_nxt = block->next;
            block_nxt = (HashMapBlock*)get_page(pool, block_addr_nxt);

            for (i = 0; i < block_nxt->n_items; i++) {
                if (get_rid_block_addr(block_nxt->table[i]) == addr&&get_rid_idx(block_nxt->table[i])==idx) {
                    hit = 1;

                    // Replace the element with the last element from the head block
                    block_nxt->table[i] = head_block->table[head_block->n_items - 1];
                    get_rid_block_addr(head_block->table[head_block->n_items - 1]) = 0;
                    // get_rid_idx(head_block->table[head_block->n_items - 1]) = -1;
                    head_block->n_items--; // Decrement the head block's item count

                    // Check if the head block is empty
                    if (head_block->n_items == 0) {
                        // If the head block is empty, free it
                        dir->directory[beg_page_item] = head_block->next;
                        head_block->next = ctl->free_block_head;
                        ctl->free_block_head = head_block_addr;
                    }

                    break;
                }
            }

            release(pool, block_addr);
            if (hit) {
                release(pool, block_addr_nxt);
                break;
            } else {
                block_addr = block_addr_nxt;
                block = block_nxt;
            }
        }
        if (!hit) release(pool, block_addr);
    }
    release(pool, head_block_addr);
    release(pool, (beg_page + 1) * PAGE_SIZE);
    release(pool, 0);
}



RID hash_table_search(BufferPool *pool, short key, void *ptr, size_t size, hash_table_ptr_row_cmp_t cmp) {
    short i = 0, j = 0, hit = 0;
    RID invalid_rid = {{0}};
    *(off_t*)invalid_rid.data = -1;
    *(short*)(invalid_rid.data + sizeof(off_t)) = 0;
    short beg_page = key / HASH_MAP_DIR_BLOCK_SIZE;
    short beg_page_item = key % HASH_MAP_DIR_BLOCK_SIZE;
    printf("beg_page_item:%d\n",beg_page_item);


    HashMapControlBlock* ctl = (HashMapControlBlock*)get_page(pool, 0);
    if (ctl == NULL) {
        printf("Error: Failed to get control block\n");
        return invalid_rid;
    }
    HashMapDirectoryBlock* dir = (HashMapDirectoryBlock*)get_page(pool, (beg_page + 1) * PAGE_SIZE);
    if (dir == NULL) {
        printf("Error: Failed to get directory block\n");
        release(pool, 0); 
        return invalid_rid;
    }

    

    off_t block_addr = 0, block_addr_nxt = 0;
    HashMapBlock* block = NULL, *block_nxt = NULL;

    if (dir->directory[beg_page_item] == 0) {
        release(pool, (beg_page + 1) * PAGE_SIZE);
        release(pool, 0);
        return invalid_rid; // Invalid, so return early after releasing resources
    }
    // printf("\nPRINT:\n");
    // for(i = 0;i<HASH_MAP_DIR_BLOCK_SIZE;i++)
    // {
    //     off_t ba = dir->directory[i];
    //     if (dir->directory[i] == 0) {
    //     continue;
    //      }
    //     HashMapBlock* b = (HashMapBlock*)get_page(pool, ba);
    //     for(j = 0;j<b->n_items;j++)
    //     {
    //         RID rid = b->table[j];
    //         printf("DIR:%d RID IDX: %d ADDR:%d\n",i,get_rid_idx(rid),get_rid_block_addr(rid));
    //     }
    //     release(pool, ba);

    // }
    // printf("\n\n");
    
    block_addr = dir->directory[beg_page_item];
    block = (HashMapBlock*)get_page(pool, block_addr);


    for (i = 0; i < block->n_items; i++){
        // printf("IDX NUMBER:%d\n",block->n_items);
        RID rid = block->table[i];
        printf("RID IDX: %d ADDR:%lld\n",get_rid_idx(rid),get_rid_block_addr(rid));
        if (!cmp(ptr, size, rid)) {
            hit = 1;
            printf("FIND SAME1!\n");
            release(pool, block_addr);
            release(pool, (beg_page + 1) * PAGE_SIZE);
            release(pool, 0);
            return rid;
        }
        else{
                // printf("CMP ERROR!\n");
            }
    }
    if (!hit) {
        while (block->next != 0) {
            // printf("AAA");
            block_addr_nxt = block->next;
            block_nxt = (HashMapBlock*)get_page(pool, block_addr_nxt);
            for (i = 0; i < block_nxt->n_items; i++) {
                RID rid = block_nxt->table[i];
                if (!cmp(ptr, size, rid)) {
                    hit = 1;
                    printf("FIND SAME2!\n");
                    release(pool, block_addr_nxt);
                    release(pool, block_addr);
                    release(pool, (beg_page + 1) * PAGE_SIZE);
                    release(pool, 0);
                    return rid;
                }
                else{
                    // printf("CMP ERROR!\n");
                }
            }
            release(pool, block_addr);
            
            block_addr = block_addr_nxt;
            block = block_nxt;
            
        }
        release(pool, block_addr);
    }
    // if(!hit) printf("NOT HIT!\n");
    release(pool, (beg_page + 1) * PAGE_SIZE);
    release(pool, 0);
    // printf("NOT FOUND\n");
    return invalid_rid;
}


void print_hash_table(BufferPool *pool) {
    HashMapControlBlock *ctrl = (HashMapControlBlock*)get_page(pool, 0);
    HashMapDirectoryBlock *dir_block;
    off_t block_addr, next_addr;
    HashMapBlock *block;
    int i, j;
    printf("----------HASH TABLE----------\n");
    for (i = 0; i < ctrl->max_size; ++i) {
        dir_block = (HashMapDirectoryBlock*)get_page(pool, (i / HASH_MAP_DIR_BLOCK_SIZE + 1) * PAGE_SIZE);
        if (dir_block->directory[i % HASH_MAP_DIR_BLOCK_SIZE] != 0) {
            printf("%d:", i);
            block_addr = dir_block->directory[i % HASH_MAP_DIR_BLOCK_SIZE];
            while (block_addr != 0) {
                block = (HashMapBlock*)get_page(pool, block_addr);
                printf("  [" FORMAT_OFF_T "]", block_addr);
                printf("{");
                for (j = 0; j < block->n_items; ++j) {
                    if (j != 0) {
                        printf(", ");
                    }
                    print_rid(block->table[j]);
                }
                printf("}");
                next_addr = block->next;
                release(pool, block_addr);
                block_addr = next_addr;
            }
            printf("\n");
        }
        release(pool, (i / HASH_MAP_DIR_BLOCK_SIZE + 1) * PAGE_SIZE);
    }
    release(pool, 0);
    printf("------------------------------\n");
}