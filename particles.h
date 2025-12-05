#ifndef PARTICLES_H
#define PARTICLES_H

#include <allegro.h>
#include <math.h>

#define MAX_PARTICLES 200

typedef struct {
    float x, y;
    float vx, vy;
    int color;
    int life;      // Tempo de vida em frames
    int active;
    int size;      // 1 = pixel, 2 = bloquinho
} Particle;

Particle particles[MAX_PARTICLES];

void init_particles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = 0;
    }
}

// Cria uma explosão circular
void spawn_explosion(int x, int y, int count, int color) {
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = 1;
            particles[i].x = x;
            particles[i].y = y;
            particles[i].color = color;
            particles[i].life = 20 + (rand() % 20); // Vive entre 20 e 40 frames
            particles[i].size = (rand() % 2) + 1;   // Tamanho 1 ou 2
            
            // Física de Explosão (Direção Aleatória)
            float angle = (float)(rand() % 360) * 0.01745; // Graus para Radianos
            float speed = (float)((rand() % 30) + 10) / 10.0; // Velocidade 1.0 a 4.0
            
            particles[i].vx = cos(angle) * speed;
            particles[i].vy = sin(angle) * speed;
            
            spawned++;
            if (spawned >= count) break;
        }
    }
}

// Cria faíscas direcionais (bom para quando a bala acerta)
void spawn_sparks(int x, int y, int count, int color, int direction_x) {
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = 1;
            particles[i].x = x;
            particles[i].y = y;
            particles[i].color = color;
            particles[i].life = 10 + (rand() % 10);
            particles[i].size = 1;
            
            // Faíscas voam na direção oposta ao impacto (aproximadamente)
            particles[i].vx = direction_x * ((rand() % 30) / 10.0); 
            particles[i].vy = ((rand() % 40) - 20) / 10.0; // Espalha no Y
            
            spawned++;
            if (spawned >= count) break;
        }
    }
}

void update_particles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            
            // Atrito (as partículas desaceleram)
            particles[i].vx *= 0.95;
            particles[i].vy *= 0.95;
            
            particles[i].life--;
            if (particles[i].life <= 0) {
                particles[i].active = 0;
            }
        }
    }
}

void draw_particles(BITMAP* buffer) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            if (particles[i].size == 1) {
                putpixel(buffer, (int)particles[i].x, (int)particles[i].y, particles[i].color);
            } else {
                rectfill(buffer, (int)particles[i].x, (int)particles[i].y, 
                         (int)particles[i].x + 2, (int)particles[i].y + 2, particles[i].color);
            }
        }
    }
}

#endif