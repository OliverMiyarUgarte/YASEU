#ifndef COLLISIONS_H
#define COLLISIONS_H

// DEFINIÇÃO DOS NOVOS TAMANHOS
#define PLAYER_HITBOX 8  // Sprite 16x16 -> Raio 8


#include "partic~1.h"

// Variáveis globais necessárias
extern int player_x, player_y;
extern int player_health;
extern int game_over;
extern int player_hit_timer;
extern bool player_invincible;
extern int enemy_counter; 

void check_player_bullet_collision() {
   if(player_invincible) return; 

   for (int i = 0; i < MAX_BULLETS; i++) {
       // Verifica apenas balas inimigas ativas
       if (bullets[i].active && bullets[i].is_enemy_bullet) {
           int dx = player_x - bullets[i].x;
           int dy = player_y - bullets[i].y;
           
           // Usa o novo tamanho do player (8) + raio da bala
           int combined_radius = PLAYER_HITBOX + bullets[i].radius;

           if ((dx*dx + dy*dy) < (combined_radius * combined_radius)) {
               bullets[i].active = 0; 
               player_health -= bullets[i].damage; 
               player_invincible = true; 
               player_hit_timer = PLAYER_INVINCIBILITY_TIME; 

               if(player_health <= 0) {
                   game_over = 1;
                   // EFEITO DE MORTE: Explosão Grande (50 partículas)
                   spawn_explosion(player_x, player_y, 50, makecol(255, 200, 200));
                   spawn_sparks(player_x, player_y, 20, makecol(255, 50, 0), 0);
               } else {
                   // EFEITO DE DANO: Faíscas de Impacto (Vermelho/Laranja)
                   // O '0' no final é a direção (espalha para os lados)
                   spawn_sparks(player_x, player_y, 10, makecol(255, 100, 100), 0);
                   spawn_sparks(player_x, player_y, 5, makecol(255, 255, 0), 1);
               }
           }
       }
   }
}

void check_player_enemy_collision() {
   for (int i = 0; i < MAX_ENEMIES; i++) {
       if (enemies[i].active) {
           
           // --- COLISÃO ESPECIAL COM LASERS DO BOSS ---
           if (enemies[i].is_boss) {
               int laser_half_w = enemies[i].laser_width / 2;
               
               // 1. Colisão Laser Vertical (Fase 2 e 3)
               if (enemies[i].laser_active == 3 || enemies[i].laser_active == 11) {
                   if ((player_x + PLAYER_HITBOX > enemies[i].x - laser_half_w) && 
                       (player_x - PLAYER_HITBOX < enemies[i].x + laser_half_w)) {
                        
                        if (!player_invincible) {
                           player_health -= 2; 
                           player_invincible = true;
                           player_hit_timer = PLAYER_INVINCIBILITY_TIME;

                           // EFEITO: Queimadura de Laser (Faíscas Rosas/Roxas)
                           spawn_sparks(player_x, player_y, 15, makecol(255, 100, 255), 0);

                           if(player_health <= 0) {
                               game_over = 1;
                               spawn_explosion(player_x, player_y, 50, makecol(255, 255, 255)); // Explosão Final
                           }
                           return;
                       }
                   }
               }

               // 2. Colisão Laser Horizontal (Fase 3)
               if (enemies[i].laser_active == 11) {
                   int laser_y = enemies[i].y + 20; 
                   
                   if ((player_y + PLAYER_HITBOX > laser_y - laser_half_w) && 
                       (player_y - PLAYER_HITBOX < laser_y + laser_half_w)) {
                        
                        if (!player_invincible) {
                           player_health -= 2; 
                           player_invincible = true;
                           player_hit_timer = PLAYER_INVINCIBILITY_TIME;
                           
                           // EFEITO: Queimadura de Laser Horizontal
                           spawn_sparks(player_x, player_y, 15, makecol(255, 100, 255), 1); // Direção 1

                           if(player_health <= 0) {
                               game_over = 1;
                               spawn_explosion(player_x, player_y, 50, makecol(255, 255, 255));
                           }
                           return;
                       }
                   }
               }
           }

           // --- COLISÃO COM O CORPO DO INIMIGO (Círculo vs Círculo) ---
           int dx = player_x - enemies[i].x;
           int dy = player_y - enemies[i].y;
           
           int combined_radius = PLAYER_HITBOX + enemies[i].radius;
          
           if ((dx*dx + dy*dy) < (combined_radius * combined_radius)) {
               if(player_invincible) return;
               
               if(enemies[i].is_boss) {
                   player_health -= 3; 
                   // EFEITO: Batida seca no Boss (Explosão média vermelha)
                   spawn_explosion(player_x, player_y, 20, makecol(255, 50, 50));
               } else {
                   enemies[i].active = 0; // Inimigo comum explode
                   player_health -= 1; 
                   
                   // EFEITO: Inimigo vira poeira cósmica
                   spawn_explosion(enemies[i].x, enemies[i].y, 30, makecol(255, 150, 0));
                   // EFEITO: Player solta faísca do impacto
                   spawn_sparks(player_x, player_y, 10, makecol(200, 200, 200), 0);
               }
               
               player_invincible = true; 
               player_hit_timer = PLAYER_INVINCIBILITY_TIME; 
               
               if(player_health <= 0) {
                   game_over = 1;
                   // Explosão Final do Player (Branca Brilhante)
                   spawn_explosion(player_x, player_y, 60, makecol(255, 255, 255));
               }
               return;  
           }
       }
   }
}

void check_bullet_enemy_collisions() {
   for (int i = 0; i < MAX_BULLETS; i++) {
       // Verifica apenas balas do jogador contra inimigos
       if (bullets[i].active && !bullets[i].is_enemy_bullet) {
           for (int j = 0; j < MAX_ENEMIES; j++) {
               if (enemies[j].active) {
                   int dx = bullets[i].x - enemies[j].x;
                   int dy = bullets[i].y - enemies[j].y;
                   
                   // Raio da bala + Raio do inimigo (12 ou 25)
                   int combined_radius = bullets[i].radius + enemies[j].radius;
                   
                   if ((dx*dx + dy*dy) < (combined_radius * combined_radius)) { 
                       bullets[i].active = 0;
                       enemies[j].health -= bullets[i].damage;
                       spawn_sparks(bullets[i].x, bullets[i].y, 3, makecol(255, 255, 0), bullets[i].speedx > 0 ? -1 : 1);

                       if (enemies[j].health <= 0) {
                           enemies[j].active = 0;
                           enemy_counter++;

                           int color = enemies[j].is_boss ? makecol(255, 0, 255) : makecol(255, 100, 0);
                           int qtd = enemies[j].is_boss ? 50 : 15; // Boss explode MUITO mais
                           
                           spawn_explosion(enemies[j].x, enemies[j].y, qtd, color);
                       }
                       break; 
                   }
               }
           }
       }
   }
}

void show_enemy_counter(BITMAP* buffer){
   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "KILLS: %d", enemy_counter);
}

#endif