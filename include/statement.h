#if !defined(SDB_STATEMENT_H)
#define SDB_STATEMENT_H

#include "input.h"
#include "table.h"
#include "tokenizer.h" // TODO: for now 

typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT
} StatementType;

typedef struct {
    StatementType type;
    SDBTokenizedStatement tokenized;
} SDBStatement;


typedef enum {
    EXECUTE_TABLE_FULL,
    EXECUTE_DUP_KEY,
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE
} ExecuteResult;

typedef enum {
    STATEMENT_PREPARE_SUCCESS,
    STATEMENT_PREPARE_ERROR,
    STATEMENT_PREPARE_SYNTAX_ERROR
} StatementPrepareStatus;


StatementPrepareStatus sdb_statement_prepare(SDBInputBuffer *buffer, SDBStatement *statement);
ExecuteResult sdb_statement_execute(SDBStatement *statement, SDBTable *table);


#endif // SDB_STATEMENT_H
