void draw_heart(BITMAP* buffer, BITMAP* heart){
    for(int i = 0; i < player_health; i++){
        masked_blit(heart, buffer, 0, 0, 5 + i * (heart->w + 5), 190, heart->w, heart->h);
    }
}