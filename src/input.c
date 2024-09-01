#include "input.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

// --------------------- Types ---------------------
SDBInputBuffer *sdb_input_buffer_create(void) {
    SDBInputBuffer *buffer = malloc(sizeof(*buffer));
    buffer->buffer = NULL;
    buffer->buffer_len = 0;
    buffer->input_len = 0;
    return buffer;
}

// -------------------- Statics ---------------------
static void readline_to_buffer(SDBInputBuffer *input);
static void sdb_input_prompt(void);


void sdb_input_buffer_close(SDBInputBuffer *input) {
    assert(input->buffer != NULL && "buffer should not be NULL");
    free(input->buffer);
    free(input);
}

void sdb_input_buffer_copy(SDBInputBuffer *src, SDBInputBuffer *dest) {
    assert(src != NULL);
    assert(dest != NULL);
    assert(src->input_len < src->buffer_len);

    if (src->buffer_len > dest->buffer_len) {
        dest->buffer = realloc(dest->buffer, src->buffer_len);
    }
    
    dest->buffer_len = src->buffer_len;
    dest->input_len = src->input_len;
    strncpy(dest->buffer, src->buffer, src->input_len); // TODO: fix
}

void sdb_input_read(SDBInputBuffer *buffer) {
    do {
        sdb_input_prompt();
        readline_to_buffer(buffer);
    } while (buffer->input_len <= 0);
    assert(buffer->buffer != NULL && "NULL buffer after read");
    assert(buffer->input_len > 0 && "Empty buffer after read");
}

static void readline_to_buffer(SDBInputBuffer *input) {
    ssize_t nBytes = getline(&input->buffer, &input->buffer_len, stdin);
    if (nBytes <= 0) {
        perror("Failed to read input line");
        exit(EXIT_FAILURE);
    }
    input->input_len = nBytes-1;
    input->buffer[input->input_len] = '\0';
#ifdef DEBUG_LOG
    printf("[DBG] input:\"%s\" length:%ld\n", input->buffer, input->input_len);
#endif
}

static void sdb_input_prompt(void) {
    printf("sdb > ");
}