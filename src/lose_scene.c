//
//  lose_scene.c
//  panda_knight
//

#include "lose_scene.h"
#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static ALLEGRO_BITMAP* lose_bitmap;
static Button backButton;
static int timer_countdown;


static void init(void) {
    timer_countdown = 120;
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "UI_Button.png", "UI_Button_hovered.png");
    lose_bitmap = al_load_bitmap("panda_lose.png");
    if (!lose_bitmap) {
        game_abort("Failed to load lose bitmap");
    }
}

static void update(void) {

    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
    }
    
    timer_countdown--;
    if (timer_countdown == 0) {
        change_scene(create_menu_scene());
    }

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
    al_draw_scaled_bitmap(lose_bitmap,
        0, 0, 64, 64,
        SCREEN_W / 2 - 64, SCREEN_H / 2 - 64, 128, 128, 0);
}

static void destroy(void) {
    destroy_button(&backButton);
}

Scene create_lose_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "lose";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}

