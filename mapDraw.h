#ifndef MAPDRAW_H
#define MAPDRAW_H

#include <allegro.h>
#include "tree.h"
#include <math.h>

#define MAP_START_X  80
#define MAP_END_X   (SCREEN_W - 80)
#define MAP_TOP_Y    30
#define MAP_LEVEL_GAP 20

void draw_map(BITMAP* buffer, TreeNode* root);

void draw_node(BITMAP* buffer, TreeNode* node, int x, int y);

void draw_connections(BITMAP* buffer, TreeNode* node, int x, int y, int depth, int max_depth);

static int node_color(NodeType type) {
    switch (type) {
        case NODE_NORMAL:   return makecol(0, 200, 255);  // cyan
        case NODE_MINIBOSS: return makecol(255, 100, 100); // red-ish
        case NODE_EVENT:    return makecol(255, 255, 100); // yellow
        case NODE_STORE:    return makecol(100, 255, 100); // green
        case NODE_BOSS:     return makecol(255, 0, 255);   // magenta
        default:            return makecol(255, 255, 255);
    }
}

void draw_node(BITMAP* buffer, TreeNode* node, int x, int y) {
    int color = node_color(node->type);
    circlefill(buffer, x, y, 6, color);
    circle(buffer, x, y, 6, makecol(0, 0, 0));

    char text[8];
    sprintf(text, "%d", node->id);
    textout_ex(buffer, font, text, x - 4, y - 4, makecol(0, 0, 0), -1);
}

void draw_connections(BITMAP* buffer, TreeNode* node, int x, int y, int depth, int max_depth) {
    if (!node) return;

    int y_pos = MAP_TOP_Y + depth * MAP_LEVEL_GAP;

    int total_width = MAP_END_X - MAP_START_X;
    int spacing = total_width / (1 << depth);

    draw_node(buffer, node, x, y_pos);

    if (node->left) {
        int left_x = x - spacing / 2;
        int next_y = MAP_TOP_Y + (depth + 1) * MAP_LEVEL_GAP;
        line(buffer, x, y_pos, left_x, next_y, makecol(200, 200, 200));
        draw_connections(buffer, node->left, left_x, y_pos + MAP_LEVEL_GAP, depth + 1, max_depth);
    }

    if (node->right) {
        int right_x = x + spacing / 2;
        int next_y = MAP_TOP_Y + (depth + 1) * MAP_LEVEL_GAP;
        line(buffer, x, y_pos, right_x, next_y, makecol(200, 200, 200));
        draw_connections(buffer, node->right, right_x, y_pos + MAP_LEVEL_GAP, depth + 1, max_depth);
    }
}

void draw_map(BITMAP* buffer, TreeNode* root) {
    if (!root) return;
    int mid_x = (MAP_START_X + MAP_END_X) / 2;
    draw_connections(buffer, root, mid_x, MAP_TOP_Y, 0, 5);
}

#endif 