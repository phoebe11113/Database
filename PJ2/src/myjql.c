#include "myjql.h"

#include "buffer_pool.h"
#include "table.h"
#include "str.h"
#include "hash_map.h"
#include <limits.h>
#include <malloc.h>

typedef struct {
    RID key;
    RID value;
} Record;

void read_record(Table *table, RID rid, Record *record) {
    table_read(table, rid, (ItemPtr)record);
}

RID write_record(Table *table, const Record *record) {
    return table_insert(table, (ItemPtr)record, sizeof(Record));
}

void delete_record(Table *table, RID rid) {
    table_delete(table, rid);
}

BufferPool bp_idx;
Table tbl_rec;
Table tbl_str;



int ptr_row_cmp(void *key, size_t key_len, RID rid) {
    
    Record record;
    read_record(&tbl_rec, rid, &record);
    StringRecord str_record;
    read_string(&tbl_str, record.key, &str_record);
    if (get_str_chunk_size(&(str_record.chunk)) == 0){ 
        if(key_len==0) return 0;
        return 1;
        }
    size_t i = 0;
    while (has_next_char(&str_record) && i != key_len) {
        char a, b;
        
        a = ((char*)key)[i];
        i++;
        b = next_char(&tbl_str, &str_record);
        // printf("%c,%c ",a,b);
        if (a != b) 
            return a > b ? 1 : -1;
    }
    if (i == key_len && has_next_char(&str_record)) 
        return -1;
    if (i != key_len && !has_next_char(&str_record)) 
        return 1;
    return 0;
}

#define HASH_TABLE_DIR_BLOCKS 1000
#define HASH_TABLE_SIZE (HASH_MAP_DIR_BLOCK_SIZE * HASH_TABLE_DIR_BLOCKS)

short hash_function(const char *key, size_t key_len) {
    unsigned short hash = 0;  
    for (size_t i = 0; i < key_len; i++) {
        hash = hash * 31 + key[i]; 
    }
    return (short)(hash % (unsigned short)(HASH_TABLE_SIZE)); 
}

void myjql_init() {
    hash_table_init("rec.idx", &bp_idx, HASH_TABLE_SIZE);
    table_init(&tbl_rec, "rec.data", "rec.fsm");
    table_init(&tbl_str, "str.data", "str.fsm");
}

void myjql_close() {
    validate_buffer_pool(&bp_idx);
    validate_buffer_pool(&tbl_rec.data_pool);
    validate_buffer_pool(&tbl_rec.fsm_pool);
    validate_buffer_pool(&tbl_str.data_pool);
    validate_buffer_pool(&tbl_str.fsm_pool);
    hash_table_close(&bp_idx);
    table_close(&tbl_rec);
    table_close(&tbl_str);
}

size_t myjql_get(const char *key, size_t key_len, char *value, size_t max_size) {
    RID rid;
    short key_hash = hash_function(key, key_len);
    rid = hash_table_search(&bp_idx,key_hash,(void*)key,key_len, &ptr_row_cmp);
    printf("FIND RID IDX:%d ADDR:%lld\n",get_rid_idx(rid),get_rid_block_addr(rid));
    if(get_rid_block_addr(rid) == -1 && get_rid_idx(rid) == 0)
        return -1;
    Record rec;
    read_record(&tbl_rec,rid,&rec);

    StringRecord Str_rec;
    read_string(&tbl_str,rec.value,&Str_rec);
    return load_string(&tbl_str,&Str_rec,value,max_size);
}

void myjql_set(const char *key, size_t key_len, const char *value, size_t value_len) {
    RID rid;
    short key_hash = hash_function(key, key_len);
    printf("Key: %s, Hash: %d\n", key, key_hash);
    rid = hash_table_search(&bp_idx,key_hash,(void*)key,key_len, &ptr_row_cmp);
    printf("FIND RID IDX:%d ADDR:%lld\n",get_rid_idx(rid),get_rid_block_addr(rid));
    if(get_rid_block_addr(rid) != -1){
        printf("DELETE!\n");
        myjql_del(key,key_len);
        }
    

    RID k,v;
    k = write_string(&tbl_str,key,key_len);
    v = write_string(&tbl_str,value,value_len);

    Record rec;
    rec.key = k;
    rec.value = v;

    RID hash_rid;
    hash_rid =  write_record(&tbl_rec,&rec);
    printf("FIND RID IDX:%d ADDR:%lld\n",get_rid_idx(hash_rid),get_rid_block_addr(hash_rid));
    hash_table_insert(&bp_idx,key_hash,hash_rid);
    printf("SUCCESSFULLY SET!\n");
    

    return;
}

void myjql_del(const char *key, size_t key_len) {
    RID rid;
    short key_hash = hash_function(key, key_len);
    rid = hash_table_search(&bp_idx,key_hash,(void*)key,key_len, &ptr_row_cmp);
    if(get_rid_block_addr(rid) == -1 && get_rid_idx(rid) == 0)
        return ;

    Record rec;
    read_record(&tbl_rec,rid,&rec);

    hash_table_pop(&bp_idx,key_hash,rid);

    delete_string(&tbl_str,rec.key);
    delete_string(&tbl_str,rec.value);

    delete_record(&tbl_rec,rid);
}

void myjql_analyze() {
    printf("Record Table:\n");
    analyze_table(&tbl_rec);
    printf("String Table:\n");
    analyze_table(&tbl_str);
}