#include "statement.h"
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "table.h"

// TODO: might be nice to free this at the end :)
static SqueelInputBuffer *input_copy = NULL; 

StatementPrepareStatus squeel_statement_prepare(SqueelInputBuffer *input, SqueelStatement *statement) {
    if (input_copy == NULL) {
        input_copy = squeel_input_buffer_create();
        input_copy->buffer = calloc(input->buffer_len, 1);
    }

    squeel_input_buffer_copy(input, input_copy);

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

ExecuteResult squeel_insert_execute(SqueelStatement *statement, SqueelTable *table) {

    if (table->num_rows >= squeel_table_max_rows()) {
        return EXECUTE_TABLE_FULL;
    }
    SqueelRow row_to_insert;
    row_from_statement(&statement->tokenized, &row_to_insert);
    squeel_serialize_row(&row_to_insert, row_slot(table, table->num_rows));
    table->num_rows++;

    return EXECUTE_SUCCESS;
} 

void print_row(SqueelRow *row) {
    printf("{ \"id\": %u, \"username\": \"%s\", \"email\": \"%s\" }\n", row->id, row->username, row->email);
}

ExecuteResult squeel_select_execute(SqueelStatement *statement, SqueelTable *table) {
    UNUSED(statement);
    SqueelRow row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        squeel_deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
} 

ExecuteResult squeel_statement_execute(SqueelStatement *statement, SqueelTable *table) {
    switch (statement->type)
    {
    case STATEMENT_INSERT:
        return squeel_insert_execute(statement, table);
    case STATEMENT_SELECT:
        return squeel_select_execute(statement, table);
    default: 
        return EXECUTE_FAILURE;
    }
} 