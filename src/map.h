#ifndef map_h
#define map_h

#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "utility.h"

// IF YOU WANT TO INCREASE YOUR MAP SIZE OR ENEMY SIZE MAKE SURE TO INCREASE DEFINE VARIABLE BELOW IF IT EXCEED THE MAXIMUM
#define MAX_MAP_ROW 100
#define MAX_MAP_COL 100
#define MAX_ENEMY_SPAWN 100

/*
    Map Tiles type
    Hint : Some type might need a background, to configure it try to modify "get_map_offset(Map * map)"
*/

// block
typedef enum _BLOCK_TYPE{
    FLOOR,
    WALL,
    DOOR_CLOSE,
    DOOR_OPEN,
    HOLE,
    COIN,
    BUTTOM,
    NOTHING
} BLOCK_TYPE;

// coin
typedef enum _COIN_STATUS {
    APPEAR,
    DISAPPEARING,
    DISAPPEAR
} COIN_STATUS;

typedef struct Coin_{
    Point coord;
    COIN_STATUS status;
    int coin_tick;
    int disappearing_tick;
}Coin;

// door
typedef enum _DOOR_STATUS {
    CLOSED,
    OPENNING,
    CLOSING,
    OPEN
} DOOR_STATUS;

typedef struct Door_{
    Point coord;
    DOOR_STATUS status;
    int open_tick;
    int close_tick;
}Door;

typedef enum _BUTTOM_STATUS {
    PRESSED,
    PRESSING,
    UNPRESSED
} BUTTOM_STATUS;

typedef struct Buttom_{
    Point coord;
    BUTTOM_STATUS status;
    int pressing_tick;
    Point linked_door;
}Buttom;

// Map Struct
typedef struct Map_{
    uint8_t ** map; // Map array
    Point ** offset_assets; // Assets
    
    int row, col;
    
    // Map assets
    ALLEGRO_BITMAP* assets;
    ALLEGRO_BITMAP* coin_assets;
    ALLEGRO_BITMAP* door_assets;
    ALLEGRO_BITMAP* buttom_assets;

    // Coin Properties
    ALLEGRO_SAMPLE* coin_audio;
    Coin** CoinList;
    int coin_counter;
    
    //door properties
    Door** DoorList;
    int door_counter;
    
    
    //buttom properties
    Buttom** ButtomList;
    
    
    // Spawn Coordinate
    Point Spawn;
    Point EnemySpawn[MAX_ENEMY_SPAWN];
    char EnemyCode[MAX_ENEMY_SPAWN];
    uint8_t EnemySpawnSize;

} Map;



/*
    MAP FUNCTION
    Feel free to add more if you have some idea or interaction with the map
 */
Map create_map(char * path, uint8_t type); // Create a map based on given file path
void draw_map(Map * map, Point cam); // Draw the map
void update_map(Map * map, Point player_coord, int * total_coins); // Update map : you might want add some parameter here
void destroy_map(Map * map); // Destroy map

bool isWalkable(BLOCK_TYPE block);

#endif /* map_h */
