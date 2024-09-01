#if !defined(SQUEEL_TABLE_H)
#define SQUEEL_TABLE_H

#include <stdint.h>
#include "tokenizer.h"
#include "pager.h"

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 256

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} SqueelRow;


#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t num_rows;
    //void *pages[TABLE_MAX_PAGES];
    SqueelPager *pager;
} SqueelTable;


void squeel_serialize_row(SqueelRow *src, void *dest);
void squeel_deserialize_row(void *src, SqueelRow *dest);
void *row_slot(SqueelTable *table, uint32_t row_num);

SqueelTable *squeel_db_open(const char *db_filename);
void squeel_db_close(SqueelTable *table);

uint32_t squeel_table_max_rows(void);
uint32_t squeel_table_row_per_page(void);

void row_from_statement(SqueelTokenizedStatement *statment, SqueelRow *out);

#endif // SQUEEL_TABLE_H
