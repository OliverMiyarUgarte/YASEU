#ifndef ENEMIES_H
#define ENEMIES_H

#include <allegro.h>
#include <math.h>    // Necessário para atan2, cos, sin
#include "tree.h"    // Necessário para reconhecer NODE_BOSS

// Declaração como extern para evitar erro de redefinição
// (As variáveis reais estão criadas no main.c)
extern int global_enemy_hp;
extern int global_spawn_rate;
extern int ENEMY_RADIUS;
extern int level_kill_target; 
extern int enemy_counter; 
extern int player_x; 
extern int player_y; 

// Flags de controle do Boss
int boss_spawned = 0;
int boss_defeated = 0;

typedef struct {
   int x, y;
   int active;
   int shoot_cooldown;
   int health; 
   int max_health;
   int is_boss; 
   int radius;  
   int vx;      
   
   // VARIÁVEIS DE ESTADO
   int laser_active;    
   int laser_timer;     
   int laser_width; 
   int attack_alternator; 
} Enemy;

Enemy enemies[MAX_ENEMIES];

void init_enemies() {
   boss_spawned = 0;
   boss_defeated = 0;
   for (int i = 0; i < MAX_ENEMIES; i++) {
       enemies[i].active = 0;
   }
}

void spawn_enemy(int node_type) {
   static int spawn_timer = 0;
   spawn_timer++;
   
   // --- BOSS SPAWN ---
   if (node_type == NODE_BOSS) {
       if (enemy_counter < level_kill_target) {
           // Fase de Elites: código cai para o spawn normal abaixo
       } 
       else {
           if (!boss_spawned) {
               // Mata todos para o boss entrar
               for(int j=0; j<MAX_ENEMIES; j++) enemies[j].active = 0;

               enemies[0].active = 1;
               enemies[0].x = SCREEN_WIDTH / 2;
               enemies[0].y = 50;
               enemies[0].is_boss = 1;
               enemies[0].health = 80;  
               enemies[0].max_health = 80;
               enemies[0].shoot_cooldown = 60;
               enemies[0].vx = 2; 
               enemies[0].radius = 25; 
               
               enemies[0].laser_active = 0;
               enemies[0].laser_timer = 0;
               enemies[0].laser_width = 0;
               enemies[0].attack_alternator = 0;
               
               boss_spawned = 1;
           }
           return; // Não spawna mais ninguém
       }
   }

   // --- SPAWN NORMAL ---
   if (spawn_timer >= global_spawn_rate) {
       spawn_timer = 0;
       for (int i = 0; i < MAX_ENEMIES; i++) {
           if (!enemies[i].active) {
               int random_x = rand() % ((SCREEN_WIDTH * 3 / 4) - (SCREEN_WIDTH / 4)) + (SCREEN_WIDTH / 4);
               if (random_x - 16 < SCREEN_WIDTH / 4) random_x += 16;
               if (random_x + 16 > (SCREEN_WIDTH * 3 / 4) - (SCREEN_WIDTH / 4)) random_x -= 16;

               enemies[i].x = random_x;
               enemies[i].y = -20;  
               enemies[i].active = 1;
               enemies[i].is_boss = 0;
               enemies[i].vx = 0;
               enemies[i].shoot_cooldown = rand() % ENEMY_SHOOT_COOLDOWN;
               enemies[i].health = global_enemy_hp;
               enemies[i].max_health = global_enemy_hp;
               enemies[i].radius = ENEMY_RADIUS;
               enemies[i].laser_active = 0; 
               break;
           }
       }
   }
}

void update_enemies() {
   for (int i = 0; i < MAX_ENEMIES; i++) {
       if (enemies[i].active) {
           
           if (enemies[i].is_boss) {
               float hp_percent = (float)enemies[i].health / (float)enemies[i].max_health;
               
               // FASE 3: PERSEGUIÇÃO + CAOS (HP < 30%)
               if (hp_percent < 0.3) {
                   if (enemies[i].laser_active != 11) {
                       int target_x = player_x;
                       int target_y = player_y - 80;
                       if (enemies[i].x < target_x) enemies[i].x += 2; else enemies[i].x -= 2;
                       if (enemies[i].y < target_y && enemies[i].y < SCREEN_HEIGHT - 100) enemies[i].y += 1;
                       else if (enemies[i].y > target_y) enemies[i].y -= 1;
                   }

                   if (enemies[i].laser_active == 10) { 
                       enemies[i].laser_timer--;
                       if (enemies[i].laser_timer <= 0) { enemies[i].laser_active = 11; enemies[i].laser_timer = 60; enemies[i].laser_width = 2; }
                   }
                   else if (enemies[i].laser_active == 11) { 
                       if (enemies[i].laser_width < 25) enemies[i].laser_width += 3;
                       enemies[i].laser_timer--;
                       if (enemies[i].laser_timer <= 0) { enemies[i].laser_active = 0; enemies[i].shoot_cooldown = 40; enemies[i].attack_alternator = 0; }
                   }
                   else {
                       if (enemies[i].shoot_cooldown > 0) enemies[i].shoot_cooldown--;
                       else {
                           if (enemies[i].attack_alternator == 0) {
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, 0, 4);
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, 0, -4);
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, 4, 0);
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, -4, 0);
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, 3, 3);
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, -3, 3);
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, 3, -3);
                               shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, -3, -3);
                               enemies[i].shoot_cooldown = 40;
                               enemies[i].attack_alternator = 1; 
                           } else {
                               enemies[i].laser_active = 10; enemies[i].laser_timer = 45; 
                           }
                       }
                   }
               }
               // FASE 2: VARREDURA PING-PONG (HP < 60%)
               else if (hp_percent < 0.6) {
                   if (enemies[i].laser_active == 0) { enemies[i].laser_active = 1; }
                   else if (enemies[i].laser_active == 1) { 
                       int target_x = (enemies[i].attack_alternator == 0) ? 40 : SCREEN_WIDTH - 40;
                       if (enemies[i].x > target_x) enemies[i].x -= 4; else enemies[i].x += 4;
                       if (abs(enemies[i].x - target_x) < 5) { enemies[i].x = target_x; enemies[i].laser_active = 2; enemies[i].laser_timer = 60; }
                   }
                   else if (enemies[i].laser_active == 2) { 
                       enemies[i].laser_timer--;
                       if (enemies[i].laser_timer <= 0) { enemies[i].laser_active = 3; enemies[i].laser_width = 2; }
                   }
                   else if (enemies[i].laser_active == 3) { 
                       if (enemies[i].laser_width < 30) enemies[i].laser_width += 2;
                       if (enemies[i].attack_alternator == 0) { // Esq -> Dir
                           enemies[i].x += 2;
                           if (enemies[i].x >= (SCREEN_WIDTH / 2)) { // Para no meio
                               enemies[i].laser_active = 0; enemies[i].shoot_cooldown = 90; enemies[i].attack_alternator = 1;
                           }
                       } else { // Dir -> Esq
                           enemies[i].x -= 2;
                           if (enemies[i].x <= (SCREEN_WIDTH / 2)) { // Para no meio
                               enemies[i].laser_active = 0; enemies[i].shoot_cooldown = 90; enemies[i].attack_alternator = 0;
                           }
                       }
                   }
               }
               // FASE 1: BARRAGEM MIRADA
               else {
                   enemies[i].laser_active = 0;
                   int center_x = SCREEN_WIDTH / 2;
                   if(enemies[i].x < center_x - 2) enemies[i].x += 2;
                   else if(enemies[i].x > center_x + 2) enemies[i].x -= 2;

                   if (enemies[i].shoot_cooldown > 0) { enemies[i].shoot_cooldown--; } 
                   else {
                       float dx = player_x - enemies[i].x;
                       float dy = player_y - (enemies[i].y + 20);
                       float base_angle = atan2(dy, dx);
                       float speed = 4.0; float spread = 0.25;

                       for (int k = -3; k <= 3; k++) {
                           float angle = base_angle + (k * spread);
                           int vx = (int)(cos(angle) * speed);
                           int vy = (int)(sin(angle) * speed);
                           shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, vx, vy);
                       }
                       enemies[i].shoot_cooldown = 60; 
                   }
               }
           } 
           // IA NORMAL
           else {
               enemies[i].y += 2;
               if(rand()%10 < 3) { if(rand()%2) enemies[i].x++; else enemies[i].x--; }
               if (enemies[i].shoot_cooldown > 0) enemies[i].shoot_cooldown--;
               else {
                   shoot_bullet(enemies[i].x, enemies[i].y + ENEMY_RADIUS, 1);
                   enemies[i].shoot_cooldown = ENEMY_SHOOT_COOLDOWN + (rand() % 30);
               }
               if (enemies[i].y > SCREEN_HEIGHT + 20) enemies[i].active = 0;
           }
       }
   }
}

void check_boss_death() {
    if (boss_spawned) {
        int boss_alive = 0;
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active && enemies[i].is_boss) boss_alive = 1;
        }
        if (!boss_alive) boss_defeated = 1;
    }
}

void draw_enemies(BITMAP* buffer, BITMAP* enemy_bmp, BITMAP* boss_bmp) {
   for (int i = 0; i < MAX_ENEMIES; i++) {
       if (enemies[i].active) {
           if (enemies[i].is_boss) {
               
               // ... (Lógica de desenho dos Lasers - MANTIDA IGUAL) ...
               // (Copie a parte dos lasers do seu código anterior se precisar, 
               //  ou use o bloco abaixo se quiser garantir que está atualizado)
               int lx = enemies[i].x; int ly = enemies[i].y + 20;
               if (enemies[i].laser_active == 2) {
                   if ((enemies[i].laser_timer / 4) % 2 == 0) line(buffer, lx, ly, lx, SCREEN_HEIGHT, makecol(255, 0, 0));
               } else if (enemies[i].laser_active == 3) {
                   int w = enemies[i].laser_width;
                   rectfill(buffer, lx - w/2, ly, lx + w/2, SCREEN_HEIGHT, makecol(255, 255, 255));
                   rect(buffer, lx - w/2, ly, lx + w/2, SCREEN_HEIGHT, makecol(100, 200, 255));
               } else if (enemies[i].laser_active == 10) {
                   if ((enemies[i].laser_timer / 4) % 2 == 0) {
                       line(buffer, lx, ly, lx, SCREEN_HEIGHT, makecol(255, 0, 0));
                       line(buffer, 0, ly, SCREEN_WIDTH, ly, makecol(255, 0, 0));
                   }
               } else if (enemies[i].laser_active == 11) {
                   int w = enemies[i].laser_width;
                   rectfill(buffer, lx - w/2, ly, lx + w/2, SCREEN_HEIGHT, makecol(255, 200, 255));
                   rectfill(buffer, 0, ly - w/2, SCREEN_WIDTH, ly + w/2, makecol(255, 200, 255));
                   circlefill(buffer, lx, ly, w, makecol(255, 255, 255));
               }

               // Sprite do Boss
               masked_blit(boss_bmp, buffer, 0, 0, enemies[i].x - boss_bmp->w/2, enemies[i].y - boss_bmp->h/2, boss_bmp->w, boss_bmp->h);
               
               // --- BARRA DE VIDA & NOME ---
               int bar_w = 150;
               int current_w = (enemies[i].health * bar_w) / enemies[i].max_health;
               int bar_color = makecol(0,255,0); 
               float pct = (float)enemies[i].health / enemies[i].max_health;
               if(pct < 0.6) bar_color = makecol(255,255,0); 
               if(pct < 0.3) bar_color = makecol(255,0,0);   
               
               // Desenha a barra (Y: 20 até 25)
               rectfill(buffer, SCREEN_WIDTH/2 - bar_w/2, 20, SCREEN_WIDTH/2 + bar_w/2, 25, makecol(50,50,50));
               rectfill(buffer, SCREEN_WIDTH/2 - bar_w/2, 20, SCREEN_WIDTH/2 - bar_w/2 + current_w, 25, bar_color);
               rect(buffer, SCREEN_WIDTH/2 - bar_w/2, 20, SCREEN_WIDTH/2 + bar_w/2, 25, makecol(255,255,255));
               
               // MUDANÇA AQUI: Nome "Catchulo" EMBAIXO da barra (Y = 28)
               textout_centre_ex(buffer, font, "Catchulo", SCREEN_WIDTH/2, 28, bar_color, -1);

           } else {
               // ... (Código dos inimigos normais mantido igual) ...
               masked_blit(enemy_bmp, buffer, 0, 0, enemies[i].x - enemy_bmp->w / 2, enemies[i].y - enemy_bmp->h / 2, enemy_bmp->w, enemy_bmp->h);
               if (enemies[i].max_health > 3) {
                   int block = 4; int gap = 1;
                   int total = (enemies[i].health * block) + ((enemies[i].health - 1) * gap);
                   int sx = enemies[i].x - (total / 2); int sy = enemies[i].y - enemies[i].radius - 8;
                   for(int h = 0; h < enemies[i].health; h++) {
                       int bx = sx + h * (block + gap);
                       int color = makecol(0, 255, 0); 
                       if(enemies[i].health <= enemies[i].max_health / 2) color = makecol(255, 255, 0);
                       if(enemies[i].health <= 2) color = makecol(255, 0, 0);
                       rectfill(buffer, bx, sy, bx + block, sy + 3, color);
                       rect(buffer, bx, sy, bx + block, sy + 3, makecol(0,0,0));
                   }
               } else {
                   int bw = 16; int cur = (enemies[i].health * bw) / enemies[i].max_health;
                   int sx = enemies[i].x - (bw / 2); int sy = enemies[i].y - enemies[i].radius - 6;
                   rectfill(buffer, sx, sy, sx + bw, sy + 2, makecol(50,50,50));
                   int color = makecol(0, 255, 0); if (enemies[i].health == 1) color = makecol(255, 0, 0);
                   rectfill(buffer, sx, sy, sx + cur, sy + 2, color);
               }
           }
       }
   }
}

#endif