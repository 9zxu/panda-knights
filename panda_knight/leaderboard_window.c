//
//  leaderboard_window.c
//  panda_knight
//

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "leaderboard_window.h"
#include "game_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"
#include "shop_window.h"

//Leader current_record;
Leader leader[4];
Leader current_player;

int leader_count;

static Button name_button;
bool name_setting_mode;

int compare_leaders(const void *a, const void *b);

void leaderboard_init(void){
    
    for(int i=0; i<4; i++){
        leader[i].username = calloc(10,sizeof(char));
        strcpy(leader[i].username, "no_name");
        leader[i].coin_obtained = 0;
        leader[i].health_left = 0;
        leader[i].time_elapsed = 0;
        leader[i].input_idx = 0;
    }
    current_player.username = calloc(10,sizeof(char));
    strcpy(current_player.username, "no_name");
    current_player.coin_obtained = 0;
    current_player.health_left = 0;
    current_player.time_elapsed = 0;
    current_player.input_idx = 0;
    
    /*
     load data and close file
     */
    game_log("Current working directory: %s\n", getcwd(NULL, 0));
    
    FILE * leaderboard_file;
    leaderboard_file = fopen("leaderboard.txt", "r");
    if (!leaderboard_file) {
        game_log("Error: %s\n", strerror(errno));
        return;
    }
    
    for(int i=0; i<3; i++){
        fscanf(leaderboard_file, "%s %d %d %lf",
               leader[i].username,
               &leader[i].coin_obtained,
               &leader[i].health_left,
               &leader[i].time_elapsed);
    }
    
    fclose(leaderboard_file);

    return;
}

void leaderboard_update(int coin_obtained, int health_left, double time_elapsed){
    game_log("updaing leaderboard with %d %d %.2f" ,coin_obtained, health_left, time_elapsed);
    
    //set value
    current_player.coin_obtained = coin_obtained;
    current_player.health_left = health_left;
    current_player.time_elapsed = time_elapsed;
    
    leader[3] = current_player;
    //sort
    qsort(leader, 4, sizeof(Leader), compare_leaders);

    return;
}

void leaderboard_destroy(void){
    //save file
    game_log("saveing leaderboard...");
    
    FILE * leaderboard_file;
    
    // get current working directory
    game_log("Current working directory: %s\n", getcwd(NULL, 0));
    
    leaderboard_file = fopen("leaderboard.txt","w");
    if (!leaderboard_file){
        game_abort("Error: %s\n", strerror(errno));
    }
    
    //save record
    for(int i=0; i<4; i++){
        fprintf(leaderboard_file, "%s %2d %2d %.2f\n",
                leader[i].username, leader[i].coin_obtained, leader[i].health_left, leader[i].time_elapsed);
    }
    fclose(leaderboard_file);
    
    // free the name
    for(int i=0; i<4; i++){
        free(leader[i].username);
    }
    
//    free(current_player.username);
    return;
}

void draw_leaderboard(void){
    
    /*
     transparent background
     */
    // Draw a transparent dark rectangle over the screen
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_rectangle(
        0, 0, SCREEN_W, SCREEN_H,
        al_map_rgba(0, 0, 0, 255*0.8) // Black with 50% opacity (128 out of 255)
    );
    //title
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        150,
        ALLEGRO_ALIGN_CENTER,
        "LEADERBOARD"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(245, 205, 51),
        SCREEN_W / 2,
        145,
        ALLEGRO_ALIGN_CENTER,
        "LEADERBOARD"
    );
    
   
    
    al_draw_multiline_text(P3_FONT, al_map_rgb(255, 0, 255), SCREEN_W / 2 , SCREEN_H / 2 + 2*TILE_SIZE,
                           TILE_SIZE*10,
                           al_get_font_line_height(P3_FONT)+5, ALLEGRO_ALIGN_CENTER,
                           "sorted by : coins + health - time\n"
                           "username should less than 9 characters\n"
                           "no space are allowed"
                           );
    al_draw_scaled_bitmap(heart_bitmap,
                          0, 0, 32, 32,
                          500, 235,
                          TILE_SIZE, TILE_SIZE,
                          0); //flip
    al_draw_scaled_bitmap(coin_bitmap,
                          0, 0, 16, 16,
                          400 , 230,
                          TILE_SIZE, TILE_SIZE,
                          0); // flip
    al_draw_text(P2_FONT, al_map_rgb(199, 207, 221), SCREEN_W - 100, 250,
                  ALLEGRO_ALIGN_RIGHT,
                  "time");
    /*
     leaderboard content
     */
    // leader_count
    
    
    for(int i=0; i<3; i++){
        
        al_draw_textf(P2_FONT, al_map_rgb(255,255,255), 100, 300+i*40, ALLEGRO_ALIGN_LEFT,
                      "%d  %s",i+1,leader[i].username);

        al_draw_textf(P2_FONT, al_map_rgb(255,255,255), 450, 300+i*40,
                      ALLEGRO_ALIGN_RIGHT,
                      "%02d",leader[i].coin_obtained);
        al_draw_textf(P2_FONT, al_map_rgb(255,255,255), 550, 300+i*40,
                      ALLEGRO_ALIGN_RIGHT,
                      "%02d",leader[i].health_left);
        al_draw_textf(P2_FONT, al_map_rgb(255,255,255), SCREEN_W - 100 , 300+i*40,
                      ALLEGRO_ALIGN_RIGHT,
                      "%.2f",leader[i].time_elapsed);
        
        
    }
    //last time gameplay
    al_draw_textf(P2_FONT, al_map_rgb(199, 207, 221), 100, 300+4*40, ALLEGRO_ALIGN_LEFT,
                  "%d  %s",0,current_player.username);

    al_draw_textf(P2_FONT, al_map_rgb(199, 207, 221), 450, 300+4*40,
                  ALLEGRO_ALIGN_RIGHT,
                  "%02d",current_player.coin_obtained);
    al_draw_textf(P2_FONT, al_map_rgb(199, 207, 221), 550, 300+4*40,
                  ALLEGRO_ALIGN_RIGHT,
                  "%02d",current_player.health_left);
    al_draw_textf(P2_FONT, al_map_rgb(199, 207, 221), SCREEN_W - 100 , 300+4*40,
                  ALLEGRO_ALIGN_RIGHT,
                  "%.2f",current_player.time_elapsed);
    
}

/*
 qsort compare function
 */
int compare_leaders(const void *a, const void *b) {
    const Leader *leaderA = (const Leader *)a;
    const Leader *leaderB = (const Leader *)b;

    // Calculate the sorting metric: coin + health - time_elapsed
    double scoreA = leaderA->coin_obtained + leaderA->health_left - leaderA->time_elapsed*0.3;
    double scoreB = leaderB->coin_obtained + leaderB->health_left - leaderB->time_elapsed*0.3;

    // Sort in descending order: larger score comes first
    if (scoreA > scoreB) return -1;
    if (scoreA < scoreB) return 1;
    return 0;
}



/*
 name setting
 */

void init_name(void){
    name_button.x = 250;
    name_button.y = 375;
    name_button.w = TILE_SIZE*5;
    name_button.h = 40;
    name_button.hovered = false;
    name_setting_mode = false;
}

void update_name(void){
    
    update_button(&name_button);
    if (mouseState.buttons && name_button.hovered == true)
    {
        name_setting_mode = true;
//        current_player.input_idx = 0;
    }
    
    if ((mouseState.buttons && !name_button.hovered))
    {
        name_setting_mode = false;
        if (strlen(current_player.username)==0){
            strcpy(current_player.username,"no_name");
        }
    }
}

void draw_name(void){
    al_draw_rectangle(
                      name_button.x, name_button.y,
                      name_button.x + name_button.w, name_button.y + name_button.h,
                      al_map_rgb(225, 255, name_setting_mode ? 0:255),
                      1 ); // thickness
    //name_button.hovered ? al_map_rgb(245, 205, 51) : al_map_rgb(252, 244, 49)
    al_draw_textf(
                  P2_FONT,
                  al_map_rgb(225, 255, 0),
                  name_button.x + name_button.w / 2 ,
                  name_button.y,
                  ALLEGRO_ALIGN_CENTER,
                  "%s", current_player.username
                  );
}

void destroy_name(void) {
    destroy_button(&name_button);
}
