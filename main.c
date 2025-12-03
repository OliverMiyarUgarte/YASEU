#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define PLAYER_RADIUS 5
#define MAX_BULLETS 10
#define MAX_ENEMIES 5
#define ENEMY_SHOOT_COOLDOWN 90
#define PLAYER_INVINCIBILITY_TIME 60

// Variáveis Globais
int player_x = 100, player_y = 100;
int player_health = 5;
int game_over = 0;
int bullet_cooldown = 1;
int ENEMY_RADIUS; 
int player_hit_timer = 0;
bool player_invincible = false;

// Variáveis de Progressão
int current_bg_color;
int level_kill_target;      
int global_enemy_hp = 3;
int global_spawn_rate = 30;
int enemy_counter = 0; 
char* level_message = "";

// INCLUDES
#include "tree.h"        
#include "bullets.h"
#include "enemies.h"
#include "collis~1.h" 
#include "menu.h"
#include "mapCre~1.h"
#include "mapDraw.h"
#include "heart.h"
#include "upgrades.h"

// Função auxiliar para carregar sprites
BITMAP* load_bitmap_resized(const char* filename, int new_w, int new_h) {
    BITMAP* temp = load_bitmap(filename, NULL);
    if (!temp) return NULL;
    BITMAP* resized = create_bitmap(new_w, new_h);
    if (!resized) { destroy_bitmap(temp); return NULL; }
    clear_to_color(resized, bitmap_mask_color(temp));
    masked_stretch_blit(temp, resized, 0, 0, temp->w, temp->h, 0, 0, new_w, new_h);
    destroy_bitmap(temp);
    return resized;
}

void configure_level(NodeType type) {
    switch (type) {
        case NODE_NORMAL:
            current_bg_color = makecol(0, 0, 0);
            global_enemy_hp = 3; global_spawn_rate = 40; level_kill_target = 10; 
            level_message = "MISSION: ELIMINATE 10 TARGETS"; break;
        case NODE_MINIBOSS:
            current_bg_color = makecol(40, 0, 0);
            global_enemy_hp = 6; global_spawn_rate = 60; level_kill_target = 5;  
            level_message = "MISSION: DESTROY 5 HEAVY UNITS"; break;
        case NODE_EVENT:
            current_bg_color = makecol(0, 0, 40);
            global_enemy_hp = 1; global_spawn_rate = 15; level_kill_target = 30; 
            level_message = "SWARM: KILL 30 ENEMIES"; break;
        case NODE_STORE:
            current_bg_color = makecol(0, 40, 0); 
            global_enemy_hp = 0; global_spawn_rate = 9999; level_kill_target = 0; 
            level_message = "REPAIR STATION";
            if(player_health < 10) player_health += 5;
            if(player_health > 10) player_health = 10; break;
        case NODE_BOSS:
            current_bg_color = makecol(50, 0, 50);
            global_enemy_hp = 8; 
            global_spawn_rate = 50; 
            level_kill_target = 5; 
            level_message = "FINAL BATTLE"; break;
    }
}

int main(void) {
    srand(time(NULL));
    if (allegro_init() != 0) return 1;

    install_keyboard(); install_timer();
    set_color_depth(8);
    if (set_gfx_mode(GFX_AUTODETECT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) != 0) return 1;

    BITMAP *buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    PALETTE palette; generate_332_palette(palette); set_palette(palette);

    // --- CARREGAMENTO DE ASSETS ---
    BITMAP *player = load_bitmap("player.bmp", NULL);
    if (!player) { allegro_message("Erro: player.bmp"); return 1; }

    BITMAP *fundo = load_bitmap("fundo.bmp", NULL);
    if (!fundo) { allegro_message("Erro: fundo.bmp"); return 1; }

    BITMAP *loja_bg = load_bitmap("loja.bmp", NULL);
    if (!loja_bg) { 
        loja_bg = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
        clear_to_color(loja_bg, makecol(20, 50, 20)); 
    }

    // --- CARREGAMENTO DOS FUNDOS DO BOSS ---
    BITMAP *boss_bg1 = load_bitmap("FundoBoss1.bmp", NULL);
    BITMAP *boss_bg2 = load_bitmap("FundoBoss2.bmp", NULL);
    BITMAP *boss_bg3 = load_bitmap("FundoBoss3.bmp", NULL);

    // Fallback silencioso (Se falhar, usa fundo normal)
    if(!boss_bg1) boss_bg1 = fundo;
    if(!boss_bg2) boss_bg2 = fundo;
    if(!boss_bg3) boss_bg3 = fundo;

    BITMAP *enemy_bmp = load_bitmap("enemy.bmp", NULL);
    if (!enemy_bmp) { allegro_message("Erro: enemy.bmp"); return 1; }
    ENEMY_RADIUS = enemy_bmp->w / 2; 
    
    BITMAP *boss_bmp = load_bitmap("boss.bmp", NULL);
    if (!boss_bmp) boss_bmp = enemy_bmp; 

    BITMAP *heart = load_bitmap("heart.bmp", NULL);
    if (!heart) { allegro_message("Erro: heart.bmp"); return 1; }

    BITMAP *playerBullet1 = load_bitmap("bullet1.bmp", NULL);
    BITMAP *playerBullet2 = load_bitmap("bullet2.bmp", NULL);
    BITMAP *playerBullet3 = load_bitmap("bullet3.bmp", NULL);

    if (!playerBullet1) { allegro_message("Erro: bullet bitmaps"); return 1; }

    bool exit_program = false;

    while (!exit_program) {
        player_health = 10; player_x = 100; player_y = 100;
        game_over = 0; init_bullets(); init_enemies();

        int menu_cooldown = 0;
        while (!key[KEY_ENTER]) {
            draw_menu(buffer);
            if (key[KEY_0] && menu_cooldown<=0) { selectbullet(0); menu_cooldown=10; }
            if (key[KEY_1] && menu_cooldown<=0) { selectbullet(1); menu_cooldown=10; }
            if (key[KEY_2] && menu_cooldown<=0) { selectbullet(2); menu_cooldown=10; }
            if(menu_cooldown > 0) menu_cooldown--;
            if (key[KEY_ESC]) { exit_program = true; break; }
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); rest(16);
        }
        while (key[KEY_ENTER]) { rest(10); } 
        if (exit_program) break;

        TreeNode *campaign_map = generate_map(5);
        TreeNode *current_node = campaign_map;
        int selected_child_index = 0; int input_delay = 0;
        bool campaign_running = true;

        while (campaign_running && !key[KEY_ESC]) {
            bool selecting_path = true;
            if (current_node->num_children == 0) {
                clear_to_color(buffer, makecol(0,0,0));
                textout_centre_ex(buffer, font, "GALAXY SAVED!", SCREEN_W/2, SCREEN_H/2, makecol(255,255,0), -1);
                textout_centre_ex(buffer, font, "Press ENTER to Restart", SCREEN_W/2, SCREEN_H/2 + 20, makecol(255,255,255), -1);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_HEIGHT);
                rest(1000); while(!key[KEY_ENTER]) { rest(10); } 
                campaign_running = false; break; 
            }

            while (selecting_path) {
                clear_to_color(buffer, makecol(20, 20, 30));
                draw_map(buffer, campaign_map, current_node, selected_child_index);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_HEIGHT);
                if (input_delay > 0) input_delay--;
                if (key[KEY_LEFT] && input_delay == 0 && selected_child_index > 0) { selected_child_index--; input_delay = 10; }
                if (key[KEY_RIGHT] && input_delay == 0 && selected_child_index < current_node->num_children - 1) { selected_child_index++; input_delay = 10; }
                if (key[KEY_ENTER] && input_delay == 0) {
                    current_node = current_node->children[selected_child_index];
                    selected_child_index = 0; selecting_path = false; rest(200);
                }
                if (key[KEY_ESC]) { campaign_running = false; selecting_path = false; }
                rest(16);
            }
            if (!campaign_running) break;

            configure_level(current_node->type);
            
            // POSICIONAMENTO
            if (current_node->type == NODE_STORE) {
                player_x = 160; 
                player_y = 150;
            } else {
                player_x = 100; 
                player_y = 150;
            }
            
            init_enemies();
            for (int i = 0; i < MAX_BULLETS; i++) { bullets[i].active = 0; }
            enemy_counter = 0; 

            clear_to_color(buffer, makecol(0,0,0));
            textout_centre_ex(buffer, font, level_message, SCREEN_W/2, SCREEN_H/2, makecol(255,255,255), -1);
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_HEIGHT); rest(1500);

            bool level_running = true;
            int repair_station_timer = 200; 
            bool boss_warning_shown = false;

            while (level_running) {
                if (game_over || player_health <= 0) {
                    textout_centre_ex(screen, font, "YOU DIED", SCREEN_W/2, SCREEN_H/2, makecol(255,0,0), -1);
                    rest(2000);
                    campaign_running = false; level_running = false; continue;
                }
                
                // CONDICÕES DE VITÓRIA / LÓGICA DO BOSS
                if (current_node->type == NODE_BOSS) {
                    if (enemy_counter >= level_kill_target) {
                         // --- INTRODUÇÃO DRAMÁTICA DO BOSS ---
                         if(!boss_warning_shown) {
                             // 1. Tela Preta
                             clear_to_color(buffer, makecol(0,0,0));
                             
                             // 2. Texto Vermelho Centralizado
                             textout_centre_ex(buffer, font, "DEFEAT THE BOSS", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 10, makecol(255,0,0), -1);
                             
                             // 3. Renderiza na tela
                             blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                             
                             // 4. Pausa Dramática (3 segundos)
                             rest(3000);
                             
                             boss_warning_shown = true;
                         }
                    }
                    check_boss_death();
                    if (boss_defeated) level_running = false;
                } 
                // ... (o resto das condições else if continua igual) ...
                else if (current_node->type != NODE_STORE) {
                    if (enemy_counter >= level_kill_target) level_running = false;
                } else {
                    repair_station_timer--;
                    if(repair_station_timer <= 0) level_running = false;
                }

                // ======================================
                // DESENHO DO FUNDO (BACKGROUND)
                // ======================================
                
                // 1. FASE DA LOJA
                if (current_node->type == NODE_STORE) {
                    stretch_blit(loja_bg, buffer, 0, 0, loja_bg->w, loja_bg->h, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                } 
                // 2. FASE DO BOSS (Com troca dinâmica de fundo)
                else if (current_node->type == NODE_BOSS && boss_spawned) {
                    // Calcula HP do Boss (Índice 0)
                    float hp_percent = (float)enemies[0].health / (float)enemies[0].max_health;
                    
                    BITMAP* current_boss_bg;
                    
                    if (hp_percent < 0.3) {
                        current_boss_bg = boss_bg3; // Fase Final
                    } else if (hp_percent < 0.6) {
                        current_boss_bg = boss_bg2; // Fase 2
                    } else {
                        current_boss_bg = boss_bg1; // Fase 1
                    }
                    
                    // CORREÇÃO AQUI: Usa stretch_blit mas respeitando a margem central (80 a 240)
                    // Isso garante que a imagem preencha o buraco de 160x200 perfeitamente
                    stretch_blit(current_boss_bg, buffer, 0, 0, current_boss_bg->w, current_boss_bg->h, 80, 0, 160, 200);
                }
                // 3. FASES NORMAIS (Elite, Evento, Comum)
                else {
                    clear_to_color(buffer, current_bg_color);
                    masked_blit(fundo, buffer, 0, 0, 80, 0, fundo->w, fundo->h);
                }

                if (key[KEY_UP] && player_y - PLAYER_RADIUS > 0) player_y -= 4;
                if (key[KEY_DOWN] && player_y + PLAYER_RADIUS < SCREEN_HEIGHT) player_y += 4;
                if (key[KEY_LEFT] && player_x - PLAYER_RADIUS > SCREEN_WIDTH / 4) player_x -= 4;
                if (key[KEY_RIGHT] && player_x + PLAYER_RADIUS < (SCREEN_WIDTH * 3) / 4) player_x += 4;
                if (key[KEY_SPACE] && bullet_cooldown <= 0) shoot_bullet(player_x, player_y + 5, 0);
                if (key[KEY_ESC]) { campaign_running = false; level_running = false; }
                if (player_y + PLAYER_RADIUS >= SCREEN_HEIGHT) player_y = SCREEN_HEIGHT - PLAYER_RADIUS;
                
                if (current_node->type != NODE_STORE) spawn_enemy(current_node->type);
                
                update_bullets(); update_enemies();
                check_bullet_enemy_collisions(); check_player_bullet_collision(); check_player_enemy_collision();

                if (player_invincible) { player_hit_timer--; if (player_hit_timer <= 0) player_invincible = false; }
                if (!player_invincible || (player_hit_timer % 4) < 2) masked_blit(player, buffer, 0, 0, player_x - player->w/2, player_y - player->h/2, player->w, player->h);

                draw_bullets(buffer, playerBullet1, playerBullet1, playerBullet2, playerBullet3);
                draw_enemies(buffer, enemy_bmp, boss_bmp);

                // --- HUD / BARRAS LATERAIS ---
                rectfill(buffer, 0, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT, makecol(59, 68, 75));
                rectfill(buffer, (SCREEN_WIDTH * 3) / 4, 0, SCREEN_WIDTH, SCREEN_HEIGHT, makecol(59, 68, 75));
                
                if(current_node->type == NODE_BOSS && enemy_counter >= level_kill_target) {
                     textout_centre_ex(buffer, font, "DEFEAT BOSS", 160, 10, makecol(255,0,0), -1);
                } else if (current_node->type != NODE_STORE) {
                    show_enemy_counter(buffer); 
                }
                
                draw_heart(buffer, heart);
                draw_bullet_mag(buffer, playerBullet1, playerBullet1, playerBullet2, playerBullet3);
                draw_double(buffer, SCREEN_WIDTH * (4.5 / 5), 10, (bullet_cooldown / 60.0));

                if (current_node->type != NODE_STORE && current_node->type != NODE_BOSS) {
                    rectfill(buffer, 80, 5, 240, 8, makecol(100,100,100)); 
                    int w = (enemy_counter * 160) / level_kill_target; if(w>160) w=160;
                    rectfill(buffer, 80, 5, 80 + w, 8, makecol(100, 200, 255));
                } else if (current_node->type == NODE_STORE) {
                    textout_centre_ex(buffer, font, "REPAIRING...", 160, 5, makecol(0,255,0), -1);
                    textout_centre_ex(buffer, font, "STATION", 160, SCREEN_HEIGHT/2 - 20, makecol(255,255,255), -1);
                }

                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); rest(16);
            }

            // --- TELA DE VITÓRIA DA FASE ---
            if (!game_over && campaign_running && current_node->type != NODE_STORE) {
                rectfill(buffer, 60, SCREEN_HEIGHT/2 - 25, 260, SCREEN_HEIGHT/2 + 25, makecol(0,0,0));
                rect(buffer, 60, SCREEN_HEIGHT/2 - 25, 260, SCREEN_HEIGHT/2 + 25, makecol(255,255,255));

                char* win_msg = "SECTOR SECURED";
                if(current_node->type == NODE_MINIBOSS) win_msg = "THREAT NEUTRALIZED";
                if(current_node->type == NODE_BOSS) win_msg = "TARGET DESTROYED"; 

                textout_centre_ex(buffer, font, win_msg, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 12, makecol(0,255,0), -1);
                textout_centre_ex(buffer, font, "PRESS ENTER TO DEPART", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 5, makecol(180,180,180), -1);
                
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                
                rest(500); // Pequena pausa para evitar input acidental
                
                // 1. Limpa o buffer (se já estiver segurando)
                while(key[KEY_ENTER]) { rest(10); }

                // 2. Espera apertar
                while(!key[KEY_ENTER]) { rest(10); }

                // 3. CORREÇÃO CRÍTICA: Espera SOLTAR o Enter
                // O código só sai daqui quando você tira o dedo da tecla.
                while(key[KEY_ENTER]) { rest(10); }
            }
        }
        if (campaign_map) destroy_tree(campaign_map);
    }
    destroy_bitmap(player); destroy_bitmap(fundo); destroy_bitmap(loja_bg); 
    if(boss_bg1 != fundo) destroy_bitmap(boss_bg1);
    if(boss_bg2 != fundo) destroy_bitmap(boss_bg2);
    if(boss_bg3 != fundo) destroy_bitmap(boss_bg3);
    destroy_bitmap(enemy_bmp); if(boss_bmp != enemy_bmp) destroy_bitmap(boss_bmp);
    destroy_bitmap(playerBullet1); destroy_bitmap(playerBullet2); destroy_bitmap(playerBullet3);
    destroy_bitmap(heart); destroy_bitmap(buffer);
    allegro_exit();
    return 0;
}
END_OF_MAIN()