//
//  shop_window.c
//  panda_knight
//
#include "shop_window.h"
#include "game_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"
#include "shop_window.h"


Item * ItemList;


void shop_init(void){
    ItemList = (Item*)malloc(MAX_ITEM*sizeof(Item));
    
    for(int i=0; i<MAX_ITEM; i++){
        ItemList[i].available = true;
        ItemList[i].affordable = false;
        ItemList[i].hovered = false;
        
    }
    /*
     custom setting: item list
     */
    ItemList[ANOTHER_GUN].cost = 1;//6
    ItemList[ANOTHER_GUN].item_button = button_create(
                                                      TILE_SIZE + 200 * ANOTHER_GUN, SCREEN_H / 2,
                                                      100, 100,
                                                      "sniper.png", "coin_icon.png");
    ItemList[RAINBOW_BALL].cost = 9;
    ItemList[RAINBOW_BALL].item_button = button_create(
                                                       TILE_SIZE + 200 * RAINBOW_BALL, SCREEN_H / 2,
                                                       100, 100,
                                                       "fireball.png", "coin_icon.png");
}

void shop_update(int * coin_obtained){
    for(int i=0; i<MAX_ITEM; i++){
        update_button(&ItemList[i].item_button);
        ItemList[i].affordable = ItemList[i].cost <= *(coin_obtained) ? true: false;
        
        //purchase interaction :

        if (mouseState.buttons && ItemList[i].item_button.hovered && ItemList[i].affordable && ItemList[i].available)
        {
            *(coin_obtained) -= ItemList[i].cost;
            ItemList[i].available = false;
            game_log("Item %d Purchased", i);
            return;
        }
    }
}

void shop_destroy(void){
    for(int i=0; i<MAX_ITEM; i++){
        destroy_button(&ItemList[i].item_button);
    }
    free(ItemList);
}

void draw_shop(int * coin_obtained){
    /*
     transparent background
     */
    // Enable blending
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    // Draw a transparent dark rectangle over the screen
    al_draw_filled_rectangle(
        0, 0, SCREEN_W, SCREEN_H,
        al_map_rgba(0, 0, 0, 128) // Black with 50% opacity (128 out of 255)
    );
    // shop title
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        150,
        ALLEGRO_ALIGN_CENTER,
        "shop"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(232, 84, 53),
        SCREEN_W / 2,
        145,
        ALLEGRO_ALIGN_CENTER,
        "shop"
    );
    al_draw_multiline_textf(P2_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2 , 60,
                            TILE_SIZE*10, al_get_font_line_height(P3_FONT)+5, ALLEGRO_ALIGN_CENTER,
                            "coins :  %d\n", *(coin_obtained));
    al_draw_multiline_text(P3_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2 , SCREEN_H - TILE_SIZE*2,
                           TILE_SIZE*10,
                           al_get_font_line_height(P3_FONT)+5, ALLEGRO_ALIGN_CENTER,
                           "press 1 to change weapon after purching it\n"
                           "price in red -> not affordable"
                           );
    
    /*
     items in the store
     */
    for(int i=0; i<MAX_ITEM; i++){
        if (!ItemList[i].available) continue;
        draw_button(ItemList[i].item_button);
        if (ItemList[i].item_button.hovered)
        {
            int can_afford = ItemList[i].affordable;
            al_draw_textf(
                P1_FONT,
                al_map_rgb(255, can_afford ? 255 : 0, 0),
                ItemList[i].item_button.x + ItemList[i].item_button.w / 2,
                ItemList[i].item_button.y + ItemList[i].item_button.h + 20,
                ALLEGRO_ALIGN_CENTER,
                "%d",ItemList[i].cost
            );
        }
        
    }
    
}
