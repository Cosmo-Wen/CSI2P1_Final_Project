#include "global.h"
#include "character.h"

void menu_init();
int menu_process(ALLEGRO_EVENT event);
void menu_draw();
void menu_destroy();

void game_scene_init();
int game_scene_process(ALLEGRO_EVENT event);
void game_scene_draw();
void game_scene_destroy();

void about_scene_init();
void about_process(ALLEGRO_EVENT event);
void about_scene_draw();
void about_scene_destroy();


void victory_init();
void victory_draw();
int victory_process(ALLEGRO_EVENT event);
void victory_destroy();

void defeat_init();
void defeat_draw();
int defeat_process(ALLEGRO_EVENT event);
void defeat_destroy();

void hl_draw(int pos);

void pause_init();
int pause_process(ALLEGRO_EVENT event);
void pause_destroy();
void pause_draw();
