#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include "btree.h"
#include "table.h"
#include "stdio.h"

uint32_t* internal_node_child(SDBInternalNode* node, uint32_t child_num) {
    uint32_t num_keys = node->num_keys;
    assert(child_num <= num_keys && "Tried to access child out of bounds");
     if (child_num == num_keys) {
        return &node->right_child;
    } else {
        return &node->children[child_num].child;
    }
}

uint32_t get_node_max_key(SDBTreeNode* node) {
    if (node->type & SDB_INTERNAL_NODE) {
        SDBInternalNode *parent = (SDBInternalNode *)node;
        return parent->children[parent->num_keys-1].key;
    } else if (node->type & SDB_LEAF_NODE) {
        SDBLeafNode *leaf = (SDBLeafNode *)node;
        return leaf->cells[leaf->num_cells-1].key;
    } else {
        assert(false);
    }
}

void print_constants(void) {
    printf("ROW_SIZE: %u\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %lu\n",  sizeof(SDBTreeNode));
    printf("LEAF_NODE_SPACE_FOR_CELLS: %lu\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %lu\n",       LEAF_NODE_MAX_CELLS);
}

