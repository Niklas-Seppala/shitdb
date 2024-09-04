#if !defined(SDB_TREES_H)
#define SDB_TREES_H

#include "tableconsts.h"
#include "pager.h"

#define SDB_LEAF_NODE     0x1
#define SDB_INTERNAL_NODE 0x2
#define SDB_ROOT_NODE     0x4

#define NODE_HEADER_SIZE (sizeof(uint8_t) + sizeof(uint32_t))

#define LEAF_NODE_CELL_SIZE (sizeof(SDBCell))
#define LEAF_NODE_HEADER_SIZE (sizeof(uint32_t) + sizeof(uint32_t))
#define LEAF_NODE_SPACE_FOR_CELLS (PAGE_SIZE - (LEAF_NODE_HEADER_SIZE + LEAF_NODE_HEADER_SIZE))
#define LEAF_NODE_MAX_CELLS (LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE)
#define LEAF_NODE_RIGHT_SPLIT_COUNT ((LEAF_NODE_MAX_CELLS + 1) / 2)
#define LEAF_NODE_LEFT_SPLIT_COUNT ((LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT)

#define INTERNAL_NODE_HEADER_SIZE (sizeof(uint32_t) + sizeof(uint32_t))
#define INTERNAL_NODE_SPACE_FOR_CELLS (PAGE_SIZE - (LEAF_NODE_CELL_SIZE + INTERNAL_NODE_HEADER_SIZE))
#define INTERNAL_NODE_MAX_CHILDREN (INTERNAL_NODE_SPACE_FOR_CELLS / sizeof(SDBNodeKeyChildPair))

/**
 * @brief A cell of a leaf node, holding a table row,
 *        identifiable by it's key.
 */
typedef struct {
    uint32_t key;
    char value[ROW_SIZE];
} SDBCell;

/**
 * @brief TODO:
 * 
 */
typedef struct {
    uint32_t key;
    uint32_t child;
} SDBNodeKeyChildPair;

/**
 * @brief A generic type for B-tree node, representing both
 *        internal and leaf nodes, identifiable by type field.
 *        Both types of nodes share common header section, located
 *        at the beginning of the struct.
 * 
 * @code
 *        |------------------------------------|
 *        |               HEADER               |
 *        |------------------------------------|
 *        |   type   |      parent node        |
 *        | 8-bytes  |       32-bytes          |
 *        |------------------------------------|
 * 
 */
typedef struct {
    // --------------------------------
    // Common header
    // NOTE: Header size size is likely to grow in the future
    // --------------------------------

    // Type of this node, either leaf (0x1) or internal (0x2).
    // 3rd bit (0x4) is root flag
    uint8_t type;
    // page numeber of the parent node, accessable from SDBPager object.
    uint32_t parent_ptr;
    
    // Rest of the page is reserved for the node body.
    // Both leaf and internal nodes make use of all the
    // available page. (PAGE_SIZE - 40 bits)
    union SDBNodeBody {
        // --------------------------------
        // Internal-node body
        // --------------------------------
        struct {
            // Number of current keys stored in this internal node.
            // For the max amount of cells that can be stored,
            // see INTERNAL_NODE_MAX_CHILDREN macro in btree.h
            uint32_t num_keys;
            // Right-most child node pointer
            uint32_t right_child;
            SDBNodeKeyChildPair children[INTERNAL_NODE_MAX_CHILDREN];
        } internal;

        // --------------------------------
        // Leaf-node body
        // --------------------------------
        struct {
            // Number of current occupied cells. For the max amount
            // of cells that can be stored, see LEAF_NODE_MAX_CELLS
            // macro in btree.h
            uint32_t num_cells;
            uint32_t sibling;
            // Rest of the page is reserved for for holding rows,
            // identifiable by a key
            SDBCell cells[LEAF_NODE_MAX_CELLS];
        } leaf;
    } body;
} SDBTreeNode;

void print_constants(void);
uint32_t* internal_node_child(SDBTreeNode* node, uint32_t child_num);
uint32_t get_node_max_key(SDBTreeNode* node);

#endif // SDB_TREES_H
