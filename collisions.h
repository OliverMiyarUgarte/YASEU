#ifndef COLLISIONS_H
#define COLLISIONS_H

// DEFINIÇÃO DOS NOVOS TAMANHOS
#define PLAYER_HITBOX 8  // Sprite 16x16 -> Raio 8

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

               if(player_health <= 0) game_over = 1;
               return; 
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
               
               // Lógica de colisão AABB (Caixa vs Caixa) ajustada para o tamanho do player
               // Isso evita que a asa da nave atravesse o laser sem tomar dano.
               
               // 1. Colisão Laser Vertical (Fase 2 e 3)
               if (enemies[i].laser_active == 3 || enemies[i].laser_active == 11) {
                   // Se a direita do player > esquerda do laser E esquerda do player < direita do laser
                   if ((player_x + PLAYER_HITBOX > enemies[i].x - laser_half_w) && 
                       (player_x - PLAYER_HITBOX < enemies[i].x + laser_half_w)) {
                        
                        if (!player_invincible) {
                           player_health -= 2; 
                           player_invincible = true;
                           player_hit_timer = PLAYER_INVINCIBILITY_TIME;
                           if(player_health <= 0) game_over = 1;
                           return;
                       }
                   }
               }

               // 2. Colisão Laser Horizontal (Fase 3)
               if (enemies[i].laser_active == 11) {
                   int laser_y = enemies[i].y + 20; // Ponto de origem Y do laser horizontal
                   
                   if ((player_y + PLAYER_HITBOX > laser_y - laser_half_w) && 
                       (player_y - PLAYER_HITBOX < laser_y + laser_half_w)) {
                        
                        if (!player_invincible) {
                           player_health -= 2; 
                           player_invincible = true;
                           player_hit_timer = PLAYER_INVINCIBILITY_TIME;
                           if(player_health <= 0) game_over = 1;
                           return;
                       }
                   }
               }
           }

           // --- COLISÃO COM O CORPO DO INIMIGO (Círculo vs Círculo) ---
           int dx = player_x - enemies[i].x;
           int dy = player_y - enemies[i].y;
           
           // Usa o novo tamanho do player (8) + raio específico do inimigo (12 ou 25)
           int combined_radius = PLAYER_HITBOX + enemies[i].radius;
          
           if ((dx*dx + dy*dy) < (combined_radius * combined_radius)) {
               if(player_invincible) return;
               
               if(enemies[i].is_boss) {
                   player_health -= 3; // Colisão com Boss dói mais
               } else {
                   enemies[i].active = 0; // Inimigo comum explode
                   player_health -= 1; 
               }
               
               player_invincible = true; 
               player_hit_timer = PLAYER_INVINCIBILITY_TIME; 
               
               if(player_health <= 0) game_over = 1; 
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
                       if (enemies[j].health <= 0) {
                           enemies[j].active = 0;
                           enemy_counter++;
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