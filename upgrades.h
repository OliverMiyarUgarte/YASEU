typedef struct {
    const char* title;
    const char* desc1;
    const char* desc2;
    int id;
} UpgradeInfo;

UpgradeInfo all_upgrades[] = {
    {"Shooting", "Shoot 50%", "faster",        1},
    {"Reloading","Reload 1s", "faster",        2},
    {"Damage",   "Deal 50% more","damage",     3},
    {"Mag Size", "Increase mag", "size by 5",       4},
};

const int TOTAL_UPGRADES = sizeof(all_upgrades)/sizeof(all_upgrades[0]);

int upgrade_slot[3] = {0,0,0};

void randomize_upgrades() {
    int used[10] = {0};

    for (int i = 0; i < 3; i++) {
        int x;
        do {
            x = rand() % TOTAL_UPGRADES;
        } while (used[x]);

        used[x] = 1;
        upgrade_slot[i] = x;
    }
}

void draw_upgrades_menu(BITMAP* buffer) {
    clear_to_color(buffer, makecol(0, 0, 0));

    int screen_width = 320;
    int top_y = 40;
    int spacing = screen_width / 3;
    int line_gap = text_height(font) + 2;

    // opcao 1
    UpgradeInfo* A = &all_upgrades[ upgrade_slot[0] ];
    int centerA = spacing * 0 + spacing/2;

    textout_ex(buffer, font, A->title,
        centerA - text_length(font, A->title)/2, top_y,
        makecol(200,200,50), -1);

    textout_ex(buffer, font, A->desc1,
        centerA - text_length(font, A->desc1)/2, top_y+20,
        makecol(150,150,150), -1);

    textout_ex(buffer, font, A->desc2,
        centerA - text_length(font, A->desc2)/2, top_y+20+line_gap,
        makecol(150,150,150), -1);

    // opcao 2
    UpgradeInfo* B = &all_upgrades[ upgrade_slot[1] ];
    int centerB = spacing * 1 + spacing/2;

    textout_ex(buffer, font, B->title,
        centerB - text_length(font, B->title)/2, top_y,
        makecol(200,200,50), -1);

    textout_ex(buffer, font, B->desc1,
        centerB - text_length(font, B->desc1)/2, top_y+20,
        makecol(150,150,150), -1);

    textout_ex(buffer, font, B->desc2,
        centerB - text_length(font, B->desc2)/2, top_y+20+line_gap,
        makecol(150,150,150), -1);

    // opcao 3
    UpgradeInfo* C = &all_upgrades[ upgrade_slot[2] ];
    int centerC = spacing * 2 + spacing/2;

    textout_ex(buffer, font, C->title,
        centerC - text_length(font, C->title)/2, top_y,
        makecol(200,200,50), -1);

    textout_ex(buffer, font, C->desc1,
        centerC - text_length(font, C->desc1)/2, top_y+20,
        makecol(150,150,150), -1);

    textout_ex(buffer, font, C->desc2,
        centerC - text_length(font, C->desc2)/2, top_y+20+line_gap,
        makecol(150,150,150), -1);

    //heal
    const char* heal_title = "Heal";
    const char* heal_desc1 = "Ignore upgrades to heal  health";
    const char* heal_desc2 = "Press 4";

    int heal_y = 150;
    int heal_center = screen_width / 2;

    textout_ex(buffer, font, heal_title,
        heal_center - text_length(font, heal_title)/2,
        heal_y,
        makecol(50,200,50), -1);

    textout_ex(buffer, font, heal_desc1,
        heal_center - text_length(font, heal_desc1)/2,
        heal_y + 10,
        makecol(150,255,150), -1);

    textout_ex(buffer, font, heal_desc2,
        heal_center - text_length(font, heal_desc2)/2,
        heal_y + 10 + line_gap,
        makecol(150,255,150), -1);


    const char* sel_msg = "Press 1/2/3 to select upgrades";
    int x_sel = (screen_width - text_length(font, sel_msg)) / 2;
    textout_ex(buffer, font, sel_msg, x_sel, 120, makecol(200,200,200), -1);

}


void selectupgrade(int n){
    if (n==1){
        addshootingspeed(50);
    }
    if (n==2){
        addcooldown(60);
    }
    if (n==3){
        adddmg(50);
    }
    if (n==4){
        addmagsize(5);
    }
}

void draw_bullets_menu(BITMAP* buffer, BITMAP *playerBullet1, BITMAP *playerBullet2, BITMAP *playerBullet3, double* BulletCooldownReduction) {
    clear_to_color(buffer, makecol(0, 0, 0));
    
    const char* title = "Bullet Selection";
    int x_title = (SCREEN_WIDTH - text_length(font, title)) / 2;
    textprintf_ex(buffer, font, x_title, 10, makecol(200,200,50), -1, title);

    // Escolher os tiros
    const char* select_msg = "Press 1 / 2 / 3 to select bullets";
    int x_select = (SCREEN_WIDTH - text_length(font, select_msg)) / 2;
    textprintf_ex(buffer, font, x_select, 90, makecol(150,150,150), -1, select_msg);

    // desescolher os tiros
    const char* deselect_msg = "Press BACKSPACE to deselect bullets";
    textprintf_ex(buffer, font, x_select, 120, makecol(150,150,150), -1, deselect_msg);

    // Mensagem para iniciar 
    const char* start_msg = "Press ENTER to go away";
    int x_start = (SCREEN_WIDTH - text_length(font, start_msg)) / 2;
    textprintf_ex(buffer, font, x_start, 150, makecol(150,150,150), -1, start_msg);

    draw_horizontal_bullet_mag(buffer, playerBullet1, playerBullet2, playerBullet3);
}

