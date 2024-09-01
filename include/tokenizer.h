#if !defined(SDB_TOKENIZER_H)
#define SDB_TOKENIZER_H

#include "input.h"
#include <stdint.h>

typedef enum {
    INSERT_TOKEN,
    SELECT_TOKEN,
    INVALID_TOKEN
} SDBOperationToken;

typedef enum {
    TOKENIZATION_SUCCESS,
    TOKENIZATION_TOO_LONG,
    TOKENIZATION_NEGATIVE_INTEGER,
    TOKENIZATION_FAILURE
} SDBTokenizationResult;

#define MAX_KEY_SIZE   64     // todo think this over
#define MAX_VALUE_SIZE 512    // todo: broken test
typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} SDBKeyValue;

typedef struct {
    SDBOperationToken keyword;
    SDBKeyValue *key_values;
    uint32_t key_values_length;
} SDBTokenizedStatement;

SDBTokenizationResult tokenize(SDBInputBuffer *input, SDBTokenizedStatement *statement);

#endif // SDB_TOKENIZER_H
