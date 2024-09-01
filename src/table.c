#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "table.h"
#include "utils.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

// Column sizes
const uint32_t ID_SIZE = size_of_attribute(SqueelRow, id);
const uint32_t USERNAME_SIZE = size_of_attribute(SqueelRow, username);
const uint32_t EMAIL_SIZE = size_of_attribute(SqueelRow, email);
// Offsets
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
// Row size
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE; 
// Table
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

uint32_t squeel_table_max_rows(void) {
    return TABLE_MAX_ROWS;
}

uint32_t squeel_table_row_per_page(void) {
    return ROWS_PER_PAGE;
}

SqueelTable *squeel_db_open(const char *db_filename) {
    SqueelTable *table = calloc(1, sizeof(*table));
    SqueelPager *pager = squeel_pager_open(db_filename);
    table->pager = pager;
    table->num_rows = pager->f_size / ROW_SIZE;;
    return table;
}

void squeel_db_close(SqueelTable *table) {
    assert(table != NULL);
    assert(table->pager != NULL);

    SqueelPager *pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        squeel_pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }
    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree
    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            squeel_pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int rc = close(pager->fd);
    if (rc == -1) {
        perror("[ERROR] failed to close db file");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pager->pages[i];
        if (page != NULL) {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}

void squeel_serialize_row(SqueelRow *src, void *dest) {
    char *dest_bytes = (char*)dest;
    memcpy(dest_bytes + ID_OFFSET, &src->id, ID_SIZE);
    strncpy(dest_bytes + USERNAME_OFFSET, src->username, USERNAME_SIZE);
    strncpy(dest_bytes + EMAIL_OFFSET, src->email, EMAIL_SIZE);
}

void squeel_deserialize_row(void *src, SqueelRow *dest) {
    char *src_bytes = (char*)src;
    memcpy(&dest->id, src_bytes + ID_OFFSET, ID_SIZE);
    memcpy(&dest->username, src_bytes + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&dest->email, src_bytes + EMAIL_OFFSET, EMAIL_SIZE);
}

void *row_slot(SqueelTable *table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = squeel_get_page(table->pager, page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return (char*)page + byte_offset;
}

static bool get_uint32(const char *key, SqueelKeyValue *kv, uint32_t len, uint32_t *out) {
    bool found = false;
    for (uint32_t i = 0; i < len; i++) {
        if (strcmp(kv[i].key, key) == EQ) {
            *out = atoi(kv[i].value);
            // TODO: check for negative integer
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

void row_from_statement(SqueelTokenizedStatement *statment, SqueelRow *out) {
    assert(get_uint32("id", statment->key_values, statment->key_values_length, &out->id));
    assert(get_str("username", statment->key_values, statment->key_values_length, out->username, COLUMN_USERNAME_SIZE));
    assert(get_str("email", statment->key_values, statment->key_values_length, out->email, COLUMN_EMAIL_SIZE));
    return;
}