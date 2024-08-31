#include "statement.h"
#include <string.h>
#include <stdio.h>

#define EQ 0

#define UNUSED(a) ((void)a)

StatementPrepareStatus squeel_statement_prepare(SqueelInputBuffer *input, Statement *statement) {
    if (strncasecmp(input->buffer, "insert", 6) == EQ) {
        statement->type = STATEMENT_INSERT;
        int args_n = sscanf(input->buffer, "insert %u %s %s",
            &statement->row_to_insert.id,
            statement->row_to_insert.username,
            statement->row_to_insert.email);
        if (args_n < 3) {
            return STATEMENT_PREPARE_SYNTAX_ERROR;
        }
        return STATEMENT_PREPARE_SUCCESS;
    }

    if (strncasecmp(input->buffer, "select", input->input_len) == EQ) {
        statement->type = STATEMENT_SELECT;
        return STATEMENT_PREPARE_SUCCESS;
    }

    return STATEMENT_PREPARE_ERROR;
} 

ExecuteResult squeel_insert_execute(Statement *statement, Table *table) {

    if (table->num_rows >= squeel_table_max_rows()) {
        return EXECUTE_TABLE_FULL;
    }

    Row *row_to_insert = &statement->row_to_insert;
    squeel_serialize_row(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows++;

    return EXECUTE_SUCCESS;
} 

void print_row(Row *row) {
    printf("{ \"id\": %u, \"username\": \"%s\", \"email\": \"%s\" }\n", row->id, row->username, row->email);
}

ExecuteResult squeel_select_execute(Statement *statement, Table *table) {
    UNUSED(statement);
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        squeel_deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
} 

ExecuteResult squeel_statement_execute(Statement *statement, Table *table) {
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