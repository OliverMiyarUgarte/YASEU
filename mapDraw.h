#ifndef MAPDRAW_H
#define MAPDRAW_H

#include <allegro.h>
#include "tree.h"
#include <math.h>

#define MAP_START_X  60
#define MAP_END_X   (SCREEN_W - 60)
#define MAP_TOP_Y    40
#define MAP_BOTTOM_Y (SCREEN_H - 40)
#define MAP_LEVEL_GAP 60

#define NODE_RADIUS_NORMAL 7
#define NODE_RADIUS_BOSS 10
#define NODE_RADIUS_EVENT 8

typedef struct {
    int x;
    int y;
    int depth;
} NodePosition;

int node_color(NodeType type) {
    switch (type) {
        case NODE_NORMAL:   return makecol(100, 150, 255);
        case NODE_MINIBOSS: return makecol(255, 120, 80);
        case NODE_EVENT:    return makecol(255, 240, 100);
        case NODE_STORE:    return makecol(120, 255, 140);
        case NODE_BOSS:     return makecol(255, 80, 200);
        default:            return makecol(200, 200, 200);
    }
}

int get_node_radius(NodeType type) {
    switch (type) {
        case NODE_BOSS:     return NODE_RADIUS_BOSS;
        case NODE_EVENT:    return NODE_RADIUS_EVENT;
        default:            return NODE_RADIUS_NORMAL;
    }
}

const char* get_node_label(NodeType type) {
    switch (type) {
        case NODE_NORMAL:   return "E";
        case NODE_MINIBOSS: return "M";
        case NODE_EVENT:    return "?";
        case NODE_STORE:    return "$";
        case NODE_BOSS:     return "B";
        default:            return "?";
    }
}

void draw_node(BITMAP* buffer, TreeNode* node, int x, int y) {
    int radius = get_node_radius(node->type);
    int color = node_color(node->type);
    
    circlefill(buffer, x, y, radius, color);
    circle(buffer, x, y, radius, makecol(0, 0, 0));
    circle(buffer, x, y, radius - 1, makecol(255, 255, 255));
    
    textout_centre_ex(buffer, font, (char*)get_node_label(node->type), 
                      x, y - 2, makecol(0, 0, 0), -1);
}

void calculate_node_positions(TreeNode* node, int depth, int max_depth, 
                             int parent_x, int parent_y, NodePosition* positions, int* pos_count) {
    if (!node) return;
    
    int y_pos = MAP_TOP_Y + (depth * (MAP_BOTTOM_Y - MAP_TOP_Y)) / (max_depth);
    
    int sibling_count = 1;
    if (node->parent) {
        sibling_count = node->parent->num_children;
    }
    
    int child_index = 0;
    if (node->parent) {
        for (int i = 0; i < node->parent->num_children; i++) {
            if (node->parent->children[i] == node) {
                child_index = i;
                break;
            }
        }
    }
    
    int x_spread = MAP_END_X - MAP_START_X;
    int x_offset = x_spread / (sibling_count + 1);
    int x_pos = MAP_START_X + x_offset * (child_index + 1);
    
    positions[*pos_count].x = x_pos;
    positions[*pos_count].y = y_pos;
    positions[*pos_count].depth = depth;
    (*pos_count)++;
    
    for (int i = 0; i < node->num_children; i++) {
        if (node->children[i]) {
            calculate_node_positions(node->children[i], depth + 1, max_depth, 
                                   x_pos, y_pos, positions, pos_count);
        }
    }
}

void draw_connections_optimized(BITMAP* buffer, TreeNode* node, NodePosition* positions, int pos_count) {
    if (!node) return;
    
    int node_x = -1, node_y = -1;
    for (int i = 0; i < pos_count; i++) {
        if (positions[i].depth >= 0) {
            node_x = positions[i].x;
            node_y = positions[i].y;
            break;
        }
    }
    
    if (node_x == -1) return;
    
    for (int i = 0; i < node->num_children; i++) {
        if (node->children[i]) {
            int child_x = -1, child_y = -1;
            for (int j = 0; j < pos_count; j++) {
                if (positions[j].depth == positions[0].depth + 1) {
                    child_x = positions[j].x;
                    child_y = positions[j].y;
                    break;
                }
            }
            
            if (child_x != -1) {
                int mid_x = (node_x + child_x) / 2;
                int mid_y = (node_y + child_y) / 2;
                
                line(buffer, node_x, node_y, mid_x, mid_y, makecol(150, 150, 150));
                line(buffer, mid_x, mid_y, child_x, child_y, makecol(150, 150, 150));
            }
        }
    }
}

void draw_connections_recursive(BITMAP* buffer, TreeNode* node, int depth, int max_depth, 
                               int parent_x, int parent_y, int sibling_index, int sibling_count) {
    if (!node) return;
    
    int y_pos = MAP_TOP_Y + (depth * (MAP_BOTTOM_Y - MAP_TOP_Y)) / (max_depth);
    
    int x_spread = MAP_END_X - MAP_START_X;
    int x_offset = x_spread / (sibling_count + 1);
    int x_pos = MAP_START_X + x_offset * (sibling_index + 1);
    
    if (depth > 0 && parent_x >= 0) {
        line(buffer, parent_x, parent_y, x_pos, y_pos, makecol(120, 130, 150));
    }
    
    for (int i = 0; i < node->num_children; i++) {
        if (node->children[i]) {
            draw_connections_recursive(buffer, node->children[i], depth + 1, max_depth, 
                                      x_pos, y_pos, i, node->num_children);
        }
    }
}

void draw_nodes_recursive(BITMAP* buffer, TreeNode* node, int depth, int max_depth, 
                         int parent_x, int parent_y, int sibling_index, int sibling_count) {
    if (!node) return;
    
    int y_pos = MAP_TOP_Y + (depth * (MAP_BOTTOM_Y - MAP_TOP_Y)) / (max_depth);
    
    int x_spread = MAP_END_X - MAP_START_X;
    int x_offset = x_spread / (sibling_count + 1);
    int x_pos = MAP_START_X + x_offset * (sibling_index + 1);
    
    draw_node(buffer, node, x_pos, y_pos);
    
    for (int i = 0; i < node->num_children; i++) {
        if (node->children[i]) {
            draw_nodes_recursive(buffer, node->children[i], depth + 1, max_depth, 
                                x_pos, y_pos, i, node->num_children);
        }
    }
}

int get_max_depth(TreeNode* node) {
    if (!node) return 0;
    int max = 0;
    for (int i = 0; i < node->num_children; i++) {
        int child_depth = get_max_depth(node->children[i]);
        if (child_depth > max) max = child_depth;
    }
    return max + 1;
}

void draw_map(BITMAP* buffer, TreeNode* root) {
    if (!root) return;
    
    int max_depth = get_max_depth(root);
    
    rectfill(buffer, MAP_START_X - 20, MAP_TOP_Y - 20, 
            MAP_END_X + 20, MAP_BOTTOM_Y + 20, makecol(30, 35, 45));
    rect(buffer, MAP_START_X - 20, MAP_TOP_Y - 20, 
        MAP_END_X + 20, MAP_BOTTOM_Y + 20, makecol(100, 120, 140));
    
    draw_connections_recursive(buffer, root, 0, max_depth, -1, -1, 0, 1);
    draw_nodes_recursive(buffer, root, 0, max_depth, -1, -1, 0, 1);
    
    textout_ex(buffer, font, "FINAL BOSS", MAP_START_X - 10, MAP_TOP_Y - 15, 
              makecol(255, 100, 200), -1);
}

#endif
