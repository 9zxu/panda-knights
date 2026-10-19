#include <string.h>
#include "game_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"
#include "win_scene.h"
#include "lose_scene.h"
#include "setting_scene.h"
#include "shop_window.h"
#include "leaderboard_window.h"

#include <math.h>

//assets
Player player;
Map map;
enemyNode * enemyList;
BulletNode * bulletList;

// coin
//ALLEGRO_BITMAP * heart_bitmap, * coin_bitmap;
int coins_obtained;

// shop
static Button shop_button;
// pause
bool isPause;
static int timer_countdown;

// Weapon
Weapon WeaponList[2]; // weapon possessed at most 2 at once
int current_weapon_idx;
int weapon_count;
static int weapon_timer_countdown;

//count time elapse
double start_time;
double end_time;
double elapsed_time;

static void init(void){
    
    start_time = al_get_time();
    
    initEnemy();
    
    map = create_map("map0.txt", 0);
    
    player = create_player("panda2.png", map.Spawn.x, map.Spawn.y);

    enemyList = createEnemyList();
    bulletList = createBulletList();
    
    // default weapon
    current_weapon_idx = 0;
    weapon_count = 1;
    WeaponList[0] = create_weapon("guns.png", "yellow_bullet.png", 16, 8, 10);
    weapon_timer_countdown = 8;

    
    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    change_bgm("game_bgm.ogg");
    
    coins_obtained = 0;

    //shop button
    shop_button = button_create(SCREEN_W - 150, 50, 100, 100, "UI_Button.png", "UI_Button_hovered.png");
    isPause = false;
    timer_countdown = 8;
    shop_init();
  
}

static void update(void){
    
    /*
     Shop
     */
    timer_countdown--;
    if (!timer_countdown){
        timer_countdown = 8;
        if (shop_button.hovered && mouseState.buttons){
            isPause = true - isPause;
            if (isPause) game_log("entering shop");
            else game_log("leaving shop");
        }
    }
    
    if (isPause){
        //shop_update
        update_button(&shop_button);
        shop_update(&coins_obtained);
        return;
    }
    /*
     ESC to setting scene
     */
    if (keyState[ALLEGRO_KEY_ESCAPE]){
        change_scene(create_setting_scene());
        return;
    }
    /*
        [TODO Homework]ok
        
        Change the scene if winning/losing to win/lose scene
    */
    if (player.status == PLAYER_DEAD){
        change_scene(create_lose_scene());
        return;
    }
    else if (!enemyList->next){
        /*
         update leaderboard
         */
        end_time = al_get_time();
        elapsed_time = end_time-start_time;
        leaderboard_update(coins_obtained, player.health, elapsed_time);
        
        change_scene(create_win_scene());
        return;
    }
    
    update_player(&player, &map);
    if (player.status == PLAYER_DYING) return;

    Point Camera;
    /*
        [TODO HACKATHON 1-3]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Adjust it based on player position variable, then subtract it with half of the gameplay screen
    */
    Camera = (Point){ 0, 0 };
    Camera.x = player.coord.x - SCREEN_W/2;
    Camera.y = player.coord.y - SCREEN_H/2;
    
    //update weapon
    weapon_timer_countdown--;
    if (!weapon_timer_countdown){
        weapon_timer_countdown = 8;
        
        if (!ItemList[ANOTHER_GUN].available && keyState[ALLEGRO_KEY_1]){
            // if you buy another gun, you can change the weapon by pressing 1
            WeaponList[1] = create_weapon("sniper.png", "coin_icon.png", 4, 16, 4);
            current_weapon_idx = 1-current_weapon_idx;
            game_log("change to weapon %d",current_weapon_idx);
        }
    }
    update_weapon(&WeaponList[current_weapon_idx], bulletList, player.coord, Camera);
    
    updateEnemyList(enemyList, &map, &player);
    updateBulletList(bulletList, enemyList, &map);
    update_map(&map, player.coord, &coins_obtained);
    update_button(&shop_button);
}

static void draw(void){
    
    
    
    Point Camera;
    /*
        [TODO HACKATHON 1-4]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Just copy from the [TODO HACKATHON 1-3]
    */
    Camera = (Point){ 0, 0 };
    Camera.x = player.coord.x - SCREEN_W/2;
    Camera.y = player.coord.y - SCREEN_H/2;
    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);
    if (player.status != PLAYER_DYING && player.status != PLAYER_DEAD) draw_weapon(&WeaponList[current_weapon_idx], player.coord, Camera);

    /*
        [TODO Homework]ok
        
        Draw the UI of Health and Total Coins
    */
    
//    int font_height = 64; //P2
    
    al_draw_scaled_bitmap(heart_bitmap,
                          0, 0, 32, 32,
                          0, TILE_SIZE-15,TILE_SIZE, TILE_SIZE,
                          0); // flip
    al_draw_scaled_bitmap(coin_bitmap,
                          0, 0, 16, 16,
                          0, TILE_SIZE+20,TILE_SIZE, TILE_SIZE,
                          0); //flip
    
    al_draw_multiline_textf(P2_FONT, al_map_rgb(255, 255, 255), TILE_SIZE, TILE_SIZE, 300,
            al_get_font_line_height(P2_FONT), 0,
            "%02d\n"
            "%02d",
            player.health, coins_obtained);
    /*
     shop
     */
    if (isPause) draw_shop(&coins_obtained);
    
    draw_button(shop_button);
    al_draw_text(
        P1_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W - 100,
        100,
        ALLEGRO_ALIGN_CENTER,
        "shop"
    );
    al_draw_text(
        P1_FONT,
        al_map_rgb(255, 0, 255),
        SCREEN_W - 100,
        95,
        ALLEGRO_ALIGN_CENTER,
        "shop"
    );
}

static void destroy(void){
    delete_player(&player);
    for(int i=0; i<weapon_count; i++) delete_weapon(&WeaponList[i]);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
    terminateEnemy();
//    al_destroy_bitmap(heart_bitmap);
//    al_destroy_bitmap(coin_bitmap);
    /*
     shop
     */
    destroy_button(&shop_button);
    shop_destroy();
}


Scene create_game_scene(void){
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    
    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    
    return scene;
}


