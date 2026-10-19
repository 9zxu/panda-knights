//
//  shop_window.h
//  panda_knight
//

#ifndef shop_window_h
#define shop_window_h

#include "UI.h"
#include "utility.h"

#define MAX_ITEM 2

typedef enum _ItemName{
    ANOTHER_GUN,
    RAINBOW_BALL
}ItemName;

typedef struct _Item{
    bool available;
    bool affordable;
    int cost;
    
    Button item_button;
    bool hovered;
}Item;

extern Item * ItemList;

void shop_init(void);
void shop_update(int *);
void shop_destroy(void);
void draw_shop(int *);

#endif /* shop_window_h */
