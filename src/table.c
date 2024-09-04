#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "table.h"
#include "utils.h"
#include "btree.h"
#include "tableconsts.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

// Offsets
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + COLUMN_ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + COLUMN_USERNAME_SIZE;
// Table

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
        SDBTreeNode *root_node = sdb_pager_get_page(pager, 0);
        root_node->type = SDB_LEAF_NODE | SDB_ROOT_NODE;
        root_node->body.leaf.num_cells = 0;
        root_node->body.leaf.sibling = 0;
    }

    return table;
}

void sdb_close(SDBTable *table) {
    assert(table != NULL);
    assert(table->pager != NULL);

    SDBPager *pager = table->pager;
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

void sdb_serialize_row(SDBRow *src, char *dest) {
    memcpy(dest + ID_OFFSET, &src->id, COLUMN_ID_SIZE);
    strncpy(dest + USERNAME_OFFSET, src->username, COLUMN_USERNAME_SIZE);
    strncpy(dest + EMAIL_OFFSET, src->email, COLUMN_EMAIL_SIZE);
}

void sdb_deserialize_row(char *src, SDBRow *dest) {
    memcpy(&dest->id, src + ID_OFFSET, COLUMN_ID_SIZE);
    memcpy(&dest->username, src + USERNAME_OFFSET, COLUMN_USERNAME_SIZE);
    memcpy(&dest->email, src + EMAIL_OFFSET, COLUMN_EMAIL_SIZE);
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