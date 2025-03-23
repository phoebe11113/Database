#include "block.h"

#include <stdio.h>

void init_block(Block *block) {
    block->n_items = 0;
    block->head_ptr = (short)(block->data - (char*)block);
    block->tail_ptr = (short)sizeof(Block);
}

ItemPtr get_item(Block *block, short idx) {
    if (idx < 0 || idx >= block->n_items) {
        printf("get item error: idx is out of range: %d\n",idx);
        return NULL;
    }
    ItemID item_id = get_item_id(block, idx);
    if (get_item_id_availability(item_id)) {
        printf("get item error: item_id is not used\n");
        return NULL;
    }
    short offset = get_item_id_offset(item_id);
    return (char*)block + offset;
}

short new_item(Block *block, ItemPtr item, short item_size) {
    short i = 0;
    short idx = -1;
    // find if there exists free itemID
    for(i = 0; i < block->n_items; i++)
        if(get_item_id_availability(get_item_id(block, i))){ // free itemID
            if(block->tail_ptr - block->head_ptr < item_size){
                printf("new item error: available block space is not enough\n");
                return idx;
            }
            idx = i;
            break;
        }
    // if not exists, allocate a new itemID
    if(idx == -1){
        if(block->tail_ptr - block->head_ptr < item_size + (short)(sizeof(ItemID))){
            printf("new item error: available block space is not enough\n");
            return idx;
        }
        idx = block->n_items++;
        block->head_ptr += (short)(sizeof(ItemID));
    }
    // revise tail_ptr
    block->tail_ptr -= item_size;
    // revise itemID
    get_item_id(block, idx) = compose_item_id(0, block->tail_ptr, item_size);
    // add item
    ItemPtr item_idx = (char*)block + block->tail_ptr;
    for(i = item_size - 1; i >= 0; i--)
        item_idx[i] = item[i];
    // return idx
    return idx;
}

void delete_item(Block *block, short idx) {
    // check if idx is out of range
    if(idx < 0 || idx >= block->n_items){
        printf("delete item error: idx is out of range\n");
        return;
    }
    // check if idx has already been deleted
    if(get_item_id_availability(get_item_id(block, idx))){
        printf("delete item error: idx has already been deleted\n");
        return;
    }

    short i = 0;
    // delete item
    ItemID del_item_id = get_item_id(block, idx);
    short del_item_offset = get_item_id_offset(del_item_id);
    short del_item_size = get_item_id_size(del_item_id);
    get_item_id(block, idx) = compose_item_id(1, 0, 0);
    // move items and revise corresponding itemIDs
    char *beg1 = (char*)block + block->tail_ptr,
         *beg2 = (char*)block + block->tail_ptr + del_item_size;
    for(i = del_item_offset - block->tail_ptr - 1; i >= 0; i--)
        beg2[i] = beg1[i];
    for(i = 0; i < block->n_items; i++) {
        ItemID item_id = get_item_id(block, i);
        if(get_item_id_availability(item_id)) // free itemID
            continue;
        short item_offset = get_item_id_offset(item_id),
              item_size = get_item_id_size(item_id);
        if(item_offset < del_item_offset){
            item_offset += del_item_size;
            get_item_id(block, i) = compose_item_id(0, item_offset, item_size);
        }
    }
    // revise tail_ptr
    block->tail_ptr += del_item_size;
    // delete tail free itemIDs
    for(i = block->n_items - 1; i >= 0; i--){
        if(!get_item_id_availability(get_item_id(block, i))) // if available
            break;
        block->head_ptr -= (short)(sizeof(ItemID));
        block->n_items--;
    }
}

void str_printer(ItemPtr item, short item_size) {
    if (item == NULL) {
        printf("NULL");
        return;
    }
    short i;
    printf("\"");
    for (i = 0; i < item_size; ++i) {
        printf("%c", item[i]);
    }
    printf("\"");
}

void print_block(Block *block, printer_t printer) {
    short i, availability, offset, size;
    ItemID item_id;
    ItemPtr item;
    printf("----------BLOCK----------\n");
    printf("total = %d\n", block->n_items);
    printf("head = %d\n", block->head_ptr);
    printf("tail = %d\n", block->tail_ptr);
    for (i = 0; i < block->n_items; ++i) {
        item_id = get_item_id(block, i);
        availability = get_item_id_availability(item_id);
        offset = get_item_id_offset(item_id);
        size = get_item_id_size(item_id);
        if (!availability) {
            item = get_item(block, i);
        } else {
            item = NULL;
        }
        printf("%10d%5d%10d%10d\t", i, availability, offset, size);
        printer(item, size);
        printf("\n");
    }
    printf("-------------------------\n");
}

void print_rid(RID rid) {
    printf("RID(" FORMAT_OFF_T ", %d)", get_rid_block_addr(rid), get_rid_idx(rid));
}

void analyze_block(Block *block, block_stat_t *stat) {
    short i;
    stat->empty_item_ids = 0;
    stat->total_item_ids = block->n_items;
    for (i = 0; i < block->n_items; ++i) {
        if (get_item_id_availability(get_item_id(block, i))) {
            ++stat->empty_item_ids;
        }
    }
    stat->available_space = block->tail_ptr - block->head_ptr 
        + stat->empty_item_ids * sizeof(ItemID);
}

void accumulate_stat_info(block_stat_t *stat, const block_stat_t *stat2) {
    stat->empty_item_ids += stat2->empty_item_ids;
    stat->total_item_ids += stat2->total_item_ids;
    stat->available_space += stat2->available_space;
}

void print_stat_info(const block_stat_t *stat) {
    printf("==========STAT==========\n");
    printf("empty_item_ids: " FORMAT_SIZE_T "\n", stat->empty_item_ids);
    printf("total_item_ids: " FORMAT_SIZE_T "\n", stat->total_item_ids);
    printf("available_space: " FORMAT_SIZE_T "\n", stat->available_space);
    printf("========================\n");
}