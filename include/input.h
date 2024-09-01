#if !defined(SDB_INPUT_H)
#define SDB_INPUT_H

#include <stdlib.h>

typedef struct {
    char *buffer;
    size_t buffer_len;
    size_t input_len;
} SDBInputBuffer;

SDBInputBuffer *sdb_input_buffer_create(void);
void sdb_input_buffer_close(SDBInputBuffer *input);
void sdb_input_read(SDBInputBuffer *buffer);

void sdb_input_buffer_copy(SDBInputBuffer *src, SDBInputBuffer *dest);

#endif // SDB_INPUT_H
