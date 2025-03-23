#include "table.h"

#include "hash_map.h"

#include <stdio.h>
#include <string.h>

void table_init(Table *table, const char *data_filename, const char *fsm_filename) {
    init_buffer_pool(data_filename, &table->data_pool);
    hash_table_init(fsm_filename, &table->fsm_pool, PAGE_SIZE / HASH_MAP_DIR_BLOCK_SIZE);
}

void table_close(Table *table) {
    close_buffer_pool(&table->data_pool);
    hash_table_close(&table->fsm_pool);
}

off_t table_get_total_blocks(Table *table) {
    return table->data_pool.file.length / PAGE_SIZE;
}

short table_block_get_total_items(Table *table, off_t block_addr) {
    Block *block = (Block*)get_page(&table->data_pool, block_addr);
    short n_items = block->n_items;
    release(&table->data_pool, block_addr);
    return n_items;
}

void table_read(Table *table, RID rid, ItemPtr dest) {
    off_t block_addr = get_rid_block_addr(rid);
    short idx = get_rid_idx(rid);

    Block *block = (Block*)get_page(&(table->data_pool), block_addr);

    ItemID itemid = get_item_id(block, idx);
    short item_size = get_item_id_size(itemid);
    memcpy(dest, get_item(block, idx), item_size);

    release(&(table->data_pool), block_addr);
    return;
}

RID table_insert(Table *table, ItemPtr src, short size) {
    short i = 0;

    RID rid = hash_table_pop_lower_bound(&(table->fsm_pool), size);
    off_t block_addr = get_rid_block_addr(rid);
    off_t insert_addr = -1;
    Block *block = NULL;
    if(block_addr == -1){ // need new block
        insert_addr = table->data_pool.file.length;
        block = (Block*)get_page(&(table->data_pool), insert_addr);
        init_block(block);
    } else {
        insert_addr = block_addr;
        block = (Block*)get_page(&(table->data_pool), insert_addr);
    }

    short idx = new_item(block, src, size);

    short is_full = 1, avali_space = 0;
    for(i = 0; i < block->n_items; i++)
        if(get_item_id_availability(get_item_id(block, i))){ 
            is_full = 0;
            break;
        }
    if(is_full)
        avali_space = block->tail_ptr - block->head_ptr - (short)(sizeof(ItemID));
    else
        avali_space = block->tail_ptr - block->head_ptr;

    
    get_rid_block_addr(rid) = insert_addr;
    get_rid_idx(rid) = 0;
    hash_table_insert(&(table->fsm_pool), avali_space, rid);
    
    release(&(table->data_pool), insert_addr);  // Ensure release here
    get_rid_idx(rid) = idx;

    return rid;
}


void table_delete(Table *table, RID rid) {
    short i = 0;
    RID new_rid;

    off_t block_addr = get_rid_block_addr(rid);
    Block *block = (Block*)get_page(&(table->data_pool), block_addr);
    get_rid_block_addr(new_rid)=block_addr;
    get_rid_idx(new_rid)=0;
    
    short idx = get_rid_idx(rid);
    short ori_is_full = 1, ori_avail_space = 0;
    for(i = 0; i < block->n_items; i++)
        if(get_item_id_availability(get_item_id(block, i))){
            ori_is_full = 0;
            break;
        }
    if(ori_is_full)
        ori_avail_space = block->tail_ptr - block->head_ptr - (short)(sizeof(ItemID));
    else
        ori_avail_space = block->tail_ptr - block->head_ptr;
   
    delete_item(block, idx);
    
    short is_full = 1, avali_space = 0;
    for(i = 0; i < block->n_items; i++)
        if(get_item_id_availability(get_item_id(block, i))){ 
            is_full = 0;
            break;
        }
    if(is_full)
        avali_space = block->tail_ptr - block->head_ptr - (short)(sizeof(ItemID));
    else
        avali_space = block->tail_ptr - block->head_ptr;
    hash_table_pop(&(table->fsm_pool), ori_avail_space, new_rid);
    hash_table_insert(&(table->fsm_pool), avali_space, new_rid);

    release(&(table->data_pool), block_addr);  // Ensure release here
}


void print_table(Table *table, printer_t printer) {
    printf("\n---------------TABLE---------------\n");
    off_t i, total = table_get_total_blocks(table);
    off_t block_addr;
    Block *block;
    for (i = 0; i < total; ++i) {
        block_addr = i * PAGE_SIZE;
        block = (Block*)get_page(&table->data_pool, block_addr);
        printf("[" FORMAT_OFF_T "]\n", block_addr);
        print_block(block, printer);
        release(&table->data_pool, block_addr);
    }
    printf("***********************************\n");
    print_hash_table(&table->fsm_pool);
    printf("-----------------------------------\n\n");
}

void analyze_table(Table *table) {
    block_stat_t stat, curr;
    off_t i, total = table_get_total_blocks(table);
    off_t block_addr;
    Block *block;
    stat.empty_item_ids = 0;
    stat.total_item_ids = 0;
    stat.available_space = 0;
    for (i = 0; i < total; ++i) {
        block_addr = i * PAGE_SIZE;
        block = (Block*)get_page(&table->data_pool, block_addr);
        analyze_block(block, &curr);
        release(&table->data_pool, block_addr);
        accumulate_stat_info(&stat, &curr);
    }
    printf("++++++++++ANALYSIS++++++++++\n");
    printf("total blocks: " FORMAT_OFF_T "\n", total);
    total *= PAGE_SIZE;
    printf("total size: " FORMAT_OFF_T "\n", total);
    printf("occupancy: %.4f\n", 1. - 1. * stat.available_space / total);
    printf("ItemID occupancy: %.4f\n", 1. - 1. * stat.empty_item_ids / stat.total_item_ids);
    printf("++++++++++++++++++++++++++++\n\n");
}