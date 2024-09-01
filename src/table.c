#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "table.h"
#include "utils.h"
#include "trees.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

// Column sizes
const uint32_t ID_SIZE = size_of_attribute(SDBRow, id);
const uint32_t USERNAME_SIZE = size_of_attribute(SDBRow, username);
const uint32_t EMAIL_SIZE = size_of_attribute(SDBRow, email);
// Offsets
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
// Row size
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE; 
// Table
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

uint32_t sdb_row_size(void) {
    return ROW_SIZE;
}

uint32_t sdb_table_max_rows(void) {
    return TABLE_MAX_ROWS;
}

uint32_t sdb_table_row_per_page(void) {
    return ROWS_PER_PAGE;
}

SDBTable *sdb_open(const char *db_filename) {
    SDBPager *pager = sdb_pager_open(db_filename);
    SDBTable *table = calloc(1, sizeof(*table));

    table->pager = pager;
    table->root_page_num = 0;

    if (pager->num_pages == 0) {
        // New DB
        void *root_node = sdb_get_page(pager, 0);
        initialize_leaf_node(root_node);
    }

    return table;
}

void sdb_close(SDBTable *table) {
    assert(table != NULL);
    assert(table->pager != NULL);

    SDBPager *pager = table->pager;
    //uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        sdb_pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
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

void sdb_serialize_row(SDBRow *src, void *dest) {
    char *dest_bytes = (char*)dest;
    memcpy(dest_bytes + ID_OFFSET, &src->id, ID_SIZE);
    strncpy(dest_bytes + USERNAME_OFFSET, src->username, USERNAME_SIZE);
    strncpy(dest_bytes + EMAIL_OFFSET, src->email, EMAIL_SIZE);
}

void sdb_deserialize_row(void *src, SDBRow *dest) {
    char *src_bytes = (char*)src;
    memcpy(&dest->id, src_bytes + ID_OFFSET, ID_SIZE);
    memcpy(&dest->username, src_bytes + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&dest->email, src_bytes + EMAIL_OFFSET, EMAIL_SIZE);
}

static bool get_uint32(const char *key, SDBKeyValue *kv, uint32_t len, uint32_t *out) {
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

static bool get_str(const char *key, SDBKeyValue *kv, uint32_t len, char *out, uint32_t out_size) {
    bool found = false;
    for (uint32_t i = 0; i < len; i++) {
        if (strcmp(kv[i].key, key) == EQ) {
            strncpy(out, kv[i].value, out_size);
            found = true;
        }
    }
    return found;
}

void sdb_row_from_statement(SDBTokenizedStatement *statment, SDBRow *out) {
    assert(get_uint32("id", statment->key_values, statment->key_values_length, &out->id));
    assert(get_str("username", statment->key_values, statment->key_values_length, out->username, COLUMN_USERNAME_SIZE));
    assert(get_str("email", statment->key_values, statment->key_values_length, out->email, COLUMN_EMAIL_SIZE));
    return;
}