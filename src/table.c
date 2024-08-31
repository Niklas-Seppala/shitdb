#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "table.h"
#include "utils.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

// Column sizes
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
// Offsets
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
// Row size
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE; 
// Table
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

uint32_t squeel_table_max_rows(void) {
    return TABLE_MAX_ROWS;
}

uint32_t squeel_table_row_per_page(void) {
    return ROWS_PER_PAGE;
}

Table *squeel_table_create(void) {
    Table *table = calloc(1, sizeof(*table));
    return table;
}

void squeel_table_free(Table *table) {
    assert(table != NULL && "NULL table in free");
    for (uint32_t i = 0; i < table->num_rows; i++) {
        assert(table->pages[i] != NULL && "NULL page in table free");
        free(table->pages[i]);
    }
    free(table);
}

void squeel_serialize_row(Row *src, void *dest) {
    char *dest_bytes = (char*)dest;
    memcpy(dest_bytes + ID_OFFSET, &src->id, ID_SIZE);
    memcpy(dest_bytes + EMAIL_OFFSET, &src->email, EMAIL_SIZE);
    memcpy(dest_bytes + USERNAME_OFFSET, &src->username, USERNAME_SIZE);
}

void squeel_deserialize_row(void *src, Row *dest) {
    char *src_bytes = (char*)src;
    memcpy(&dest->id, src_bytes + ID_OFFSET, ID_SIZE);
    memcpy(&dest->username, src_bytes + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&dest->email, src_bytes + EMAIL_OFFSET, EMAIL_SIZE);
}

void *row_slot(Table *table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table->pages[page_num];
    if (page == NULL) {
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }

    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;


    return (char*)page + byte_offset;
}

static bool get_uint32(const char *key, SqueelKeyValue *kv, uint32_t len, uint32_t *out) {
    bool found = false;
    for (uint32_t i = 0; i < len; i++) {
        if (strcmp(kv[i].key, key) == EQ) {
            *out = atoi(kv[i].value);
            found = true;
        }
    }
    return found;
}

static bool get_str(const char *key, SqueelKeyValue *kv, uint32_t len, char *out, uint32_t out_size) {
    bool found = false;
    for (uint32_t i = 0; i < len; i++) {
        if (strcmp(kv[i].key, key) == EQ) {
            strncpy(out, kv[i].value, out_size);
            found = true;
        }
    }
    return found;
}

void row_from_statement(SqueelTokenizedStatement *statment, Row *out) {
    assert(get_uint32("id", statment->key_values, statment->key_values_length, &out->id));
    assert(get_str("username", statment->key_values, statment->key_values_length, out->username, COLUMN_USERNAME_SIZE));
    assert(get_str("email", statment->key_values, statment->key_values_length, out->email, COLUMN_EMAIL_SIZE));
    return;
}