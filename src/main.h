/*
 * Ludum Dare Jam 23 - 4/20/2012
 */
#ifndef INC_MAIN_H
#define INC_MAIN_H

/*
 * Standard Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_Image.h>
#include <SDL/SDL_ttf.h>
#include <math.h>

/*
 * Local Includes
 */
#include "hashtab.h"
#include "level.h"
#include "sound.h"
#include "behaviors.h"

/*
 * Defines
 */
#define WINDOW_WIDTH        800
#define WINDOW_HEIGHT       600
#define WINDOW_DEPTH        32
#define WINDOW_NAME         "Supercell"
#define WINDOW_FULLSCREEN   FALSE
#define SDL_VIDEO_FLAGS     SDL_HWPALETTE | SDL_DOUBLEBUF

#define DEFAULT_FONT        "Sansation_Regular.ttf"
#define MENU_FONT           "menu-font"
#define TIME_FONT           "time-font"

/*
 * Main game structure
 */
typedef struct
{
    SDL_Event*        event;
    SDL_Surface*      screen;
    hashtab*          mem;
    bool              running;
    sprite*           first_sprite;
    sprite*           last_sprite;
    level_st*         first_level;
    uint32_t          paused_time; //Keep track of total paused time.
    uint32_t          last_paused_time; //A place to keep track of the start of a pause session.

    int     (*active_ui)(int ui_action);
    void*   (*get)  (char *key);
    void    (*set)  (char *key, void *value);
    void    (*unset)(char *key, Uint8 freevalue);
    void*   (*load) (char *key);
} game_state;

/*
 * Externs
 */
extern game_state* game;
extern const float speed_base_ms;

/*
 * Global Function Prototypes
 */
extern SDL_RWops *loadresource(char *);
extern SDL_Surface* load_image(char*, char*);
extern uint32_t random_uint32(uint32_t, uint32_t);

/*
 * UI Handlers
 */
int level_ui(int ui_action);
int level_ui_load_resources();
int pause_ui(int ui_action);
int menu_ui_resource_load();
int title_menu_ui(int ui_action);
int inst_pause_menu_ui(int ui_action);
int inst_title_menu_ui(int ui_action);
int necromancy_ui(int ui_action);
int sheen_ui(int ui_action);

#define UI_INPUT 1
#define UI_RENDER 2

#endif /* INC_MAIN_H */
