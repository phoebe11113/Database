#include "str.h"

#include "table.h"

void read_string(Table *table, RID rid, StringRecord *record) {
    table_read(table, rid, record->chunk.data);
    record->idx = 0;
}

int has_next_char(StringRecord *record) {
    return !((record->idx == get_str_chunk_size(record)) && 
             (get_rid_block_addr(get_str_chunk_rid(record)) == -1));
}

char next_char(Table *table, StringRecord *record) {
    if(!has_next_char(record))
        return '\0';
    if(record->idx == get_str_chunk_size(record))
        read_string(table, get_str_chunk_rid(record), record);
    return get_str_chunk_data_ptr(record)[record->idx++];
}

int compare_string_record(Table *table, const StringRecord *a, const StringRecord *b) {
    StringRecord tmp_a = *a, tmp_b = *b;
    while(1){
        char cha = next_char(table, &tmp_a);
        char chb = next_char(table, &tmp_b);
        if(cha < chb || (cha == '\0' && chb != '\0'))
            return -1;
        else if(cha > chb || (cha != '\0' && chb == '\0'))
            return 1;
        else if(cha == '\0' && chb == '\0')
            return 0;
        else
            continue;
    }
}

RID write_string(Table *table, const char *data, off_t size) {
    int first = 0;
    RID rid;
    StringChunk chunk;
    get_rid_block_addr(rid) = -1;
    get_rid_idx(rid) = 0;

    if (size <= 0) {
        get_str_chunk_rid(&chunk) = rid;
        get_str_chunk_size(&chunk) = 0;
        rid = table_insert(table, (ItemPtr)&chunk, calc_str_chunk_size(0));
        return rid;
    }

    while(size != 0){
        short chunk_size;
        if(size % STR_CHUNK_MAX_LEN != 0){
            chunk_size = size % STR_CHUNK_MAX_LEN;
        }
        else{
            chunk_size = STR_CHUNK_MAX_LEN;
        }
        size = size - chunk_size;
        for(int i = 0; i < chunk_size ; i++){
            get_str_chunk_data_ptr(&chunk)[i] = data[size + i];
        }
        get_str_chunk_size(&chunk) = chunk_size;
        if(first != 1){
            RID null_rid ;
            get_rid_block_addr(null_rid) = -1;
            get_rid_idx(null_rid) = -1;
            get_str_chunk_rid(&chunk) = null_rid;
            first = 1;
        }
        else {
            get_str_chunk_rid(&chunk) = rid;
        }
        rid = table_insert(table, (ItemPtr)&chunk,calc_str_chunk_size(chunk_size));

    }
    return rid;
}

void delete_string(Table *table, RID rid) {
    off_t block_addr = get_rid_block_addr(rid);
    StringChunk chunk ;
    while( block_addr != -1){
        
        table_read(table,rid,(ItemPtr)&chunk);
        table_delete(table,rid);
        rid = get_str_chunk_rid(&chunk);
        block_addr = get_rid_block_addr(rid);
    }
}

void print_string(Table *table, const StringRecord *record) {
    StringRecord rec = *record;
    printf("\"");
    while (has_next_char(&rec)) {
        printf("%c", next_char(table, &rec));
    }
    printf("\"");
}

size_t load_string(Table *table, const StringRecord *record, char *dest, size_t max_size) {
    StringRecord tmp_record = *record;
    size_t cnt = 0;
    while(has_next_char(&tmp_record) && cnt < max_size)
        dest[cnt++] = next_char(table, &tmp_record);
    dest[cnt] = '\0';
    return cnt;
}

void chunk_printer(ItemPtr item, short item_size) {
    if (item == NULL) {
        printf("NULL");
        return;
    }
    StringChunk *chunk = (StringChunk*)item;
    short size = get_str_chunk_size(chunk), i;
    printf("StringChunk(");
    print_rid(get_str_chunk_rid(chunk));
    printf(", %d, \"", size);
    for (i = 0; i < size; i++) {
        printf("%c", get_str_chunk_data_ptr(chunk)[i]);
    }
    printf("\")");
}