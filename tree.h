#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_CHILDREN 3 

typedef enum {
    NODE_NORMAL,
    NODE_MINIBOSS,
    NODE_EVENT,
    NODE_STORE,
    NODE_BOSS
} NodeType;

typedef struct TreeNode {
    NodeType type;
    int id;
    struct TreeNode* children[MAX_CHILDREN]; 
    int num_children; 
    struct TreeNode* parent; 
} TreeNode;

TreeNode* create_node(int id, NodeType type);
void connect_nodes(TreeNode* parent, TreeNode* children[], int num_children);
void destroy_tree(TreeNode* root);

TreeNode* create_node(int id, NodeType type) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) return NULL;
    node->id = id;
    node->type = type;
    node->num_children = 0;
    node->parent = NULL;
    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void connect_nodes(TreeNode* parent, TreeNode* children[], int num_children) {
    if (parent && num_children <= MAX_CHILDREN) {
        parent->num_children = num_children;
        for (int i = 0; i < num_children; i++) {
            if (children[i]) {
                parent->children[i] = children[i];
                children[i]->parent = parent;
            }
        }
    }
}

void destroy_tree(TreeNode* root) {
    if (!root) return;
    for (int i = 0; i < root->num_children; i++) {
        destroy_tree(root->children[i]); 
    }
    free(root); 
}
#endif
