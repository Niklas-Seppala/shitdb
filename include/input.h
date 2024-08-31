#if !defined(SQUEEL_INPUT_H)
#define SQUEEL_INPUT_H

#include <stdlib.h>

typedef struct {
    char *buffer;
    size_t buffer_len;
    size_t input_len;
} SqueelInputBuffer;

SqueelInputBuffer *squeel_input_buffer_create(void);
void squeel_input_buffer_close(SqueelInputBuffer *input);
void squeel_input_read(SqueelInputBuffer *buffer);

void squeel_input_buffer_copy(SqueelInputBuffer *src, SqueelInputBuffer *dest);

#endif // SQUEEL_INPUT_H
