#if !defined(SDB_TREES_H)
#define SDB_TREES_H

#include "tableconsts.h"
#include "pager.h"

#define SDB_LEAF_NODE (1 << 0)
#define SDB_INTERNAL_NODE (1 << 1)
#define SDB_ROOT_NODE (1 << 2)

typedef uint8_t SDBNodeType;

typedef struct {
    SDBNodeType type;
    uint32_t parent_ptr;
} SDBGenericNode;

typedef struct {
    uint32_t num_keys;
    uint32_t right_child;
} SDBInternalHeader;

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
#define LEAF_NODE_RIGHT_SPLIT_COUNT ((LEAF_NODE_MAX_CELLS + 1) / 2)
#define LEAF_NODE_LEFT_SPLIT_COUNT ((LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT)

typedef struct {
    SDBCell cells[LEAF_NODE_MAX_CELLS];
} SDBLeafBody;

typedef struct {
    uint32_t key;
    uint32_t child;
} SDBNodeKeyChildPair;

#define INTERNAL_NODE_SPACE_FOR_CELLS (PAGE_SIZE - (sizeof(SDBGenericNode) + sizeof(SDBInternalHeader)))
#define INTERNAL_NODE_MAX_CHILDREN (INTERNAL_NODE_SPACE_FOR_CELLS / sizeof(SDBNodeKeyChildPair))

typedef struct {
    SDBGenericNode common_header;
    SDBLeafHeader leaf_header;
    SDBLeafBody body;
} SDBLeafNode;

typedef struct {
    SDBGenericNode common_header;
    SDBInternalHeader header;
    SDBNodeKeyChildPair children[INTERNAL_NODE_MAX_CHILDREN];
} SDBInternalNode;

void print_constants(void);
uint32_t* internal_node_child(SDBInternalNode* node, uint32_t child_num);
uint32_t get_node_max_key(SDBGenericNode* node);

#define asd sizeof(SDBInternalNode)
#endif // SDB_TREES_H
