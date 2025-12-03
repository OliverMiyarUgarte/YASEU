#include "fila.h"

typedef struct {
   int x, y;
   int active;
   int is_enemy_bullet;
   int speedx; // n usado
   int speedy;
   int radius;
   int cooldown;
   int damage;
   int spread;
} Bullet;

Bullet btypes[3];
Bullet bullets[MAX_BULLETS];
fila pbullets[2];

int atual = 0;
int ativa = 0;
int deucerto;

int BulletCooldownReduction = 0;

void init_bullets() {
   for (int i = 0; i < MAX_BULLETS; i++) {
       bullets[i].active = 0;
       bullets[i].is_enemy_bullet = 0;
   }


   Cria(&pbullets[0]);
   Cria(&pbullets[1]);

   //setup bullet types (ORIGINAIS)
   // Tipo 0
   btypes[0].speedx = 0; 
   btypes[0].speedy = 4; 
   btypes[0].radius = 3; // Restaurado para 3
   btypes[0].cooldown = 5;
   btypes[0].damage = 1;
   btypes[0].spread = 1;

   // Tipo 1
   btypes[1].speedx = 0; 
   btypes[1].speedy = 2; 
   btypes[1].radius = 5; // Restaurado para 5
   btypes[1].cooldown = 10;
   btypes[1].damage = 2;
   btypes[1].spread = 0;

   // Tipo 2
   btypes[2].speedx = 0;
   btypes[2].speedy = 7; 
   btypes[2].radius = 1; // Mantido 1
   btypes[2].cooldown = 2; 
   btypes[2].damage = 1;
   btypes[2].spread = 3;

}

void selectbullet(int i){
    int x;
    Insere(&pbullets[0], i, &x);
}

void deselectbullet(){
    fila aux;
    int bullet;
    int ok;
    int n = 0;
    while(!Vazia(&pbullets[0])){
        Retira(&pbullets[0], &bullet, &ok);
        Insere(&aux, bullet, &ok);
        n++;
    }

    while (n>1){
        Retira(&aux, &bullet, &ok);
        Insere(&pbullets[0], bullet, &ok);
    }
}

void shoot_bullet(int x, int y, int is_enemy_bullet) {
   if (is_enemy_bullet == 0 && bullet_cooldown > 0) {
       return;
   }

   for (int i = 0; i < MAX_BULLETS; i++) {
       if (!bullets[i].active) {
           bullets[i].x = x+10;
           bullets[i].y = y;
           bullets[i].active = 1;
           bullets[i].is_enemy_bullet = is_enemy_bullet;

           int deucerto = 1;

           if (is_enemy_bullet) {
                // VOLTA A LÓGICA ORIGINAL DO INIMIGO (Usando btypes aleatórios)
                bullets[i].x = x;
                int rand_index = rand() % 3;
               
                // Nota: O código original tinha um bug lógico aqui acessando bullets[i].spread antes de definir,
                // mas corrigi para usar btypes[rand_index].spread para manter a lógica funcional.
                bullets[i].spread = btypes[rand_index].spread; 
                
                bullets[i].speedx = (bullets[i].spread == 0) ? 0 : (rand() % 2 == 0) ? rand() % bullets[i].spread : -rand() % bullets[i].spread;
                bullets[i].speedy = btypes[rand_index].speedy;
                bullets[i].radius = btypes[rand_index].radius;
                bullets[i].cooldown = btypes[rand_index].cooldown;
                bullets[i].damage = btypes[rand_index].damage;

           } else { // player bullet
               if (Vazia(&pbullets[0]) && Vazia(&pbullets[1])) { // no bullet selected
                    bullets[i].active = 0;
                   break;
               }

               if (!Vazia(&pbullets[ativa])) {
                   Retira(&pbullets[ativa], &atual, &deucerto);
                   Insere(&pbullets[1 - ativa], atual, &deucerto);
                    bullet_cooldown = btypes[atual].cooldown;
                    bullets[i].speedx = (btypes[atual].spread == 0) ? 0 : (rand() % 2 == 0) ? rand() % btypes[atual].spread : -rand() % btypes[atual].spread;
                    bullets[i].speedy = btypes[atual].speedy;
                    bullets[i].radius = btypes[atual].radius;
                    bullets[i].cooldown = btypes[atual].cooldown;
                    bullets[i].damage = btypes[atual].damage;
               } else {
                   ativa = 1 - ativa; 
                   bullet_cooldown = Nelementos(&pbullets[ativa])*10 - BulletCooldownReduction;
                   bullets[i].active = 0;
               }
           }
           break;
       }
   }
}

void draw_current_cooldown(BITMAP* buffer){
    char str_buffer[10];
    snprintf(str_buffer,sizeof(str_buffer), "%.1f", bullet_cooldown);
    textprintf_ex(buffer, font, SCREEN_WIDTH*(4.5/5), 10, makecol(10, 200, 10), -1, "%.1f" ,  str_buffer);
}

void draw_bullet_cooldown(BITMAP* buffer, fila* f){
    double cd = (Nelementos(f)*10 - BulletCooldownReduction)/60;
    char str_buffer[10];
    (str_buffer,sizeof(str_buffer), "%.1f", cd);

    textprintf_ex(buffer, font, SCREEN_WIDTH*(4.5/5), 10, makecol(10, 200, 10), -1, "%.1f" , bullet_cooldown/60.0);
}

void draw_double(BITMAP *buffer, int x, int y, double value){
    textprintf_ex(buffer, font, x, y, makecol(10, 200, 10), -1, "%.1f", value);
}



void draw_bullet_mag(BITMAP* buffer, BITMAP* enemy_bullet1, BITMAP* playerBullet1, BITMAP* playerBullet2, BITMAP* playerBullet3) {
    fila temp;
    Cria(&temp);
    int pos = 10;
    int atual;
    while (!Vazia(&pbullets[ativa])){
        Retira(&pbullets[ativa], &atual, &deucerto);
        Insere(&temp, atual, &deucerto);
        //circlefill(buffer, SCREEN_WIDTH*(4.0/5), pos, 10 /*radius*/, makecol(255, 50, 50));
               if(atual == 0){
                    masked_blit(playerBullet1, buffer, 0, 0, SCREEN_WIDTH*(4.0/5), pos, playerBullet1->w, playerBullet1->h);
               }
               if(atual == 1){
                    masked_blit(playerBullet2, buffer, 0, 0, SCREEN_WIDTH*(4.0/5), pos, playerBullet2->w, playerBullet2->h);
               }
               if(atual == 2){
                    masked_blit(playerBullet3, buffer, 0, 0, SCREEN_WIDTH*(4.0/5), pos, playerBullet3->w, playerBullet3->h);
               }
        pos += 10;
    }
    while (!Vazia(&temp)){
        Retira(&temp, &atual, &deucerto);
        Insere(&pbullets[ativa], atual, &deucerto);
    }
}


void update_bullets() {
   if (bullet_cooldown > 0) {
       bullet_cooldown--;
   }
  
   for (int i = 0; i < MAX_BULLETS; i++) {
       if (bullets[i].active) {
           if (bullets[i].is_enemy_bullet) {
               bullets[i].x += bullets[i].speedx;
               if (bullets[i].x > SCREEN_WIDTH) { // Mantido WIDTH conforme correções recentes
                   bullets[i].active = 0;
               }
                bullets[i].y += bullets[i].speedy;
               if (bullets[i].y > SCREEN_HEIGHT) { // Mantido HEIGHT conforme correções recentes
                   bullets[i].active = 0;
               }
           }
           else {
               bullets[i].x -= bullets[i].speedx;
               if (bullets[i].x < 0) {
                   bullets[i].active = 0;
               }
               bullets[i].y -= bullets[i].speedy;
               if (bullets[i].y < 0) {
                   bullets[i].active = 0;
               }
           }
       }
   }
}

void draw_bullets(BITMAP* buffer, BITMAP* enemy_bullet1, BITMAP* playerBullet1, BITMAP* playerBullet2, BITMAP* playerBullet3) {
   for (int i = 0; i < MAX_BULLETS; i++) {
       if (bullets[i].active) {
           if (bullets[i].is_enemy_bullet) {
               // Volta ao desenho original com offset
               circlefill(buffer, bullets[i].x - 5, bullets[i].y + 5, bullets[i].radius, makecol(255, 50, 50)); 
           } else {
               // Verifica os raios originais (3, 5, 1)
               if(bullets[i].radius == 3){
                    masked_blit(playerBullet1, buffer, 0, 0, bullets[i].x - playerBullet1->w / 2 - 10, bullets[i].y - playerBullet1->h / 2 - 20, playerBullet1->w, playerBullet1->h); 
               }
               if(bullets[i].radius == 5){
                    masked_blit(playerBullet2, buffer, 0, 0, bullets[i].x - playerBullet2->w / 2 - 10, bullets[i].y - playerBullet2->h / 2 - 20, playerBullet2->w, playerBullet2->h); 
               }
               if(bullets[i].radius == 1){
                    masked_blit(playerBullet3, buffer, 0, 0, bullets[i].x - playerBullet3->w / 2 - 10, bullets[i].y - playerBullet3->h / 2 - 20, playerBullet3->w, playerBullet3->h); 
               }
           }
       }
   }
}

// NOVA FUNÇÃO: Permite criar padrões específicos (Leques, Círculos, Mirados)
void shoot_custom_bullet(int x, int y, int vx, int vy) {
   for (int i = 0; i < MAX_BULLETS; i++) {
       if (!bullets[i].active) {
           bullets[i].active = 1;
           bullets[i].is_enemy_bullet = 1; // Sempre inimigo
           bullets[i].x = x;
           bullets[i].y = y;
           bullets[i].radius = 2; // Pequena
           bullets[i].speedx = vx;
           bullets[i].speedy = vy;
           bullets[i].damage = 1;
           bullets[i].cooldown = 0;
           break;
       }
   }
}