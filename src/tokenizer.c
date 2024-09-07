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

static const uint32_t START_TOKEN_MASK = 0x1;
static const uint32_t KEY_VALUE_TOKEN_MASK = 0x2;
enum SDBTokenType {
    SELECT  = 0x11,
    INSERT  = 0x21,  
    DELETE  = 0x31,
    UPDATE  = 0x41,
    CREATE  = 0x51,

    KEY       = 0x12,
    ASSIGN    = 0x22,
    VALUE     = 0x32,

    INVALID = 0
};

static enum SDBTokenType recognize_token(const char *token) {
    if (strcasecmp(token, "INSERT") == EQ) {
        return INSERT;
    }
    if (strcasecmp(token, "SELECT") == EQ) {
        return SELECT;
    }
    return INVALID;
}

typedef struct SDBToken {
    char *value;
    struct SDBToken *next;
    struct SDBToken *prev;
    struct SDBToken *child;
    uint32_t position;
    enum SDBTokenType type;
} SDBToken;

typedef struct {
    char *str;
    char *original;
    size_t input_len;
    SDBToken *root;
} SDBTokStatement;


static void __tokenize(SDBTokStatement *statement, SDBToken *old_token, char **save_ptr) {
    char *str = statement->str;
    char *raw_token;
    if (*save_ptr == NULL) {
        // This is first token
        raw_token = __strtok_r(str, SPACE, save_ptr);
    } else {
        raw_token = __strtok_r(NULL, SPACE, save_ptr);
    }

    if (raw_token == NULL) {
        // No more tokens.
        return;
    }

    enum SDBTokenType type = recognize_token(raw_token);
    if (type == INVALID_TOKEN) {
        return;
    }

    SDBToken *new_token;
    if (statement->root == NULL) {
        // No previous token, expect start token
        if (!(type & START_TOKEN_MASK)) {
            return; // SHIT
        }

        new_token = calloc(1, sizeof(SDBToken));
        statement->root = new_token;
        new_token->position = 0;
        new_token->value = raw_token;
    } else if (old_token->type & START_TOKEN_MASK) {
        // previous token was start

        // TODO Maybe match for start tokens here
        // and branch out
    }

    if (old_token != NULL) {
        assert(new_token != NULL);
        old_token->next = new_token;
        new_token->prev = old_token;
    }
    __tokenize(statement, new_token, save_ptr);
}

SDBTokStatement *parse(SDBInputBuffer *buffer) {
    assert(buffer != NULL);
    SDBTokStatement *tokenized_statement = calloc(1, sizeof(SDBTokStatement));
    // Take a copy of the input buffer, tokenization will
    // be destructive.
    tokenized_statement->input_len = buffer->input_len;
    tokenized_statement->str = calloc(tokenized_statement->input_len, sizeof(char));
    tokenized_statement->original = buffer->buffer;
    strncpy(tokenized_statement->str, buffer->buffer, buffer->input_len);
    
    char *save_ptr = NULL;
    __tokenize(tokenized_statement, NULL, &save_ptr);

    return tokenized_statement;
}

static SDBOperationToken tokenize_keywork(SDBInputBuffer *input) {
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

static void get_things_somehow(SDBTokenizedStatement *out, const char *table_name) {
    ((void)table_name);
    out->key_values = calloc(3, sizeof(*out->key_values));
    out->key_values_length = 3;
    // TODO: this is dynamic somehow, lets pretend it is
    strcpy(out->key_values[0].key, "id");
    strcpy(out->key_values[1].key, "username");
    strcpy(out->key_values[2].key, "email");
}

static SDBTokenizationResult tokenize_insert(SDBTokenizedStatement *out) {
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

static SDBTokenizationResult tokenize_select(SDBTokenizedStatement *statement) {
    UNUSED(statement);
    return TOKENIZATION_SUCCESS; // TODO
}

SDBTokenizationResult tokenize(SDBInputBuffer *input, SDBTokenizedStatement *statement) {
    const SDBOperationToken keyword = tokenize_keywork(input);
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
