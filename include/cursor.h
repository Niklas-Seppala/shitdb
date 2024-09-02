#if !defined(SDB_CURSOR_H)
#define SDB_CURSOR_H

#include <inttypes.h>
#include <stdbool.h>
#include "table.h"


typedef struct {
    SDBTable *table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;
} SDBCursor;

void sdb_cursor_start(SDBCursor *cursor, SDBTable *table);
void sdb_cursor_find(SDBCursor *cursor, SDBTable* table, uint32_t key);
char *sdb_cursor_value(SDBCursor *cursor);
void sdb_cursor_advance(SDBCursor *cursor);



#endif // SDB_CURSOR_H
