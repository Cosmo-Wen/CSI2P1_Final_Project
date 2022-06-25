#include "scene.h"

ALLEGRO_FONT *font1 = NULL;									// 12 px font
ALLEGRO_FONT *font2 = NULL;									// 32 px font
ALLEGRO_BITMAP *background = NULL;							// background picture
ALLEGRO_BITMAP *title_screen = NULL;                        // title screen
ALLEGRO_BITMAP *move_icon, *shoot_icon;                     // icons
ALLEGRO_BITMAP *skill1_icon, *skill2_icon;                  // icons
ALLEGRO_BITMAP *trophie;									// victory
ALLEGRO_BITMAP *white_flag;									// defeat
int highlight = 0;											// highlight position
int first_time;												// draw pause screen only once

// initialize menu
void menu_init()
{
	// load fonts
	font1 = al_load_ttf_font("./font/pirulen.ttf", 20, 0);
	font2 = al_load_ttf_font("./font/pirulen.ttf", 32, 0);
	// load title screen
	title_screen = al_load_bitmap("./image/title_screen.png");
}

// actions in menu
int menu_process(ALLEGRO_EVENT event)
{
	int pos_x, pos_y;										// mouse position

	// set mouse position
	pos_x = event.mouse.x;
	pos_y = event.mouse.y;

	if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
		// highlight New Game/Continue
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 90 && pos_y < HEIGHT / 2 + 130)
			highlight = 2;
		// highlight About
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 140 && pos_y < HEIGHT / 2 + 180)
			highlight = 3;
		// highlight exit
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			highlight = 4;
		// reset highlight
		else
			highlight = 0;
	}
	// actions at menu
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		// go to game
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 90 && pos_y < HEIGHT / 2 + 130)
			judge_next_window = GAME_SCENE;
		// go to about
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 140 && pos_y < HEIGHT / 2 + 180)
			judge_next_window = ABOUT;
		// go to exit
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			return 0;
	}

	return 1;
}

// draw menu
void menu_draw()
{
	// clear to black
	al_clear_to_color(al_map_rgb(0, 0, 0));
	// button blocks
	al_draw_bitmap(title_screen, 0, 0, 0);
	al_draw_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 90, WIDTH / 2 + 150, HEIGHT / 2 + 130, al_map_rgb(255, 255, 255), 0);
	al_draw_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 140, WIDTH / 2 + 150, HEIGHT / 2 + 180, al_map_rgb(255, 255, 255), 0);
	al_draw_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 190, WIDTH / 2 + 150, HEIGHT / 2 + 230, al_map_rgb(255, 255, 255), 0);
	// title word
	al_draw_text(font2, al_map_rgb(0, 0, 0), WIDTH / 2, HEIGHT / 2 - 100 , ALLEGRO_ALIGN_CENTRE, "SOUL TONE");
	// button words
	if (new_game)
		al_draw_text(font1, al_map_rgb(0, 0, 0), WIDTH / 2, HEIGHT / 2 + 95 , ALLEGRO_ALIGN_CENTRE, "NEW GAME");
	else
		al_draw_text(font1, al_map_rgb(0, 0, 0), WIDTH / 2, HEIGHT / 2 + 95 , ALLEGRO_ALIGN_CENTRE, "CONTINUE");
	al_draw_text(font1, al_map_rgb(0, 0, 0), WIDTH / 2, HEIGHT / 2 + 145 , ALLEGRO_ALIGN_CENTRE, "ABOUT");
	al_draw_text(font1, al_map_rgb(0, 0, 0), WIDTH / 2, HEIGHT / 2 + 195 , ALLEGRO_ALIGN_CENTRE, "EXIT");
	// update display
	al_flip_display();
}

void menu_destroy()
{
	// destroy fonts
	al_destroy_font(font1);
	al_destroy_font(font2);
	// destroy title screen
	al_destroy_bitmap(title_screen);
}

// function of game_scene
void game_scene_init()
{
	char temp[50];
	// initialize character
	character_init();
	// load background
	sprintf(temp, "./image/stage_%d.jpg", stage_map);
	background = al_load_bitmap(temp);
	// load fonts
	font1 = al_load_ttf_font("./font/pirulen.ttf", 20, 0);
	font2 = al_load_ttf_font("./font/pirulen.ttf", 32, 0);
}

int game_scene_process(ALLEGRO_EVENT event)
{
	char temp[50];
	int pos_x, pos_y;										// mouse position

	// set mouse position
	pos_x = event.mouse.x;
	pos_y = event.mouse.y;

	// keyboard actions
	if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
		// set whatever key is pressed to be true
		key_state[event.keyboard.keycode] = true;
		// pause and unpause
		if (key_state[ALLEGRO_KEY_ESCAPE]) {
			judge_next_window = PAUSE;
		}
		// volume up
		else if (key_state[ALLEGRO_KEY_UP] && pause)
			volume++;
		// volume down
		else if (key_state[ALLEGRO_KEY_DOWN] && pause)
			volume--;
		// volume lower limit
		if (volume < 0)
			volume = 0;
		// volume upper limit
		else if (volume > 5)
			volume = 5;
		// set volume
		al_set_sample_instance_gain(game_instance, 1 * volume / 10.0);

	}
	// key let go
	else if (event.type == ALLEGRO_EVENT_KEY_UP)
		key_state[event.keyboard.keycode] = false;
	// actions at menu
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && pause) {
		// go to game
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 90 && pos_y < HEIGHT / 2 + 130)
			pause = 0;
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 140 && pos_y < HEIGHT / 2 + 180)
			judge_next_window = MENU;
		// go to exit
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			return 0;
	}
	// change background with room number
	if (stage_change) {
		sprintf(temp, "./image/stage_%d.jpg", stage_map);
		background = al_load_bitmap(temp);
		stage_change--;
	}
	return 1;
}

void game_scene_draw()
{
	// draw backgruond
	al_draw_bitmap(background, 0, 200, 0);
	al_draw_filled_rectangle(0, 0, 800, 200, al_map_rgb(0, 0, 0));
	// draw character
	character_draw();
	// draw score
	al_draw_textf(font2, al_map_rgb(255, 255, 255), 650, 120, ALLEGRO_ALIGN_CENTRE, "SCORE: %s", score);
	// update display
	al_flip_display();
}

void game_scene_destroy()
{
	// destroy background
	al_destroy_bitmap(background);
	// destroy character
	character_destory();
	// stop music
	al_stop_sample_instance(game_instance);
	al_destroy_sample_instance(game_instance);
}


void about_scene_init()
{
	// load fonts
	font1 = al_load_ttf_font("./font/pirulen.ttf", 20, 0);
	font2 = al_load_ttf_font("./font/pirulen.ttf", 32, 0);
	// load icons
	move_icon = al_load_bitmap("./image/move_icon.png");
	shoot_icon = al_load_bitmap("./image/shoot_icon.png");
	skill1_icon = al_load_bitmap("./image/skill1_icon.png");
	skill2_icon = al_load_bitmap("./image/skill2_icon.png");
}

void about_process(ALLEGRO_EVENT event)
{
	int pos_x, pos_y;										// mouse position

	// set mouse position
	pos_x = event.mouse.x;
	pos_y = event.mouse.y;
	// go to menu
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 395 && pos_y < HEIGHT / 2 + 435)
			judge_next_window = MENU;
	}
	if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
		// highlight back
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 395 && pos_y < HEIGHT / 2 + 435)
			highlight = 5;
		// reset highlight
		else
			highlight = 0;
	}
	// volume change
	if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
		key_state[event.keyboard.keycode] = true;
		if (key_state[ALLEGRO_KEY_UP])
			volume++;
		else if (key_state[ALLEGRO_KEY_DOWN])
			volume--;
		if (volume < 0)
			volume = 0;
		else if (volume > 5)
			volume = 5;
		al_set_sample_instance_gain(game_instance, 1 * volume / 10.0);
	}
	if (event.type == ALLEGRO_EVENT_KEY_UP) {
		key_state[event.keyboard.keycode] = false;
	}
}

void about_scene_draw()
{
	// clear to black
	al_clear_to_color(al_map_rgb(0, 0, 0));					// clear to black
	// title word and icons
	al_draw_text(font2, al_map_rgb(255, 255, 255), WIDTH / 2, 100, ALLEGRO_ALIGN_CENTER, "ABOUT");
	al_draw_bitmap(move_icon, 100, 200, 0);
	al_draw_text(font1, al_map_rgb(255, 255, 255), 175, 375, ALLEGRO_ALIGN_CENTRE, "W-A-S-D TO MOVE");
	al_draw_bitmap(shoot_icon, 550, 200, 0);
	al_draw_text(font1, al_map_rgb(255, 255, 255), 625, 375, ALLEGRO_ALIGN_CENTRE, "LEFT CLICK TO SHOOT");
	al_draw_bitmap(skill1_icon, 100, 450, 0);
	al_draw_text(font1, al_map_rgb(255, 255, 255), 175, 625, ALLEGRO_ALIGN_CENTRE, "SPACE TO SPIN BLADES");
	al_draw_bitmap(skill2_icon, 550, 450, 0);
	al_draw_text(font1, al_map_rgb(255, 255, 255), 625, 625, ALLEGRO_ALIGN_CENTRE, "RIGHT CLICK TO HEAL");
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 200, ALLEGRO_ALIGN_CENTRE, "UP-DOWN TO SET VOLUME ON THIS PAGE");
	al_draw_textf(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 250, ALLEGRO_ALIGN_CENTRE, "VOLUME: %d", volume);
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 325, ALLEGRO_ALIGN_CENTRE, "TRY TO BEAT ALL 9 ROOMS");
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 400 , ALLEGRO_ALIGN_CENTRE, "BACK");
	// button block
	al_draw_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 395, WIDTH / 2 + 150, HEIGHT / 2 + 435, al_map_rgb(255, 255, 255), 0);
	// update display
	al_flip_display();
}

void about_scene_destroy()
{
	// destroy fonts
	al_destroy_font(font1);
	al_destroy_font(font2);
	al_destroy_bitmap(move_icon);
	al_destroy_bitmap(shoot_icon);
	al_destroy_bitmap(skill1_icon);
	al_destroy_bitmap(skill2_icon);
}

void victory_init()
{
	// load fonts
	font1 = al_load_ttf_font("./font/pirulen.ttf", 20, 0);
	font2 = al_load_ttf_font("./font/pirulen.ttf", 28, 0);
	// load bgm
	victory_bgm = al_load_sample("./sound/victory.wav");
	al_reserve_samples(20);
	victory_instance = al_create_sample_instance(victory_bgm);
	al_set_sample_instance_playmode(victory_instance, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(victory_instance, al_get_default_mixer());
	al_set_sample_instance_gain(victory_instance, 1 * volume / 10.0);
	al_play_sample_instance(victory_instance);
	// load trophie
	trophie = al_load_bitmap("./image/trophie.png");
}

void victory_draw()
{
	// clear to color
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(trophie, 200, 200, 0);
	// victory
	al_draw_text(font2, al_map_rgb(0, 0, 0), WIDTH / 2, 275, ALLEGRO_ALIGN_CENTER, "VICTORY!!");
	al_draw_text(font2, al_map_rgb(0, 0, 0), WIDTH / 2, 325, ALLEGRO_ALIGN_CENTRE, "SCORE:");
	al_draw_text(font2, al_map_rgb(0, 0, 0), WIDTH / 2, 375, ALLEGRO_ALIGN_CENTRE, score);
	// exit button
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 200 , ALLEGRO_ALIGN_CENTRE, "EXIT");
	// button block
	al_draw_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 190, WIDTH / 2 + 150, HEIGHT / 2 + 230, al_map_rgb(255, 255, 255), 0);
	// update display
	al_flip_display();
}

int victory_process(ALLEGRO_EVENT event)
{
	int pos_x, pos_y;										// mouse position

	// set mouse position
	pos_x = event.mouse.x;
	pos_y = event.mouse.y;
	// go to menu
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			return 0;
	}
	if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
		// highlight back
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			highlight = 4;
		// reset highlight
		else
			highlight = 0;
	}

	return 1;
}

void victory_destroy()
{
	// destroy fonts
	al_destroy_font(font1);
	al_destroy_font(font2);
	// stop music
	al_stop_sample_instance(victory_instance);
	al_destroy_sample_instance(victory_instance);
	// destroy trophie
	al_destroy_bitmap(trophie);
}

void defeat_init()
{
	// load font
	font1 = al_load_ttf_font("./font/pirulen.ttf", 12, 0);
	font2 = al_load_ttf_font("./font/pirulen.ttf", 32, 0);
	// load bgm
	defeat_bgm = al_load_sample("./sound/defeat.wav");
	// load white flag
	white_flag = al_load_bitmap("./image/white_flag.png");
	if (defeat_bgm == NULL)
		printf("bgm fail\n");
	al_reserve_samples(20);
	defeat_instance = al_create_sample_instance(defeat_bgm);
	al_set_sample_instance_playmode(defeat_instance, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(defeat_instance, al_get_default_mixer());
	al_set_sample_instance_gain(defeat_instance, 1 * volume / 10.0);
	al_play_sample_instance(defeat_instance);
}

void defeat_draw()
{
	// clear to color
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(white_flag, 200, 200, 0);
	// victory
	al_draw_text(font2, al_map_rgb(255, 255, 255), WIDTH / 2, 250, ALLEGRO_ALIGN_CENTER, "DEFEAT!!!");
	al_draw_text(font2, al_map_rgb(0, 0, 0), WIDTH / 2 - 100, 450, ALLEGRO_ALIGN_CENTRE, score);
	// exit button
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 200 , ALLEGRO_ALIGN_CENTRE, "EXIT");
	// button block
	al_draw_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 190, WIDTH / 2 + 150, HEIGHT / 2 + 230, al_map_rgb(255, 255, 255), 0);
	// update display
	al_flip_display();
}

int defeat_process(ALLEGRO_EVENT event)
{
	int pos_x, pos_y;										// mouse position

	// set mouse position
	pos_x = event.mouse.x;
	pos_y = event.mouse.y;
	// go to menu
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			return 0;
	}
	if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
		// highlight back
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			highlight = 4;
		// reset highlight
		else
			highlight = 0;
	}

	return 1;
}

void defeat_destroy()
{
	// destroy fonts
	al_destroy_font(font1);
	al_destroy_font(font2);
	// stop music
	al_stop_sample_instance(defeat_instance);
	al_destroy_sample_instance(defeat_instance);
	// destroy white flag
	al_destroy_bitmap(white_flag);
}

void hl_draw(int pos) {
	// draw highlight depending on position
	if (pos == 1)
		al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 40, WIDTH / 2 + 150, HEIGHT / 2 + 80, al_map_rgba(100, 100, 100, 100));
	else if (pos == 2)
		al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 90, WIDTH / 2 + 150, HEIGHT / 2 + 130, al_map_rgba(100, 100, 100, 100));
	else if (pos == 3)
		al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 140, WIDTH / 2 + 150, HEIGHT / 2 + 180, al_map_rgba(100, 100, 100, 100));
	else if (pos == 4)
		al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 190, WIDTH / 2 + 150, HEIGHT / 2 + 230, al_map_rgba(100  , 100, 100, 100));
	else if (pos == 5)
		al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 395, WIDTH / 2 + 150, HEIGHT / 2 + 435, al_map_rgba(100  , 100, 100, 100));
}

void pause_init()
{
	// load fonts
	font1 = al_load_ttf_font("./font/pirulen.ttf",12, 0);
	font2 = al_load_ttf_font("./font/pirulen.ttf", 32, 0);
	first_time = 1;
}

int pause_process(ALLEGRO_EVENT event)
{
	char temp[50];
	int pos_x, pos_y;										// mouse position

	// set mouse position
	pos_x = event.mouse.x;
	pos_y = event.mouse.y;
	// keyboard actions
	if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
		// set whatever key is pressed to be true
		key_state[event.keyboard.keycode] = true;
		// pause and unpause
		if (key_state[ALLEGRO_KEY_ESCAPE]) {
			judge_next_window = GAME_SCENE;
		}
		// volume up
		else if (key_state[ALLEGRO_KEY_UP])
			volume++;
		// volume down
		else if (key_state[ALLEGRO_KEY_DOWN])
			volume--;
		// volume lower limit
		if (volume < 0)
			volume = 0;
		// volume upper limit
		else if (volume > 5)
			volume = 5;
		// set volume
		al_set_sample_instance_gain(game_instance, 1 * volume / 10.0);

	}
	// key let go
	else if (event.type == ALLEGRO_EVENT_KEY_UP)
		key_state[event.keyboard.keycode] = false;
	// actions at menu
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		// go to game
		if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 90 && pos_y < HEIGHT / 2 + 130)
			judge_next_window = GAME_SCENE;
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 140 && pos_y < HEIGHT / 2 + 180)
			judge_next_window = MENU;
		// go to exit
		else if (pos_x > WIDTH / 2 - 150 && pos_x < WIDTH / 2 + 150 && pos_y > HEIGHT / 2 + 190 && pos_y < HEIGHT / 2 + 230)
			return 0;
	}
	return 1;

}

void pause_draw()
{
	// transparent scene
	if (first_time) {
		al_draw_filled_rectangle(0, 0, WIDTH, HEIGHT, al_map_rgba(100, 100, 100, 0));
		first_time = 0;
	}
	// button blocks
	al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 90, WIDTH / 2 + 150, HEIGHT / 2 + 130, al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 140, WIDTH / 2 + 150, HEIGHT / 2 + 180, al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(WIDTH / 2 - 150, HEIGHT / 2 + 190, WIDTH / 2 + 150, HEIGHT / 2 + 230, al_map_rgb(0, 0, 0));
	// title word
	al_draw_text(font2, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 - 100 , ALLEGRO_ALIGN_CENTRE, "SOUL TONE");
	// button words
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 100 , ALLEGRO_ALIGN_CENTRE, "RESUME");
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 150 , ALLEGRO_ALIGN_CENTRE, "MENU");
	al_draw_text(font1, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 200 , ALLEGRO_ALIGN_CENTRE, "EXIT");
	// update display
	al_flip_display();
}

void pause_destroy()
{
	// load fonts
	font1 = al_load_ttf_font("./font/pirulen.ttf",12, 0);
	font2 = al_load_ttf_font("./font/pirulen.ttf", 32, 0);
}
