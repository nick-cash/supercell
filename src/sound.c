/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

/*
 * Includes
 */
#include "main.h"
#include "sound.h"

static sound_st sounds[NUM_SOUNDS];

/*
 * Function Prototypes / Externs
 */
static void load_audio_resources();

/*
 * Functions
 */

/*
 * Beep bob boop -- returns true if sound was played
 */
bool play_sound(char* sound_name)
{
    int index;
    wav_st* sound;

    if (game->active_ui == &pause_ui)
        return false;

    if ((sound = game->get(sound_name)) == NULL)
    {
        return false;
    }

    /* make sure this sound isn't already playing */
    for (index = 0; index < NUM_SOUNDS; ++index)
    {
        if (sounds[index].data == sound->buffer &&
            sounds[index].data_len != 0)
            return false;
    }

    /* Look for an empty (or finished) sound slot */
    for (index = 0; index < NUM_SOUNDS; ++index )
    {
        if (sounds[index].data_len == 0)
        {
            break;
        }
    }

    if (index == NUM_SOUNDS)
        return false;

    /* Put the sound data in the slot (it starts playing immediately) */
    SDL_LockAudio();
    sounds[index].data = sound->buffer;
    sounds[index].data_len = sound->length;
    sounds[index].data_pos = sound->buffer;
    SDL_UnlockAudio();

    return true;
}

void sound_mixer(void *unused, Uint8 *stream, int len)
{
    uint8_t i;

    if (game->active_ui == &pause_ui)
        return;

    for (i = 0; i < NUM_SOUNDS; ++i)
    {
        /* Only play if we have data left */
        if ( sounds[i].data_len == 0 )
            continue;

        /* Mix as much data as possible */
        len = (len > sounds[i].data_len ? sounds[i].data_len : len);

        SDL_MixAudio(stream, sounds[i].data_pos, len, SDL_MIX_MAXVOLUME);

        sounds[i].data_pos += len;
        sounds[i].data_len -= len;
    }
}

int8_t init_sound()
{
    SDL_AudioSpec fmt;
    uint8_t i;

    /* 16-bit stereo at 44Khz */
    fmt.freq = 44100;
    fmt.format = AUDIO_S16SYS;
    fmt.channels = 2;
    fmt.samples = 1024;
    fmt.callback = &sound_mixer;
    fmt.userdata = NULL;

    if (SDL_OpenAudio(&fmt, NULL) < 0)
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        return -1;
    }

    load_audio_resources();

    /* Initialize our sound buffer*/
    for (i = 0; i < NUM_SOUNDS; ++i)
    {
        sounds[i].data = NULL;
        sounds[i].data_pos = 0;
        sounds[i].data_len = 0;
    }

    /* we can haz da sound */
    SDL_PauseAudio(0);

    play_sound("main-song");

    return 0;
}

void load_audio_resources()
{
    wav_st* newsound;

    /* Pause Sound */
    newsound = (wav_st*)malloc(sizeof(wav_st));
    newsound->buffer = NULL; //Initialize buffer pointer to zero, just to be safe,

    if (SDL_LoadWAV("resources\\sounds\\pause1.wav", &newsound->spec, &newsound->buffer, &newsound->length) == NULL)
    {
        fprintf(stderr,"Could not open pause1.wav! %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    game->set("pause-sound", newsound);

    /* Unpause Sound */
    newsound = (wav_st*)malloc(sizeof(wav_st));
    newsound->buffer = NULL; //Initialize buffer pointer to zero, just to be safe,

    if (SDL_LoadWAV("resources\\sounds\\pause.wav", &newsound->spec, &newsound->buffer, &newsound->length) == NULL)
    {
        fprintf(stderr,"Could not open pause.wav! %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    game->set("unpause-sound", newsound);

    /* song */
    newsound = (wav_st*)malloc(sizeof(wav_st));
    newsound->buffer = NULL; //Initialize buffer pointer to zero, just to be safe,

    if (SDL_LoadWAV("resources\\sounds\\song1.wav", &newsound->spec, &newsound->buffer, &newsound->length) == NULL)
    {
        fprintf(stderr,"Could not open song1.wav! %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    game->set("main-song", newsound);

    /* bubble */
    newsound = (wav_st*)malloc(sizeof(wav_st));
    newsound->buffer = NULL; //Initialize buffer pointer to zero, just to be safe,

    if (SDL_LoadWAV("resources\\sounds\\bubble.wav", &newsound->spec, &newsound->buffer, &newsound->length) == NULL)
    {
        fprintf(stderr,"Could not open bubble.wav! %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    game->set("bubble-sound", newsound);
}
