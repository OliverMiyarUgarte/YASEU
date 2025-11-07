#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>

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
    struct TreeNode* left;
    struct TreeNode* right;
    struct TreeNode* parent;
} TreeNode;

TreeNode* create_node(int id, NodeType type);
void connect_nodes(TreeNode* parent, TreeNode* left, TreeNode* right);
void destroy_tree(TreeNode* root);
void print_tree(TreeNode* root, int depth);


TreeNode* create_node(int id, NodeType type) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) return NULL;
    node->id = id;
    node->type = type;
    node->left = node->right = node->parent = NULL;
    return node;
}

void connect_nodes(TreeNode* parent, TreeNode* left, TreeNode* right) {
    if (parent) {
        parent->left = left;
        parent->right = right;
        if (left) left->parent = parent;
        if (right) right->parent = parent;
    }
}

void destroy_tree(TreeNode* root) {
    if (!root) return;
    destroy_tree(root->left);
    destroy_tree(root->right);
    free(root);
}

void print_tree(TreeNode* root, int depth) {
    if (!root) return;
    for (int i = 0; i < depth; i++) printf("   ");
    const char* names[] = {"Normal", "MiniBoss", "Event", "Store", "Boss"};
    printf("Node %d (%s)\n", root->id, names[root->type]);
    print_tree(root->left, depth + 1);
    print_tree(root->right, depth + 1);
}
#endif 