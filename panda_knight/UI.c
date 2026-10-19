#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string.h>
#include "UI.h"
#include "utility.h"
#include "game.h"

static bool mouse_in_rect(Point mouse, RecArea rect);

Button button_create(int x, int y, int w, int h, const char* default_image_path, const char* hovered_image_path) {

	Button button;
	memset(&button, 0, sizeof(Button));

	button.default_img = al_load_bitmap(default_image_path);
	button.hovered_img = al_load_bitmap(hovered_image_path);

	if (!button.default_img) {
        game_log("failed loading button image %s", default_image_path);
	}
    if (!button.hovered_img) {
        game_log("failed loading button image %s", hovered_image_path);
    }

	button.x = x;
	button.y = y;
	button.w = w;
	button.h = h;
    
	button.hovered = false;
    button.clicked = false;

	return button;
}

void draw_button(Button button) {
	ALLEGRO_BITMAP* _img = button.hovered ? button.hovered_img : button.default_img;
	al_draw_scaled_bitmap(
		_img,
		0, 0,
		al_get_bitmap_width(_img), al_get_bitmap_height(_img),
		button.x, button.y,
		button.w, button.h, 0
	);
}

void update_button(Button* button) {
	Point mouse = { mouseState.x, mouseState.y };
	RecArea rect = { button->x, button->y, button->w, button->h };
	/*
		[TODO Hackathon 4-2] 
		
		Using function you completed before,
		determine the button if it's hovered or not (button->hovered)
	*/
    button->hovered = mouse_in_rect(mouse, rect);
}

void destroy_button(Button* button) {
	al_destroy_bitmap(button->default_img);
	al_destroy_bitmap(button->hovered_img);
}


static bool mouse_in_rect(Point mouse, RecArea rect) {
	/*
		[TODO Hackathon 4-1] 
		
		Return true if mouse is inside the rectangle
	*/
    if (mouse.x >= rect.x && mouse.y >= rect.y && mouse.x <= rect.x+rect.w && mouse.y <= rect.y+rect.h) return true;

	return false;
}



Slide slide_create(int x, int y, int w, int h, const char* default_image_path, const char* hovered_image_path) {

    Slide slide;
    memset(&slide, 0, sizeof(Slide));

    slide.default_img = al_load_bitmap(default_image_path);
    slide.hovered_img = al_load_bitmap(hovered_image_path);

    if (!slide.default_img) {
        game_log("failed loading slide image %s", default_image_path);
    }
    if (!slide.hovered_img) {
        game_log("failed loading slide image %s", hovered_image_path);
    }

    slide.x = x;
    slide.y = y;
    slide.w = w;
    slide.h = h;

    slide.hovered = false;
    slide.dragged = false;

    return slide;
}

void draw_slide(Slide slide,float volume_ratio) {
    ALLEGRO_BITMAP* _img = slide.hovered ? slide.hovered_img : slide.default_img;
    al_draw_scaled_bitmap(
        _img,
        0, 0,
        al_get_bitmap_width(_img), al_get_bitmap_height(_img),
        slide.x, slide.y,
        slide.w * volume_ratio, slide.h, 0
    );
    al_draw_rectangle(
                      slide.x, slide.y,
                      slide.x + slide.w, slide.y + slide.h,
                      slide.hovered ? al_map_rgb(245, 205, 51) : al_map_rgb(252, 244, 49),
                      1 ); // thickness
}

void update_slide(Slide* slide ,float * volume_ratio) {
    Point mouse = { mouseState.x, mouseState.y };
    RecArea rect = { slide->x, slide->y, slide->w, slide->h };
    slide->hovered = mouse_in_rect(mouse, rect);
    
    if (mouseState.buttons && slide->hovered){
        *(volume_ratio) = (mouseState.x - slide->x) / (float)slide->w;
    }

}

void destroy_slide(Slide* slide) {
    al_destroy_bitmap(slide->default_img);
    al_destroy_bitmap(slide->hovered_img);
}
