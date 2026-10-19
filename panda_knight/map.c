#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

/*
    [OFFSET CALCULATOR FUNCTIONS]
      You might want to modify this functions if you are using different assets
      In short, this functions help us to find which asset to draw
 */
static void get_map_offset(Map * map);
static Point get_wall_offset_assets(Map * map, int i, int j);
static Point get_floor_offset_assets(Map * map, int i, int j);
static Point get_hole_offset_assets(Map * map, int i, int j);
static const int offset = 16;

static bool tile_collision(Point player, Point tile_coord);

Map create_map(char * path, uint8_t type){
    Map map;

    memset(&map, 0, sizeof(Map));
    
    FILE * f = fopen(path, "r");
    if(!f){
        game_abort("Can't load map file : %s", path);
    }
    
    game_log("Using %s to load the map", path);
    
    fscanf(f, "%d %d", &map.row, &map.col);
    char ch;
    
    // Map Array
    map.map = (uint8_t **) malloc(map.row * sizeof(uint8_t *));
    for(int i=0; i<map.row; i++){
        map.map[i] = (uint8_t *) malloc(map.col * sizeof(uint8_t));
    }
    
    // Map Offset Resource Pack Array
    map.offset_assets = (Point **) malloc(map.row * sizeof(Point *));
    for(int i=0; i<map.row; i++){
        map.offset_assets[i] = (Point *) malloc(map.col * sizeof(Point));
    }
    // Coin
    map.coin_counter = 0;
    map.CoinList = (Coin **) malloc(map.row * sizeof(Coin *));
    for(int i=0; i<map.row; i++){
        map.CoinList[i] = (Coin *) malloc(map.col * sizeof(Coin));
    }
    
    //Door
    map.door_counter = 0;
    map.DoorList = (Door **) malloc(map.row * sizeof(Door *));
    for(int i=0; i<map.row; i++){
        map.DoorList[i] = (Door *) malloc(map.col * sizeof(Door));
    }
    
    //button
    map.ButtomList = (Buttom **) malloc(map.row * sizeof(Buttom *));
    for(int i=0; i<map.row; i++){
        map.ButtomList[i] = (Buttom *) malloc(map.col * sizeof(Buttom));
    }
    
    Point linked_door = (Point){-1,-1};
    // Scan the map to the array
    for(int i = 0; i < map.row; i++){
        for(int j = 0; j < map.col; j++){
            fscanf(f, " %c", &ch);
            
            map.CoinList[i][j].status = DISAPPEAR;
            map.DoorList[i][j].status = OPEN;
            map.ButtomList[i][j].status = PRESSED;
            
            switch(ch)
            {
                case '#': // Case Wall
                    map.map[i][j] = WALL;
                    break;

                case '.': // Case Floor
                    map.map[i][j] = FLOOR;
                    break;

                case 'P': // Spawn Point
                    map.map[i][j] = FLOOR;
                    map.Spawn = (Point){i, j};
                    break;

                case 'S': // Slime Enemy
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){i, j};
                    break;

                case 'C': // Coins
                    map.map[i][j] = COIN;
                    map.coin_counter++;
                    map.CoinList[i][j].status = APPEAR;
                    map.CoinList[i][j].coin_tick = 0;
                    map.CoinList[i][j].disappearing_tick = 0;
                    break;

                case '_': // Nothing
                    map.map[i][j] = HOLE;
                    break;
                
                case 'D':
                    map.map[i][j] = DOOR_CLOSE;
                    map.door_counter++;
                    map.DoorList[i][j].status = CLOSED;
                    map.DoorList[i][j].open_tick = 0;
                    map.DoorList[i][j].close_tick = 0;
                    linked_door = (Point){i,j};
                    break;
                    
                case 'B':
                    map.map[i][j] = BUTTOM;
                    map.ButtomList[i][j].status = UNPRESSED;
                    map.ButtomList[i][j].pressing_tick = 0;
                    map.ButtomList[i][j].linked_door = linked_door;
                    break;

                default: // If not determined, set it as black
                    map.map[i][j] = NOTHING;
                    break;
            }
        }
    }
    

    
    map.assets = al_load_bitmap("map_packets.png");
    if(!map.assets){
        game_abort("Can't load map assets");
    }

    map.coin_assets = al_load_bitmap("coins.png");
    if (!map.coin_assets) {
        game_abort("Can't load coin assets");
    }
    
    map.door_assets = al_load_bitmap("Door.png");
    if (!map.door_assets) {
        game_abort("Can't load door assets");
    }
    
    map.buttom_assets = al_load_bitmap("Button.png");
    if (!map.buttom_assets) {
        game_abort("Can't load buttom assets");
    }
    
    // load the offset for each tiles type
    get_map_offset(&map);
    
    //coin audio
    map.coin_audio = al_load_sample("coins.ogg");
    if(!map.coin_audio){
        game_abort("Can't load coin audio");
    }

    fclose(f);
    
    return map;
}

void draw_map(Map * map, Point cam){
    // Draw map based on the camera point coordinate
    al_clear_to_color(al_map_rgb(24, 20, 37));

    for(int i=0; i<map->row; i++){
        for(int j=0; j<map->col; j++){
            
            int dy = i * TILE_SIZE - cam.y; // destiny y axis
            int dx = j * TILE_SIZE - cam.x; // destiny x axis
            
            Point offset_asset = map->offset_assets[i][j];
            
            al_draw_scaled_bitmap(map->assets, // image
                                  offset_asset.x, offset_asset.y, 16, 16, // source x, source y, width, height
                                  dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, destiny y, destiny width, destiny height
                                  0 // flag : set 0
                                  );

            switch(map->map[i][j]){
                case COIN:
                    //homework coin ani ok
                    if (map->CoinList[i][j].status == APPEAR){
                        int coin_offset = 16*(map->CoinList[i][j].coin_tick / 8);
                        al_draw_scaled_bitmap(map->coin_assets,
                                              coin_offset, 0, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    else if (map->CoinList[i][j].status == DISAPPEARING){
                        //coin disappear animation
                        int coin_offset = 16*(map->CoinList[i][j].disappearing_tick / 8);
                        al_draw_scaled_bitmap(map->coin_assets,
                                              coin_offset, 16, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }

                    break;
                    
                case DOOR_CLOSE:
                    if (map->DoorList[i][j].status == CLOSED){
                        al_draw_scaled_bitmap(map->door_assets,
                                              0, 0, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    else if (map->DoorList[i][j].status == OPEN){
                        al_draw_scaled_bitmap(map->door_assets,
                                              0, 16, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    else if (map->DoorList[i][j].status == OPENNING){
                        int door_offset = 16*(map->DoorList[i][j].open_tick / 16);
                        al_draw_scaled_bitmap(map->door_assets,
                                              door_offset, 0, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    else if (map->DoorList[i][j].status == CLOSING){
                        int door_offset = 16*(map->DoorList[i][j].close_tick / 16);
                        al_draw_scaled_bitmap(map->door_assets,
                                              door_offset, 16, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    break;
                    
                case DOOR_OPEN:
                    al_draw_scaled_bitmap(map->door_assets,
                                          0, 16, 16, 16,
                                          dx, dy, TILE_SIZE, TILE_SIZE,
                                          0);
                    break;
                    
                case BUTTOM:
                    if (map->ButtomList[i][j].status == UNPRESSED){
                        al_draw_scaled_bitmap(map->buttom_assets,
                                              0, 0, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    else if (map->ButtomList[i][j].status == PRESSED){
                        al_draw_scaled_bitmap(map->buttom_assets,
                                              32, 0, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    else if (map->ButtomList[i][j].status == PRESSING){
                        int buttom_offset = 16*(map->ButtomList[i][j].pressing_tick / 8);
                        al_draw_scaled_bitmap(map->buttom_assets,
                                              buttom_offset, 0, 16, 16,
                                              dx, dy, TILE_SIZE, TILE_SIZE,
                                              0);
                    }
                    break;
                default:
                    break;
            }
        #ifdef DRAW_HITBOX
            al_draw_rectangle(
                dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
                al_map_rgb(0, 255, 0), 1
            );
        #endif
        }
    }
}

void update_map(Map * map, Point player_coord, int* total_coins){
    /*homework: coin collision ok
        Hint: To check if it's collide with object in map, you can use tile_collision function
        e.g. to update the coins if you touch it
    */
    for(int i = 0; i < map->row; i++){
        for(int j = 0; j < map->col; j++){
            // update coin animation tick
            if (map->CoinList[i][j].status == APPEAR)
            {
                map->CoinList[i][j].coin_tick = (map->CoinList[i][j].coin_tick+1) % 64;
            }
            else if (map->CoinList[i][j].status == DISAPPEARING)
            {
                map->CoinList[i][j].disappearing_tick++;
                if (map->CoinList[i][j].disappearing_tick >= 64)
                {
                    map->CoinList[i][j].status = DISAPPEAR;
                }
            }
            // coin collision
            Point target_coord = {j*TILE_SIZE, i*TILE_SIZE};
            if (tile_collision(player_coord, target_coord) && map->CoinList[i][j].status == APPEAR){
                if (!al_play_sample(map->coin_audio, SFX_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
                    game_log("Audio not playing, please increase your RESERVE_SAMPLES variable");
                }
                map->CoinList[i][j].status = DISAPPEARING;
                (*total_coins)++;
            }
            
            // door animation tick update
            if (map->DoorList[i][j].status == OPENNING)
            {
                map->DoorList[i][j].open_tick++;
                if (map->DoorList[i][j].open_tick >= 128)
                {
                    map->DoorList[i][j].status = OPEN;
                    map->map[i][j] = DOOR_OPEN;
                }
            }
            else if (map->DoorList[i][j].status == CLOSING)
            {
                map->DoorList[i][j].close_tick++;
                if (map->DoorList[i][j].close_tick >= 128)
                {
                    map->DoorList[i][j].status = CLOSED;
                }
            }
            //buttom animation tick update
            if (map->ButtomList[i][j].status == PRESSING)
            {
                map->ButtomList[i][j].pressing_tick++;
                if (map->ButtomList[i][j].pressing_tick >= 24)
                {
                    map->ButtomList[i][j].status = PRESSED;
                }
            }
            
            // door collision
            if (tile_collision(player_coord, target_coord) && map->ButtomList[i][j].status == UNPRESSED)
            {
                map->ButtomList[i][j].status = PRESSING;
                
                if (map->ButtomList[i][j].linked_door.x == -1)
                {
                    game_log("No door found linked with this door");
                }
                else
                {
                    int x = map->ButtomList[i][j].linked_door.x;
                    int y = map->ButtomList[i][j].linked_door.y;
                    map->DoorList[x][y].status = OPENNING;
                }
                
            }
            
//                        //if the player leave, door closed
//                        else if (!tile_collision(player_coord, door_coord) && map->DoorList[i][j].status == OPEN)
//                        {
//                            map->DoorList[i][j].status = CLOSED;
//                        }

        }
    }

}

void destroy_map(Map * map){
    for(int i = 0; i < map->row; i++){
        free(map->map[i]);
        free(map->offset_assets[i]);
        free(map->CoinList[i]);
        free(map->DoorList[i]);
        free(map->ButtomList[i]);
    }
    free(map->map);
    free(map->offset_assets);
    free(map->CoinList);
    free(map->DoorList);
    free(map->ButtomList);

    al_destroy_bitmap(map->assets);
    al_destroy_bitmap(map->coin_assets);
    al_destroy_sample(map->coin_audio);
    al_destroy_bitmap(map->door_assets);
    al_destroy_bitmap(map->buttom_assets);
}

bool isWalkable(BLOCK_TYPE block){
    if(block == FLOOR ||  block == COIN || block == BUTTOM || block == DOOR_OPEN) return true;
    return false;
}

/*
    DON'T CHANGE CODE BELOW UNLESS YOU ARE UNDERSTAND WELL
    OR WANT TO CHANGE TO DIFFERENT ASSETS
 */
static bool isWall(Map * map, int i, int j);
//static bool isFloorHole(Map * map, int i, int j);
static bool isFloor(Map * map, int i, int j);
static bool isNothing(Map * map, int i, int j);

static Point get_floor_offset_assets(Map * map, int i, int j){
    bool up = isWall(map, i-1, j);
    bool left = isWall(map, i, j-1);
    bool right = isWall(map, i, j+1);
    bool top_left = isWall(map, i-1, j-1);
    bool top_right = isWall(map, i-1, j+1);
    
    if(up && left && right){
        return (Point){ offset * 12, offset * 3 };
    }
    if(up && left){
        return (Point){ offset * 11, 0 };
    }
    if(up && right){
        return (Point){ offset * 13, 0 };
    }
    if(left){
        if(top_left)
            return (Point){ offset * 11, offset * 1 };
        else
            return (Point){ offset * 15, offset * 1 };
        
    }
    if(right){
        if(top_right)
            return (Point){ offset * 13, offset * 1 };
        else
            return (Point){ offset * 14, offset * 1 };
    }
    if(up){
        return (Point){ offset * 12, 0 };
    }
    if(top_left){
        return (Point){ offset * 11, offset * 3 };
    }
    if(top_right){
        return (Point){ offset * 13, offset * 3 };
    }
    
    return (Point){12 * offset, 4 * offset};
}

// Calculate the offset from the source assets
static Point get_wall_offset_assets(Map * map, int i, int j){
    bool up = isWall(map, i-1, j);
    bool down = isWall(map, i+1, j);
    bool left = isWall(map, i, j-1);
    bool right = isWall(map, i, j+1);

    if(up && down && left && right){
        return (Point){ 3 * offset, 5 * offset };
    }
    if(up && down && right){
        bool left_floor = isFloor(map, i, j - 1);
        bool right_down = isWall(map, i + 1, j + 1);
        bool right_up = isWall(map, i - 1, j + 1);
        if (right_down && right_up)
            return (Point) { 2 * offset, 5 * offset };
        if (left_floor) {
            return (Point) { 1 * offset, 3 * offset };
        }
        else {
            return (Point) { 4 * offset, 5 * offset };
        }
    }
    if (up && down && left) {
        bool right_floor = isFloor(map, i, j + 1);
        bool left_down = isWall(map, i + 1, j - 1);
        bool left_up = isWall(map, i - 1, j - 1);
        if (left_down && left_up)
            return (Point) { 4 * offset, 5 * offset };
        if (right_floor) {
            return (Point) { 1 * offset, 3 * offset };
        }
        else {
            return (Point) { 2 * offset, 5 * offset };
        }
    }
    if(down && right && left){
        bool down_right_wall = isWall(map, i + 1, j + 1);
        bool down_left_wall = isWall(map, i + 1, j - 1);
        bool down_right = isFloor(map, i+1, j+1);
        bool down_left = isFloor(map, i+1, j-1);
        if(down_right_wall && down_left_wall)
            return (Point) { 3 * offset, 4 * offset };
        if((down_right ^ down_left) == 0)
            return (Point){ 8 * offset, 5 * offset };
        if(down_right)
            return (Point){ 4 * offset, 4 * offset };
        if(down_left)
            return (Point){ 2 * offset, 4 * offset };
    }
    if(left && right){
        if(isFloor(map, i+1, j))
            return (Point){ 7 * offset, 5 * offset};
        else
            return (Point){ 7 * offset, 7 * offset};
    }
    if(down && up){
        bool left_ = isFloor(map, i, j-1);
        bool right_ = isFloor(map, i, j+1);
        if((left_ ^ right_) == 0)
            return (Point){ 1 * offset, 3 * offset};
        if(left_)
            return (Point){ 2 * offset, 5 * offset};
        return (Point){ 4 * offset, 5 * offset};
    }
    if(down && right){
        if(isFloor(map, i+1, j+1))
            return (Point){ 4 * offset, 5 * offset };
        else
            return (Point){ 2 * offset, 4 * offset };
    }
    if(down && left){
        if (isFloor(map, i, j + 1) && isFloor(map, i + 1, j - 1))
            return (Point) { 1 * offset, 2 * offset };
        else if(isFloor(map, i+1, j-1))
            return (Point){ 2 * offset, 5 * offset };
        else
            return (Point){ 4 * offset, 4 * offset};
    }
    if(up && right){
        if(isFloor(map, i+1, j))
            return (Point){ 2 * offset, 6 * offset};
        else
            return (Point){ 5 * offset, 6 * offset};
    }
    if(up && left){
        if(isFloor(map, i+1, j))
            return (Point){ 4 * offset, 6 * offset};
        else
            return (Point){ 6 * offset, 6 * offset};
    }
    if(left || right){
        if(isFloor(map, i+1, j))
            return (Point){ 7 * offset, 5 * offset};
        else
            return (Point){ 7 * offset, 7 * offset};
    }
    if(down){
        return (Point){ 1 * offset, 2 * offset};
    }
    if(up){
        return (Point){ 1 * offset, 4 * offset};
    }
    
    return (Point){ 0, 0 };
}

static Point get_hole_offset_assets(Map * map, int i, int j){
    bool up = isNothing(map, i-1, j);
    bool U_Floor = isFloor(map, i-1, j);
    
    if(up){
        return (Point){ 0, 0 };
    }
    if(U_Floor){
        bool UL = isFloor(map, i-1, j-1);
        bool UR = isFloor(map, i-1, j+1);
        if(UL && UR) return (Point){ offset * 13, offset * 11 };
        if(UL) return (Point){ offset * 14, offset * 11 };
        if(UR) return (Point){ offset * 12, offset * 11 };
        return (Point){ offset * 10, offset * 12 };
    }
    return (Point){ 0, 0 };
}

static Point get_nothing_offset_assets(Map * map, int i, int j){
    bool U_Floor = isFloor(map, i-1, j);
    if(U_Floor){
        bool UL = isFloor(map, i-1, j-1);
        bool UR = isFloor(map, i-1, j+1);
        if(UL && UR) return (Point){ offset * 13, offset * 11 };
        if(UL) return (Point){ offset * 14, offset * 11 };
        if(UR) return (Point){ offset * 12, offset * 11 };
        return (Point){ offset * 10, offset * 12 };
    }
    return (Point){ 0, 0 };
}

static void get_map_offset(Map * map){
    // Calculate once to use it later on when draw() function called
    for(int i = 0; i < map->row; i++){
        for(int j = 0; j < map->col; j++){
            switch(map->map[i][j]){
                case WALL:
                    map->offset_assets[i][j] = get_wall_offset_assets(map, i, j);
                    break;
                case FLOOR:
                case COIN:
                    map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                    break;
                case DOOR_CLOSE:
                    map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                    break;

                case HOLE:
                    map->offset_assets[i][j] = get_hole_offset_assets(map, i, j);
                    break;

                case NOTHING:
                default:
                    map->offset_assets[i][j] = (Point){ 0, 0 };
                    break;
            }
        }
    }
}

static bool isWall(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if(map->map[i][j] == WALL) return true;
    return false;
}

static bool isFloor(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if(map->map[i][j] == FLOOR) return true;
    return false;
}

static bool isNothing(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if(map->map[i][j] == NOTHING || map->map[i][j] == HOLE) return true;
    return false;
}

/*
    Collision
 */
static bool tile_collision(Point player, Point tile_coord){
    
    if (player.x < tile_coord.x + TILE_SIZE &&
        player.x + TILE_SIZE > tile_coord.x &&
        player.y < tile_coord.y + TILE_SIZE &&
        player.y + TILE_SIZE > tile_coord.y) {
            return true; // Rectangles collide
    } else {
        return false; // Rectangles do not collide
    }
}
