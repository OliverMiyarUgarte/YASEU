#ifndef MAPCREATOR_H
#define MAPCREATOR_H

#include "tree.h"
#include <time.h>

// CONFIGURAÇÃO DE LIMITES
#define MAX_MAP_WIDTH 5  // Máximo de nós lado a lado (5 fica ótimo em 320px)

TreeNode *generate_map(int n_levels);
NodeType random_node_type(void);
void build_level_forward(TreeNode *parent, int level, int max_level, int *id_counter, TreeNode **all_nodes, int *node_count, int *level_widths);

NodeType random_node_type(void) {
  int r = rand() % 100;
  if (r < 40) return NODE_NORMAL;       
  else if (r < 75) return NODE_EVENT;   
  else return NODE_MINIBOSS;            
}

// Adicionei o parâmetro 'level_widths' para controlar a largura
void build_level_forward(TreeNode *parent, int level, int max_level, int *id_counter, TreeNode **all_nodes, int *node_count, int *level_widths) {
  if (level > max_level || !parent || *node_count >= 250) return;
  
  // BOSS FINAL
  if (level == max_level) {
    TreeNode *final_node = create_node((*id_counter)++, NODE_BOSS);
    all_nodes[(*node_count)++] = final_node;
    
    // Contabiliza largura (embora no boss seja sempre 1)
    level_widths[level]++; 
    
    TreeNode *path[1] = {final_node};
    connect_nodes(parent, path, 1);
    return;
  }

  // --- LÓGICA DE BIFURCAÇÃO INTELIGENTE ---
  int num_children = 1;
  int next_level = level + 1;

  // Verifica quantos nós JÁ EXISTEM no próximo nível
  int current_width_next = level_widths[next_level];

  // Regra 1: Início Explosivo (Nível 1 sempre divide se possível)
  if (level == 1) {
      num_children = 2 + (rand() % 2); 
  }
  // Regra 2: Se o próximo nível já estiver "gordo" (cheio), proíbe dividir
  else if (current_width_next >= MAX_MAP_WIDTH) {
      num_children = 1; // Força linha reta para não poluir
  }
  // Regra 3: Se tiver espaço, tem 60% de chance de dividir
  else {
      if (rand() % 100 < 60) num_children = 2;
  }

  // Segurança final: Se por algum motivo num_children + width passar do limite, reduz
  if (current_width_next + num_children > MAX_MAP_WIDTH + 1) {
      num_children = 1;
  }

  TreeNode *children[MAX_CHILDREN];
  for (int i = 0; i < num_children; i++) {
    
    NodeType type;
    // Regra da Loja no Nível 3
    if (level == 3) type = NODE_STORE; 
    else type = random_node_type();

    children[i] = create_node((*id_counter)++, type);
    all_nodes[(*node_count)++] = children[i];
    
    // ATUALIZA O CONTADOR DE LARGURA
    level_widths[next_level]++;
  }
  
  connect_nodes(parent, children, num_children);
  
  for (int i = 0; i < num_children; i++) {
    build_level_forward(children[i], level + 1, max_level, id_counter, all_nodes, node_count, level_widths);
  }
}

TreeNode *generate_map(int n_levels) {
  if (n_levels <= 0) return NULL;

  TreeNode *root = create_node(0, NODE_NORMAL);
  
  TreeNode *all_nodes[256];
  int node_count = 1;
  all_nodes[0] = root;
  int id_counter = 1;

  // Array para rastrear a largura de cada nível (inicia tudo com 0)
  int level_widths[20]; 
  for(int i=0; i<20; i++) level_widths[i] = 0;
  
  level_widths[1] = 1; // Nível 1 tem a raiz

  build_level_forward(root, 1, n_levels, &id_counter, all_nodes, &node_count, level_widths);

  return root;
}

#endif