#include "GameWindow.h"

bool draw = false;											// need drawing
int window = 1;												// current window
const char *title = "Final Project";						// display title
ALLEGRO_SAMPLE *game_bgm = NULL;							// game bgm
ALLEGRO_SAMPLE *victory_bgm = NULL;							// victory bgm
ALLEGRO_SAMPLE *defeat_bgm = NULL;							// defeat bgm
ALLEGRO_SAMPLE_INSTANCE *game_instance = NULL;				// game instance
ALLEGRO_SAMPLE_INSTANCE *victory_instance = NULL;			// victory instance
ALLEGRO_SAMPLE_INSTANCE *defeat_instance = NULL;			// defeat instance
ALLEGRO_DISPLAY* display = NULL;							// display

int Game_establish()
{
	int msg = 0;											// game message

	game_init();											// game initial
	game_begin();											// game begin

	while (msg != GAME_TERMINATE) {						// run when not terminate
		msg = game_run();
		if (msg == GAME_TERMINATE)
			printf("Game Over\n");
	}
	config_save();											// save config
	game_destroy();											// destroy game

	return 0;
}

void game_init()
{
	// initializing message
	printf("Game Initializing...\n");
	// allegro initalize
	al_init();

	// init audio
	al_install_audio();
	al_init_acodec_addon();

	// Create display
	display = al_create_display(WIDTH, HEIGHT);

	// create event queue
	event_queue = al_create_event_queue();

	// Initialize Allegro settings
	al_set_window_position(display, 0, 0);
	al_set_window_title(display, title);
	al_init_primitives_addon();

	// initialize the font addon
	al_init_font_addon();

	// initialize the ttf (True Type Font) addon
	al_init_ttf_addon();

	// initialize the image addon
	al_init_image_addon();

	// initialize acodec addon
	al_init_acodec_addon();

	// install keyboard event
	al_install_keyboard();

	// install mouse event
	al_install_mouse();

	// install audio event
	al_install_audio();

	// Register event
	al_register_event_source(event_queue, al_get_display_event_source( display ));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
	fps = al_create_timer( 1.0 / FPS );
	al_register_event_source(event_queue, al_get_timer_event_source( fps )) ;

	// initialize the icon on the display
	ALLEGRO_BITMAP *icon = al_load_bitmap("./image/icon.png");
	al_set_display_icon(display, icon);
}

void game_begin()
{
	char temp[20];

	// Load sound
	game_bgm = al_load_sample("./sound/hello.mp3");
	al_reserve_samples(20);
	game_instance = al_create_sample_instance(game_bgm);

	// Loop the song until the display closes
	al_set_sample_instance_playmode(game_instance, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(game_instance, al_get_default_mixer());

	// initialize the menu before entering the loop
	menu_init();
	// config file
	if ((cfg = al_load_config_file("./game.cfg")) == NULL) {
		new_game = true;
		cfg = al_create_config();
		al_set_config_value(cfg, "", "volume", "5");
		al_set_config_value(cfg, "", "stage_map", "1");
		al_set_config_value(cfg, "", "chara.x", "340");
		al_set_config_value(cfg, "", "chara.x", "460");
		al_set_config_value(cfg, "", "heart", "20");
		al_set_config_value(cfg, "", "energy", "5");
		al_set_config_value(cfg, "", "score", "0");
		al_set_config_value(cfg, "", "boss.hp", "20");
		for (int k = 0; k <= 9; k++) {
			int count = k + 1;
			for(int i = 0; i < 3; i++){
				for(int j = 0; j < 3; j++){
					sprintf(temp, "mon[%d].state[%d][%d]", k, i, j);
					if(count > 0 && k < 9) {
						al_set_config_value(cfg, "", temp, "1");
						count--;
					}
					else if(k == 9)
						al_set_config_value(cfg, "", temp, "0");
					else
						al_set_config_value(cfg, "", temp, "0");
				}
			}
		}
	}
	strcpy(score, al_get_config_value(cfg, "", "score"));
	volume = atoi(al_get_config_value(cfg, "", "volume"));
	stage_map = atoi(al_get_config_value(cfg, "", "stage_map"));
	// set the volume of instance
	al_set_sample_instance_gain(game_instance, 1 * volume / 10.0);
	al_play_sample_instance(game_instance);
	al_start_timer(fps);
}

void game_update()
{
	// to menu
	if (judge_next_window == MENU) {
		if (window == ABOUT)
			about_scene_destroy();
		menu_init();
		judge_next_window = 0;
		window = MENU;
		pause = 0;
		highlight = 0;
	}
	// to game scene
	else if (judge_next_window == GAME_SCENE) {
		if (window == MENU) {
			menu_destroy();
			game_scene_init();
		}
		else if (window == PAUSE)
			pause_destroy();
		judge_next_window = 0;
		window = GAME_SCENE;
		pause = 0;
		highlight = 0;
	}
	// to about
	else if (judge_next_window == ABOUT) {
		if (window == MENU)
			menu_destroy();
		about_scene_init();
		judge_next_window = 0;
		window = ABOUT;
		pause = 0;
		highlight = 0;
	}
	// to pause
	else if(judge_next_window == PAUSE) {
		pause_init();
		judge_next_window = 0;
		window = PAUSE;
		pause = 1;
		highlight = 0;
	}
	// to victory
	else if (judge_next_window == VICTORY) {
		game_scene_destroy();
		victory_init();
		judge_next_window = 0;
		window = VICTORY;
		pause = 0;
		highlight = 0;
	}
	// to defeat
	else if (judge_next_window == DEFEAT) {
		game_scene_destroy();
		defeat_init();
		judge_next_window = 0;
		window = DEFEAT;
		pause = 0;
		highlight = 0;
	}
	// update character if at game scene
	if (window == GAME_SCENE) {
		character_update();
	}
}

int process_event()
{
	// Request the event
	ALLEGRO_EVENT event;
	al_wait_for_event(event_queue, &event);

	// process the event of other component
	if (window == MENU){
		if (!menu_process(event))
			return GAME_TERMINATE;
	}
	else if (window == GAME_SCENE){
		if (!game_scene_process(event))
			return GAME_TERMINATE;
		if (!pause)
			character_process(event);
	}
	else if (window == PAUSE) {
		if (!pause_process(event))
			return GAME_TERMINATE;
	}
	else if (window == ABOUT)
		about_process(event);
	else if (window == VICTORY) {
		if (!victory_process(event))
			return GAME_TERMINATE;
	}
	else if (window == DEFEAT) {
		if (!defeat_process(event))
			return GAME_TERMINATE;
	}
	// Shutdown our program
	if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		return GAME_TERMINATE;
	else if (event.type == ALLEGRO_EVENT_TIMER)
		if (event.timer.source == fps)
			draw = true;
	if (draw)
		game_update();

	return 0;
}

void game_draw()
{
	// draw current window
	if (window == MENU)
		menu_draw();
	else if (window == GAME_SCENE)
		game_scene_draw();
	else if (window == ABOUT)
		about_scene_draw();
	else if (window == VICTORY)
		victory_draw();
	else if (window == DEFEAT)
		defeat_draw();
	if (highlight)
		hl_draw(highlight);
	if (pause)
		pause_draw();

	al_flip_display();
}

int game_run()
{
	int error = 0;

	// draw if need draw
	if (draw){
		game_draw();
		draw = false;
	}
	// process error
	if (!al_is_event_queue_empty(event_queue))
		error = process_event();

	return error;
}

void config_save()
{
	char temp[20];

	// save unsaved aspects of config
	temp[0] = volume + '0';
	temp[1] = '\0';
	al_set_config_value(cfg, "", "volume", temp);
	al_set_config_value(cfg, "", "score", score);
	temp[0] = stage_map + '0';
	al_set_config_value(cfg, "", "stage_map", temp);
	al_save_config_file("game.cfg", cfg);
	al_destroy_config(cfg);
	if (window == VICTORY || window == DEFEAT)
		remove("./game.cfg");
}

void game_destroy()
{
	// Make sure you destroy all things
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
	if (window == DEFEAT)
		defeat_destroy();
	else if (window == VICTORY)
		victory_destroy();
}
