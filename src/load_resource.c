/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

/*
 * Includes
 */
#include "main.h"

SDL_RWops *loadresource(char *filename)
{
    SDL_RWops *ops;
    char *filetarget;
    int length, i;

    filetarget = malloc(length = ((strlen("resources\\") + strlen(filename) + 1) * sizeof(char)));

    for(i = 0; i < length; i++)
    {
        filetarget[i] = 0;
    }

    strcat(filetarget, "resources\\");
    strcat(filetarget, filename);

    ops = SDL_RWFromFile(filetarget, "rb");

    free(filetarget);

    return ops;
}

SDL_Surface* load_image(char* filename, char* hashtab_name)
{
    SDL_RWops *rwops;
    SDL_Surface *surface;

    rwops = loadresource(filename);

    if (rwops)
    {
        if (filename[strlen(filename)-3] == 'j')
        {
            surface = IMG_LoadJPG_RW(rwops);
        }
        else
        {
            surface = IMG_LoadPNG_RW(rwops);
        }

        SDL_FreeRW(rwops);
    }
    else
    {
        return NULL;
    }

    if (hashtab_name && hashtab_name[0] != '\0')
    {
        game->set(hashtab_name, surface);
    }

    return surface;
}
