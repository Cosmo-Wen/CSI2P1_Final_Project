#include "character.h"

// the state of character
enum {STOP = 0, MOVE, ATK, SKILL, MOVE_ATK, MOVE_SKILL};				// possible states
ALLEGRO_SAMPLE_INSTANCE *mon_bomb_Sound;								// explosion instance
ALLEGRO_BITMAP *health_pack;											// health pack picture
ALLEGRO_BITMAP *energy_pack;											// energy pack picture
ALLEGRO_SAMPLE_INSTANCE *bomb_Sound;									// explosion sound
ALLEGRO_SAMPLE_INSTANCE *skill_Sound;									// skill sound
ALLEGRO_SAMPLE_INSTANCE *boss_skill_Sound;								// boss special sound
int score_count;														// score count
int full;																// if full at boss room
int dir_x[4] = {1 ,0 ,-1 ,0};											// direction x
int dir_y[4] = {0 ,1 ,0 , -1};											// direction y
typedef struct {
	int x, y;															// position
	int gun_x[4];														// gun position
	int gun_y[4];
	ALLEGRO_BITMAP *gun[4];												// gun picture
	int gun_vx[4], gun_vy[4];											// gun speed
	int gun_time;														// skill animation time
	int gun_time_count;													// skill time limit
	int state;															// state
	int hp;																// hp
	ALLEGRO_BITMAP *skill_pic[8];										// skill animation picture
	ALLEGRO_BITMAP *boss_pic;											// boss picture
} BBEG;																	// boss structure

typedef struct {
	int x, y;															// position
	bool taken;															// if taken
	int type;	 														// 0 is health pack, 1 is mana
} Item;																	// item structure

typedef struct character
{
	int x, y; 															// the position of image
	int width, height; 													// the width and height of image
	bool dir; 															// left: false, right: true
	int state; 															// the state of character
	ALLEGRO_BITMAP *img_move[2];										// move image
	ALLEGRO_BITMAP *img_atk[2];											// atk image
	ALLEGRO_BITMAP *gun_pic;											// gun picture
	ALLEGRO_BITMAP *skill_pic[2];										// skill picture
	ALLEGRO_SAMPLE_INSTANCE *atk_Sound;									// atk sound
	ALLEGRO_SAMPLE_INSTANCE *bomb_Sound;								// explosion sound
	ALLEGRO_SAMPLE_INSTANCE *shout_Sound;								// shout sound
	int anime; 															// counting the time of animation
	int anime_time; 													// indicate how long the animation
	int gun_x, gun_y; 													// gun position
	int skill_x[2], skill_y[2];											// skill position
	int skill_vx[2], skill_vy[2];										// skill speed
	double theta[2];													// skill angle
	bool skill_state;													// skill stae
	int skill_time;														// skill cd
	int skill_time_count;												// skill cd limite
	int heart; 															// health
	int energy; 														// energy
	bool invincible;													// god mode
	int pos_x, pos_y;													// mouse position
	float gun_vx, gun_vy; 												// gun speed
	int gunfar_x, gunfar_y; 											// character position when monster shoots
	ALLEGRO_BITMAP *hp, *half_hp, *mana;								// icons
}Character;																// character structure

typedef struct monster
{
	int x[3][3], y[3][3]; 												// the position of image
	int vx[3][3], vy[3][3];												// monster speed
	int gunfar_x[3][3], gunfar_y[3][3];									// monster position when monster shoots
	ALLEGRO_BITMAP *monster[3][3];										// monster picture
	ALLEGRO_BITMAP *bomb;												// explosion image
	ALLEGRO_BITMAP *gun[3][3];											// gun image
	ALLEGRO_SAMPLE_INSTANCE *bomb_Sound;								// explosion sound
	int gun_x[3][3], gun_y[3][3];										// gun position
	double gun_vx[3][3], gun_vy[3][3];									// gun speed
	float theta[3][3];													// gun angle
	int state[3][3];													// state
	int counter;														// monster count
	int gun_time[3][3];													// gun cd
	int gun_time_count;													// gun cd limit
}Monster;

BBEG boss;																// boss
Monster mon[10];														// monster room 1~9
Character chara;														// character
Item item[10];															// item of 9 rooms
ALLEGRO_SAMPLE *sample = NULL;											// music sample variable
void character_init(){
	// load character images
	for(int i = 1 ; i <= 2 ; i++){
		char temp[50];
		sprintf( temp, "./image/char_move%d.png", i );
		chara.img_move[i-1] = al_load_bitmap(temp);
	}
	for(int i = 1 ; i <= 2 ; i++){
		char temp[50];
		sprintf( temp, "./image/char_atk%d.png", i );
		chara.img_atk[i-1] = al_load_bitmap(temp);
	}
	for(int i = 1 ; i <= 2 ; i++){
		char temp[50];
		sprintf( temp, "./image/fire%d.png", i );
		chara.skill_pic[i-1] = al_load_bitmap(temp);
	}
	chara.gun_pic = al_load_bitmap("./image/gun.png");
	for(int i = 1 ; i <= 9 ; i++){
		mon[i].bomb = al_load_bitmap("./image/bomb.png");}
	for(int k = 1 ; k <= 9 ; k++){
		for(int j=1; j<=3; j++){
			for(int i = 1 ; i <= 3 ; i++){
				char temp[50];
				sprintf( temp, "./image/monster%d.png", 3*(j-1) + i );
				mon[k].monster[j-1][i-1] = al_load_bitmap(temp);
			}
		}
	}
	for(int k = 1 ; k <= 9 ; k++){
		for(int j = 1; j <= 3 ; j++){
			for(int i = 1 ; i <= 3 ; i++){
				char temp[50];
				sprintf( temp, "./image/monster_gun%d.png", 3*(j-1) + i );
				mon[k].gun[j-1][i-1] = al_load_bitmap(temp);
			}
		}
	}
	chara.hp = al_load_bitmap("./image/hp.png");
	chara.half_hp = al_load_bitmap("./image/half_hp.png");
	chara.mana = al_load_bitmap("./image/energy.png");
	boss.boss_pic = al_load_bitmap("./image/boss.png");
	for(int i = 1 ; i <= 8 ; i++){
		char temp[50];
		sprintf( temp, "./image/bossskill%d.png", i );
		boss.skill_pic[i-1] = al_load_bitmap(temp);
	}
	for(int i = 1 ; i <= 4 ; i++){
		char temp[50];
		sprintf( temp, "./image/boss_gun.png");
		boss.gun[i-1] = al_load_bitmap(temp);
		if(boss.gun[i-1] == NULL)printf("%d = no pic",i);
	}
	// load effective sound
	sample = al_load_sample("./sound/atk_sound.mp3");
	chara.atk_Sound  = al_create_sample_instance(sample);
	al_set_sample_instance_playmode(chara.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(chara.atk_Sound, al_get_default_mixer());

	sample = al_load_sample("./sound/bomb.mp3");
	mon_bomb_Sound  = al_create_sample_instance(sample);
	al_set_sample_instance_playmode(chara.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(mon_bomb_Sound, al_get_default_mixer());

	sample = al_load_sample("./sound/skill.mp3");
	skill_Sound  = al_create_sample_instance(sample);
	al_set_sample_instance_playmode(chara.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(skill_Sound, al_get_default_mixer());

	sample = al_load_sample("./sound/shout.mp3");
	chara.shout_Sound  = al_create_sample_instance(sample);
	al_set_sample_instance_playmode(chara.shout_Sound, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(chara.shout_Sound, al_get_default_mixer());

	sample = al_load_sample("./sound/boss_sound.mp3");
	boss_skill_Sound  = al_create_sample_instance(sample);
	al_set_sample_instance_playmode(boss_skill_Sound, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(boss_skill_Sound, al_get_default_mixer());

	// initial the geometric information of character
	chara.width = al_get_bitmap_width(chara.img_move[0]);
	chara.height = al_get_bitmap_height(chara.img_move[0]);

	// check for god mode
	if (al_get_config_value(cfg, "", "god") != NULL)
		chara.invincible = true;
	else
		chara.invincible = false;

	// config initialize information
	chara.x = atoi(al_get_config_value(cfg, "", "chara.x"));
	chara.y = atoi(al_get_config_value(cfg, "", "chara.y"));
	chara.heart = atoi(al_get_config_value(cfg, "", "heart"));
	chara.energy = atoi(al_get_config_value(cfg, "", "energy"));
	chara.dir = false;
	atoi(al_get_config_value(cfg, "", "score"));

	//initial skill balls position
	chara.theta[0] = 0;
	chara.theta[1] = 3.1416;
	chara.skill_state = false;

	// initial the animation component
	chara.state = STOP;
	chara.anime = 0;
	chara.anime_time = 30;
	chara.skill_time = 0;
	for(int i = 1 ; i <= 9 ; i++){
		mon[i].gun_time_count = 150;
	}
	chara.skill_time_count = -10000;

	// initialze monster information
	for (int k = 0; k <= 9; k++) {
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				char temp[20];
				sprintf(temp, "mon[%d].state[%d][%d]", k, i, j);
				mon[k].state[i][j] = atoi(al_get_config_value(cfg, "", temp));
				if (mon[k].state[i][j]) {
					if(k != 9){
						mon[k].x[i][j] = 200 * i + 150;
						mon[k].y[i][j] = 350 + 200*j;
					}
					else{
						mon[k].x[i][j] = rand() % ( WIDTH / 3) + 85;
						mon[k].y[i][j] = rand() % ( HEIGHT / 3 + 100 ) + 275;
					}
					mon[k].theta[i][j] = (6.2832/9) * (3 * i + j) - 0.5;
					mon[k].vx[i][j] = 3 * cos(mon[k].theta[i][j]);
					mon[k].vy[i][j] = 3 * sin(mon[k].theta[i][j]);
					mon[k].gun_x[i][j] = 100 + 200*i;
					mon[k].gun_y[i][j] = 350 + 200*j;
					mon[k].gunfar_x[i][j] = mon[k].x[i][j];
					mon[k].gunfar_y[i][j] = mon[k].y[i][j];
					mon[k].gun_vx[i][j] = 0;
					mon[k].gun_vy[i][j] = 0;
				}
				else {
					mon[k].x[i][j] = -100000;
					mon[k].y[i][j] = -100000;
					mon[k].theta[i][j] = 0;
					mon[k].vx[i][j] = 0;
					mon[k].vy[i][j] = 0;
					mon[k].gun_x[i][j] = -100000;
					mon[k].gun_y[i][j] = -100000;
					mon[k].gunfar_x[i][j] = mon[k].x[i][j];
					mon[k].gunfar_y[i][j] = mon[k].y[i][j];
					mon[k].gun_vx[i][j] = 0;
					mon[k].gun_vy[i][j] = 0;
				}
			}
		}
	}

	// initialize packs
	health_pack = al_load_bitmap("./image/health_pack.png");
	energy_pack = al_load_bitmap("./image/energy_pack.png");
	for (int i = 1; i <= 8; i++) {
		item[i].x = (rand() % (400)) + 200;
		item[i].y = (rand() % (400)) + 400;
		item[i].type = (rand() % 2);
		item[i].taken = false;
	}
	item[9].taken = true;
	for (int i = 1; i < 10; i++)
		mon[i].counter = 0;

	// initalize shooting positions
	chara.gunfar_x = chara.x + 50;
	chara.gunfar_y = chara.y + 50;

	// initialize boss information
	boss.x = WIDTH / 2 + 200;
	boss.y = HEIGHT / 2 + 100;
	boss.gun_time_count = 270;
	boss.state = STOP;
	boss.hp = atoi(al_get_config_value(cfg, "", "boss.hp"));
	for (int i = 0; i < 4; i++) {
		boss.gun_x[i] = boss.x + dir_x[i] * 100;
		boss.gun_y[i] = boss.y + dir_y[i] * 100;
		boss.gun_time = 61;
	}
}

void character_process(ALLEGRO_EVENT event){
	// process the animation
	if( event.type == ALLEGRO_EVENT_TIMER ){
		if( event.timer.source == fps ){
			chara.anime++;
			chara.anime %= chara.anime_time;
			chara.skill_time++;
			boss.gun_time++;
			boss.gun_time %= boss.gun_time_count;
			for(int i = 0 ; i < 3 ; i++){
				for(int j = 0 ; j < 3 ; j++){
					mon[stage_map].gun_time[i][j]++;
					mon[stage_map].gun_time[i][j] %= mon[stage_map].gun_time_count;
				}
			}
		}
		// process the keyboard event
	}else if( event.type == ALLEGRO_EVENT_KEY_DOWN ){
		key_state[event.keyboard.keycode] = true;
		if(event.keyboard.keycode == ALLEGRO_KEY_SPACE && chara.energy > 0){
			chara.skill_time_count = chara.skill_time;
			chara.energy--;
		}
		chara.anime = 0;
	}else if( event.type == ALLEGRO_EVENT_KEY_UP ){
		if(event.keyboard.keycode != ALLEGRO_KEY_SPACE)key_state[event.keyboard.keycode] = false;
		key_state[ALLEGRO_KEY_SPACE] = true;
	}
	else if( event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN ){
		if( event.mouse.button == 1 && (chara.gun_x < 0 || chara.gun_x > WIDTH || chara.gun_y < 0 || chara.gun_y > HEIGHT)){
			chara.gun_x = chara.x + 30;
			chara.gun_y = chara.y + 60;
			chara.pos_x = event.mouse.x;
			chara.pos_y = event.mouse.y;
			chara.gun_vx = 20 * cos(atan2(chara.pos_y - chara.gun_y,chara.pos_x - chara.gun_x)); // speed caclculation
			chara.gun_vy = 20 * sin(atan2(chara.pos_y - chara.gun_y,chara.pos_x - chara.gun_x));
			mouse_state = true;
		}else if( event.mouse.button == 2 && chara.energy > 0 && chara.heart < 20){
			chara.heart += 3;
			chara.energy--;
			mouse_state = false;
			if (chara.heart > 20)
				chara.heart = 20;
		}else if( event.mouse.button == 2 && chara.energy == 0){
			mouse_state = false;
		}
	}
	// calculate shooting line
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			mon[stage_map].gun_vx[i][j] = 5*cos(atan2(chara.gunfar_y + 40 - mon[stage_map].gunfar_y[i][j],chara.gunfar_x - mon[stage_map].gunfar_x[i][j]));
			mon[stage_map].gun_vy[i][j] = 5*sin(atan2(chara.gunfar_y + 40 - mon[stage_map].gunfar_y[i][j],chara.gunfar_x  - mon[stage_map].gunfar_x[i][j]));
		}
	}
	// calculating shooting speed
	for(int i = 0 ; i < 4 ; i++){
		boss.gun_vx[i] = 5*cos(atan2(chara.y + 50 - boss.gun_y[i] ,chara.x + 50 - boss.gun_x[i]));
		boss.gun_vy[i] = 5*sin(atan2(chara.y + 50 - boss.gun_y[i] ,chara.x + 50 - boss.gun_x[i]));
	}

	// if dead
	if (chara.heart <= 0)
		judge_next_window = DEFEAT;
}

void character_update(){
	char temp[20];

	// gun position and if gun hits
	if(!stage_state[stage_map]){
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				if(mon[stage_map].state[i][j] && mon[stage_map].gun_time[i][j] == mon[stage_map].gun_time_count - 1){
					mon[stage_map].gun_x[i][j] = mon[stage_map].x[i][j];
					mon[stage_map].gun_y[i][j] = mon[stage_map].y[i][j];
					mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
					mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
					chara.gunfar_x = chara.x + 50;
					chara.gunfar_y = chara.y + 50;
				}
				// monster move and direction
				if(mon[stage_map].x[i][j] <= 100 || mon[stage_map].x[i][j] >= WIDTH - 200){
					mon[stage_map].vx[i][j] = mon[stage_map].vx[i][j] * (-1);}
				if(mon[stage_map].y[i][j] <= 327 || mon[stage_map].y[i][j] >= HEIGHT - 200){
					mon[stage_map].vy[i][j] = mon[stage_map].vy[i][j] * (-1);}
				if(((mon[stage_map].y[i][j] + 100 >= boss.y - 100 && mon[stage_map].x[i][j] + 100 >= boss.x - 100) || (mon[stage_map].y[i][j] <= boss.y + 100 && mon[stage_map].x[i][j] + 100 >= boss.x - 100)) && stage_map == 9){
					mon[stage_map].vy[i][j] = mon[stage_map].vy[i][j] * (-1);}
				if((mon[stage_map].x[i][j] + 100 >= boss.x - 100 && mon[stage_map].y[i][j] + 100 >= boss.y - 100 && mon[stage_map].y[i][j] <= boss.y + 100) && stage_map == 9){
					mon[stage_map].vx[i][j] = mon[stage_map].vx[i][j] * (-1);
				}
				mon[stage_map].x[i][j] += mon[stage_map].vx[i][j];
				mon[stage_map].y[i][j] += mon[stage_map].vy[i][j];
				mon[stage_map].gun_x[i][j] +=  mon[stage_map].gun_vx[i][j];
				mon[stage_map].gun_y[i][j] +=  mon[stage_map].gun_vy[i][j];
				// if hit by gun
				if (chara.x + 20 < mon[stage_map].gun_x[i][j] + 15 && chara.x + 80 > mon[stage_map].gun_x[i][j] + 15 && chara.y < mon[stage_map].gun_y[i][j] +15 && chara.y + 100 > mon[stage_map].gun_y[i][j] + 15) {
					al_set_sample_instance_gain(chara.shout_Sound, 1 * volume / 10.0);
					al_play_sample_instance(chara.shout_Sound);
					mon[stage_map].gun_x[i][j] = -100000;
					mon[stage_map].gun_y[i][j] = -100000;
					mon[stage_map].gun_time[i][j] = 0;
					if (!chara.invincible){
						chara.heart--;
					}
				}
				// if hit by gun and monster is dead
				else if(chara.x + 20 < mon[stage_map].gun_x[i][j] + 15 && chara.x + 80 > mon[stage_map].gun_x[i][j] + 15 && chara.y < mon[stage_map].gun_y[i][j] +15 && chara.y + 100 > mon[stage_map].gun_y[i][j] + 15&& (mon[stage_map].x[i][j] < 0 || mon[stage_map].x[i][j] > WIDTH || mon[stage_map].y[i][j] < 0 || mon[stage_map].y[i][j] > HEIGHT)){
					al_set_sample_instance_gain(chara.shout_Sound, 1 * volume / 10.0);
					al_play_sample_instance(chara.shout_Sound);
					mon[stage_map].gun_x[i][j] = -100000;
					mon[stage_map].gun_y[i][j] = -100000;
					if (!chara.invincible)
						chara.heart--;
				}
				// if bullet hit bullet when monster is dead
				if(mon[stage_map].gun_x[i][j] + 15 > chara.gun_x && mon[stage_map].gun_x[i][j] + 15 < chara.gun_x + 37 && mon[stage_map].gun_y[i][j] + 15 > chara.gun_y && mon[stage_map].gun_y[i][j] + 15 < chara.gun_y + 37  && (mon[stage_map].x[i][j]<0 || mon[stage_map].x[i][j] > WIDTH || mon[stage_map].y[i][j] < 0 || mon[stage_map].y[i][j] > HEIGHT) ){
					mon[stage_map].gun_x[i][j] = -100000;
					mon[stage_map].gun_y[i][j] = -100000;
					mon[stage_map].gun_time[i][j] = 0;
				}
				// if bullet hit bullet
				else if(mon[stage_map].gun_x[i][j] + 15 > chara.gun_x && mon[stage_map].gun_x[i][j] + 15 < chara.gun_x + 37 && mon[stage_map].gun_y[i][j] + 15 > chara.gun_y && mon[stage_map].gun_y[i][j] + 15 < chara.gun_y + 37 && mon[stage_map].x[i][j] > 0 && mon[stage_map].x[i][j] < WIDTH && mon[stage_map].y[i][j] > 0 && mon[stage_map].y[i][j] < HEIGHT){
					mon[stage_map].gun_x[i][j] = -100000;
					mon[stage_map].gun_y[i][j] = -100000;
					mon[stage_map].gun_time[i][j] = 0;
				}
				// if bullet and skill hit
				for(int k = 0 ; k < 2 ; k++){
					if(mon[stage_map].gun_x[i][j] + 15 > chara.skill_x[k] && mon[stage_map].gun_x[i][j] + 15 < chara.skill_x[k] + 50 && mon[stage_map].gun_y[i][j] + 15 > chara.skill_y[k] && mon[stage_map].gun_y[i][j] + 15 < chara.skill_y[k] + 50 &&(mon[stage_map].x[i][j] < 0 || mon[stage_map].x[i][j] > WIDTH || mon[stage_map].y[i][j] < 0 || mon[stage_map].y[i][j] > HEIGHT)){
						mon[stage_map].gun_x[i][j] = -100000;
						mon[stage_map].gun_y[i][j] = -100000;
					}
					else if(mon[stage_map].gun_x[i][j] + 15 > chara.skill_x[k] && mon[stage_map].gun_x[i][j] + 15 < chara.skill_x[k] + 50 && mon[stage_map].gun_y[i][j] + 15 > chara.skill_y[k] && mon[stage_map].gun_y[i][j] + 15 < chara.skill_y[k] + 50){
						mon[stage_map].gun_x[i][j] = -100000;
						mon[stage_map].gun_y[i][j] = -100000;
						mon[stage_map].gun_time[i][j] = 0;
					}
				}
				// monster gun cd
				if(mon[stage_map].gun_y[i][j] < 200){
					mon[stage_map].gun_x[i][j] = 100000;
					mon[stage_map].gun_y[i][j] = 100000;
					mon[stage_map].gun_time[i][j] = 0;
				}
			}
		}
	}

	// use the idea of finite state machine to deal with different state
	// if skill cd
	if(chara.skill_time - chara.skill_time_count >= 90)
		key_state[ALLEGRO_KEY_SPACE] = false;
	// move and shoot
	if( key_state[ALLEGRO_KEY_W] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.y -= 4;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}else if( key_state[ALLEGRO_KEY_A] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.dir = false;
		chara.x -= 4;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}else if( key_state[ALLEGRO_KEY_S] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.y += 4;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}else if( key_state[ALLEGRO_KEY_D] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.dir = true;
		chara.x += 4;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}
	else if( key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_A] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.dir = false;
		chara.x -= 2;
		chara.y -= 2;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}else if( key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_D] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.dir = true;
		chara.x += 2;
		chara.y -= 2;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}else if( key_state[ALLEGRO_KEY_S] && key_state[ALLEGRO_KEY_A] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.dir = false;
		chara.x -= 2;
		chara.y += 2;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}else if( key_state[ALLEGRO_KEY_S] && key_state[ALLEGRO_KEY_D] && mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.dir = true;
		chara.x += 2;
		chara.y += 2;
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = MOVE_ATK;
	}
	// move and use skill
	else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_A]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);
			chara.skill_x[i] += chara.skill_vx[i] - 2;
			chara.skill_y[i] += chara.skill_vy[i] - 2;
			chara.theta[i] += 0.07;
		}
		chara.dir = false;
		chara.x -= 2;
		chara.y -= 2;
		chara.state = MOVE_SKILL;
	}else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_D]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);
			chara.skill_x[i] += chara.skill_vx[i] + 2;
			chara.skill_y[i] += chara.skill_vy[i] - 2;
			chara.theta[i] += 0.07;
		}
		chara.dir = true;
		chara.x += 2;
		chara.y -= 2;
		chara.state = MOVE_SKILL;
	}else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_S] && key_state[ALLEGRO_KEY_A]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);
			chara.skill_x[i] += chara.skill_vx[i] - 2;
			chara.skill_y[i] += chara.skill_vy[i] + 2;
			chara.theta[i] += 0.07;
		}
		chara.dir = false;
		chara.x -= 2;
		chara.y += 2;
		chara.state = MOVE_SKILL;
	}else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_S] && key_state[ALLEGRO_KEY_D]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);
			chara.skill_x[i] += chara.skill_vx[i] + 2;
			chara.skill_y[i] += chara.skill_vy[i] + 2;
			chara.theta[i] += 0.07;
		}
		chara.dir = true;
		chara.x += 2;
		chara.y += 2;
		chara.state = MOVE_SKILL;
	}else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_W]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);
			chara.skill_x[i] += chara.skill_vx[i];
			chara.skill_y[i] += chara.skill_vy[i] - 4;
			chara.theta[i] += 0.07;
		}
		chara.y -= 4;
		chara.state = MOVE_SKILL;
	}else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_A]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
			chara.skill_x[i] += chara.skill_vx[i] - 4;
			chara.skill_y[i] += chara.skill_vy[i];
			chara.theta[i] += 0.07;
		}
		chara.dir = false;
		chara.x -= 4;
		chara.state = MOVE_SKILL;
	}else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_S]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
			chara.skill_x[i] += chara.skill_vx[i];
			chara.skill_y[i] += chara.skill_vy[i] + 4;
			chara.theta[i] += 0.07;
		}
		chara.y += 4;
		chara.state = MOVE_SKILL;
	}else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_D]){
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
			chara.skill_x[i] += chara.skill_vx[i] + 4;
			chara.skill_y[i] += chara.skill_vy[i];
			chara.theta[i] += 0.07;
		}
		chara.dir = true;
		chara.x += 4;
		chara.state = MOVE_SKILL;
	}
	// use skill
	else if(chara.skill_time - chara.skill_time_count <= 150 && key_state[ALLEGRO_KEY_SPACE]){///////////////////////////////////
		for(int i = 0 ; i < 2 ; i++){
			chara.skill_x[i] = chara.x + 30;
			chara.skill_y[i] = chara.y + 50;
			chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
			chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
			chara.skill_x[i] += chara.skill_vx[i];
			chara.skill_y[i] += chara.skill_vy[i];
			chara.theta[i] += 0.07;
		}
		chara.state = SKILL;
	}
	// move
	else if(key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_A]){
		chara.dir = false;
		chara.x -= 2;
		chara.y -= 2;
		chara.state = MOVE;
	}else if( key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_D]){
		chara.dir = true;
		chara.x += 2;
		chara.y -= 2;
		chara.state = MOVE;
	}else if( key_state[ALLEGRO_KEY_S] && key_state[ALLEGRO_KEY_A]){
		chara.dir = false;
		chara.x -= 2;
		chara.y += 2;
		chara.state = MOVE;
	}else if( key_state[ALLEGRO_KEY_S] && key_state[ALLEGRO_KEY_D]){
		chara.dir = true;
		chara.x += 2;
		chara.y += 2;
		chara.state = MOVE;
	}else if( key_state[ALLEGRO_KEY_W] ){
		chara.y -= 4;
		chara.state = MOVE;
	}else if( key_state[ALLEGRO_KEY_A] ){
		chara.dir = false;
		chara.x -= 4;
		chara.state = MOVE;
	}else if( key_state[ALLEGRO_KEY_S] ){
		chara.y += 4;
		chara.state = MOVE;
	}else if( key_state[ALLEGRO_KEY_D] ){
		chara.dir = true;
		chara.x += 4;
		chara.state = MOVE;
	}
	// only atk
	else if( mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
		chara.gun_x += chara.gun_vx;
		chara.gun_y += chara.gun_vy;
		chara.state = ATK;
	}
	// stop
	else if(chara.gun_x <= -42 || chara.gun_x >= WIDTH || chara.gun_y <= -42 || chara.gun_y >= HEIGHT){
		chara.state = STOP;
	}else if( chara.anime == chara.anime_time-1 ){
		chara.anime = 0;
		chara.state = STOP;
	}else if ( chara.anime == 0 ){
		chara.state = STOP;
	}
	// skill
	else if( key_state[ALLEGRO_KEY_SPACE] ){
		chara.gun_x += 2*chara.gun_vx;
		chara.gun_y += 2*chara.gun_vy;
		chara.state = SKILL;
	}
	// boss cd
	if(boss.state == SKILL && boss.gun_time == boss.gun_time_count - 1){
		boss.state = STOP;
	}
	// boss gun
	if(stage_map == 9 && boss.hp > 0){
		for(int i = 0 ; i < 4 ; i++){
			boss.gun_x[i] += boss.gun_vx[i];
			boss.gun_y[i] += boss.gun_vy[i];
			if(chara.x + 20 < boss.gun_x[i] + 15 && chara.x + 80 > boss.gun_x[i] + 15 && chara.y < boss.gun_y[i] +15 && chara.y + 100 > boss.gun_y[i] + 15){
				al_set_sample_instance_gain(chara.shout_Sound, 1 * volume / 10.0);
				al_play_sample_instance(chara.shout_Sound);
				boss.gun_x[i] = boss.x + dir_x[i] * 100;
				boss.gun_y[i] = boss.y + dir_y[i] * 100;
				if(!chara.invincible){
					chara.heart--;}
			}
			if(boss.gun_x[i] + 15 > chara.gun_x && boss.gun_x[i] + 15 < chara.gun_x + 37 && boss.gun_y[i] + 15 > chara.gun_y && boss.gun_y[i] + 15 < chara.gun_y + 37 ){
				boss.gun_x[i] = boss.x + dir_x[i] * 100;
				boss.gun_y[i] = boss.y + dir_y[i] * 100;
			}
			for(int k = 0 ; k < 2 ; k++){
				if(boss.gun_x[i] + 15 > chara.skill_x[k] && boss.gun_x[i] + 15 < chara.skill_x[k] + 50 && boss.gun_y[i] + 15 > chara.skill_y[k] && boss.gun_y[i] + 15 < chara.skill_y[k] + 50){
					boss.gun_x[i] = boss.x + dir_x[i] * 100;
					boss.gun_y[i] = boss.y + dir_y[i] * 100;
				}
			}
		}
	}
	// boss special stae
	if(boss.gun_time == 0 && full <= 4){
		boss.state = SKILL;
	}
	// boss special - return a monster
	if( boss.gun_time == 45 && stage_map == 9){
		int rand_x, rand_y;
		full = 0;
		rand_x = rand() % 3;
		rand_y = rand() % 3;
		for(int i = 0 ; i < 3 ; i++){
			for(int j = 0 ; j < 3 ; j++){
				if(mon[stage_map].state[i][j] == 1){
					full++;
				}
			}
		}
		if(full <= 4){
			while(((rand_x == 1 && rand_y == 2) || mon[stage_map].state[rand_x][rand_y] == 1) && full < 4){
				rand_x = rand() % 3;
				rand_y = rand() % 3;
				full = 0;
			}
			if(full <= 4){
				mon[stage_map].state[rand_x][rand_y] = 1;
				mon[stage_map].x[rand_x][rand_y] = WIDTH / 2 -50 - 10 * rand_x;
				mon[stage_map].y[rand_x][rand_y] = HEIGHT / 2 + 100 - 5 * rand_y;
				mon[stage_map].theta[rand_x][rand_y] = (6.5/9) * (3 * rand_x + rand_y) - 0.3;
				mon[stage_map].vx[rand_x][rand_y] = 3 * cos(mon[stage_map].theta[rand_x][rand_y]);
				mon[stage_map].vy[rand_x][rand_y] = 3 * sin(mon[stage_map].theta[rand_x][rand_y]);
				mon[stage_map].gun_x[rand_x][rand_y] = 100 + 200 * rand_x;
				mon[stage_map].gun_y[rand_x][rand_y] = 350 + 200 * rand_y;
				mon[stage_map].gunfar_x[rand_x][rand_y] = mon[stage_map].x[rand_x][rand_y];
				mon[stage_map].gunfar_y[rand_x][rand_y] = mon[stage_map].y[rand_x][rand_y];
				mon[stage_map].gun_vx[rand_x][rand_y] = 0;
				mon[stage_map].gun_vy[rand_x][rand_y] = 0;
			}
		}
	}
	// room border
	if(chara.gun_y <= 200)
		chara.gun_y = -100;
	if(chara.x <= 75)
		chara.x = 75;
	if(chara.x >= WIDTH - 200)
		chara.x = WIDTH - 200;
	if(chara.y < 300)
		chara.y = 300;
	if(chara.y >= HEIGHT - 200)
		chara.y = HEIGHT - 200;
	// teleportation to other rooms
	switch(stage_map) {
        case 1: {
            if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map++;
                stage_change = 1;
                chara.x = 80;
                chara.y = 560;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

            break;
        }
		case 2: {
            if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map++;
                stage_change = 1;
                chara.x = 80;
                chara.y = 560;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }
            else if (chara.x == 75 && chara.y > 540 && chara.y < 570) {
                stage_map--;
                stage_change = 1;
                chara.x = 590;
                chara.y = 560;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

             break;
        }
		case 3: {
            if (chara.x > 320 && chara.x < 340 && chara.y == 300) {
                stage_map++;
                stage_change = 1;
                chara.x = 340;
                chara.y = 790;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }
            else if (chara.x == 75 && chara.y > 540 && chara.y < 570) {
                stage_map--;;
                stage_change = 1;
                chara.x = 590;
                chara.y = 560;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

            break;
        }
		case 4: {
            if (chara.x > 330 && chara.x < 350 && chara.y == 300) {
                stage_map++;
                stage_change = 1;
                chara.x = 340;
                chara.y = 790;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }
            else if (chara.x > 330 && chara.x < 350 && chara.y == 800) {
                stage_map--;
                stage_change = 1;
                chara.x = 330;
                chara.y = 310;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }
            break;
        }
		case 5: {
            if (chara.x == 75 && chara.y > 550 && chara.y < 570) {
                stage_map++;
                stage_change = 1;
                chara.x = 555;
                chara.y = 590;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
             }
             else if (chara.x > 330 && chara.x < 350 && chara.y == 800) {
                stage_map--;
                stage_change = 1;
                chara.x = 340;
                chara.y = 310;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

            break;
        }
		case 6: {
            if (chara.x == 75 && chara.y > 540 && chara.y < 560) {
                stage_map++;
                stage_change = 1;
                chara.x = 590;
                chara.y = 555;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }
            else if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map--;
                stage_change = 1;
                chara.x = 85;
                chara.y = 560;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

            break;
		}
		case 7: {
            if (chara.x > 350 && chara.x < 360 && chara.y == 800) {
                stage_map++;
                stage_change = 1;
                chara.x = 345;
                chara.y = 310;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }
            else if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map--;
                stage_change = 1;
                chara.x = 85;
                chara.y = 550;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

            break;
        }
		case 8: {
            if (chara.x == 600 && chara.y > 530 && chara.y < 560) {
                stage_map++;
                boss.state = STOP;
                stage_change = 1;
                chara.x = 85;
                chara.y = 545;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                boss.gun_time = 61;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }
            else if (chara.x > 335 && chara.x < 355 && chara.y == 300) {
                stage_map--;
                stage_change = 1;
                chara.x = 355;
                chara.y = 790;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

            break;
        }
		case 9: {
            if (chara.x == 75 && chara.y > 530 && chara.y < 560) {
                stage_map--;
                stage_change = 1;
                chara.x = 590;
                chara.y = 545;
                chara.gunfar_x = chara.x + 50;
                chara.gunfar_y = chara.y + 50;
                for(int i = 0 ; i < 3 ; i++){
                    for(int j = 0 ; j < 3 ; j++){
                        mon[stage_map].gunfar_x[i][j] = mon[stage_map].x[i][j] + 50;
                        mon[stage_map].gunfar_y[i][j] = mon[stage_map].y[i][j] + 50;
                    }
                }
            }

            break;
        }
	}
	// gun disappear if change room
	if(stage_change){
		chara.gun_x = -100000;
		chara.gun_y = -100000;}
	// save information to config
	itoa(chara.x, temp, 10);
	al_set_config_value(cfg, "", "chara.x", temp);
	itoa(chara.y, temp, 10);
	al_set_config_value(cfg, "", "chara.y", temp);
	itoa(chara.heart, temp, 10);
	al_set_config_value(cfg, "", "heart", temp);
	itoa(chara.energy, temp, 10);
	al_set_config_value(cfg, "", "energy", temp);
	itoa(boss.hp, temp, 10);
	al_set_config_value(cfg, "", "boss.hp", temp);
	for (int k = 0; k <= 9; k++) {
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				sprintf(temp, "mon[%d].state[%d][%d]", k, i, j);
				if (mon[k].state[i][j])
					al_set_config_value(cfg, "", temp, "1");
				else
					al_set_config_value(cfg, "", temp, "0");
			}
		}
	}
	// item placement
	if (stage_state[stage_map] && !item[stage_map].taken) {
		if (chara.x + 50 > item[stage_map].x - 50 && chara.x + 50 < item[stage_map].x + 50 && chara.y + 50 > item[stage_map].y - 50 && chara.y + 50 < item[stage_map].y + 50) {
			if (item[stage_map].type == 0) {
				chara.heart += 5;
				score_count += 250;
				itoa(score_count , score , 10);
				if (chara.heart > 20)
					chara.heart = 20;
			}
			else {
				chara.energy += 2;
				score_count += 250;
				itoa(score_count , score , 10);
				if (chara.energy > 5)
					chara.energy = 5;
			}
			item[stage_map].taken = true;
		}
	}
	// if boss is dead then victory
	if(boss.hp <= 0){
		judge_next_window = VICTORY;
	}
}

void character_draw(){
	// with the state, draw corresponding image
	// monster and their gun
	if( !stage_state[stage_map]){
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				if(mon[stage_map].state[i][j] == 1){
					if(mon[stage_map].vx[i][j]>0){
						al_draw_bitmap(mon[stage_map].monster[i][j], mon[stage_map].x[i][j] , mon[stage_map].y[i][j], ALLEGRO_FLIP_HORIZONTAL);
						al_draw_bitmap(mon[stage_map].gun[i][j], mon[stage_map].gun_x[i][j], mon[stage_map].gun_y[i][j], 0);}
					else{
						al_draw_bitmap(mon[stage_map].monster[i][j], mon[stage_map].x[i][j] , mon[stage_map].y[i][j], 0);
						al_draw_bitmap(mon[stage_map].gun[i][j], mon[stage_map].gun_x[i][j], mon[stage_map].gun_y[i][j], ALLEGRO_FLIP_HORIZONTAL);}
				}
			}
		}
	}
	// character animations
	if( chara.state == STOP ){
		if( chara.dir )
			al_draw_bitmap(chara.img_move[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
		else
			al_draw_bitmap(chara.img_move[0], chara.x, chara.y, 0);
	}else if( chara.state == MOVE ){
		if( chara.dir ){
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_move[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			}else{
				al_draw_bitmap(chara.img_move[1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			}
		}else{
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_move[0], chara.x, chara.y, 0);
			}else{
				al_draw_bitmap(chara.img_move[1], chara.x, chara.y, 0);
			}
		}
	}else if( chara.state == ATK ){
		if( chara.pos_x - 40 >= chara.x){
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
				al_set_sample_instance_gain(chara.atk_Sound, 1 * volume / 10.0);
				al_play_sample_instance(chara.atk_Sound);
			}
		}else{
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, 0);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, 0);
				al_set_sample_instance_gain(chara.atk_Sound, 1 * volume / 10.0);
				al_play_sample_instance(chara.atk_Sound);
			}
		}
		// character gun
		al_draw_bitmap(chara.gun_pic, chara.gun_x, chara.gun_y, 0);
		// draw explosion effect when hit by bullet
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				if(mon[stage_map].state[i][j] == 1 && mon[stage_map].x[i][j] - chara.gun_x < 0 && mon[stage_map].x[i][j] - chara.gun_x > -50 && mon[stage_map].y[i][j] - chara.gun_y < 0 && mon[stage_map].y[i][j] - chara.gun_y >-50 ){
					al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
					al_set_sample_instance_gain(mon_bomb_Sound, 1 * volume / 10.0);
					al_play_sample_instance(mon_bomb_Sound);
					mon[stage_map].x[i][j] = -100000;
					mon[stage_map].y[i][j] = -100000;
					mon[stage_map].state[i][j] = 0;
					mon[stage_map].counter++;
					chara.gun_x = -100000;
					chara.gun_y = -100000;
					al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
					score_count += 100;
					itoa(score_count , score , 10);
				}
			}
		}
		// hit boss if bullet
		if(stage_map == 9){
			if(boss.hp > 0 && boss.x - 100 < chara.gun_x && boss.x + 100 > chara.gun_x && boss.y - 100 < chara.gun_y && boss.y + 100 > chara.gun_y ){
				boss.hp--;
				chara.gun_x = -100000;
				chara.gun_y = -100000;
				al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
				al_set_sample_instance_gain(mon_bomb_Sound, 1 * volume / 10.0);
				al_play_sample_instance(mon_bomb_Sound);
			}
		}
	}
	// draw skill
	else if(chara.state == SKILL){
		if( chara.anime < chara.anime_time/2 ){
			al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
		}else{
			al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			al_set_sample_instance_gain(skill_Sound, 1 * volume / 10.0);
			al_play_sample_instance(skill_Sound);
		}
		for(int i = 0 ; i < 2 ; i++){
			al_draw_bitmap(chara.skill_pic[i], chara.skill_x[i], chara.skill_y[i], 0);
		}
		// if hit monster
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				for(int k = 0 ; k < 2 ; k++){
					if(chara.skill_x[k] - mon[stage_map].x[i][j] < 100 && chara.skill_x[k] - mon[stage_map].x[i][j] > -50 && chara.skill_y[k] - mon[stage_map].y[i][j] < 90 && chara.skill_y[k] - mon[stage_map].y[i][j] > -50 ){
						mon[stage_map].x[i][j] = -100000;
						mon[stage_map].y[i][j] = -100000;
						mon[stage_map].state[i][j] = 0;
						mon[stage_map].counter++;
						score_count += 100;
						itoa(score_count , score , 10);
					}
				}
			}
		}
		// if hit boss
		if(stage_map == 9){
			for(int k = 0 ; k < 2 ; k++){
				if(boss.hp > 0 && boss.x - 100 < chara.skill_x[k] && boss.x + 100 > chara.skill_x[k] && boss.y - 100 < chara.skill_y[k] && boss.y + 100 > chara.skill_y[k] ){
					boss.hp--;
				}
			}
		}
	}
	// draw skill
	else if (chara.state == MOVE_SKILL){
		if( chara.dir ){
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
				al_set_sample_instance_gain(skill_Sound, 1 * volume / 10.0);
				al_play_sample_instance(skill_Sound);
			}
		}else{
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, 0);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, 0);
				al_set_sample_instance_gain(skill_Sound, 1 * volume / 10.0);
				al_play_sample_instance(skill_Sound);
			}
		}
		for(int i = 0 ; i < 2 ; i++){
			al_draw_bitmap(chara.skill_pic[i], chara.skill_x[i], chara.skill_y[i], 0);
		}
		// skill hit monster
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				for(int k = 0 ; k < 2 ; k++){
					if(chara.skill_x[k] - mon[stage_map].x[i][j] < 100 && chara.skill_x[k] - mon[stage_map].x[i][j] > -50 && chara.skill_y[k] - mon[stage_map].y[i][j] < 90 && chara.skill_y[k] - mon[stage_map].y[i][j] > -50 ){
						mon[stage_map].x[i][j] = -100000;
						mon[stage_map].y[i][j] = -100000;
						mon[stage_map].state[i][j] = 0;
						mon[stage_map].counter++;
						score_count += 200;
						itoa(score_count , score , 10);
					}
				}
			}
		}
	}
	// move and attack animation
	else if( chara.state == MOVE_ATK ){
		if( chara.dir ){
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			}
		}else{
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, 0);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, 0);
			}
		}
		if( chara.pos_x - 40 >= chara.x){
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
				al_set_sample_instance_gain(chara.atk_Sound, 1 * volume / 10.0);
				al_play_sample_instance(chara.atk_Sound);
			}
		}else{
			if( chara.anime < chara.anime_time/2 ){
				al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, 0);
			}else{
				al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, 0);
				al_set_sample_instance_gain(chara.atk_Sound, 1 * volume / 10.0);
				al_play_sample_instance(chara.atk_Sound);
			}
		}
		al_draw_bitmap(chara.gun_pic, chara.gun_x, chara.gun_y, 0);
		// hit monster with bullet
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				if(mon[stage_map].state[i][j] == 1 && mon[stage_map].x[i][j] - chara.gun_x < 0 && mon[stage_map].x[i][j] - chara.gun_x > -50 && mon[stage_map].y[i][j] - chara.gun_y < 0 && mon[stage_map].y[i][j] - chara.gun_y >-50 ){
					al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
					al_set_sample_instance_gain(mon_bomb_Sound, 1 * volume / 10.0);
					al_play_sample_instance(mon_bomb_Sound);
					mon[stage_map].x[i][j] = -100000;
					mon[stage_map].y[i][j] = -100000;
					mon[stage_map].state[i][j] = 0;
					mon[stage_map].counter++;
					chara.gun_x = -100000;
					chara.gun_y = -100000;
					al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
					score_count += 100;
					itoa(score_count , score , 10);
				}
			}
		}
		// boss hit by bullet
		if(stage_map == 9){
			if(boss.hp > 0 && boss.x - 100 < chara.gun_x && boss.x + 100 > chara.gun_x && boss.y - 100 < chara.gun_y && boss.y + 100 > chara.gun_y ){
				boss.hp--;
				chara.gun_x = -100000;
				chara.gun_y = -100000;
				al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
				al_set_sample_instance_gain(mon_bomb_Sound, 1 * volume / 10.0);
				al_play_sample_instance(mon_bomb_Sound);
			}
		}
	}
	// boss special animation
	if(stage_map == 9){
		// boss gun
		for(int i = 0 ; i < 4 ; i++){
			al_draw_bitmap(boss.gun[i], boss.gun_x[i], boss.gun_y[i], 0);
		}
	}
	if( boss.state == STOP && stage_map == 9){
		al_draw_bitmap(boss.boss_pic, boss.x - 100, boss.y - 100, 0);
	}else if(boss.state == SKILL && stage_map == 9){
		if( boss.gun_time < 7 ){
			al_set_sample_instance_gain(boss_skill_Sound, 1 * volume / 10.0);
			al_play_sample_instance(boss_skill_Sound);
			al_draw_bitmap(boss.skill_pic[0], boss.x - 100, boss.y - 100, 0);
		}else if( boss.gun_time >= 7 && boss.gun_time < 15){
			al_draw_bitmap(boss.skill_pic[1], boss.x - 100, boss.y - 100, 0);
		}else if(boss.gun_time >= 15 && boss.gun_time < 22){
			al_draw_bitmap(boss.skill_pic[2], boss.x - 100, boss.y - 100, 0);
		}else if(boss.gun_time >= 22 && boss.gun_time < 30){
			al_draw_bitmap(boss.skill_pic[3], boss.x - 100, boss.y - 100, 0);
		}else if(boss.gun_time >= 30 && boss.gun_time < 37){
			al_draw_bitmap(boss.skill_pic[4], boss.x - 100, boss.y - 100, 0);
		}else if(boss.gun_time >= 37 && boss.gun_time < 45){
			al_draw_bitmap(boss.skill_pic[5], boss.x - 100, boss.y - 100, 0);
		}else if(boss.gun_time >= 45 && boss.gun_time < 52){
			al_draw_bitmap(boss.skill_pic[6], boss.x - 100, boss.y - 100, 0);
		}else if(boss.gun_time >= 52 && boss.gun_time < 60){
			al_draw_bitmap(boss.skill_pic[7], boss.x - 100, boss.y - 100, 0);
		}else{
			boss.state = STOP;
			al_draw_bitmap(boss.boss_pic, boss.x - 100, boss.y - 100, 0);
		}
	}
	// draw hearts
	for (int i = 0; i < chara.heart / 2; i++)
		al_draw_bitmap(chara.hp, 20 + 60 * i, 20, 0);
	// draw half hearts
	if (chara.heart & 1)
		al_draw_bitmap(chara.half_hp, 20 + 60 * ((chara.heart - 1) / 2), 20, 0);
	// draw energy
	for (int i = 0; i < chara.energy; i++)
		al_draw_bitmap(chara.mana, 20 + 60 * i, 80, 0);
	// check if room clear
	if (mon[stage_map].counter > stage_map && stage_map != 9)
		stage_state[stage_map] = true;
	// draw item if item taken
	if (stage_state[stage_map] == true && !item[stage_map].taken) {
		if (item[stage_map].type == 0)
			al_draw_bitmap(health_pack, item[stage_map].x - 50, item[stage_map].y - 50, 0);
		else
			al_draw_bitmap(energy_pack, item[stage_map].x - 50, item[stage_map].y - 50, 0);
	}
	// draw health bar for boss
	if (stage_map == 9) {
		al_draw_rectangle(boss.x - 100, boss.y + 105, boss.x + 100, boss.y + 115, al_map_rgb(0, 0, 0), 2);
		al_draw_filled_rectangle(boss.x - 98, boss.y + 107, boss.x - 98 + 200 * boss.hp / 20, boss.y + 113, al_map_rgb(0, 255, 0));
	}
}
void character_destory(){
	al_destroy_bitmap(chara.img_atk[0]);
	al_destroy_bitmap(chara.img_atk[1]);
	al_destroy_bitmap(chara.img_move[0]);
	al_destroy_bitmap(chara.img_move[1]);
	al_destroy_sample_instance(chara.atk_Sound);
}
