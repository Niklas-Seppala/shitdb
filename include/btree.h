#if !defined(SDB_TREES_H)
#define SDB_TREES_H

#include "tableconsts.h"
#include "pager.h"

#define SDB_LEAF_NODE     0x1
#define SDB_INTERNAL_NODE 0x2
#define SDB_ROOT_NODE     0x4

typedef struct {
    uint32_t key;
    char value[ROW_SIZE];
} SDBCell;

typedef struct {
    uint32_t key;
    uint32_t child;
} SDBNodeKeyChildPair;

#define NODE_HEADER_SIZE (sizeof(uint8_t) + sizeof(uint32_t))

#define LEAF_NODE_CELL_SIZE (sizeof(SDBCell))
#define LEAF_NODE_HEADER_SIZE (sizeof(uint32_t))
#define LEAF_NODE_SPACE_FOR_CELLS (PAGE_SIZE - (LEAF_NODE_CELL_SIZE + LEAF_NODE_HEADER_SIZE))
#define LEAF_NODE_MAX_CELLS (LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE)
#define LEAF_NODE_RIGHT_SPLIT_COUNT ((LEAF_NODE_MAX_CELLS + 1) / 2)
#define LEAF_NODE_LEFT_SPLIT_COUNT ((LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT)

#define INTERNAL_NODE_HEADER_SIZE (sizeof(uint32_t) + sizeof(uint32_t))
#define INTERNAL_NODE_SPACE_FOR_CELLS (PAGE_SIZE - (LEAF_NODE_CELL_SIZE + INTERNAL_NODE_HEADER_SIZE))
#define INTERNAL_NODE_MAX_CHILDREN (INTERNAL_NODE_SPACE_FOR_CELLS / sizeof(SDBNodeKeyChildPair))

/*
typedef struct {
    uint32_t num_cells;
    SDBCell cells[LEAF_NODE_MAX_CELLS];
} SDBLeafNodeBody;

typedef struct {
    uint32_t num_keys;
    uint32_t right_child;
    SDBNodeKeyChildPair children[INTERNAL_NODE_MAX_CHILDREN];
} SDBInternalNodeBody;
*/

/**
 * @brief 
 */
typedef struct {
    uint8_t type;
    uint32_t parent_ptr;
    
    union SDBNodeBody {

        /**
         * @brief 
         */
        struct {
            uint32_t num_keys;
            uint32_t right_child;
            SDBNodeKeyChildPair children[INTERNAL_NODE_MAX_CHILDREN];
        } internal;

        /**
         * @brief 
         */
        struct {
            uint32_t num_cells;
            SDBCell cells[LEAF_NODE_MAX_CELLS];
        } leaf;
    } body;
} SDBTreeGenericNode;

void print_constants(void);
uint32_t* internal_node_child(SDBTreeGenericNode* node, uint32_t child_num);
uint32_t get_node_max_key(SDBTreeGenericNode* node);


#define asd sizeof(SDBInternalNode)
#endif // SDB_TREES_H
