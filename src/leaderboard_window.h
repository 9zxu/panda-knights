//
//  leaderboard_window.h
//  panda_knight
//

#ifndef leaderboard_window_h
#define leaderboard_window_h

#include "UI.h"
#include "utility.h"

typedef struct _Leader{
    char * username;
    int input_idx;
    int coin_obtained;
    int health_left;
    double time_elapsed;
    
}Leader;

extern Leader leader[4];
extern bool name_setting_mode;
extern Leader current_player;

void leaderboard_init(void);
void leaderboard_update(int coin_obtained, int health_left, double time_elapsed);
void leaderboard_destroy(void);
void draw_leaderboard(void);

void init_name(void);
void update_name(void);
void draw_name(void);
void destroy_name(void);

#endif /* leaderboard_window_h */
