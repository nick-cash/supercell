/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

/*
 * Defines
 */
#define NUM_SOUNDS     2

/*
 * Structures and typedefs
 */
typedef struct wav_st
{
    SDL_AudioSpec spec;
    uint32_t      length;
    uint8_t*      buffer;
} wav_st;

typedef struct sound_st
{
    uint8_t*      data;
    uint8_t*      data_pos;
    uint32_t      data_len;
} sound_st;

/*
 * Function Prototypes / Externs
 */
extern int8_t init_sound();
extern bool play_sound(char*);

#endif // SOUND_H_INCLUDED
