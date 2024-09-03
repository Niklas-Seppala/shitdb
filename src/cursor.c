#include "cursor.h"
#include <assert.h>
#include "btree.h"
#include <stdio.h>

void sdb_cursor_start(SDBCursor *cursor, SDBTable *table) {
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;

    SDBTreeNode *root_node = sdb_pager_get_page(table->pager, table->root_page_num);
    assert(root_node->type & SDB_LEAF_NODE && "Only leaf node for now");
    cursor->end_of_table = (root_node->body.leaf.num_cells == 0);
}

static void leaf_node_find(SDBCursor *cursor, SDBTable* table, uint32_t page_num, uint32_t key) {
    SDBTreeNode *node = sdb_pager_get_page(table->pager, page_num);

    assert(node->type & SDB_LEAF_NODE && "Only leaf node for now");

    cursor->table = table;
    cursor->page_num = page_num;

    // Binary search
    uint32_t min_index = 0;
    uint32_t one_past_max_index = node->body.leaf.num_cells;

    while (one_past_max_index != min_index) {
        uint32_t index = (min_index + one_past_max_index) / 2;
        uint32_t key_at_index = node->body.leaf.cells[index].key;
        if (key == key_at_index) {
            cursor->cell_num = index;
            return;
        }
        if (key < key_at_index) {
            one_past_max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    cursor->cell_num = min_index;
}

/*
Return the position of the given key.
If the key is not present, return the position
where it should be inserted
*/
void sdb_cursor_find(SDBCursor *cursor, SDBTable* table, uint32_t key) {
    uint32_t root_page_num = table->root_page_num;
    SDBTreeNode* root_node = sdb_pager_get_page(table->pager, root_page_num);

    if (root_node->type & SDB_LEAF_NODE) {
        leaf_node_find(cursor, table, root_page_num, key);
    } else {
        printf("Need to implement searching an internal node\n");
        exit(EXIT_FAILURE);
    }
}

char *sdb_cursor_value(SDBCursor *cursor) {
    uint32_t page_num = cursor->page_num;
    SDBTreeNode *page = sdb_pager_get_page(cursor->table->pager, page_num);

    assert(page->type & SDB_LEAF_NODE && "Only leaf nodes for now");
    return page->body.leaf.cells[cursor->cell_num].value;
}

void sdb_cursor_advance(SDBCursor *cursor) {
    uint32_t page_num = cursor->page_num;
    SDBTreeNode *page = sdb_pager_get_page(cursor->table->pager, page_num);
    assert(page->type & SDB_LEAF_NODE && "Only leaf nodes for now");

    assert(cursor->cell_num <= page->body.leaf.num_cells && "Skipping cell");
    cursor->cell_num++;
    if (cursor->cell_num >= page->body.leaf.num_cells) {
        cursor->end_of_table = true;
    }
}