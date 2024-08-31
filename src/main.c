#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define EQ 0
#define CMD_EXIT "exit"

typedef struct {
    char *buffer;
    size_t buffer_len;
    size_t input_len;
} SQUEEL_InputBuffer;

SQUEEL_InputBuffer *SQEEL_InputBuffer_create() {
    SQUEEL_InputBuffer *buffer = malloc(sizeof(*buffer));
    buffer->buffer = NULL;
    buffer->buffer_len = 0;
    buffer->input_len = 0;
    return buffer;
}

void SQEEL_InputBuffer_close(SQUEEL_InputBuffer *buffer) {
    if (buffer->buffer != NULL) {
        free(buffer->buffer);
    }
    free(buffer);
}

static void read_input(SQUEEL_InputBuffer *buffer) {
    ssize_t n = getline(&buffer->buffer, &buffer->buffer_len, stdin);
    if (n == -1) {
        perror("Failed to read input line");
    }
    buffer->input_len = n-1;
    buffer->buffer[buffer->input_len] = '\0';
#ifdef DEBUG
    printf("INPUT:\"%s\" LEN:%ld\n", buffer->buffer, buffer->input_len);
#endif
}

static void prompt_blocks(SQUEEL_InputBuffer *buffer) {
    printf("squeel > ");
    read_input(buffer);
}

int main(void) {
    SQUEEL_InputBuffer *buffer = SQEEL_InputBuffer_create();
    while (true) {
        prompt_blocks(buffer);

        // Process input
        if (strncmp(CMD_EXIT, buffer->buffer, buffer->input_len) == EQ) {
            printf("Goodbye\n");
            break;
        }
    }
    SQEEL_InputBuffer_close(buffer);
    return EXIT_SUCCESS;
}
