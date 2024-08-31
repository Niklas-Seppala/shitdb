#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "input.h"
#include "meta.h"
#include "statement.h"
#include "table.h"


int main(void) {
    SqueelInputBuffer *input = squeel_input_buffer_create();
    Table *table = squeel_table_create();

    while (true) {
        squeel_input_read(input);

        if (squeel_meta_is_meta_command(input->buffer)) {
           squeel_meta_handle_command(input);
           continue;
        }

        Statement statement;
        switch (squeel_statement_prepare(input, &statement)) {
        case STATEMENT_PREPARE_SUCCESS:
            break;
        case STATEMENT_PREPARE_SYNTAX_ERROR:
            printf("Syntax Error. Could not parse \"%s\"\n", input->buffer);
            continue;
        case STATEMENT_PREPARE_ERROR:
            printf("Failed to prepare statement from \"%s\"\n", input->buffer);
            continue;
        }

        switch (squeel_statement_execute(&statement, table)) {
        case EXECUTE_SUCCESS:
            break;
        case EXECUTE_TABLE_FULL:
            printf("[ERROR] Execute [\"%s\"] : Table is full\n", input->buffer);
            break;
        case EXECUTE_FAILURE:
            printf("[ERROR] Execute [\"%s\"] : Unknown error\n", input->buffer);
            break;
        }
    }

    squeel_table_free(table);
    squeel_input_buffer_close(input);
    return EXIT_SUCCESS;
}
