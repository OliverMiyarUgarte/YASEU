typedef struct {
   int x, y;
   int active;
   int shoot_cooldown;
   int health; 
} Enemy;

Enemy enemies[MAX_ENEMIES];

void init_enemies() {
   for (int i = 0; i < MAX_ENEMIES; i++) {
       enemies[i].active = 0;
       enemies[i].shoot_cooldown = rand() % ENEMY_SHOOT_COOLDOWN; 
   }
}

void spawn_enemy() {
   static int spawn_timer = 0;
   spawn_timer++;
  
   if (spawn_timer >= 30) {
       spawn_timer = 0;
       for (int i = 0; i < MAX_ENEMIES; i++) {
           if (!enemies[i].active) {
                int random = rand() % ((SCREEN_WIDTH * 3 / 4) - (SCREEN_WIDTH / 4)) + (SCREEN_WIDTH / 4);
                if (random -16 < SCREEN_WIDTH / 4){
                    random += 16;
                }
                if (random +16 >(SCREEN_WIDTH * 3 / 4) - (SCREEN_WIDTH / 4)){
                    random -= 16;
                }
               enemies[i].x = random;
               enemies[i].y = 0;  
               enemies[i].active = 1;
               enemies[i].shoot_cooldown = rand() % ENEMY_SHOOT_COOLDOWN;
               enemies[i].health = 3;
               break;
           }
       }
   }
}




void update_enemies() {
   for (int i = 0; i < MAX_ENEMIES; i++) {
       if (enemies[i].active) {
           int direction = rand() % 2;
           enemies[i].y += 1;
           if(direction){enemies[i].x += rand() % 4 - 1;}else{enemies[i].x -= rand() % 4 - 1;}   
          
           if (enemies[i].shoot_cooldown > 0) {
               enemies[i].shoot_cooldown--;
           } else {
                shoot_bullet(enemies[i].x, enemies[i].y + ENEMY_RADIUS, 1);
               enemies[i].shoot_cooldown = ENEMY_SHOOT_COOLDOWN + (rand() % 30);
           }
          
           if (enemies[i].y > SCREEN_HEIGHT) {
               enemies[i].active = 0;
           }
       }
   }
}


void draw_enemies(BITMAP* buffer, BITMAP* enemy) {
   for (int i = 0; i < MAX_ENEMIES; i++) {
       if (enemies[i].active) {
            masked_blit(enemy, buffer, 0, 0, enemies[i].x - enemy->w / 2, enemies[i].y - enemy->h / 2, enemy->w, enemy->h); //Adicionei
           int health_bar_width = ENEMY_RADIUS * 2;
           int health_bar_height = 2;
           int health_bar_color = makecol(0, 255, 0); 
           if (enemies[i].health == 2) health_bar_color = makecol(255, 255, 0);
           if (enemies[i].health == 1) health_bar_color = makecol(255, 0, 0);


           rectfill(buffer, enemies[i].x - ENEMY_RADIUS, enemies[i].y - ENEMY_RADIUS - 5,
                    enemies[i].x - ENEMY_RADIUS + (enemies[i].health * health_bar_width / 3),
                    enemies[i].y - ENEMY_RADIUS - 3, health_bar_color);
       }
   }
}