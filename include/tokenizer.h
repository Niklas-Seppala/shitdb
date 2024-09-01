#if !defined(SQUEEL_TOKENIZER_H)
#define SQUEEL_TOKENIZER_H

#include "input.h"
#include <stdint.h>

typedef enum {
    INSERT_TOKEN,
    SELECT_TOKEN,
    INVALID_TOKEN
} SqueelOperationToken;

typedef enum {
    TOKENIZATION_SUCCESS,
    TOKENIZATION_TOO_LONG,
    TOKENIZATION_NEGATIVE_INTEGER,
    TOKENIZATION_FAILURE
} SqueelTokenizationResult;

#define MAX_KEY_SIZE   64     // todo think this over
#define MAX_VALUE_SIZE 512    // todo: broken test
typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} SqueelKeyValue;

typedef struct {
    SqueelOperationToken keyword;
    SqueelKeyValue *key_values;
    uint32_t key_values_length;
} SqueelTokenizedStatement;

SqueelTokenizationResult tokenize(SqueelInputBuffer *input, SqueelTokenizedStatement *statement);

#endif // SQUEEL_TOKENIZER_H
