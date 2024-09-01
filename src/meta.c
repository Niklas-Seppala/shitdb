#include "meta.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "utils.h"

#define META_PREFIX '.'

bool squeel_meta_is_meta_command(const char *command) {
    return command[0] == META_PREFIX;
}

static enum SqueelMetaCommand parse_meta(SqueelInputBuffer *input) {
    if (strncasecmp(CMD_EXIT, input->buffer, input->input_len) == EQ) {
       return EXIT;
    }
    return UNRECOGNICED;
}

void squeel_meta_handle_command(SqueelInputBuffer *input, SqueelTable *table) {
    switch (parse_meta(input))
    {
    case EXIT:
        squeel_db_close(table);
        printf("Goodbye\n");
        exit(EXIT_SUCCESS);
    case UNRECOGNICED:
        printf("Unrecognized command \"%s\"\n", input->buffer);
        break;
    default:
        assert(false && "Should never get here");
    }
}