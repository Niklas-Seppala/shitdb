#include "statement.h"
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "table.h"
#include "cursor.h"
#include "trees.h"

// TODO: might be nice to free this at the end :)
static SDBInputBuffer *input_copy = NULL; 

void leaf_node_insert(SDBCursor* cursor, uint32_t key, SDBRow* value) {
    void* node = sdb_get_page(cursor->table->pager, cursor->page_num);

    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= leaf_node_max_cells()) {
        // Node full
        printf("Need to implement splitting a leaf node.\n");
        exit(EXIT_FAILURE);
    }

    if (cursor->cell_num < num_cells) {
        // Make room for new cell
        for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), leaf_node_cell_size());
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->cell_num)) = key;
    sdb_serialize_row(value, leaf_node_value(node, cursor->cell_num));
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
    void* node = sdb_get_page(table->pager, table->root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= leaf_node_max_cells()) {
        return EXECUTE_TABLE_FULL;
    }

    SDBRow row_to_insert;
    sdb_row_from_statement(&statement->tokenized, &row_to_insert);
    uint32_t key = row_to_insert.id;
    
    SDBCursor cursor;
    sdb_cursor_find(&cursor, table, key);

    if (cursor.cell_num < num_cells) {
        uint32_t key_at = *leaf_node_key(node, cursor.cell_num);
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