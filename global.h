#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#define GAME_TERMINATE -1
#define MENU 1
#define ABOUT 2
#define GAME_SCENE 3
#define PAUSE 4
#define VICTORY 1000
#define DEFEAT 2000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// note that you can't assign initial value here!
extern const float FPS;
extern const int WIDTH;
extern const int HEIGHT;
extern bool key_state[ALLEGRO_KEY_MAX];
extern int judge_next_window;
extern int stage_map;
extern int stage_change;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *fps;
extern int highlight;
extern int window;
extern int pause;
extern int volume;
extern bool new_game;
extern ALLEGRO_DISPLAY* display;
extern ALLEGRO_SAMPLE *game_bgm;
extern ALLEGRO_SAMPLE *victory_bgm;
extern ALLEGRO_SAMPLE *defeat_bgm;
extern ALLEGRO_SAMPLE_INSTANCE *game_instance;
extern ALLEGRO_SAMPLE_INSTANCE *victory_instance;
extern ALLEGRO_SAMPLE_INSTANCE *defeat_instance;
extern ALLEGRO_CONFIG *cfg;
extern bool mouse_state;
extern bool stage_state[10];
extern int pause;
extern char score[20];
#endif

