#ifndef LEVEL_H
#define LEVEL_H

#include "SDL/SDL.h"

#define OBJECT_STATUS_NONEXIST 0 //Not malloced
#define OBJECT_STATUS_EXIST 1
#define OBJECT_STATUS_CARRIED 2 //Render together with player
#define OBJECT_STATUS_THROWED 3
#define OBJECT_STATUS_HIT 4
#define OBJECT_STATUS_GONE 5 //Skip
#define ENEMY_STATUS_NONEXIST 0 //Not malloced
#define ENEMY_STATUS_EXIST 1
#define ENEMY_STATUS_CARRIED 2 //Render together with player
#define ENEMY_STATUS_THROWED 3
#define ENEMY_STATUS_HIT 4
#define ENEMY_STATUS_DYING 5#define ENEMY_STATUS_DEAD 6 //Skip

#define PLAYER_STATUS_EXIST 1
#define PLAYER_STATUS_CARRYING 2 //Render together with object
#define PLAYER_STATUS_CLIMBING 3#define PLAYER_STATUS_FINISHED 4
#define PLAYER_STATUS_DYING 5#define PLAYER_STATUS_DEAD 6
#define PLAYER_STATUS_GAMEOVER 7

typedef struct {
    SDL_Surface *tiledata; //Malloced
    unsigned char horizflag;
    unsigned char floorflag;
    unsigned char ceilflag;
    SDL_Surface *current; //Not malloced; just a pointer to the current (animated) surface
    unsigned char animcycle;
} TITUS_tile;

typedef struct {
    unsigned int number;
    unsigned char supporting;
    unsigned char flash;
    unsigned char flipped;
    SDL_Surface *spritedata; //Malloced
} TITUS_sprite;

typedef struct {
    unsigned char status;
    TITUS_sprite sprite;
    unsigned char animcycle;
    int x;
    int y;
    int initx;
    int inity;
    unsigned char initanimcycle;
    int initstatus;
} TITUS_object;

typedef struct {
    unsigned char status;
    TITUS_sprite sprite;
    unsigned char animcycle;
    int x;
    int y;
    unsigned int type;
    int speedx;
    unsigned char speedy;
    unsigned char hp;
    int power;
    int centerx;
    unsigned int rangex;
    unsigned int delay;
    unsigned char direction;
    unsigned int rangey;
    int initx;
    int inity;
    int initstatus;
    unsigned char initanimcycle;
    unsigned char inithp;
} TITUS_enemy;

typedef struct {
    unsigned char exists;
    unsigned char replacetile;
    int x;
    int y;
} TITUS_bonus;

typedef struct {
    unsigned char exists;
    unsigned int entrancex;
    unsigned int entrancey;
    unsigned int exitx;
    unsigned int exity;
    unsigned int screenx;
    unsigned int screeny;
    unsigned char scrolling;
} TITUS_gate;

typedef struct {
    unsigned char exists;
    TITUS_sprite sprite;
    int x;
    int y;
    unsigned char speed;
    unsigned int range;
    unsigned char direction;
    int initx;
    int inity;
} TITUS_elevator;

typedef struct {
    unsigned char status;
    TITUS_sprite sprite;
    unsigned char animcycle;
    int x;
    int y;
    unsigned char speed;
    unsigned char direction;
    unsigned char hp;
    TITUS_object *carry_object; //Pointer to the object the player carries; NULL if no object
    int initx;
    int inity;
    unsigned char inithp;
    unsigned char keyleft;
    unsigned char keyright;
    unsigned char keyup;
    unsigned char keydown;
    unsigned char keyspace;
} TITUS_player;

typedef struct {
    unsigned char levelnumber;
    char title[41];
    int height;
    int width; //Usually 256
    unsigned char **tilemap; //Malloced, usually 256 * height
    TITUS_tile tile[256];
    SDL_Surface **spritedata; //Not malloced (on an original level); pointer to a global spritedata variable
    int spritecount;
    int finishx, finishy;
    int screenx, screeny;
    unsigned char scrolling;
    int lives, extrabonus;
    TITUS_enemy *boss; //Pointer to the boss; NULL if there is no boss
    TITUS_object *finish_object; //Pointer to the required object to carry to finish; NULL if there is no such object
    SDL_PixelFormat *pixelformat; //Malloced

    TITUS_player player;

    TITUS_object *object; //Malloced
    int objectcount;
    TITUS_enemy *enemy; //Malloced
    int enemycount;
    TITUS_bonus *bonus; //Malloced
    int bonuscount;
    TITUS_gate *gate; //Malloced
    int gatecount;
    TITUS_elevator *elevator; //Malloced
    int elevatorcount;

    int vscrollimit;
    int hscrollimit;
} TITUS_level;

int loadlevel(TITUS_level *level, unsigned char *leveldata, int leveldatasize, SDL_Surface **sprites);
int freelevel(TITUS_level *level);
int loadpixelformat(SDL_PixelFormat **pixelformat);
int freepixelformat(SDL_PixelFormat **pixelformat);

#endif
