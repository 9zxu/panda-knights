#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static Button SetKeys[4];
static Slide bgm_slide;
static Slide sfx_slide;
static int timer_countdown;

static void init(void) {
    timer_countdown = 7;
    /*
     init button
     */
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "UI_Button.png", "UI_Button_hovered.png");
    
    /*
     init keybindings
     */
    memset(SetKeys, 0, sizeof(Button));
    for(int i=0; i<4; i++)
    {
        SetKeys[i].x = SCREEN_W / 2 + (i-2) * TILE_SIZE;
        SetKeys[i].y = TILE_SIZE*2 - 20; //SCREEN_H / 2 - TILE_SIZE / 2
        SetKeys[i].w = TILE_SIZE;
        SetKeys[i].h = TILE_SIZE;
        SetKeys[i].hovered = false;
    }
    
    /*
     init slide
     */
    bgm_slide = slide_create(SCREEN_W / 2 - 2 * TILE_SIZE, TILE_SIZE * 3.9, 4 * TILE_SIZE, 0.4 * TILE_SIZE, "slider.png", "slider_hovered.png");
    sfx_slide = slide_create(SCREEN_W / 2 - 2 * TILE_SIZE, TILE_SIZE * 5.9, 4 * TILE_SIZE, 0.4 * TILE_SIZE, "slider.png", "slider_hovered.png");
}

static void update(void) {

    update_button(&backButton);
    update_slide(&bgm_slide,&BGM_VOLUME);
    update_slide(&sfx_slide,&SFX_VOLUME);
    
    /*
     back button
     */
    timer_countdown--;
    if (!timer_countdown){
        timer_countdown = 7;
        if (mouseState.buttons && backButton.hovered == true) {
            change_scene(create_menu_scene());
            return;
        }
    }
        
    /*
    custom key
     */
    for(int i=0; i<4; i++)
    {
        update_button(&SetKeys[i]);
        
        // find keybindings
        if(KeyBindings[i].KeySettingMode)
        {
            for (int key = ALLEGRO_KEY_A; key <= ALLEGRO_KEY_DOWN; key++)
            {
                if (keyState[key])
                {
                    // Assign the pressed key to the corresponding action
                    KeyBindings[i].KeyBinding = key;
                    game_log("Key %d bound for direction %d successfully!",key , i);
                    break; // Exit after binding
                }
            }
        }
        
        // update state
        if (mouseState.buttons && SetKeys[i].hovered == true)
        {
            KeyBindings[i].KeySettingMode = true;
            continue;
        }
        
        if ((mouseState.buttons && !SetKeys[i].hovered))
        {
            KeyBindings[i].KeySettingMode = false;
            continue;
        }
    }
}

static void draw(void) {
    
    draw_button(backButton);
    draw_slide(bgm_slide,BGM_VOLUME);
    draw_slide(sfx_slide,SFX_VOLUME);
    
    /*
     button text
     */
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
    
    /*
     custom keys
     */
    
    // draw key setting block
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        TILE_SIZE,
        ALLEGRO_ALIGN_CENTER,
        "up down left right"
    );
    
    for(int i=0; i<4; i++)
    {

        al_draw_rectangle(
                          SetKeys[i].x, SetKeys[i].y,
                          SetKeys[i].x + SetKeys[i].w, SetKeys[i].y + SetKeys[i].h,
                          SetKeys[i].hovered ? al_map_rgb(245, 205, 51) : al_map_rgb(252, 244, 49),
                          1 ); // thickness
        
        al_draw_textf(
                     P2_FONT,
                     al_map_rgb(225, KeyBindings[i].KeySettingMode? 0:255, SetKeys[i].hovered ? 0:255),
                     SetKeys[i].x + TILE_SIZE / 2 ,
                     SetKeys[i].y + TILE_SIZE / 2 - 15,
                     ALLEGRO_ALIGN_CENTER,
                      "%d", KeyBindings[i].KeyBinding
                     );
        
    }
    
    /*
     sliders text
     */
    al_draw_textf(
                 P2_FONT,
                 al_map_rgb(225, 225, 225),
                 SCREEN_W / 2,
                 TILE_SIZE*3,
                 ALLEGRO_ALIGN_CENTER,
                  "BGM Volume: %.f",BGM_VOLUME*100
                 );
    al_draw_textf(
                 P2_FONT,
                 al_map_rgb(225, 225, 225),
                 SCREEN_W / 2,
                 TILE_SIZE*5,
                 ALLEGRO_ALIGN_CENTER,
                  "SFX Volume: %.f", SFX_VOLUME*100
                 );
    /*
     hint text
     */
    al_draw_multiline_text(P3_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2 , TILE_SIZE*7, TILE_SIZE*10,
            al_get_font_line_height(P3_FONT)+5, ALLEGRO_ALIGN_CENTER,
            "KEYS are shown as keycodes: WSAD as default\n"
            "click the box to set custom keys\n"
            "drag on the slider to change VOLUME\n"
            "click BACK button to adapt volume setting\n"
            "press ESC in game will jump to setting page\n"
            "kill all the enemies to WIN");
}

static void destroy(void) {
    destroy_button(&backButton);
}

Scene create_setting_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "setting";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
