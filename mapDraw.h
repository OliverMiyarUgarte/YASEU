#ifndef MAPDRAW_H
#define MAPDRAW_H

#include <allegro.h>
#include "tree.h"
#include <math.h>

#define MAP_START_X  20
#define MAP_END_X    (SCREEN_W - 20)
#define MAP_TOP_Y    40
#define MAP_BOTTOM_Y (SCREEN_H - 40)

#define NODE_RADIUS_NORMAL 6
#define NODE_RADIUS_BOSS 9
#define NODE_RADIUS_EVENT 7

// Cores baseadas no tipo
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

const char* get_node_label(NodeType type) {
    switch (type) {
        case NODE_NORMAL:   return "";
        case NODE_MINIBOSS: return "!";
        case NODE_EVENT:    return "?";
        case NODE_STORE:    return "$";
        case NODE_BOSS:     return "B";
        default:            return "";
    }
}

void draw_single_node(BITMAP* buffer, TreeNode* node, int x, int y, int is_current) {
    int radius = (node->type == NODE_BOSS) ? NODE_RADIUS_BOSS : NODE_RADIUS_NORMAL;
    if (node->type == NODE_EVENT) radius = NODE_RADIUS_EVENT;

    int color = node_color(node->type);
    
    if (is_current) {
        circlefill(buffer, x, y, radius + 2, makecol(255, 255, 255));
    }

    circlefill(buffer, x, y, radius, color);
    circle(buffer, x, y, radius, makecol(0, 0, 0)); 
    
    const char* label = get_node_label(node->type);
    if (label[0] != '\0') {
        textout_centre_ex(buffer, font, (char*)label, x, y - 3, makecol(0, 0, 0), -1);
    }
}

void draw_tree_recursive(BITMAP* buffer, TreeNode* node, int depth, int max_depth, 
                        int min_x, int max_x, TreeNode* current_player_node, int selected_child_index) {
    if (!node) return;

    // Y invertido (de baixo pra cima)
    int y_pos = MAP_BOTTOM_Y - (depth * (MAP_BOTTOM_Y - MAP_TOP_Y)) / (max_depth > 0 ? max_depth : 1);
    int x_pos = (min_x + max_x) / 2;

    int is_current = (node == current_player_node);

    draw_single_node(buffer, node, x_pos, y_pos, is_current);

    if (node->num_children > 0) {
        int width_per_child = (max_x - min_x) / node->num_children;

        for (int i = 0; i < node->num_children; i++) {
            if (node->children[i]) {
                int child_min_x = min_x + (i * width_per_child);
                int child_max_x = child_min_x + width_per_child;
                int child_x_center = (child_min_x + child_max_x) / 2;
                
                int child_y_pos = MAP_BOTTOM_Y - ((depth + 1) * (MAP_BOTTOM_Y - MAP_TOP_Y)) / max_depth;

                // Lógica do Cursor Amarelo
                if (is_current && i == selected_child_index) {
                    line(buffer, x_pos, y_pos - 5, child_x_center, child_y_pos + 5, makecol(255, 255, 0));
                    rect(buffer, child_x_center - 11, child_y_pos - 11, child_x_center + 11, child_y_pos + 11, makecol(255, 255, 0));
                } else {
                    line(buffer, x_pos, y_pos - 5, child_x_center, child_y_pos + 5, makecol(150, 150, 150));
                }

                draw_tree_recursive(buffer, node->children[i], depth + 1, max_depth, 
                                  child_min_x, child_max_x, current_player_node, selected_child_index);
            }
        }
    }
}

int get_max_depth(TreeNode* node) {
    if (!node) return 0;
    int max = 0;
    for (int i = 0; i < node->num_children; i++) {
        int d = get_max_depth(node->children[i]);
        if (d > max) max = d;
    }
    return max + 1;
}

// Assinatura atualizada com 4 argumentos
void draw_map(BITMAP* buffer, TreeNode* root, TreeNode* current_node, int selected_child_index) {
    if (!root) return;
    
    int max_depth = get_max_depth(root);
    if (max_depth < 1) max_depth = 1;

    rectfill(buffer, 10, 10, SCREEN_W-10, SCREEN_H-10, makecol(30, 35, 45));
    rect(buffer, 10, 10, SCREEN_W-10, SCREEN_H-10, makecol(100, 120, 140));
    
    textout_centre_ex(buffer, font, "- TACTICAL MAP -", SCREEN_W/2, 15, makecol(255, 255, 255), -1);
    textout_centre_ex(buffer, font, "ARROWS: Select Path  |  ENTER: Deploy", SCREEN_W/2, SCREEN_H - 20, makecol(150, 150, 150), -1);

    draw_tree_recursive(buffer, root, 0, max_depth - 1, 20, SCREEN_W - 20, current_node, selected_child_index);
}

#endif