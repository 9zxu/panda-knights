#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "loading_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "leaderboard_window.h"

static Button settingButton;
static Button leaderboard_button;
bool ViewLeaderboard;
static int timer_countdown;
int animation_tick;

static void init(void) {
    settingButton = button_create(SCREEN_W / 2 - 200, 600, 400, 100, "UI_Button.png", "UI_Button_hovered.png");
    
    change_bgm("menu_bgm.ogg");
    
    /*
     leaderboard button
     */
    leaderboard_button = button_create(SCREEN_W - 150, 600, 32, 32, "trophy.png", "trophy.png");
    ViewLeaderboard = false;
    timer_countdown = 8;
    init_name();
    animation_tick = 0;
}

static void update(void) {
    
    /*
     leaderboatd viewing
     */
    
    timer_countdown--;
    if (!timer_countdown){
        timer_countdown = 8;
        if (leaderboard_button.hovered && mouseState.buttons){
            ViewLeaderboard = true - ViewLeaderboard;
            if (ViewLeaderboard) game_log("entering leaderboard");
            else game_log("leaving leaderboard");
        }
    }
    
    if (ViewLeaderboard){
        //shop_update
        update_button(&leaderboard_button);
        return;
    }
    
    //update leaderboard button
    update_button(&leaderboard_button);
    animation_tick++;
    if (animation_tick > 80) animation_tick = 0;
    
    // update name input
    update_name();
    
    /*
     setting scene
     */
    update_button(&settingButton);
    if (keyState[ALLEGRO_KEY_ENTER]) {
        change_scene(create_loading_scene());
    }

    /*
        [TODO HACKATHON 4-3] 
        
        Change scene to setting scene when the button is pressed
    */
    if (mouseState.buttons && settingButton.hovered == true){
        change_scene(create_setting_scene());
    }
    
    
    
}

static void draw(void) {
    // Title Text
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        225,
        ALLEGRO_ALIGN_CENTER,
        "PANDA KNIGHTS"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(199, 207, 221),
        SCREEN_W / 2,
        220,
        ALLEGRO_ALIGN_CENTER,
        "PANDA KNIGHTS"
    );

    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        500,
        ALLEGRO_ALIGN_CENTER,
        "PRESS  [ ENTER ]  TO PLAY"
    );
    
    al_draw_text(
        P3_FONT,
        al_map_rgb(199, 207, 221),
        SCREEN_W / 2,
        SCREEN_H - 70,
        ALLEGRO_ALIGN_CENTER,
        "first-time play please refer to setting page"
    );

    // button
    draw_button(settingButton);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        600 + 28 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        600 + 31 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );
    
    /*
     leaderboard button
     */
    draw_name();
    
    if (ViewLeaderboard){
        draw_leaderboard();
    }
    
//    draw_button(leaderboard_button);
    
    al_draw_text(
        P3_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W - 115,
        675,
        ALLEGRO_ALIGN_CENTER,
        "LDB"
    );
    
    // draw leaderboard button animation
    int offset = 32 * (animation_tick / 8);
    al_draw_tinted_scaled_rotated_bitmap_region(leaderboard_button.default_img,
                                                offset, 0, 32, 32,
                                                al_map_rgb(255, 255, 255),
                                                0, 0, leaderboard_button.x, leaderboard_button.y,
                                                TILE_SIZE / leaderboard_button.w, TILE_SIZE / leaderboard_button.h, //scale
                                                0, 0);
    
}

static void destroy(void) {
    destroy_button(&settingButton);
    destroy_button(&leaderboard_button);
    destroy_name();
}


Scene create_menu_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "menu";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}

