#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define PLAYER_RADIUS 5
#define MAX_BULLETS 200
#define MAX_ENEMIES 5
#define ENEMY_SHOOT_COOLDOWN 90
#define PLAYER_INVINCIBILITY_TIME 60

// Taxa de atualização lógica (60 vezes por segundo)
#define LOGIC_RATE 30

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
#include "mapcre~1.h"
#include "mapDraw.h"
#include "heart.h"
#include "upgrades.h" 
#include "partic~1.h" 

// --- SISTEMA DE TIMER FIXO ---
// Essa variável conta quantos "pulsos" de lógica o jogo deve processar
volatile int ticks = 0;
void ticker() {
    ticks++;
}
END_OF_FUNCTION(ticker)

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
            global_enemy_hp = 1; global_spawn_rate = 8; level_kill_target = 50;
            level_message = "SURVIVE THE SWARM"; break;
        case NODE_STORE:
            current_bg_color = makecol(0, 40, 0); 
            global_enemy_hp = 0; global_spawn_rate = 9999; level_kill_target = 0; 
            level_message = "DOCKING AT STATION...";
            break;
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

    install_keyboard(); 
    install_timer(); // Instala o sistema de timer
    
    // CONFIGURA O TIMER PARA 60 TICKS POR SEGUNDO
    // Isso garante que a função 'ticker' seja chamada 60x por segundo
    LOCK_VARIABLE(ticks);
    LOCK_FUNCTION(ticker);
    install_int_ex(ticker, BPS_TO_TIMER(LOGIC_RATE));

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

    BITMAP *boss_bg1 = load_bitmap("FundoBoss1.bmp", NULL);
    BITMAP *boss_bg2 = load_bitmap("FundoBoss2.bmp", NULL);
    BITMAP *boss_bg3 = load_bitmap("FundoBoss3.bmp", NULL);
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
    int debug_boss_mode = 0;
    int FirstShop = 1;
    int menu_cooldown = 0;


    while (!exit_program) {
        // Reset do jogo
        player_health = 5; 
        player_x = 100; player_y = 100;
        game_over = 0; 
        init_bullets(); 
        init_enemies();
        init_particles();
        FirstShop = 1;
        
        // --- LOOP DO MENU PRINCIPAL ---
        while (!key[KEY_ENTER]) {
            draw_menu(buffer);

            if (key[KEY_B]) { debug_boss_mode = 1; exit_program = false; break; }
            if (key[KEY_ESC]) { exit_program = true; break; }
            
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); 
            rest(16);
        }
        while (key[KEY_ENTER]) { rest(10); } 
        if (exit_program) break;

        // --- PREPARAÇÃO DE BALAS ---
        int prep_cooldown = 10;
        double dummy_cd_reduction = 0; 
        while (1) {
            Transfere(&pbullets[1], &pbullets[0]); 
            clear_to_color(buffer, makecol(20, 20, 40)); 
            draw_bullets_menu(buffer, playerBullet1, playerBullet2, playerBullet3, &dummy_cd_reduction);
            textout_centre_ex(buffer, font, "PREPARE FOR DEPLOYMENT", SCREEN_WIDTH/2, 170, makecol(255, 50, 50), -1);
            textprintf_ex(buffer, font, 20, 40, makecol(10, 200, 10), -1, "Reload CD: %.2f", ((Nelementos(&pbullets[0])*10.0)/60));
            textprintf_ex(buffer, font, 20, 60, makecol(10, 200, 10), -1, "Max Mag: %d", getmagsize());

            if(key[KEY_1] && prep_cooldown <= 0 && (Nelementos(&pbullets[0]) < getmagsize())){ selectbullet(0); prep_cooldown = 10; }
            if(key[KEY_2] && prep_cooldown <= 0 && (Nelementos(&pbullets[0]) < getmagsize())){ selectbullet(1); prep_cooldown = 10; addcooldown(-5); }
            if(key[KEY_3] && prep_cooldown <= 0 && (Nelementos(&pbullets[0]) < getmagsize())){ selectbullet(2); prep_cooldown = 10; addcooldown(5); }
            if(key[KEY_BACKSPACE] && prep_cooldown <= 0){
                int deselected = deselectbullet();
                if (deselected == 1){addcooldown(5);} if (deselected == 2){addcooldown(-5);}
                prep_cooldown = 10;
            }
            if(key[KEY_ENTER] && prep_cooldown <= 0) {
                if (Vazia(&pbullets[0])) textout_centre_ex(buffer, font, "NO AMMO SELECTED!", SCREEN_WIDTH / 2, 100, makecol(255, 50, 50), -1);
                else { while (key[KEY_ENTER]){ rest(10); }; break; }
            }
            if(prep_cooldown > 0) prep_cooldown--;
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            rest(16);
        }

        // --- MAPA ---
        TreeNode *campaign_map;
        if (debug_boss_mode) {
            TreeNode* root = create_node(0, NODE_NORMAL);
            TreeNode* boss = create_node(1, NODE_BOSS);
            TreeNode* children[1] = {boss};
            connect_nodes(root, children, 1);
            campaign_map = root;
            addshootingspeed(50); adddmg(50); addmagsize(10);
        } else {
            campaign_map = generate_map(5);
        }

        TreeNode *current_node = campaign_map;
        int selected_child_index = 0; int input_delay = 0;
        bool campaign_running = true;

        while (campaign_running && !key[KEY_ESC]) {
            bool selecting_path = true;
            if (current_node->num_children == 0) {
                clear_to_color(buffer, makecol(0,0,0));
                textout_centre_ex(buffer, font, "GALAXY SAVED!", SCREEN_WIDTH/2, SCREEN_HEIGHT/2, makecol(255,255,0), -1);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                rest(2000); campaign_running = false; break; 
            }

            while (selecting_path) {
                clear_to_color(buffer, makecol(20, 20, 30));
                draw_map(buffer, campaign_map, current_node, selected_child_index);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
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
            player_x = 100; player_y = 150;
            init_enemies();
            for (int i = 0; i < MAX_BULLETS; i++) { bullets[i].active = 0; }
            enemy_counter = 0; 

            // --- LÓGICA ESPECIAL PARA A LOJA ---
            if ((current_node->type == NODE_STORE) || (FirstShop == 1)) {

                // 1. INTRODUÇÃO DA LOJA (ANIMAÇÃO DOCKING)
                clear_to_color(buffer, makecol(0,0,0));
                stretch_blit(loja_bg, buffer, 0, 0, loja_bg->w, loja_bg->h, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                rest(500);

                menu_cooldown = 10;
                resetmag();
                // ----------------------------------------------------
                // 2. PASSO UM: MENU DE UPGRADES
                // ----------------------------------------------------
                randomize_upgrades();
                while (1) { 
                    clear_to_color(buffer, makecol(20, 20, 30)); 
                    draw_upgrades_menu(buffer, FirstShop); 

                    if (key[KEY_1] && menu_cooldown <= 0) { selectupgrade(all_upgrades[upgrade_slot[0]].id); break; }
                    if (key[KEY_2] && menu_cooldown <= 0) { selectupgrade(all_upgrades[upgrade_slot[1]].id); break; }
                    if (key[KEY_3] && menu_cooldown <= 0) { selectupgrade(all_upgrades[upgrade_slot[2]].id); break; }
                    if (key[KEY_4] && menu_cooldown <= 0) { 
                        if (player_health < 5) player_health += 5; 
                        while(key[KEY_4]) { rest(10); } break; 
                    }
                    if (key[KEY_ENTER] && menu_cooldown <= 0) break; 
                    if (menu_cooldown > 0) menu_cooldown--;
                    blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); rest(16);
                }
                while(key[KEY_ENTER] || key[KEY_1] || key[KEY_2] || key[KEY_3] || key[KEY_4]) { rest(10); }
                menu_cooldown = 10;

                FirstShop = 0;


                // ----------------------------------------------------
                // 3. PASSO DOIS: MENU DE BALAS (RELOAD)
                // ----------------------------------------------------
                while (1) {
                    Transfere(&pbullets[1], &pbullets[0]); 
                    double cdreduction = getcooldown();
                    clear_to_color(buffer, makecol(20, 30, 20)); 
                    draw_bullets_menu(buffer, playerBullet1, playerBullet2, playerBullet3, &cdreduction);
                    // ... (Textos de reload e mag size igual ao anterior) ...
                    if(key[KEY_1] && menu_cooldown <= 0 && (Nelementos(&pbullets[0]) < getmagsize())){selectbullet(0); menu_cooldown = 10;}
                    if(key[KEY_2] && menu_cooldown <= 0 && (Nelementos(&pbullets[0]) < getmagsize())){selectbullet(1); menu_cooldown = 10; addcooldown(-5);}
                    if(key[KEY_3] && menu_cooldown <= 0 && (Nelementos(&pbullets[0]) < getmagsize())){selectbullet(2); menu_cooldown = 10; addcooldown(5);}
                    if(key[KEY_BACKSPACE] && menu_cooldown <= 0){
                        int deselected = deselectbullet();
                        if (deselected == 1){addcooldown(5);} if (deselected == 2){addcooldown(-5);}
                        menu_cooldown = 10;
                    }
                    if(key[KEY_ENTER] && menu_cooldown <= 0) {
                        if (!Vazia(&pbullets[0])) { while (key[KEY_ENTER]){ rest(10); }; break; }
                    }
                    if(menu_cooldown > 0) menu_cooldown--;
                    blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); rest(16);
                }
            } 
            else {
                clear_to_color(buffer, makecol(0,0,0));
                textout_centre_ex(buffer, font, level_message, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, makecol(255,255,255), -1);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); rest(1000);
            }

            bool level_running = true;
            int repair_station_timer = 200; 
            bool boss_warning_shown = false;

            // Reinicia o acumulador de tempo
            ticks = 0;

            while (level_running) {
                // =========================================================
                // 1. UPDATE LÓGICO (Fixo a 60 vezes por segundo)
                // =========================================================
                // O loop roda enquanto houver 'ticks' pendentes. 
                // Isso garante que se o PC der uma engasgada, ele roda o loop mais vezes
                // para compensar e manter a velocidade do jogo constante.
                while (ticks > 0) {
                    if (game_over || player_health <= 0) {
                        break; 
                    }
                    
                    // Checagem de Fim de Fase
                    if (current_node->type == NODE_BOSS) {
                        if (enemy_counter >= level_kill_target && !boss_warning_shown) boss_warning_shown = true;
                        check_boss_death();
                        if (boss_defeated) level_running = false;
                    } else if (current_node->type != NODE_STORE) {
                        if (enemy_counter >= level_kill_target) level_running = false;
                    } else {
                        repair_station_timer--;
                        if(repair_station_timer <= 0) level_running = false;
                    }

                    // Movimentação FIXA (Sem delta_time)
                    if (key[KEY_UP] && player_y - PLAYER_RADIUS > 0) player_y -= 4;
                    if (key[KEY_DOWN] && player_y + PLAYER_RADIUS < SCREEN_HEIGHT) player_y += 4;
                    if (key[KEY_LEFT] && player_x - PLAYER_RADIUS > SCREEN_WIDTH / 4) player_x -= 4;
                    if (key[KEY_RIGHT] && player_x + PLAYER_RADIUS < (SCREEN_WIDTH * 3) / 4) player_x += 4;
                    
                    if (key[KEY_SPACE] && bullet_cooldown <= 0) shoot_bullet(player_x, player_y + 5, 0);
                    if (key[KEY_ESC]) { campaign_running = false; level_running = false; }

                    // Cheats
                    if (current_node->type == NODE_BOSS && boss_spawned) {
                         if (key[KEY_F1]) enemies[0].health = enemies[0].max_health;
                         if (key[KEY_F2]) enemies[0].health = (int)(enemies[0].max_health * 0.55);
                         if (key[KEY_F3]) enemies[0].health = (int)(enemies[0].max_health * 0.25);
                         if (key[KEY_K]) enemies[0].health = 0;
                    }
                    if (key[KEY_I]) player_health = 999;

                    if (current_node->type != NODE_STORE) spawn_enemy(current_node->type);

                    update_bullets(); 
                    update_enemies(); 
                    update_particles();
                    
                    check_bullet_enemy_collisions(); 
                    check_player_bullet_collision(); 
                    check_player_enemy_collision();

                    if (player_invincible) { 
                        player_hit_timer--; // Decremento fixo
                        if (player_hit_timer <= 0) player_invincible = false; 
                    }
                    
                    // Consome 1 tick
                    ticks--;
                } // Fim do while(ticks)

                if (!level_running && !game_over) break; // Sai se venceu
                
                if (game_over) {
                    textout_centre_ex(screen, font, "YOU DIED", SCREEN_WIDTH/2, SCREEN_HEIGHT/2, makecol(255,0,0), -1);
                    rest(2000);
                    campaign_running = false; 
                    break;
                }

                // =========================================================
                // 2. RENDERIZAÇÃO (FPS Livre)
                // =========================================================
                if (current_node->type == NODE_STORE) {
                    stretch_blit(loja_bg, buffer, 0, 0, loja_bg->w, loja_bg->h, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                } else if (current_node->type == NODE_BOSS && boss_spawned) {
                    float hp_percent = (float)enemies[0].health / (float)enemies[0].max_health;
                    BITMAP* current_boss_bg = (hp_percent < 0.3) ? boss_bg3 : (hp_percent < 0.6 ? boss_bg2 : boss_bg1);
                    masked_blit(current_boss_bg, buffer, 0, 0, 80, 0, fundo->w, fundo->h);
                } else {
                    clear_to_color(buffer, current_bg_color);
                    masked_blit(fundo, buffer, 0, 0, 80, 0, fundo->w, fundo->h);
                }

                if (!player_invincible || (player_hit_timer % 4) < 2) 
                    masked_blit(player, buffer, 0, 0, player_x - player->w/2, player_y - player->h/2, player->w, player->h);

                draw_bullets(buffer, playerBullet1, playerBullet1, playerBullet2, playerBullet3);
                draw_enemies(buffer, enemy_bmp, boss_bmp);
                draw_particles(buffer);

                rectfill(buffer, 0, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT, makecol(59, 68, 75));
                rectfill(buffer, (SCREEN_WIDTH * 3) / 4, 0, SCREEN_WIDTH, SCREEN_HEIGHT, makecol(59, 68, 75));
                
                if(boss_warning_shown && current_node->type == NODE_BOSS && !boss_spawned) {
                     textout_centre_ex(buffer, font, "WARNING: BOSS APPROACHING", SCREEN_WIDTH/2, SCREEN_HEIGHT/2, makecol(255,0,0), -1);
                }
                else if(current_node->type == NODE_BOSS && enemy_counter >= level_kill_target) {
                     textout_centre_ex(buffer, font, "DEFEAT BOSS", 160, 10, makecol(255,0,0), -1);
                } else if (current_node->type != NODE_STORE) {
                    show_enemy_counter(buffer); 
                }
                
                draw_heart(buffer, heart);
                draw_bullet_mag(buffer, playerBullet1, playerBullet2, playerBullet3);
                draw_double(buffer, SCREEN_WIDTH * (4.5 / 5), 10, (bullet_cooldown / 60.0));

                if (current_node->type != NODE_STORE && current_node->type != NODE_BOSS) {
                    rectfill(buffer, 80, 5, 240, 8, makecol(100,100,100)); 
                    int w = (enemy_counter * 160) / level_kill_target; if(w>160) w=160;
                    rectfill(buffer, 80, 5, 80 + w, 8, makecol(100, 200, 255));
                } else if (current_node->type == NODE_STORE) {
                    textout_centre_ex(buffer, font, "REPAIRS COMPLETE", 160, 5, makecol(0,255,0), -1);
                    textout_centre_ex(buffer, font, "SYSTEMS READY", 160, SCREEN_HEIGHT/2 - 20, makecol(255,255,255), -1);
                }

                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); 
                
                // Mágica para não fritar a CPU: dá um descanso mínimo
                // O controle de velocidade real é feito pelo while(ticks) acima
                rest(0); 
            }

            if (!game_over && campaign_running && current_node->type != NODE_STORE) {
                rectfill(buffer, 60, SCREEN_HEIGHT/2 - 25, 260, SCREEN_HEIGHT/2 + 25, makecol(0,0,0));
                rect(buffer, 60, SCREEN_HEIGHT/2 - 25, 260, SCREEN_HEIGHT/2 + 25, makecol(255,255,255));
                char* win_msg = "SECTOR SECURED";
                if(current_node->type == NODE_MINIBOSS) win_msg = "THREAT NEUTRALIZED";
                if(current_node->type == NODE_BOSS) win_msg = "TARGET DESTROYED"; 
                textout_centre_ex(buffer, font, win_msg, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 12, makecol(0,255,0), -1);
                textout_centre_ex(buffer, font, "PRESS ENTER TO DEPART", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 5, makecol(180,180,180), -1);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                rest(500); 
                while(key[KEY_ENTER]) { rest(10); } 
                while(!key[KEY_ENTER]) { rest(10); }
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