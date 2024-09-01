#include "tokenizer.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

static const char *SPACE      = " ";
static const char *ASSIGN     = "=";
static const char *INSERT_STR = "INSERT";
static const char *SELECT_STR = "SELECT";

static SqueelOperationToken tokenize_keywork(SqueelInputBuffer *input) {
    const char *keyword = strtok(input->buffer, SPACE);
    if (strcasecmp(keyword, INSERT_STR) == EQ) {
        return INSERT_TOKEN;
    }
    if (strcasecmp(keyword, SELECT_STR) == EQ) {
        return SELECT_TOKEN;
    }
    return INVALID_TOKEN;
}

static int get_index_of_char(const char *str, char c) {
    char *pos = strchr(str, c);
    if (pos != NULL) {
        return pos - str;
    }
    return -1;
}

static void get_things_somehow(SqueelTokenizedStatement *out, const char *table_name) {
    ((void)table_name);
    out->key_values = calloc(3, sizeof(*out->key_values));
    out->key_values_length = 3;
    // TODO: this is dynamic somehow, lets pretend it is
    strcpy(out->key_values[0].key, "id");
    strcpy(out->key_values[1].key, "username");
    strcpy(out->key_values[2].key, "email");
}

static SqueelTokenizationResult tokenize_insert(SqueelTokenizedStatement *out) {
    get_things_somehow(out, "user");

    char *token = NULL;
    uint32_t count = 0;
    while ((token = strtok(NULL, SPACE)) != NULL) {
         char *left;
        const char *right;
        int32_t split_index;
        if ((split_index = get_index_of_char(token, *ASSIGN)) == -1) {
            // TODO: LOG
            return TOKENIZATION_FAILURE;
        }
        token[split_index] = '\0'; // TODO: destructive, make sure to have a copy
        left = token;
        right = token + split_index + 1;
        bool key_found = false;
        for (uint32_t i = 0; i < out->key_values_length; i++) {
            if (strcmp(out->key_values[i].key, left) == EQ) {
                // TODO: check for lenght
                strncpy(out->key_values[i].value, right, MAX_VALUE_SIZE); // TODO: fix 
                key_found = true;
                count++;
                break;
            }
        }
        if (!key_found) {
            // TODO LOG
            return TOKENIZATION_FAILURE;
        }
    }
    if (count < out->key_values_length) {
        // TODO: LOG
        return TOKENIZATION_FAILURE;
    }
    
    return TOKENIZATION_SUCCESS;
}

static SqueelTokenizationResult tokenize_select(SqueelTokenizedStatement *statement) {
    UNUSED(statement);
    return TOKENIZATION_SUCCESS; // TODO
}

SqueelTokenizationResult tokenize(SqueelInputBuffer *input, SqueelTokenizedStatement *statement) {
    const SqueelOperationToken keyword = tokenize_keywork(input);
    statement->keyword = keyword;
    switch (keyword)
    {
    case INSERT_TOKEN: {
        return tokenize_insert(statement);
    }
    case SELECT_TOKEN: {
        return tokenize_select(statement);
    }
    case INVALID_TOKEN:
        return TOKENIZATION_FAILURE;
    default: return TOKENIZATION_FAILURE;
    }
}
