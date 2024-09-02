#if !defined(SDB_TREES_H)
#define SDB_TREES_H

#include "tableconsts.h"

#define SDB_LEAF_NODE (1 << 0)
#define SDB_INTERNAL_NODE (1 << 1)
#define SDB_ROOT_NODE (1 << 2)

typedef uint8_t SDBNodeType;

typedef struct {
    SDBNodeType type;
    uint32_t parent_ptr;
} SDBGenericNode;

typedef struct {
    SDBGenericNode header;
} SDBInternalNode;

typedef struct {
    uint32_t num_cells;
} SDBLeafHeader;

typedef struct {
    uint32_t key;
    char value[ROW_SIZE];
} SDBCell;

#define LEAF_NODE_CELL_SIZE ((uint32_t)(sizeof(SDBCell)))
#define LEAF_NODE_SPACE_FOR_CELLS ((uint32_t)(PAGE_SIZE - ((sizeof(SDBGenericNode) + sizeof(SDBLeafHeader)))))
#define LEAF_NODE_MAX_CELLS ((uint32_t)(LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE))

typedef struct {
    SDBCell cells[LEAF_NODE_MAX_CELLS];
} SDBLeafBody;

typedef struct {
    SDBGenericNode common_header;
    SDBLeafHeader leaf_header;
    SDBLeafBody body;
} SDBLeafNode;

void print_leaf_node(SDBGenericNode* node);
void print_constants(void);

#endif // SDB_TREES_H
