#if !defined(SQUEEL_TABLE_H)
#define SQUEEL_TABLE_H

#include <stdint.h>
#include "tokenizer.h"

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 256

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;


#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;


void squeel_serialize_row(Row *src, void *dest);
void squeel_deserialize_row(void *src, Row *dest);
void *row_slot(Table *table, uint32_t row_num);

Table *squeel_table_create(void);
void squeel_table_free(Table *table);

uint32_t squeel_table_max_rows(void);
uint32_t squeel_table_row_per_page(void);

void row_from_statement(SqueelTokenizedStatement *statment, Row *out);

#endif // SQUEEL_TABLE_H
