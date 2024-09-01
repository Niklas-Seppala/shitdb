#if !defined(SDB_TREES_H)
#define SDB_TREES_H

typedef enum {
    TREE_NODE_INTERNAL,
    TREE_NODE_LEAF
} SDBNodeType;

uint32_t leaf_node_max_cells(void);
uint32_t leaf_node_cell_size(void);
uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
void initialize_leaf_node(void* node);
void print_leaf_node(void* node);
void print_constants(void);
SDBNodeType get_node_type(void* node);
void set_node_type(void* node, SDBNodeType type);


#endif // SDB_TREES_H
