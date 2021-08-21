// Main Header File
#ifndef INC_ZB2
#define INC_ZB2

// Standard Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_Image.h>
#include <SDL/SDL_ttf.h>

// Defines
#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32

// Hash table stuff
struct hashlist_el
{
    char *name;
    void *value;
    struct hashlist_el *next;
};

typedef struct hashlist_el hashlist;

typedef struct
{
    int tabsize;
    hashlist **table;
} hashtab;


/**
 * struct zb2_game_state
 * Game state structure that keeps track of current input and drawing responsibilities.
 * I'm making this a structure though and passing it to the active ui handler so that it can
 * pass data back to the main function should it end up being necessary.  To change ui handler,
 * update active_ui. (And if you have long running processes, maybe clear the event queue.)
 */
typedef struct
{
    int (*active_ui)(void *zb2);
    SDL_Event *event;
    SDL_Surface *screen;
    Uint8 active; //Whether or not this call of the UI actually has an event to process.
    hashtab *mem;
    void * (*get) (char *key);
    void (*set) (char *key, void *value);
    void (*unset)(char *key, Uint8 freevalue);
    void * (*load) (char *key);
    /*We should add some kind of way to add cleanup functions to run at exit.*/
} zb2_game_state;

hashtab *init_hashtab(int table_size);
uint32_t chksum_crc32(unsigned char *block, unsigned int length);
void  hashtabsystem_init();
void  hashtab_set(hashtab *table, char *key, void *value);
void* hashtab_get(hashtab *table, char *key);
void  hashtab_unset(hashtab *table, char *key, Uint8 freevalue);
void  set_default_hashtab(hashtab *table);

/*
 * Convenience functions for use with zb2_game_state structures.
 * After setting a default hashtable, these can be used directly from a
 * properly initialized structure, almost like a class call.
 */
void  default_hashtab_set(char *key, void *value);
void  default_hashtab_unset(char *key, Uint8 freevalue);
void* default_hashtab_get(char *key);
void* default_hashtab_loadresource(char* key);

/* startscreen.c */
int start_screen_ui(void *zb2_state_pointer);

/* dialogue.c */
int dialogue_ui(void *zb2_state_ptr);

/* main.c */
void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b);
void test_sdl(SDL_Surface* screen, int h);
int main(int argc, char* argv[]);
SDL_RWops *loadresource(char *filename);

/* init.c */
void setup(zb2_game_state* zb2);
void cleanup();

#endif /* INC_ZB2 */
