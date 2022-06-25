#include "global.h"

// variables for global usage
const float FPS = 60.0;
const int WIDTH = 800;
const int HEIGHT = 1000;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *fps = NULL;
bool key_state[ALLEGRO_KEY_MAX] = {false};
int judge_next_window = 0;
int stage_map = 0;
int stage_change = 0;
int volume = 5;
bool mouse_state = false;
bool stage_state[10] = {false};
bool new_game = false;
ALLEGRO_CONFIG *cfg = NULL;
int pause = 0;
char score[20];
