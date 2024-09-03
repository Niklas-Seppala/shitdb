#include "statement.h"
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "table.h"
#include "cursor.h"
#include <assert.h>
#include "btree.h"

// TODO: might be nice to free this at the end :)
static SDBInputBuffer *input_copy = NULL; 

static void create_new_root(SDBTable *table, uint32_t right_child_page_num) {
    /*
    Handle splitting the root.
    Old root copied to new page, becomes left child.
    Address of right child passed in.
    Re-initialize root page to contain the new root node.
    New root node points to two children.
    */

    SDBInternalNode *root = sdb_pager_get_page(table->pager, table->root_page_num);
    void *right_child = sdb_pager_get_page(table->pager, right_child_page_num);
    UNUSED(right_child);
    uint32_t left_child_page_num = sdb_pager_unused_page_num(table->pager);
    void *left_child = sdb_pager_get_page(table->pager, left_child_page_num);

    /* Left child has data copied from old root */
    memcpy(left_child, root, PAGE_SIZE);
    // Clear root bit.
    ((SDBTreeNode *)left_child)->type &= ~SDB_ROOT_NODE;

    root->common_header.type = SDB_INTERNAL_NODE | SDB_ROOT_NODE;
    root->num_keys = 1;
    *internal_node_child(root, 0) = left_child_page_num;
    uint32_t left_child_max_key = get_node_max_key(left_child);
    root->children[0].key = left_child_max_key;
    root->right_child = right_child_page_num;
}

 /*
 Create a new node and move half the cells over.
 Insert the new value in one of the two nodes.
 Update parent or create a new parent.
 */
static void leaf_node_split_and_insert(SDBCursor* cursor, uint32_t key, SDBRow* value) {
    UNUSED(key); // TODO
    SDBLeafNode *old_node = sdb_pager_get_page(cursor->table->pager, cursor->page_num);
    
    uint32_t new_page_num = sdb_pager_unused_page_num(cursor->table->pager);
    SDBLeafNode *new_node = sdb_pager_get_page(cursor->table->pager, new_page_num);
    
    new_node->num_cells = 0;
    new_node->common_header.type = SDB_LEAF_NODE;

    /*
insert id=1 username=a email=a
insert id=2 username=a email=a
insert id=3 username=a email=a
insert id=4 username=a email=a
insert id=5 username=a email=a
insert id=6 username=a email=a
insert id=7 username=a email=a
insert id=8 username=a email=a
insert id=9 username=a email=a
insert id=10 username=a email=a
insert id=11 username=a email=a
insert id=12 username=a email=a
insert id=13 username=a email=a
insert id=14 username=a email=a

    All existing keys plus new key should be divided
    evenly between old (left) and new (right) nodes.
    Starting from the right, move each key to correct position.
    */
    for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
        SDBLeafNode *dest_node;
        if (i >= (int32_t)LEAF_NODE_LEFT_SPLIT_COUNT) {
            dest_node = new_node;
        } else {
            dest_node = old_node;
        }
        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        char *cell_addr = (char *)&dest_node->cells[index_within_node];

        if (i == (int32_t)cursor->cell_num) {
            // Insert the new value
            sdb_serialize_row(value, cell_addr);
            dest_node->cells[index_within_node].key = key;
        } else if (i > (int32_t)cursor->cell_num) {
            // Copy existing cell
            memcpy(cell_addr, &old_node->cells[i-1], LEAF_NODE_CELL_SIZE);
        } else {
            // Copy existing value
            memcpy(cell_addr, &old_node->cells[i], LEAF_NODE_CELL_SIZE);
        }
    }

    old_node->num_cells = LEAF_NODE_LEFT_SPLIT_COUNT;
    new_node->num_cells = LEAF_NODE_RIGHT_SPLIT_COUNT;

    if (old_node->common_header.type & SDB_ROOT_NODE) {
        // TODO: return create_new_root(cursor->table, new_page_num);
        create_new_root(cursor->table, new_page_num);
    } else {
        printf("Need to implement updating parent after split\n");
        exit(EXIT_FAILURE);
    }

}

void leaf_node_insert(SDBCursor* cursor, uint32_t key, SDBRow* value) {
    SDBTreeNode* page = sdb_pager_get_page(cursor->table->pager, cursor->page_num);
    assert(page->type & SDB_LEAF_NODE && "Only feal nodes for now");
    SDBLeafNode *node = (SDBLeafNode*) page;

    uint32_t num_cells = node->num_cells;
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node full
        leaf_node_split_and_insert(cursor, key, value);
        return;
    }

    if (cursor->cell_num < num_cells) {
        // Make room for new cell
        for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
            memcpy(&node->cells[i], &node->cells[i-1], LEAF_NODE_CELL_SIZE);
        }
    }

    node->num_cells++;
    node->cells[cursor->cell_num].key = key;
    char *dest = node->cells[cursor->cell_num].value;
    sdb_serialize_row(value, dest);
}


StatementPrepareStatus sdb_statement_prepare(SDBInputBuffer *input, SDBStatement *statement) {
    if (input_copy == NULL) {
        input_copy = sdb_input_buffer_create();
        input_copy->buffer = calloc(input->buffer_len, 1);
    }

    sdb_input_buffer_copy(input, input_copy);

    if (tokenize(input_copy, &statement->tokenized) == TOKENIZATION_FAILURE) {
        return STATEMENT_PREPARE_SYNTAX_ERROR;
    }

    switch (statement->tokenized.keyword)
    {
    case INSERT_TOKEN:
        statement->type = STATEMENT_INSERT;
        break;
    case SELECT_TOKEN:
        statement->type = STATEMENT_SELECT;
        break;
    case INVALID_TOKEN:
        return STATEMENT_PREPARE_SYNTAX_ERROR;
    }
    return STATEMENT_PREPARE_SUCCESS;
}

ExecuteResult sdb_insert_execute(SDBStatement *statement, SDBTable *table) {
    SDBTreeNode* page = sdb_pager_get_page(table->pager, table->root_page_num);
    assert(page->type & SDB_LEAF_NODE);
    SDBLeafNode *node = (SDBLeafNode *)page;

    uint32_t num_cells = node->num_cells;

    SDBRow row_to_insert;
    sdb_row_from_statement(&statement->tokenized, &row_to_insert);
    uint32_t key = row_to_insert.id;
    
    SDBCursor cursor;
    sdb_cursor_find(&cursor, table, key);

    if (cursor.cell_num < num_cells) {
        uint32_t key_at = node->cells[cursor.cell_num].key;
        if (key_at == key) {
            return EXECUTE_DUP_KEY;
        }
    }

    leaf_node_insert(&cursor, row_to_insert.id, &row_to_insert);
    return EXECUTE_SUCCESS;
} 

void print_row(SDBRow *row) {
    printf("{ \"id\": %u, \"username\": \"%s\", \"email\": \"%s\" }\n", row->id, row->username, row->email);
}

ExecuteResult sdb_select_execute(SDBStatement *statement, SDBTable *table) {
    UNUSED(statement);
    SDBRow row;
    SDBCursor cursor;
    sdb_cursor_start(&cursor, table);
    while (!(cursor.end_of_table)) {
        sdb_deserialize_row(sdb_cursor_value(&cursor), &row);
        print_row(&row);
        sdb_cursor_advance(&cursor);
    }
    return EXECUTE_SUCCESS;
} 

ExecuteResult sdb_statement_execute(SDBStatement *statement, SDBTable *table) {
    switch (statement->type)
    {
    case STATEMENT_INSERT:
        return sdb_insert_execute(statement, table);
    case STATEMENT_SELECT:
        return sdb_select_execute(statement, table);
    default: 
        return EXECUTE_FAILURE;
    }
} 


void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    printf("  ");
  }
}

void print_tree(SDBPager* pager, uint32_t page_num, uint32_t indentation_level) {
    SDBTreeNode* node = sdb_pager_get_page(pager, page_num);
    uint32_t num_keys, child;

    if (node->type & SDB_LEAF_NODE) {
        SDBLeafNode *leaf = (SDBLeafNode *)node;
        num_keys = leaf->num_cells;
        indent(indentation_level);
        printf("- leaf (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++) {
            indent(indentation_level + 1);
            printf("- %d\n", leaf->cells[i].key);
        }
    }

    if (node->type & SDB_INTERNAL_NODE) {
        SDBInternalNode *parent = (SDBInternalNode *)node;
        num_keys = parent->num_keys;
        indent(indentation_level);
        printf("- internal (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++) {
            child = *internal_node_child(parent, i);
            print_tree(pager, child, indentation_level + 1);
            indent(indentation_level + 1);
            printf("- key %d\n", parent->children[i].key);
        }
        child = parent->right_child;
        print_tree(pager, child, indentation_level + 1);
    }
}