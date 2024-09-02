#include <stdbool.h>
#include <inttypes.h>
#include "trees.h"
#include "table.h"
#include "stdio.h"

void print_leaf_node(SDBGenericNode* node) {
    if (node->type & SDB_LEAF_NODE) {
        SDBLeafNode * leaf = (SDBLeafNode *)node;
        uint32_t cells = leaf->leaf_header.num_cells;
        printf("leaf (size %d)\n", cells);
        for (uint32_t i = 0; i < cells; i++) {
            printf("  - %d : %d\n", i, leaf->body.cells[i].key);
        }
    }
}

void print_constants(void) {
    printf("ROW_SIZE: %u\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %lu\n",  sizeof(SDBGenericNode));
    printf("LEAF_NODE_HEADER_SIZE: %lu\n",    sizeof(SDBLeafHeader));
    printf("LEAF_NODE_CELL_SIZE: %lu\n",      sizeof(SDBLeafBody));
    printf("LEAF_NODE_SPACE_FOR_CELLS: %u\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %u\n",       LEAF_NODE_MAX_CELLS);
}

