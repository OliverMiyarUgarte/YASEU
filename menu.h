void draw_menu(BITMAP* buffer) {
    clear_to_color(buffer, makecol(0, 0, 0));
    
    int screen_width = 320;

    //Título
    const char* title = "YASEU";
    int escala = 4;

    int largura_original = text_length(font, title);
    int altura_original = text_height(font);

    BITMAP *title_buffer = create_bitmap(largura_original, altura_original);
    
    if (title_buffer) {
        clear_to_color(title_buffer, bitmap_mask_color(buffer)); // Fundo transparente
        textout_ex(title_buffer, font, title, 0, 0, makecol(10, 200, 10), -1);

        int largura_final = largura_original * escala;
        int x_title = (screen_width - largura_final) / 2;

        masked_stretch_blit(title_buffer, buffer, 0, 0,largura_original, altura_original,x_title, 10,largura_final, altura_original * escala);

        destroy_bitmap(title_buffer);
    }

    // Créditos 
    const char* created = "Created by";
    int x_created = (screen_width - text_length(font, created)) / 2;
    textprintf_ex(buffer, font, x_created, 60, makecol(10, 200, 10), -1, created);

    const char* author1 = "Guilherme Aoki";
    int x_author1 = (screen_width - text_length(font, author1)) / 2;
    textprintf_ex(buffer, font, x_author1, 75, makecol(10, 200, 10), -1, author1);

    const char* author2 = "Lucas Yoshimura";
    int x_author2 = (screen_width - text_length(font, author2)) / 2;
    textprintf_ex(buffer, font, x_author2, 90, makecol(10, 200, 10), -1, author2);

    const char* author3 = "Oliver Ugarte";
    int x_author3 = (screen_width - text_length(font, author3)) / 2;
    textprintf_ex(buffer, font, x_author3, 105, makecol(10, 200, 10), -1, author3);

    // Mensagem para iniciar 
    const char* start_msg = "Press ENTER to start";
    int x_start = (screen_width - text_length(font, start_msg)) / 2;
    textprintf_ex(buffer, font, x_start, 150, makecol(10, 200, 10), -1, start_msg);


}
