#if !defined(SQUEEL_STATEMENT_H)
#define SQUEEL_STATEMENT_H

#include "input.h"
#include "table.h"

typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;


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


StatementPrepareStatus squeel_statement_prepare(SqueelInputBuffer *buffer, Statement *statement);
ExecuteResult squeel_statement_execute(Statement *statement, Table *table);


#endif // SQUEEL_STATEMENT_H
