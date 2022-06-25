#include "character.h"

// the state of character
enum {STOP = 0, MOVE, ATK, SKILL, MOVE_ATK, MOVE_SKILL};
ALLEGRO_SAMPLE_INSTANCE *mon_bomb_Sound;

ALLEGRO_BITMAP *health_pack;
ALLEGRO_BITMAP *energy_pack;
ALLEGRO_SAMPLE_INSTANCE *bomb_Sound;

typedef struct {
    int x, y;
    bool taken;
    int type; // 0 is health pack, 1 is mana
} Item;

typedef struct character
{
    int x, y; // the position of image
    int width, height; // the width and height of image
    bool dir; // left: false, right: true
    int state; // the state of character
    ALLEGRO_BITMAP *img_move[2];
    ALLEGRO_BITMAP *img_atk[2];
    ALLEGRO_BITMAP *gun_pic;
    ALLEGRO_BITMAP *skill_pic[2];
    ALLEGRO_BITMAP *skill_gun_pic;
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    ALLEGRO_SAMPLE_INSTANCE *bomb_Sound;
    ALLEGRO_SAMPLE_INSTANCE *shout_Sound;
    int anime; // counting the time of animation
    int anime_time; // indicate how long the animation
    int gun_x, gun_y; // gun position
    int skill_x[2], skill_y[2];//技能球球位置
    int skill_vx[2], skill_vy[2];//技能球球速度
    double theta[2];//球的轉動俯角
    bool skill_state;
    int skill_time;
    int skill_time_count;
    int heart; // 血量
    int energy; // 能量
    bool invincible;
    int pos_x, pos_y;//mouse position
    float gun_vx, gun_vy; //gun speed
    int monster_x[3][3], monster_y[3][3];
    int monster_gun_x[3][3], monster_gun_y[3][3];//怪物子彈
    float monster_gun_vx[3][3], monster_gun_vy[3][3];
    ALLEGRO_BITMAP *hp, *half_hp, *mana;
}Character;

typedef struct monster
{
    int x[3][3], y[3][3]; // the position of image
    int vx[3][3], vy[3][3];
    ALLEGRO_BITMAP *monster[3][3];
    ALLEGRO_BITMAP *bomb;
    ALLEGRO_BITMAP *gun[3][3];
    ALLEGRO_SAMPLE_INSTANCE *bomb_Sound;
    int gun_x[3][3], gun_y[3][3];//怪物子彈
    float gun_vx[3][3], gun_vy[3][3];
    float theta[3][3];
    int state[3][3];
    int counter;
}Monster;

Monster mon[10];
Character chara;
Item item[9];
ALLEGRO_SAMPLE *sample = NULL;
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
    chara.skill_gun_pic = al_load_bitmap("./image/skill_gun.png");
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
    // load effective sound
    sample = al_load_sample("./sound/atk_sound.mp3");
    chara.atk_Sound  = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(chara.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(chara.atk_Sound, al_get_default_mixer());

    sample = al_load_sample("./sound/bomb.mp3");
    mon_bomb_Sound  = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(chara.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(mon_bomb_Sound, al_get_default_mixer());

    sample = al_load_sample("./sound/shout.mp3");
    chara.shout_Sound  = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(chara.shout_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(chara.shout_Sound, al_get_default_mixer());

    // initial the geometric information of character
    chara.width = al_get_bitmap_width(chara.img_move[0]);
    chara.height = al_get_bitmap_height(chara.img_move[0]);
    if (al_get_config_value(cfg, "", "god") != NULL)
        chara.invincible = true;
    else
        chara.invincible = false;
    chara.x = atoi(al_get_config_value(cfg, "", "chara.x"));
    chara.y = atoi(al_get_config_value(cfg, "", "chara.y"));
    chara.heart = atoi(al_get_config_value(cfg, "", "heart"));
    chara.energy = atoi(al_get_config_value(cfg, "", "energy"));
    chara.dir = false;

    //initial skill balls position
    chara.theta[0] = 0;
    chara.theta[1] = 3.1416;
    chara.skill_state = false;

    // initial the animation component
    chara.state = STOP;
    chara.anime = 0;
    chara.anime_time = 30;
    chara.skill_time = 0;
    chara.skill_time_count = -10000;
    //初始能量、生命值
    for (int k = 0; k <= 9; k++) {
        int count = k;
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                mon[k].x[i][j] = 200 * i + 150;
                mon[k].y[i][j] = 350 + 200*j;
                mon[k].theta[i][j] = (6.2832/9) * (3 * i + j);
                mon[k].vx[i][j] = 3 * cos(mon[k].theta[i][j]);
                mon[k].vy[i][j] = 3 * sin(mon[k].theta[i][j]);
                mon[k].gun_x[i][j] = 100 + 200*i;
                mon[k].gun_y[i][j] = 350 + 200*j;
                mon[k].state[i][j] = 0;
                if(count > 0)mon[k].state[i][j] = 1;
                count--;
            }
        }
    }
    health_pack = al_load_bitmap("./image/health_pack.png");
    energy_pack = al_load_bitmap("./image/energy_pack.png");
    for (int i = 1; i <= 8; i++) {
        item[i].x = (rand() % (400)) + 200;
        item[i].y = (rand() % (400)) + 400;
        item[i].type = (rand() % 2);
        item[i].taken = false;
    }
    for (int i = 1; i < 10; i++)
        mon[i].counter = 0;
}

void character_process(ALLEGRO_EVENT event){
    // process the animation
    if( event.type == ALLEGRO_EVENT_TIMER ){
        if( event.timer.source == fps ){
            chara.anime++;
            chara.anime %= chara.anime_time;
            chara.skill_time++;
        }
    // process the keyboard event
    }else if( event.type == ALLEGRO_EVENT_KEY_DOWN ){
        key_state[event.keyboard.keycode] = true;
        if(event.keyboard.keycode == ALLEGRO_KEY_SPACE){
        chara.skill_time_count = chara.skill_time;
        }
        chara.anime = 0;
    }else if( event.type == ALLEGRO_EVENT_KEY_UP ){
        if(event.keyboard.keycode != ALLEGRO_KEY_SPACE)key_state[event.keyboard.keycode] = false;
        key_state[ALLEGRO_KEY_SPACE] = true;
    }
    else if( event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN ){
        chara.gun_x = chara.x + 30;
        chara.gun_y = chara.y + 60;
        chara.pos_x = event.mouse.x;
        chara.pos_y = event.mouse.y;
        if( event.mouse.button == 1 ){
        chara.gun_vx = 10 * cos(atan2(chara.pos_y - chara.gun_y,chara.pos_x - chara.gun_x)); //計算子彈x分量的飛行速度
        chara.gun_vy = 10 * sin(atan2(chara.pos_y - chara.gun_y,chara.pos_x - chara.gun_x)); //計算子彈y分量的飛行速度
        mouse_state = true;
        }else if( event.mouse.button == 2 && chara.energy > 0){
        chara.gun_vx = 20 * cos(atan2(chara.pos_y - chara.gun_y,chara.pos_x - chara.gun_x)); //計算子彈x分量的飛行速度
        chara.gun_vy = 20 * sin(atan2(chara.pos_y - chara.gun_y,chara.pos_x - chara.gun_x)); //計算子彈y分量的飛行速度
        chara.energy--;
        mouse_state = true;
        }else if( event.mouse.button == 2 && chara.energy == 0){
         mouse_state = false;
        }
    }
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            mon[stage_map].gun_vx[i][j] = 2 * cos(atan2(chara.y + 40 - mon[stage_map].gun_y[i][j],chara.x + 25 - mon[stage_map].gun_x[i][j]));//計算怪獸子彈速度
            mon[stage_map].gun_vy[i][j] = 2 * sin(atan2(chara.y + 40 - mon[stage_map].gun_y[i][j],chara.x + 25 - mon[stage_map].gun_x[i][j]));//計算怪獸子彈速度
        }
    }
    ///////////////////////////////////////////////////////
    if (stage_state[stage_map] && !item[stage_map].taken) {
        if (chara.x + 50 > item[stage_map].x - 50 && chara.x + 50 < item[stage_map].x + 50 && chara.y + 50 > item[stage_map].y - 50 && chara.y + 50 < item[stage_map].y + 50) {
            if (item[stage_map].type == 0) {
                chara.heart += 5;
                if (chara.heart > 20)
                    chara.heart = 20;
            }
            else {
                chara.energy += 2;
                if (chara.energy > 5)
                    chara.energy = 5;
            }
            item[stage_map].taken = true;
        }
    }
    if (chara.heart == 0)
        judge_next_window = DEFEAT;
}

void character_update(){
    char temp[20];
    if(!stage_state[stage_map]){
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(mon[stage_map].x[i][j] <= 100 || mon[stage_map].x[i][j] >= WIDTH - 200)mon[stage_map].vx[i][j] = mon[stage_map].vx[i][j] * (-1);
                if(mon[stage_map].y[i][j] <= 327 || mon[stage_map].y[i][j] >= HEIGHT - 200)mon[stage_map].vy[i][j] = mon[stage_map].vy[i][j] * (-1);
                mon[stage_map].x[i][j] += mon[stage_map].vx[i][j];
                mon[stage_map].y[i][j] += mon[stage_map].vy[i][j];
                mon[stage_map].gun_x[i][j] +=  mon[stage_map].gun_vx[i][j];
                mon[stage_map].gun_y[i][j] +=  mon[stage_map].gun_vy[i][j];
                if((mon[stage_map].gun_x[i][j] - chara.x < 25 && mon[stage_map].gun_x[i][j] - chara.x > -25) && (mon[stage_map].gun_y[i][j] - chara.y < 25 && mon[stage_map].gun_y[i][j] - chara.y > -25)&& mon[stage_map].x[i][j] > 0 && mon[stage_map].x[i][j] < WIDTH && mon[stage_map].y[i][j] > 0 && mon[stage_map].y[i][j] < HEIGHT){
                al_set_sample_instance_gain(chara.shout_Sound, 1 * volume / 5.0);
                    al_play_sample_instance(chara.shout_Sound);
                    mon[stage_map].gun_x[i][j] = mon[stage_map].x[i][j] ;
                    mon[stage_map].gun_y[i][j] = mon[stage_map].y[i][j] ;
                    if (!chara.invincible)
                        chara.heart--;
                    //判斷是否被子彈打到
                }else if((mon[stage_map].gun_x[i][j] - chara.x < 95 && mon[stage_map].gun_x[i][j] - chara.x > -25) && (mon[stage_map].gun_y[i][j] - chara.y < 92 && mon[stage_map].gun_y[i][j] - chara.y > -32)&& (mon[stage_map].x[i][j] < 0 || mon[stage_map].x[i][j] > WIDTH || mon[stage_map].y[i][j] < 0 || mon[stage_map].y[i][j] > HEIGHT)){
                    al_set_sample_instance_gain(chara.shout_Sound, 1 * volume / 5.0);
                    al_play_sample_instance(chara.shout_Sound);
                    mon[stage_map].gun_x[i][j] = -100000;
                    mon[stage_map].gun_y[i][j] = -100000;
                }
                if(mon[stage_map].gun_x[i][j] - chara.gun_x < 25 && mon[stage_map].gun_x[i][j] - chara.gun_x > -25 && mon[stage_map].gun_y[i][j] - chara.gun_y < 25 && mon[stage_map].gun_y[i][j] - chara.gun_y > -25 && (mon[stage_map].x[i][j]<0 || mon[stage_map].x[i][j] > WIDTH || mon[stage_map].y[i][j] < 0 || mon[stage_map].y[i][j] > HEIGHT) ){
                    mon[stage_map].gun_x[i][j] = -100000;
                    mon[stage_map].gun_y[i][j] = -100000;
                    if (!chara.invincible)
                        chara.heart--;
                }
                if((mon[stage_map].gun_x[i][j] - chara.gun_x < 25 && mon[stage_map].gun_x[i][j] - chara.gun_x > -25) && (mon[stage_map].gun_y[i][j] - chara.gun_y < 25 && mon[stage_map].gun_y[i][j] - chara.gun_y > -25) && mon[stage_map].x[i][j] > 0 && mon[stage_map].x[i][j] < WIDTH && mon[stage_map].y[i][j] > 0 && mon[stage_map].y[i][j] < HEIGHT){
                    mon[stage_map].gun_x[i][j] = mon[stage_map].x[i][j] ;
                    mon[stage_map].gun_y[i][j] = mon[stage_map].y[i][j] ;
                }
                //子彈與子彈可互相抵消
            for(int k = 0 ; k < 2 ; k++){
                if(mon[stage_map].gun_x[i][j] - chara.skill_x[k] < 25 && mon[stage_map].gun_x[i][j] - chara.skill_x[k] > -25 && mon[stage_map].gun_y[i][j] - chara.skill_y[k] < 25 && mon[stage_map].gun_y[i][j] - chara.skill_y[k] > -25 &&(mon[stage_map].x[i][j] < 0 || mon[stage_map].x[i][j] > WIDTH || mon[stage_map].y[i][j] < 0 || mon[stage_map].y[i][j] > HEIGHT)){
                    mon[stage_map].gun_x[i][j] = -100000;
                    mon[stage_map].gun_y[i][j] = -100000;
                    }
                else if(mon[stage_map].gun_x[i][j] - chara.skill_x[k] < 25 && mon[stage_map].gun_x[i][j] - chara.skill_x[k] > -25 && mon[stage_map].gun_y[i][j] - chara.skill_y[k] < 25 && mon[stage_map].gun_y[i][j] - chara.skill_y[k] > -25 && mon[stage_map].x[i][j] > 0 && mon[stage_map].x[i][j] < WIDTH && mon[stage_map].y[i][j] > 0 && mon[stage_map].y[i][j] < HEIGHT){
                    mon[stage_map].gun_x[i][j] = mon[stage_map].x[i][j] ;
                    mon[stage_map].gun_y[i][j] = mon[stage_map].y[i][j] ;
                    }
                }
            }
        }
    }

    // use the idea of finite state machine to deal with different state
    if(chara.skill_time - chara.skill_time_count >=150)key_state[ALLEGRO_KEY_SPACE] = false;
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
        }else if(key_state[ALLEGRO_KEY_SPACE] && key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_A]){
        for(int i = 0 ; i < 2 ; i++){
            chara.skill_x[i] = chara.x + 30;
            chara.skill_y[i] = chara.y + 50;
            chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
            chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
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
            chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
            chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
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
            chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
            chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
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
            chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
            chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
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
            chara.skill_vx[i] = 120 *  cos(chara.theta[i]);//計算skill x分量的飛行速度
            chara.skill_vy[i] = 120 *  sin(chara.theta[i]);//計算skill y分量的飛行速度
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
    }else if(chara.skill_time - chara.skill_time_count <= 150 && key_state[ALLEGRO_KEY_SPACE]){
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
    }else if(key_state[ALLEGRO_KEY_W] && key_state[ALLEGRO_KEY_A]){
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
    }else if( mouse_state && chara.gun_x >= -42 && chara.gun_x <= WIDTH && chara.gun_y >= -42 && chara.gun_y <= HEIGHT){
        chara.gun_x += chara.gun_vx;
        chara.gun_y += chara.gun_vy;
        chara.state = ATK;
    }else if(chara.gun_x <= -42 || chara.gun_x >= WIDTH || chara.gun_y <= -42 || chara.gun_y >= HEIGHT){
        chara.state = STOP;
    }else if( chara.anime == chara.anime_time-1 ){
        chara.anime = 0;
        chara.state = STOP;
    }else if ( chara.anime == 0 ){
        chara.state = STOP;
    }else if( key_state[ALLEGRO_KEY_SPACE] ){
        chara.gun_x += 2*chara.gun_vx;
        chara.gun_y += 2*chara.gun_vy;
        chara.state = SKILL;
    }
    if(chara.gun_y <= 200)chara.gun_y = -100;
    if(chara.x <= 75) //*change*
        chara.x = 75; //*change*
    if(chara.x >= WIDTH - 200)
        chara.x = WIDTH - 200;
    if(chara.y < 300)
        chara.y = 300;
    if(chara.y >= HEIGHT - 200)
        chara.y = HEIGHT - 200; //建立邊界

    switch(stage_map) {
        case 1: {
            if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map++;
                stage_change = 1;
                chara.x = 80;
                chara.y = 560;
            }

            break;
        }
        case 2: {
            if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map++;
                stage_change = 1;
                chara.x = 80;
                chara.y = 560;
            }
            else if (chara.x == 75 && chara.y > 540 && chara.y < 570) {
                stage_map--;
                stage_change = 1;
                chara.x = 590;
                chara.y = 560;
            }

            break;
        }
        case 3: {
            if (chara.x > 320 && chara.x < 340 && chara.y == 300) {
                stage_map++;
                stage_change = 1;
                chara.x = 340;
                chara.y = 790;
            }
            else if (chara.x == 75 && chara.y > 540 && chara.y < 570) {
                stage_map--;;
                stage_change = 1;
                chara.x = 590;
                chara.y = 560;
            }

            break;
        }
        case 4: {
            if (chara.x > 330 && chara.x < 350 && chara.y == 300) {
                stage_map++;
                stage_change = 1;
                chara.x = 340;
                chara.y = 790;
            }
            else if (chara.x > 330 && chara.x < 350 && chara.y == 800) {
                stage_map--;
                stage_change = 1;
                chara.x = 330;
                chara.y = 310;
            }

            break;
        }
        case 5: {
            if (chara.x == 75 && chara.y > 550 && chara.y < 570) {
                stage_map++;
                stage_change = 1;
                chara.x = 555;
                chara.y = 590;
            }
            else if (chara.x > 330 && chara.x < 350 && chara.y == 800) {
                stage_map--;
                stage_change = 1;
                chara.x = 340;
                chara.y = 310;
            }

            break;
        }
        case 6: {
            if (chara.x == 75 && chara.y > 540 && chara.y < 560) {
                stage_map++;
                stage_change = 1;
                chara.x = 590;
                chara.y = 555;
            }
            else if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map--;
                stage_change = 1;
                chara.x = 85;
                chara.y = 560;
            }

            break;
        }
        case 7: {
            if (chara.x > 350 && chara.x < 360 && chara.y == 800) {
                stage_map++;
                stage_change = 1;
                chara.x = 345;
                chara.y = 310;
            }
            else if (chara.x == 600 && chara.y > 540 && chara.y < 570) {
                stage_map--;
                stage_change = 1;
                chara.x = 85;
                chara.y = 550;
            }

            break;
        }
        case 8: {
            if (chara.x == 600 && chara.y > 530 && chara.y < 560) {
                stage_map++;
                stage_change = 1;
                chara.x = 85;
                chara.y = 545;
            }
            else if (chara.x > 335 && chara.x < 355 && chara.y == 300) {
                stage_map--;
                stage_change = 1;
                chara.x = 355;
                chara.y = 790;
            }

            break;
        }
        case 9: {
            if (chara.x == 75 && chara.y > 530 && chara.y < 560) {
                stage_map--;
                stage_change = 1;
                chara.x = 590;
                chara.y = 545;
            }

            break;
        }
    }

    itoa(chara.x, temp, 10);
    al_set_config_value(cfg, "", "chara.x", temp);
    itoa(chara.y, temp, 10);
    al_set_config_value(cfg, "", "chara.y", temp);
    itoa(chara.heart, temp, 10);
    al_set_config_value(cfg, "", "heart", temp);
    itoa(chara.energy, temp, 10);
    al_set_config_value(cfg, "", "energy", temp);
}

void character_draw(){
    // with the state, draw corresponding image

    if( !stage_state[stage_map]){
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(mon[stage_map].state[i][j] == 1){
                al_draw_bitmap(mon[stage_map].monster[i][j], mon[stage_map].x[i][j] , mon[stage_map].y[i][j], 0);
                al_draw_bitmap(mon[stage_map].gun[i][j], mon[stage_map].gun_x[i][j], mon[stage_map].gun_y[i][j], 0);
                }
            }
        }
    }
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
                al_set_sample_instance_gain(chara.atk_Sound, 1 * volume / 5.0);
                al_play_sample_instance(chara.atk_Sound);
            }
        }else{
            if( chara.anime < chara.anime_time/2 ){
                al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, 0);
            }else{
                al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, 0);
                al_set_sample_instance_gain(chara.atk_Sound, 1 * volume / 5.0);
                al_play_sample_instance(chara.atk_Sound);
            }
        }
        al_draw_bitmap(chara.gun_pic, chara.gun_x, chara.gun_y, 0);
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(mon[stage_map].state[i][j] == 1 && mon[stage_map].x[i][j] - chara.gun_x < 0 && mon[stage_map].x[i][j] - chara.gun_x > -50 && mon[stage_map].y[i][j] - chara.gun_y < 0 && mon[stage_map].y[i][j] - chara.gun_y >-50 ){
                    al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
                    al_set_sample_instance_gain(mon_bomb_Sound, 1 * volume / 5.0);
                    al_play_sample_instance(mon_bomb_Sound);
                    mon[stage_map].x[i][j] = -100000;
                    mon[stage_map].y[i][j] = -100000;
                    mon[stage_map].counter++;
                    mon[stage_map].state[i][j] = 0;
                    chara.gun_x = -100000;
                    chara.gun_y = -100000;
                    al_draw_bitmap(mon[stage_map].bomb, chara.gun_x, chara.gun_y, 0);
                    //如果怪獸被打到，移到遠處
                }
                //如果怪獸全死，stage的狀態為true
            }
        }
    }else if(chara.state == SKILL){
            if( chara.anime < chara.anime_time/2 ){
                al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
            }else{
                al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
                al_play_sample_instance(chara.atk_Sound);
            }
            for(int i = 0 ; i < 2 ; i++){
                al_draw_bitmap(chara.skill_pic[i], chara.skill_x[i], chara.skill_y[i], 0);
            }
            for(int i=0;i<3;i++){
                for(int j=0;j<3;j++){
                        for(int k = 0 ; k < 2 ; k++){
                            if(chara.skill_x[k] - mon[stage_map].x[i][j] < 100 && chara.skill_x[k] - mon[stage_map].x[i][j] > -50 && chara.skill_y[k] - mon[stage_map].y[i][j] < 90 && chara.skill_y[k] - mon[stage_map].y[i][j] > -50 ){
                            mon[stage_map].x[i][j] = -100000;
                            mon[stage_map].y[i][j] = -100000;
                            mon[stage_map].state[i][j] = 0;
                            mon[stage_map].counter++;
                            }
                        }
                    }
                }
    }else if (chara.state == MOVE_SKILL){
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
            for(int i = 0 ; i < 2 ; i++){
                al_draw_bitmap(chara.skill_pic[i], chara.skill_x[i], chara.skill_y[i], 0);
            }
            for(int i=0;i<3;i++){
                for(int j=0;j<3;j++){
                        for(int k = 0 ; k < 2 ; k++){
                            if(chara.skill_x[k] - mon[stage_map].x[i][j] < 20 && chara.skill_x[k] - mon[stage_map].x[i][j] > -20 && chara.skill_y[k] - mon[stage_map].y[i][j] < 20 && chara.skill_y[k] - mon[stage_map].y[i][j] > -20 ){
                            mon[stage_map].x[i][j] = -100000;
                            mon[stage_map].y[i][j] = -100000;
                            mon[stage_map].counter++;
                            mon[stage_map].state[i][j] = 0;
                            }
                        }
                    }
                }
    }else if( chara.state == MOVE_ATK ){
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
                al_play_sample_instance(chara.atk_Sound);
            }
        }else{
            if( chara.anime < chara.anime_time/2 ){
                al_draw_bitmap(chara.img_atk[0], chara.x, chara.y, 0);
            }else{
                al_draw_bitmap(chara.img_atk[1], chara.x, chara.y, 0);
                al_play_sample_instance(chara.atk_Sound);
            }
        }
        al_draw_bitmap(chara.gun_pic, chara.gun_x, chara.gun_y, 0);
    }
    for (int i = 0; i < chara.heart / 2; i++)
        al_draw_bitmap(chara.hp, 20 + 60 * i, 20, 0);
    if (chara.heart & 1)
        al_draw_bitmap(chara.half_hp, 20 + 60 * ((chara.heart - 1) / 2), 20, 0);
    for (int i = 0; i < chara.energy; i++)
        al_draw_bitmap(chara.mana, 20 + 60 * i, 80, 0);
    if (mon[stage_map].counter >= stage_map)
        stage_state[stage_map] = true;
    if (stage_state[stage_map] == true && !item[stage_map].taken) {
        if (item[stage_map].type == 0)
            al_draw_bitmap(health_pack, item[stage_map].x - 50, item[stage_map].y - 50, 0);
        else
            al_draw_bitmap(energy_pack, item[stage_map].x - 50, item[stage_map].y - 50, 0);
    }
}
void character_destory(){
    al_destroy_bitmap(chara.img_atk[0]);
    al_destroy_bitmap(chara.img_atk[1]);
    al_destroy_bitmap(chara.img_move[0]);
    al_destroy_bitmap(chara.img_move[1]);
    al_destroy_sample_instance(chara.atk_Sound);
}
