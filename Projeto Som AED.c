#include <allegro.h>

SAMPLE *som_morte_player;
SAMPLE *som_morte_inimigo;
SAMPLE *som_item;

int main() {
    allegro_init();
    install_keyboard();
    install_timer();

    // Instalar sistema de áudio (digital)
    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0) {
        allegro_message("Erro ao iniciar o som!");
        return 1;
    }

    // Carregar sons
    som_morte_player  = load_sample("som_morte_player.wav");
    som_morte_inimigo = load_sample("som_morte_inimigo.wav");
    som_item          = load_sample("som_item.wav");

    if (!som_morte_player || !som_morte_inimigo || !som_item) {
        allegro_message("Erro ao carregar algum arquivo de som!");
        return 1;
    }

    allegro_message("Sons carregados com sucesso!\nPressione ESC para sair.");

    // Loop principal só para demonstração
    while (!key[KEY_ESC]) {

        if (key[KEY_1])  // Simula morte do personagem
            play_sample(som_morte_player, 255, 128, 1000, 0);

        if (key[KEY_2])  // Simula morte do inimigo
            play_sample(som_morte_inimigo, 255, 128, 1000, 0);

        if (key[KEY_3])  // Simula pegar item
            play_sample(som_item, 255, 128, 1000, 0);

        rest(50);
    }

    destroy_sample(som_morte_player);
    destroy_sample(som_morte_inimigo);
    destroy_sample(som_item);

    return 0;
}
END_OF_MAIN();

