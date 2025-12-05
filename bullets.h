#include "fila.h"

// Extern declaration for delta time (defined in main.c)
extern double delta_time;

typedef struct {
   int x, y;
   int active;
   int is_enemy_bullet;
   int speedx; // n usado
   int speedy;
   int radius;
   double cooldown;
   double damage;
   int spread;
} Bullet;

Bullet btypes[3];
Bullet bullets[MAX_BULLETS];
fila pbullets[2];

int atual = 0;
int ativa = 0;
int deucerto;

int BulletCooldownReduction = 0;
int ShootingSpeedBonus = 0;
int DmgBonus= 0;
int maxmagsize = 5;

void addcooldown(int n){
    BulletCooldownReduction += n;
}
int getcooldown(){
    return BulletCooldownReduction;
}

void addshootingspeed(int n){
    ShootingSpeedBonus += n;
}
int getshootingspeed(){
    return ShootingSpeedBonus;
}

void adddmg(int n){
    DmgBonus += n;
}
int getdmg(){
    return DmgBonus;
}

void addmagsize(int n){
    maxmagsize += n;
}
int getmagsize(){
    return maxmagsize;
}


void init_bullets() {
   for (int i = 0; i < MAX_BULLETS; i++) {
       bullets[i].active = 0;
       bullets[i].is_enemy_bullet = 0;
   }


   Cria(&pbullets[0]);
   Cria(&pbullets[1]);

   //setup bullet types
   // Tipo 0 (médio)
   btypes[0].speedx = 0; 
   btypes[0].speedy = 4; 
   btypes[0].radius = 3; 
   btypes[0].cooldown = 5;
   btypes[0].damage = 1;
   btypes[0].spread = 1;

   // Tipo 1 (grande)
   btypes[1].speedx = 0; 
   btypes[1].speedy = 2; 
   btypes[1].radius = 5; 
   btypes[1].cooldown = 10;
   btypes[1].damage = 2;
   btypes[1].spread = 0;

   // Tipo 2 (pequeno)
   btypes[2].speedx = 0;
   btypes[2].speedy = 7; 
   btypes[2].radius = 1; 
   btypes[2].cooldown = 2; 
   btypes[2].damage = 1;
   btypes[2].spread = 3;

}

void selectbullet(int i){
    int x;
    if (Nelementos(&pbullets[0]) < maxmagsize){
        Insere(&pbullets[0], i, &x);
    } 
}

int deselectbullet() {
    int bullet;
    int ok;

    if (Vazia(&pbullets[0])) {
        return -1; 
    }

    Node *atual = pbullets[0].primeiro;
    Node *anterior = NULL;

    while (atual->next != NULL) {
        anterior = atual;
        atual = atual->next;
    }

    bullet = atual->info;

    if (anterior == NULL) {
        pbullets[0].primeiro = NULL;
        pbullets[0].ultimo   = NULL;
    } else {
        anterior->next = NULL;
        pbullets[0].ultimo = anterior;
    }

    free(atual);
    return bullet;
}

// 1. Nova função para verificar recarga automaticamente
void check_auto_reload() {
    // Se a fila ativa está vazia...
    if (Vazia(&pbullets[ativa])) {
        
        // ...e a fila de reserva TEM balas (significa que temos munição para recarregar)
        if (!Vazia(&pbullets[1 - ativa])) {
            
            // Troca o pente (Swap)
            ativa = 1 - ativa; 
            
            // Aplica o tempo de recarga (Cooldown Longo)
            bullet_cooldown = (Nelementos(&pbullets[ativa]) * 10 - BulletCooldownReduction);
            
            // Segurança para o cooldown não ficar negativo ou zero instantâneo
            if (bullet_cooldown < 5) bullet_cooldown = 5;
        }
    }
}


void shoot_bullet(int x, int y, int is_enemy_bullet) {
   // Se for player e estiver em cooldown, não faz nada
   if (is_enemy_bullet == 0 && bullet_cooldown > 0) {
       return;
   }
   
   // Se for player e não tiver bala em NENHUMA fila, não atira
   if (is_enemy_bullet == 0 && Vazia(&pbullets[ativa]) && Vazia(&pbullets[1 - ativa])) {
       return;
   }

   for (int i = 0; i < MAX_BULLETS; i++) {
       if (!bullets[i].active) {
           bullets[i].active = 1;
           bullets[i].is_enemy_bullet = is_enemy_bullet;
           
           // Posição ajustada (centralizada) conforme correção anterior
           bullets[i].x = x; 
           bullets[i].y = y;

           int deucerto = 1;

           if (is_enemy_bullet) {
                // ... (Lógica do inimigo mantida igual) ...
                int rand_index = rand() % 3;
                bullets[i].spread = btypes[rand_index].spread; 
                bullets[i].speedx = (bullets[i].spread == 0) ? 0 : (rand() % 2 == 0) ? rand() % bullets[i].spread : -rand() % bullets[i].spread;
                bullets[i].speedy = btypes[rand_index].speedy;
                bullets[i].radius = btypes[rand_index].radius;
                bullets[i].cooldown = btypes[rand_index].cooldown;
                bullets[i].damage = btypes[rand_index].damage;

           } else { // Player Bullet
               
               // Se a fila ativa estiver vazia aqui, a recarga automática vai cuidar no próximo frame.
               // Apenas impedimos de atirar "nada".
               if (Vazia(&pbullets[ativa])) {
                   bullets[i].active = 0; // Cancela a criação da bala
                   break;
               }

               // Lógica de tirar da fila e atirar
               Retira(&pbullets[ativa], &atual, &deucerto);
               Insere(&pbullets[1 - ativa], atual, &deucerto); // Joga pro descarte

               bullet_cooldown = btypes[atual].cooldown;
               
               bullets[i].speedx = (btypes[atual].spread == 0) ? 0 : (rand() % 2 == 0) ? rand() % btypes[atual].spread : -rand() % btypes[atual].spread;
               bullets[i].speedy = btypes[atual].speedy;
               bullets[i].radius = btypes[atual].radius;
               
               // Aplica bônus
               double cd_bonus = btypes[atual].cooldown * (ShootingSpeedBonus/100.0);
               bullets[i].cooldown = btypes[atual].cooldown - cd_bonus;
               
               double dmg_bonus = btypes[atual].damage * (DmgBonus/100.0);
               bullets[i].damage = btypes[atual].damage + dmg_bonus;
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

void draw_bullet_mag(BITMAP* buffer, BITMAP* playerBullet1, BITMAP* playerBullet2, BITMAP* playerBullet3) {
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

void draw_horizontal_bullet_mag(BITMAP* buffer, BITMAP* playerBullet1, BITMAP* playerBullet2, BITMAP* playerBullet3) {
    fila temp;
    Cria(&temp);

    int count = 0;
    int atual;


    while (!Vazia(&pbullets[ativa])) {
        Retira(&pbullets[ativa], &atual, &deucerto);
        Insere(&temp, atual, &deucerto);
        count++;
    }


    while (!Vazia(&temp)) {
        Retira(&temp, &atual, &deucerto);
        Insere(&pbullets[ativa], atual, &deucerto);
    }

    if (count == 0) return;


    int spacing = 12;   
    int total_width = count * spacing;
    int start_x = (SCREEN_WIDTH / 2) - (total_width / 2);
    int y = 75;

    int angle = itofix(64); 

    Cria(&temp);
    while (!Vazia(&pbullets[ativa])) {
        Retira(&pbullets[ativa], &atual, &deucerto);
        Insere(&temp, atual, &deucerto);

        BITMAP* sprite = NULL;

        if (atual == 0) sprite = playerBullet1;
        if (atual == 1) sprite = playerBullet2;
        if (atual == 2) sprite = playerBullet3;

        rotate_sprite(buffer, sprite, start_x, y, angle);

        start_x += spacing;
    }

    while (!Vazia(&temp)) {
        Retira(&temp, &atual, &deucerto);
        Insere(&pbullets[ativa], atual, &deucerto);
    }
}

void resetmag(){
    Transfere(&pbullets[1], &pbullets[0]);
    ativa = 0;
    bullet_cooldown = 0;
}

void update_bullets() {
   // Chama a recarga automática todo frame
   check_auto_reload();

   if (bullet_cooldown > 0) {
       bullet_cooldown -= delta_time * 60.0;  // Convert delta_time to frame-like units
   }
  
   for (int i = 0; i < MAX_BULLETS; i++) {
       if (bullets[i].active) {
           if (bullets[i].is_enemy_bullet) {
               bullets[i].x += (int)(bullets[i].speedx * delta_time * 60.0);
               if (bullets[i].x > SCREEN_WIDTH) bullets[i].active = 0;
               bullets[i].y += (int)(bullets[i].speedy * delta_time * 60.0);
               if (bullets[i].y > SCREEN_HEIGHT) bullets[i].active = 0;
           }
           else {
               bullets[i].x -= bullets[i].speedx;
               if (bullets[i].x < 0) bullets[i].active = 0;
               bullets[i].y -= bullets[i].speedy;
               if (bullets[i].y < 0) bullets[i].active = 0;
           }
       }
   }
}

void draw_bullets(BITMAP* buffer, BITMAP* enemy_bullet1, BITMAP* playerBullet1, BITMAP* playerBullet2, BITMAP* playerBullet3) {
   for (int i = 0; i < MAX_BULLETS; i++) {
       if (bullets[i].active) {
           if (bullets[i].is_enemy_bullet) {
               // DEBUG: Se quiser ver a hitbox real do inimigo, descomente a linha abaixo
               // circle(buffer, bullets[i].x, bullets[i].y, bullets[i].radius, makecol(255, 0, 0));
               
               // Ajeitei aqui também: removi o deslocamento (-5, +5) do círculo
               circlefill(buffer, bullets[i].x, bullets[i].y, bullets[i].radius, makecol(255, 50, 50)); 
           } else {
               // --- CORREÇÃO AQUI ---
               // O sprite é desenhado subtraindo METADE da largura/altura para ficar centralizado.
               // Removi o "-10" e "-20" extras que causavam o deslocamento visual.
               
               if(bullets[i].radius == 3){
                    masked_blit(playerBullet1, buffer, 0, 0, 
                        bullets[i].x - playerBullet1->w / 2, 
                        bullets[i].y - playerBullet1->h / 2, 
                        playerBullet1->w, playerBullet1->h); 
               }
               if(bullets[i].radius == 5){
                    masked_blit(playerBullet2, buffer, 0, 0, 
                        bullets[i].x - playerBullet2->w / 2, 
                        bullets[i].y - playerBullet2->h / 2, 
                        playerBullet2->w, playerBullet2->h); 
               }
               if(bullets[i].radius == 1){
                    masked_blit(playerBullet3, buffer, 0, 0, 
                        bullets[i].x - playerBullet3->w / 2, 
                        bullets[i].y - playerBullet3->h / 2, 
                        playerBullet3->w, playerBullet3->h); 
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

