#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define PLAYER_RADIUS 5
#define MAX_BULLETS 10
#define MAX_ENEMIES 5
#define ENEMY_RADIUS 8
#define ENEMY_SHOOT_COOLDOWN 90

int player_x = 100, player_y = 100;
int game_over = 0;
int bullet_cooldown = 0;

#include "bullets.h"
#include "enemies.h"
#include "collisions.h"
#include "menu.h"
#include "mapCreator.h"
#include "mapDraw.h"

int main(void) {
   if (allegro_init() != 0) {
       printf("Error initializing Allegro!\n");
       return 1;
   }
  
   install_keyboard();
   install_timer();


   set_color_depth(8);
   if (set_gfx_mode(GFX_AUTODETECT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) != 0) {
       printf("Error setting graphics mode!\n");
       return 1;
   }


   BITMAP* buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
   if (!buffer) {
       printf("Error creating off-screen buffer!\n");
       return 1;
   }


   PALETTE palette;
   generate_332_palette(palette);
   set_palette(palette);


   init_bullets();
   init_enemies();

   //select player bullets (temp)
   
   selectbullet(0); // normal
   selectbullet(0);
   selectbullet(0);
   selectbullet(1); // big
   selectbullet(1);
   selectbullet(1);
   selectbullet(1);
   selectbullet(2); // fast
   selectbullet(2);
   selectbullet(2); 

   //Avião
    BITMAP *player = load_bitmap("player.bmp", NULL);

    if(!player){
        allegro_message("Erro ao carregar player.bmp!");
        return 1;
    }

    //Background
    BITMAP *fundo = load_bitmap("fundo.bmp", NULL);

    if(!fundo){
        allegro_message("Erro ao carregar fundo.bmp!");
        return 1;
    }

    //Enemy
    BITMAP *enemy = load_bitmap("enemy.bmp", NULL);

    if(!enemy){
        allegro_message("Erro ao carregar enemy.bmp!");
        return 1;
    }

    //playerBullet
    BITMAP *playerBullet1 = load_bitmap("bullet1.bmp", NULL);
    BITMAP *playerBullet2 = load_bitmap("bullet2.bmp", NULL);
    BITMAP *playerBullet3 = load_bitmap("bullet3.bmp", NULL);


    if(!playerBullet1){
        allegro_message("Erro ao carregar playerBullet1.bmp!");
        return 1;
    }
    if(!playerBullet2){
        allegro_message("Erro ao carregar playerBullet2.bmp!");
        return 1;
    }
    if(!playerBullet3){
        allegro_message("Erro ao carregar playerBullet3.bmp!");
        return 1;
    }

    //Menu
    draw_menu(buffer);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    readkey();

    TreeNode* game_map = generate_map(4);
    print_tree(game_map, 0);
    TreeNode* map_root = generate_map(4);

    // Map selection screen
    while (!key[KEY_SPACE]) {
        clear_to_color(buffer, makecol(20, 20, 30));
        textout_centre_ex(buffer, font, "Press SPACE to start", SCREEN_W / 2, 10, makecol(255, 255, 255), -1);

        draw_map(buffer, map_root);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_HEIGHT);
        rest(16);

        if (key[KEY_ESC]) {
            destroy_tree(map_root);
            allegro_exit();
            return 0;
        }
    }

   

   while (1) {
       if (game_over) {
           textout_ex(screen, font, "Game Over! Press ESC to exit.", SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, makecol(255, 0, 0), -1);
           if (key[KEY_ESC]) {
               break;
           }
           rest(16);
           continue;
       }

    //Background
       clear_to_color(buffer, makecol(0, 0, 0));
       blit(fundo, buffer, 0, 0, 80, 0, fundo->w, fundo->h);



       if (key[KEY_UP] && player_y - PLAYER_RADIUS > 0) { player_y -= 4; }
       if (key[KEY_DOWN] && player_y + PLAYER_RADIUS < SCREEN_HEIGHT) { player_y += 4; }
       if (key[KEY_LEFT] && player_x - PLAYER_RADIUS > SCREEN_WIDTH / 4) { player_x -= 4; }
       if (key[KEY_RIGHT] && player_x + PLAYER_RADIUS < (SCREEN_WIDTH * 3) / 4) { player_x += 4; }
       if (key[KEY_SPACE] && bullet_cooldown == 0) { shoot_bullet(player_x, player_y + 5, 0); }
      
       if (key[KEY_ESC]) { break; }


       if (player_y + PLAYER_RADIUS >= SCREEN_HEIGHT) {
           player_y = SCREEN_HEIGHT - PLAYER_RADIUS;
       }


       spawn_enemy();
       update_bullets();
       update_enemies();
       check_bullet_enemy_collisions();
       check_player_bullet_collision();
       check_player_enemy_collision();


       //Desenehando avião
       masked_blit(player, buffer, 0, 0, player_x, player_y, player->w, player->h);

       //Desenhando balas
       draw_bullets(buffer, playerBullet1, playerBullet1, playerBullet2, playerBullet3);

       //Deseenhando inimigos
       draw_enemies(buffer, enemy);


       rectfill(buffer, 0, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT, makecol(59, 68, 75));
       rectfill(buffer, (SCREEN_WIDTH * 3) / 4, 0, SCREEN_WIDTH, SCREEN_HEIGHT, makecol(59, 68, 75));
       show_enemy_counter(buffer);


       blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


       rest(16);
   }
   
   destroy_tree(map_root);
   destroy_tree(game_map);
   destroy_bitmap(player);
   destroy_bitmap(fundo);
   destroy_bitmap(enemy);
   destroy_bitmap(playerBullet1);
   destroy_bitmap(playerBullet2);
   destroy_bitmap(playerBullet3);
   destroy_bitmap(buffer);
   allegro_exit();
   return 0;
}
END_OF_MAIN()
