#include "input.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

// --------------------- Types ---------------------
SqueelInputBuffer *squeel_input_buffer_create(void) {
    SqueelInputBuffer *buffer = malloc(sizeof(*buffer));
    buffer->buffer = NULL;
    buffer->buffer_len = 0;
    buffer->input_len = 0;
    return buffer;
}

// -------------------- Statics ---------------------
static void readline_to_buffer(SqueelInputBuffer *input);
static void squeel_input_prompt(void);


void squeel_input_buffer_close(SqueelInputBuffer *input) {
    assert(input->buffer != NULL && "buffer should not be NULL");
    free(input->buffer);
    free(input);
}

void squeel_input_buffer_copy(SqueelInputBuffer *src, SqueelInputBuffer *dest) {
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

void squeel_input_read(SqueelInputBuffer *buffer) {
    do {
        squeel_input_prompt();
        readline_to_buffer(buffer);
    } while (buffer->input_len <= 0);
    assert(buffer->buffer != NULL && "NULL buffer after read");
    assert(buffer->input_len > 0 && "Empty buffer after read");
}

static void readline_to_buffer(SqueelInputBuffer *input) {
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

static void squeel_input_prompt(void) {
    printf("squeel-db > ");
}