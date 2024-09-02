#include "meta.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "utils.h"
#include "pager.h"
#include "statement.h"

#define META_PREFIX   '.'
#define CMD_EXIT      ".exit"
#define CMD_BTREE     ".btree"
#define CMD_CONST     ".const"

bool sdb_meta_is_meta_command(const char *command) {
    return command[0] == META_PREFIX;
}

static enum SDBMetaCommand parse_meta(SDBInputBuffer *input) {
    if (strncasecmp(CMD_EXIT, input->buffer, input->input_len) == EQ) {
       return EXIT;
    }
    if (strncasecmp(CMD_BTREE, input->buffer, input->input_len) == EQ) {
       return BTREE;
    }
    if (strncasecmp(CMD_CONST, input->buffer, input->input_len) == EQ) {
       return CONST;
    }
    return UNRECOGNICED;
}

void sdb_meta_handle_command(SDBInputBuffer *input, SDBTable *table) {
    switch (parse_meta(input))
    {
    case EXIT:
        sdb_close(table);
        printf("Goodbye\n");
        exit(EXIT_SUCCESS);
    case BTREE:
        printf("Tree:\n");
        print_tree(table->pager, 0, 0);
        break;
    case CONST:
        printf("Constants:\n");
        print_constants();
        break;
    case UNRECOGNICED:
        printf("Unrecognized command \"%s\"\n", input->buffer);
        break;
    default:
        assert(false && "Should never get here");
    }
}