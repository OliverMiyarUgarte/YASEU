#ifndef ENEMIES_H
#define ENEMIES_H

#include <allegro.h>
#include <math.h>    // Necessário para atan2, cos, sin
#include "tree.h"    // Necessário para reconhecer NODE_BOSS


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
               enemies[0].health = 200;  
               enemies[0].max_health = 200;
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
               enemies[i].y = -15;  
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
                   
                   // 1. Limpeza de Estados
                   if (enemies[i].laser_active >= 1 && enemies[i].laser_active <= 6) {
                       enemies[i].laser_width = 0; 
                       enemies[i].laser_active = 0;
                       enemies[i].attack_alternator = 0; 
                   }
                
                   // 2. MOVIMENTAÇÃO INTELIGENTE
                   // Lógica:
                   // - Se estiver na Espiral (alternator == 0): NÃO SE MEXE.
                   // - Se estiver atirando Laser (active == 11): NÃO SE MEXE.
                   // - Só se mexe quando estiver PREPARANDO o laser (mirando em você).
                
                   if (enemies[i].attack_alternator != 0 && enemies[i].laser_active != 11)
                   {
                       int target_x = player_x;
                       int target_y = player_y - 80;
                
                       if (enemies[i].x < target_x)
                           enemies[i].x += 2;
                       else
                           enemies[i].x -= 2;
                       if (enemies[i].y < target_y && enemies[i].y < SCREEN_HEIGHT - 100)
                           enemies[i].y += 1;
                       else if (enemies[i].y > target_y)
                           enemies[i].y -= 1;
                   }
                
                   // 3. Máquina de Estados de Ataque
                
                   // ESTADO: PREPARANDO LASER (Aviso visual)
                   if (enemies[i].laser_active == 10)
                   {
                       enemies[i].laser_timer--;
                       if (enemies[i].laser_timer <= 0)
                       {
                           enemies[i].laser_active = 11;
                           enemies[i].laser_timer = 60;
                           enemies[i].laser_width = 2;
                       }
                   }
                   // ESTADO: DISPARANDO LASER
                   else if (enemies[i].laser_active == 11)
                   {
                       if (enemies[i].laser_width < 25)
                           enemies[i].laser_width += 3;
                       enemies[i].laser_timer--;
                
                       if (enemies[i].laser_timer <= 0)
                       {
                           enemies[i].laser_active = 0;
                           enemies[i].shoot_cooldown = 40;
                           enemies[i].attack_alternator = 0; // Volta para Espiral
                       }
                   }
                   // ESTADO: COOLDOWN OU ESPIRAL
                   else
                   {
                       if (enemies[i].shoot_cooldown > 0)
                           enemies[i].shoot_cooldown--;
                       else
                       {
                           // --- ATAQUE 1: ESPIRAL DA MORTE (PARADO) ---
                           if (enemies[i].attack_alternator == 0)
                           {
                
                               static float spiral_angle = 0;
                               static int spiral_shots = 0;
                
                               // Dispara 3 balas
                               for (int k = 0; k < 3; k++)
                               {
                                   float angle = spiral_angle + (k * 2.0944);
                                   int vx = (int)(cos(angle) * 5);
                                   int vy = (int)(sin(angle) * 5);
                                   shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, vx, vy);
                               }
                
                               spiral_angle += 0.2;
                               spiral_shots++;
                               enemies[i].shoot_cooldown = 5;
                
                               // Quando termina a espiral, vai pro Laser
                               if (spiral_shots > 40)
                               {
                                   enemies[i].attack_alternator = 1; // AGORA ELE VAI COMEÇAR A SE MEXER PARA MIRAR
                                   enemies[i].shoot_cooldown = 60;
                                   enemies[i].laser_active = 10;
                                   enemies[i].laser_timer = 45;
                                   spiral_shots = 0;
                               }
                           }
                           // --- ATAQUE 2: Laser (Fallthrough) ---
                           else
                           {
                               enemies[i].laser_active = 10;
                               enemies[i].laser_timer = 45;
                           }
                       }
                   }
               }
            // FASE 2: VARREDURA(HP < 60%)
               else if (hp_percent < 0.6){
                   // ESTADO 0: DECISÃO
                   if (enemies[i].laser_active == 0) {
                       // Sorteia qual ataque usar (50/50)
                       if (rand() % 2 == 0) {
                           enemies[i].laser_active = 1; // Vai para o Laser Sweep
                       } else {
                           enemies[i].laser_active = 4; // Vai para o Dash Horizontal
                       }
                   }
                
                   // =========================================================
                   // ATAQUE A: VARREDURA PING-PONG (Original)
                   // =========================================================
                   
                   // Estado 1: Indo para o canto
                   else if (enemies[i].laser_active == 1) { 
                       // Define o lado baseado no alternador (para variar esq/dir)
                       int target_x = (enemies[i].attack_alternator == 0) ? 40 : SCREEN_WIDTH - 40;
                       
                       if (enemies[i].x > target_x) enemies[i].x -= 4; else enemies[i].x += 4;
                       
                       if (abs(enemies[i].x - target_x) < 5) { 
                           enemies[i].x = target_x; 
                           enemies[i].laser_active = 2; 
                           enemies[i].laser_timer = 60; 
                       }
                   }
                   // Estado 2: Carregando Laser
                   else if (enemies[i].laser_active == 2) { 
                       enemies[i].laser_timer--;
                       if (enemies[i].laser_timer <= 0) { enemies[i].laser_active = 3; enemies[i].laser_width = 2; }
                   }
                   // Estado 3: Disparando e Varrendo
                   else if (enemies[i].laser_active == 3) { 
                       if (enemies[i].laser_width < 30) enemies[i].laser_width += 2;
                       
                       if (enemies[i].attack_alternator == 0) { // Esq -> Dir
                           enemies[i].x += 2;
                           if (enemies[i].x >= (SCREEN_WIDTH / 2)) { 
                               enemies[i].laser_active = 0; enemies[i].shoot_cooldown = 60; enemies[i].attack_alternator = 1;
                           }
                       } else { // Dir -> Esq
                           enemies[i].x -= 2;
                           if (enemies[i].x <= (SCREEN_WIDTH / 2)) { 
                               enemies[i].laser_active = 0; enemies[i].shoot_cooldown = 60; enemies[i].attack_alternator = 0;
                           }
                       }
                   }
                
                   // =========================================================
                   // ATAQUE B: DASH HORIZONTAL RÁPIDO (Novo)
                   // =========================================================
                   
                   // Estado 4: Preparação (Teleporte)
                   else if (enemies[i].laser_active == 4) {
                       // Escolhe lado: 0 = Vem da Esquerda, 1 = Vem da Direita
                       int side = rand() % 2;
                       enemies[i].attack_alternator = side; 
                
                       // Alinha Y com o jogador (mira na sua altura)
                       enemies[i].y = player_y;
                       
                       // Teleporta para fora da tela
                       if (side == 0) enemies[i].x = -60;
                       else enemies[i].x = SCREEN_WIDTH + 60;
                
                       enemies[i].laser_active = 5; // Vai para Aviso
                       enemies[i].laser_timer = 45; // Tempo do aviso (linha vermelha)
                   }
                   // Estado 5: Aviso Visual
                   else if (enemies[i].laser_active == 5) {
                       enemies[i].laser_timer--;
                       
                       // Segue levemente o Y do player (Tracking suave) para não ser injusto
                       if (enemies[i].y < player_y) enemies[i].y += 1;
                       else if (enemies[i].y > player_y) enemies[i].y -= 1;
                
                       if (enemies[i].laser_timer <= 0) enemies[i].laser_active = 6;
                   }
                   // Estado 6: O Dash (Execução)
                   else if (enemies[i].laser_active == 6) {
                       int speed = 15; // Velocidade bem alta
                
                       if (enemies[i].attack_alternator == 0) enemies[i].x += speed; // Esq -> Dir
                       else enemies[i].x -= speed; // Dir -> Esq
                
                       // Se saiu da tela, acabou
                       if (enemies[i].x > SCREEN_WIDTH + 100 || enemies[i].x < -100) {
                           enemies[i].laser_active = 0; // Volta para decisão
                           enemies[i].shoot_cooldown = 40; // Pausa para respirar
                       }
                   }
                }
               // FASE 1
               else{
                   // 1. Limpeza de segurança
    // 1. Limpeza de segurança
    if(enemies[i].laser_active != 0) { 
        enemies[i].laser_active = 0; enemies[i].laser_width = 0; 
    }

    // 2. Movimentação: PÊNDULO (Vai e Vem)
    // Se attack_alternator for 0, vai pra Direita. Se for 1, vai pra Esquerda.
    int speed = 3;
    
    if (enemies[i].attack_alternator == 0) {
        enemies[i].x += speed;
        if (enemies[i].x > SCREEN_WIDTH - 40) enemies[i].attack_alternator = 1; // Bateu na parede dir
    } else {
        enemies[i].x -= speed;
        if (enemies[i].x < 40) enemies[i].attack_alternator = 0; // Bateu na parede esq
    }

    // Mantém altura fixa
    int target_y = 40;
    if (enemies[i].y < target_y) enemies[i].y += 2;
    else if (enemies[i].y > target_y) enemies[i].y -= 2;

    // 3. Lógica de Tiro: TAPETE DE BOMBAS
    if (enemies[i].shoot_cooldown > 0) { 
        enemies[i].shoot_cooldown--; 
    } 
    else {
        // Dispara 3 balas em leque PARA BAIXO, aproveitando o movimento
        // Bala 1: Cai reta
        shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, 0, 5);
        
        // Bala 2: Cai um pouco pra esquerda (Previsão)
        shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, -2, 4);
        
        // Bala 3: Cai um pouco pra direita
        shoot_custom_bullet(enemies[i].x, enemies[i].y + 20, 2, 4);

        // Cooldown ajustado para o movimento
        enemies[i].shoot_cooldown = 15; 
    }
                }
           } 
           // IA NORMAL
           else {
                // TIPO 1: HEAVY UNITS (Perseguidores Lentos) - HP >= 6
               if (enemies[i].max_health == 6) {
                   // ... (Código dos Heavy Units que já fizemos antes: perseguem e atiram mirado) ...
                   // (Mantenha o código do passo anterior aqui)
                   if (enemies[i].x < player_x) enemies[i].x += 1;
                   else if (enemies[i].x > player_x) enemies[i].x -= 1;
                   if (enemies[i].y < player_y) enemies[i].y += 1;
                   else if (enemies[i].y > player_y) enemies[i].y -= 1;
                   
                   if (enemies[i].shoot_cooldown > 0) enemies[i].shoot_cooldown--;
                   else {
                       float dx = player_x - enemies[i].x; float dy = player_y - enemies[i].y;
                       float angle = atan2(dy, dx);
                       int vx = (int)(cos(angle) * 3.0); int vy = (int)(sin(angle) * 3.0);
                       shoot_custom_bullet(enemies[i].x, enemies[i].y, vx, vy);
                       enemies[i].shoot_cooldown = ENEMY_SHOOT_COOLDOWN + 20;
                   }
               }
               
               // TIPO 2: SWARM/EVENTO (Kamikazes Rápidos) - HP == 1 (NOVO!)
               else if (enemies[i].max_health == 1) {
                   // 1. Velocidade Alta: Caem muito mais rápido que o normal
                   enemies[i].y += 5; // Normal é 2
                   
                   // 2. Movimento Lateral Errático (Tremida)
                   // Isso dificulta prever exatamente onde ele vai cair
                   int jitter = (rand() % 3) - 1; // -1, 0, ou 1
                   enemies[i].x += jitter;

                   // 3. SEM TIROS
                   // Note que não tem nenhum código de "shoot_bullet" aqui.
                   // O perigo é apenas a colisão.

                   // Despawn ao sair da tela
                   if (enemies[i].y > SCREEN_HEIGHT + 20) enemies[i].active = 0;
               }

               // TIPO 3: INIMIGOS NORMAIS (Padrão) - HP entre 2 e 5
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
           // ============================================================
           // LÓGICA DE DESENHO DO BOSS
           // ============================================================
           if (enemies[i].is_boss) {
               
               int lx = enemies[i].x; 
               int ly = enemies[i].y + 20;

               // --- FASE 2: AVISO DO DASH HORIZONTAL (Estado 5) ---
               if (enemies[i].laser_active == 5) {
                   if ((enemies[i].laser_timer / 4) % 2 == 0) { // Piscar
                       int color = makecol(255, 100, 0); // Laranja
                       
                       // Linha de trajeto
                       rectfill(buffer, 0, enemies[i].y - 2, SCREEN_WIDTH, enemies[i].y + 2, color);
                       
                       // Ícone de Perigo "!" no lado de origem
                       int symbol_x = (enemies[i].attack_alternator == 0) ? 20 : SCREEN_WIDTH - 20;
                       circlefill(buffer, symbol_x, enemies[i].y, 10, color);
                       textout_centre_ex(buffer, font, "!", symbol_x, enemies[i].y - 4, makecol(255,255,255), -1);
                   }
               }

               // --- FASE 2: LASER VERTICAL (Varredura) ---
               // Carregando (Linha fina)
               else if (enemies[i].laser_active == 2) {
                   if ((enemies[i].laser_timer / 4) % 2 == 0) 
                       line(buffer, lx, ly, lx, SCREEN_HEIGHT, makecol(255, 0, 0));
               } 
               // Atirando (Feixe Grosso)
               else if (enemies[i].laser_active == 3) {
                   int w = enemies[i].laser_width;
                   rectfill(buffer, lx - w/2, ly, lx + w/2, SCREEN_HEIGHT, makecol(255, 255, 255)); // Miolo Branco
                   rect(buffer, lx - w/2, ly, lx + w/2, SCREEN_HEIGHT, makecol(100, 200, 255));      // Borda Azul
               } 

               // --- FASE 3: LASER EM CRUZ (Caos) ---
               // Aviso (Linhas finas)
               else if (enemies[i].laser_active == 10) {
                   if ((enemies[i].laser_timer / 4) % 2 == 0) {
                       line(buffer, lx, ly, lx, SCREEN_HEIGHT, makecol(255, 0, 0)); // Vertical
                       line(buffer, 0, ly, SCREEN_WIDTH, ly, makecol(255, 0, 0));   // Horizontal
                   }
               } 
               // Atirando (Feixes Grossos)
               else if (enemies[i].laser_active == 11) {
                   int w = enemies[i].laser_width;
                   // Vertical
                   rectfill(buffer, lx - w/2, ly, lx + w/2, SCREEN_HEIGHT, makecol(255, 200, 255));
                   // Horizontal
                   rectfill(buffer, 0, ly - w/2, SCREEN_WIDTH, ly + w/2, makecol(255, 200, 255));
                   // Centro (Explosão)
                   circlefill(buffer, lx, ly, w, makecol(255, 255, 255));
               }

               // --- SPRITE DO BOSS ---
               // Só desenha se estiver numa coordenada "visível" ou próxima da tela
               // Isso evita tentar desenhar na coordenada -60 ou +400 durante o teleporte do Dash
               if (enemies[i].x > -100 && enemies[i].x < SCREEN_WIDTH + 100) {
                   masked_blit(boss_bmp, buffer, 0, 0, enemies[i].x - boss_bmp->w/2, enemies[i].y - boss_bmp->h/2, boss_bmp->w, boss_bmp->h);
               }
               
               // --- HUD DO BOSS (Barra de Vida e Nome) ---
               int bar_w = 150;
               // Proteção matemática para a barra não desenhar invertida se HP < 0
               int safe_health = (enemies[i].health < 0) ? 0 : enemies[i].health;
               int current_w = (safe_health * bar_w) / enemies[i].max_health;
               
               int bar_color = makecol(0,255,0); // Verde
               float pct = (float)enemies[i].health / enemies[i].max_health;
               if(pct < 0.6) bar_color = makecol(255,255,0); // Amarelo
               if(pct < 0.3) bar_color = makecol(255,0,0);   // Vermelho
               
               // Desenha Fundo Cinza
               rectfill(buffer, SCREEN_WIDTH/2 - bar_w/2, 20, SCREEN_WIDTH/2 + bar_w/2, 25, makecol(50,50,50));
               // Desenha Vida Atual
               rectfill(buffer, SCREEN_WIDTH/2 - bar_w/2, 20, SCREEN_WIDTH/2 - bar_w/2 + current_w, 25, bar_color);
               // Desenha Borda Branca
               rect(buffer, SCREEN_WIDTH/2 - bar_w/2, 20, SCREEN_WIDTH/2 + bar_w/2, 25, makecol(255,255,255));
               
               textout_centre_ex(buffer, font, "Catchulo", SCREEN_WIDTH/2, 28, bar_color, -1);
           } 
           
           // ============================================================
           // LÓGICA DE DESENHO DOS INIMIGOS COMUNS
           // ============================================================
           else {
               masked_blit(enemy_bmp, buffer, 0, 0, enemies[i].x - enemy_bmp->w / 2, enemies[i].y - enemy_bmp->h / 2, enemy_bmp->w, enemy_bmp->h);
               
               // Heavy Units / Minibosses (Barra segmentada grande)
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
                       rect(buffer, bx, sy, bx + block, sy + 3, makecol(0,0,0)); // Contorno preto
                   }
               } 
               // Inimigos Fracos (Barra fina simples)
               else {
                   int bw = 16; 
                   // Proteção para vida negativa
                   int safe_hp = (enemies[i].health < 0) ? 0 : enemies[i].health;
                   int cur = (safe_hp * bw) / enemies[i].max_health;
                   
                   int sx = enemies[i].x - (bw / 2); int sy = enemies[i].y - enemies[i].radius - 6;
                   
                   rectfill(buffer, sx, sy, sx + bw, sy + 2, makecol(50,50,50));
                   int color = makecol(0, 255, 0); 
                   if (enemies[i].health == 1) color = makecol(255, 0, 0);
                   
                   rectfill(buffer, sx, sy, sx + cur, sy + 2, color);
               }
           }
       }
   }
}

#endif