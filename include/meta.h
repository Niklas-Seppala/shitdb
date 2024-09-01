#if !defined(SQUEEL_META_H)
#define SQUEEL_META_H

#include <stdbool.h>
#include "input.h"
#include "table.h"

#define CMD_EXIT ".exit"
enum SqueelMetaCommand {
    EXIT,            // CMD_EXIT
    UNRECOGNICED
};


bool squeel_meta_is_meta_command(const char *command);
void squeel_meta_handle_command(SqueelInputBuffer *input, SqueelTable *table);


#endif // SQUEEL_META_H
