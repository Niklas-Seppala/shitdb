#if !defined(SDB_META_H)
#define SDB_META_H

#include <stdbool.h>
#include "input.h"
#include "table.h"

enum SDBMetaCommand {
    EXIT,            // CMD_EXIT
    BTREE,           // CMD_BTREE
    CONST,           // CMD_CONST
    UNRECOGNICED
};

bool sdb_meta_is_meta_command(const char *command);
void sdb_meta_handle_command(SDBInputBuffer *input, SDBTable *table);

#endif // SDB_META_H
