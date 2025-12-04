// --- ARQUIVO: menu.h ---

// Variável global ou estática para o título cacheado
BITMAP *title_bitmap_cache = NULL;

void init_menu_assets() {
    if (title_bitmap_cache != NULL) return; // Já criado

    const char* title = "YASEU";
    int w = text_length(font, title);
    int h = text_height(font);
    
    // Cria o buffer pequeno original
    BITMAP *temp = create_bitmap(w, h);
    clear_to_color(temp, makecol(255, 0, 255)); // Magic pink para transparência
    textout_ex(temp, font, title, 0, 0, makecol(10, 200, 10), -1);

    // Cria o bitmap final já na escala 4x
    int escala = 4;
    title_bitmap_cache = create_bitmap(w * escala, h * escala);
    clear_to_color(title_bitmap_cache, makecol(255, 0, 255)); // Fundo transparente

    // Faz o stretch uma única vez no início do jogo
    stretch_blit(temp, title_bitmap_cache, 0, 0, w, h, 0, 0, w * escala, h * escala);
    
    destroy_bitmap(temp); // Destrói o temporário
}

// Chame isso quando fechar o jogo para não vazar memória
void destroy_menu_assets() {
    if (title_bitmap_cache) {
        destroy_bitmap(title_bitmap_cache);
        title_bitmap_cache = NULL;
    }
}

void draw_menu(BITMAP* buffer) {
    // Garante que o asset existe (lazy initialization)
    if (!title_bitmap_cache) init_menu_assets();

    clear_to_color(buffer, makecol(0, 0, 0));
    int screen_width = SCREEN_W; // Usa a largura real da Allegro

    // 1. Desenha o Título Otimizado
    if (title_bitmap_cache) {
        int x_title = (screen_width - title_bitmap_cache->w) / 2;
        masked_blit(title_bitmap_cache, buffer, 0, 0, x_title, 10, title_bitmap_cache->w, title_bitmap_cache->h);
    }

    // 2. Créditos ATUALIZADOS
    int color_green = makecol(10, 200, 10);
    
    textprintf_centre_ex(buffer, font, screen_width/2, 60, color_green, -1, "Created by");
    textprintf_centre_ex(buffer, font, screen_width/2, 75, color_green, -1, "Guilherme Aoki");
    textprintf_centre_ex(buffer, font, screen_width/2, 90, color_green, -1, "Lucas Yoshimura");
    textprintf_centre_ex(buffer, font, screen_width/2, 105, color_green, -1, "Oliver Ugarte");
    textprintf_centre_ex(buffer, font, screen_width/2, 120, color_green, -1, "Nicolas Albino");

    // 3. Start
    if ((retrace_count / 30) % 2 == 0) { // Efeito de piscar (Blink)
        textprintf_centre_ex(buffer, font, screen_width/2, 175, color_green, -1, "Press ENTER to start");
    }
}