#ifndef MAPCREATOR_H
#define MAPCREATOR_H

#include "tree.h"
#include <time.h>

TreeNode* generate_map(int n_levels);

NodeType random_node_type(void);

NodeType random_node_type(void) {
    int r = rand() % 100;
    if (r < 50) return NODE_NORMAL;
    else if (r < 70) return NODE_EVENT;
    else if (r < 85) return NODE_STORE;
    else return NODE_MINIBOSS;
}

TreeNode* generate_map(int n_levels) {
    if (n_levels <= 0) return NULL;

    srand(time(NULL));

    TreeNode* root = create_node(0, NODE_BOSS);
    TreeNode* current_level[8];
    TreeNode* next_level[8];

    int curr_count = 1;
    current_level[0] = root;

    int id_counter = 1;

    for (int level = 1; level < n_levels; level++) {
        int next_count = 0;

        for (int i = 0; i < curr_count; i++) {
            TreeNode* parent = current_level[i];

            TreeNode* left = create_node(id_counter++, random_node_type());
            TreeNode* right = NULL;
            
            right = create_node(id_counter++, random_node_type());

            connect_nodes(parent, left, right);

            next_level[next_count++] = left;
            if (right) next_level[next_count++] = right;
        }

        for (int j = 0; j < next_count; j++) {
            current_level[j] = next_level[j];
        }
        curr_count = next_count;
    }

    for (int i = 0; i < curr_count; i++) {
        TreeNode* boss = create_node(id_counter++, NODE_NORMAL);
        connect_nodes(current_level[i], boss, NULL);
    }

    return root;
}
#endif 