//
//  win_scene.c
//  panda_knight
//

#include "win_scene.h"
#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static int timer_countdown;
static Button backButton;
ALLEGRO_BITMAP * win_bitmap;
ALLEGRO_SAMPLE* win_audio;


static void init(void) {
    timer_countdown = 120;
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "UI_Button.png", "UI_Button_hovered.png");
    win_bitmap = al_load_bitmap("panda_win.png");
    if (!win_bitmap) {
        game_abort("Failed to load win bitmap");
    }
    //init and play win audio
    win_audio = al_load_sample("win.ogg");
    if (!win_audio) {
        game_log("No BGM File found [%s], no sound will be played", "win.ogg");
    }
    if (!al_play_sample(win_audio, SFX_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
        game_log("Audio not playing, please increase your RESERVE_SAMPLES variable");
    }
    
}

static void update(void) {

    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
    }
//    timer_countdown--;
//    if (timer_countdown == 0) {
//        change_scene(create_menu_scene());
//    }

}

static void draw(void) {
    // button
    draw_button(backButton);
    //button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        650 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    
    al_draw_scaled_bitmap(win_bitmap,
        0, 0, 64, 64,
        SCREEN_W / 2 - 64, SCREEN_H / 2 - 64, 128, 128, 0);
    
    
}

static void destroy(void) {
    destroy_button(&backButton);
    al_destroy_bitmap(win_bitmap);
}

Scene create_win_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "win";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
