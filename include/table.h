#if !defined(SDB_TABLE_H)
#define SDB_TABLE_H

#include <stdint.h>
#include "tokenizer.h"
#include "pager.h"
#include "tableconsts.h"

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} SDBRow;



typedef struct {
    SDBPager *pager;
    uint32_t root_page_num;
} SDBTable;


void sdb_serialize_row(SDBRow *src, char *dest);
void sdb_deserialize_row(char *src, SDBRow *dest);
SDBTable *sdb_open(const char *db_filename);
void sdb_close(SDBTable *table);
uint32_t sdb_table_max_rows(void);
uint32_t sdb_table_row_per_page(void);
uint32_t sdb_row_size(void);
void sdb_row_from_statement(SDBTokenizedStatement *statment, SDBRow *out);

#endif // SDB_TABLE_H
