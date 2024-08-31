#if !defined(SQUEEL_STATEMENT_H)
#define SQUEEL_STATEMENT_H

#include "input.h"
#include "table.h"
#include "tokenizer.h" // TODO: for now 

typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT
} StatementType;

typedef struct {
    StatementType type;
    SqueelTokenizedStatement tokenized;
} SqueelStatement;


typedef enum {
    EXECUTE_TABLE_FULL,
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE
} ExecuteResult;

typedef enum {
    STATEMENT_PREPARE_SUCCESS,
    STATEMENT_PREPARE_ERROR,
    STATEMENT_PREPARE_SYNTAX_ERROR
} StatementPrepareStatus;


StatementPrepareStatus squeel_statement_prepare(SqueelInputBuffer *buffer, SqueelStatement *statement);
ExecuteResult squeel_statement_execute(SqueelStatement *statement, Table *table);


#endif // SQUEEL_STATEMENT_H
