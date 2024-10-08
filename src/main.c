#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "input.h"
#include "meta.h"
#include "statement.h"
#include "table.h"


static void cleanup(SDBStatement *statement) {
    if (statement->tokenized.key_values != NULL) {
        free(statement->tokenized.key_values);
        statement->tokenized.key_values = NULL;
    }
}

int main(const int argc, const char** argv) {

    if (argc < 2) {
        printf("Missing database filename");
        exit(EXIT_FAILURE);
    }

    const char *db_filename = argv[1];

    SDBInputBuffer *input = sdb_input_buffer_create();
    SDBTable *table = sdb_open(db_filename);

    while (true) {
        sdb_input_read(input);

        if (sdb_meta_is_meta_command(input->buffer)) {
           sdb_meta_handle_command(input, table);
           continue;
        }

        SDBStatement statement = {0};
        switch (sdb_statement_prepare(input, &statement)) {
        case STATEMENT_PREPARE_SUCCESS:
            break; // switch break
        case STATEMENT_PREPARE_SYNTAX_ERROR:
            printf("Syntax Error. Could not parse \"%s\"\n", input->buffer);
            goto CLEAN_UP;
        case STATEMENT_PREPARE_ERROR:
            goto CLEAN_UP;
            printf("Failed to prepare statement from \"%s\"\n", input->buffer);
        }

        switch (sdb_statement_execute(&statement, table)) {
        case EXECUTE_SUCCESS:
            break; // switch break
        case EXECUTE_TABLE_FULL:
            printf("[ERROR] Execute [\"%s\"] : Table is full\n", input->buffer);
            break; // switch break
        case EXECUTE_DUP_KEY:
            printf("[ERROR] Execute [\"%s\"] : Duplicate key\n", input->buffer);
            break; // switch break
        case EXECUTE_FAILURE:
            printf("[ERROR] Execute [\"%s\"] : Unknown error\n", input->buffer);
            break; // switch break
        }
        CLEAN_UP:
            cleanup(&statement);
    }

    sdb_close(table);
    sdb_input_buffer_close(input);
    return EXIT_SUCCESS;
}
