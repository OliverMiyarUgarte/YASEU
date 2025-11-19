#ifndef MAPCREATOR_H
#define MAPCREATOR_H

#include "tree.h"
#include <time.h>

TreeNode *generate_map(int n_levels);
NodeType random_node_type(void);
void build_level_forward(TreeNode *parent, int level, int max_level, int *id_counter, TreeNode **all_nodes, int *node_count);

NodeType random_node_type(void) {
  int r = rand() % 100;
  if (r < 40)
    return NODE_NORMAL;
  else if (r < 65)
    return NODE_EVENT;
  else if (r < 80)
    return NODE_STORE;
  else
    return NODE_MINIBOSS;
}

void build_level_forward(TreeNode *parent, int level, int max_level, int *id_counter, TreeNode **all_nodes, int *node_count) {
  if (level > max_level || !parent) return;
  
  if (level == max_level) {
    TreeNode *final_node = create_node((*id_counter)++, NODE_NORMAL);
    all_nodes[(*node_count)++] = final_node;
    TreeNode *path[1] = {final_node};
    connect_nodes(parent, path, 1);
    return;
  }

  int num_children = 1;
  if (rand() % 100 < 35) num_children = 2;

  TreeNode *children[MAX_CHILDREN];
  for (int i = 0; i < num_children; i++) {
    children[i] = create_node((*id_counter)++, random_node_type());
    all_nodes[(*node_count)++] = children[i];
  }
  
  connect_nodes(parent, children, num_children);
  
  for (int i = 0; i < num_children; i++) {
    build_level_forward(children[i], level + 1, max_level, id_counter, all_nodes, node_count);
  }
}

TreeNode *generate_map(int n_levels) {
  if (n_levels <= 0)
    return NULL;

  if (srand(time(NULL)), rand() % 2) {}

  TreeNode *root = create_node(0, NODE_BOSS);
  TreeNode *all_nodes[256];
  int node_count = 1;
  all_nodes[0] = root;
  
  int id_counter = 1;

  TreeNode *starting_nodes[3];
  int num_starting_paths = 2 + rand() % 2;
  for (int i = 0; i < num_starting_paths; i++) {
    starting_nodes[i] = create_node(id_counter++, NODE_NORMAL);
    all_nodes[node_count++] = starting_nodes[i];
  }
  connect_nodes(root, starting_nodes, num_starting_paths);
  
  for (int i = 0; i < num_starting_paths; i++) {
    build_level_forward(starting_nodes[i], 1, n_levels - 1, &id_counter, all_nodes, &node_count);
  }

  return root;
}

#endif
