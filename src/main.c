/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

/*
 * Includes
 */
#include "main.h"
#include <time.h>

/*
 * Our global game state live here
 */
game_state* game;

/*
 * Function prototypes
 */
static void init();
static void cleanup();

extern Sint8 init_fonts();
extern level_st* load_level(char*);

/*
 * Functions
 */

/*
 * Main -- it all starts here
 */
int main(int argc, char *argv[])
{
    int ui_response = 0;

    /* Perform Setup */
    game = (game_state*)malloc(sizeof(game_state));

    if (game == NULL)
    {
        exit(EXIT_FAILURE);
    }

    init();

    /* Run the game */
    while (game->running)
    {
        /* Grab input */
        while (SDL_PollEvent(game->event))
        {
            switch (game->event->type)
            {
                case SDL_QUIT:
                    exit(EXIT_SUCCESS);
                    break;

                default:
                    ui_response = game->active_ui(UI_INPUT);
                    break;
            }
        }

        /*If UI response tells us to exit the program, we should.*/
        if(ui_response == 1)
        {
            break;
        }

        /* Update game */
        ui_response = game->active_ui(UI_RENDER);

        /* Render */
        if (SDL_MUSTLOCK(game->screen))
        {
            if (SDL_LockSurface(game->screen) == -1)
            {
                SDL_Delay(10);
                continue;
            }
        }

        SDL_Flip(game->screen);

        if (SDL_MUSTLOCK(game->screen))
        {
            //We've gotten here... that means the screen is locked.
            //We should unlock it.
            SDL_UnlockSurface(game->screen);
        }

        /* Give up the CPU */
        SDL_Delay(10);
    }

    /* Cleanup */
    cleanup();

    return EXIT_SUCCESS;
}

/*
 * Init -- Perform system and game setup
 */
void init( void )
{
    Uint32 video_flags = SDL_VIDEO_FLAGS;

    /* If this fails there is no reason to proceed */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		exit(EXIT_FAILURE);
	}

    /* Check to see if we have some VRAM to use */
    const SDL_VideoInfo* video_info = SDL_GetVideoInfo();

	if (video_info->hw_available)
	{
		video_flags |= SDL_HWSURFACE;
	}
	else
	{
		video_flags |= SDL_SWSURFACE;
	}

    /* Make sure we cleanup when this program stops */
	atexit(SDL_Quit);

	/* Create our main surface */
	game->screen = SDL_SetVideoMode(WINDOW_WIDTH,
                                    WINDOW_HEIGHT,
                                    WINDOW_DEPTH,
                                    video_flags);

	if (game->screen == NULL)
	{
        SDL_Quit();
        exit(EXIT_FAILURE);
	}

	/* Setup Window Caption */
	SDL_WM_SetCaption(WINDOW_NAME, NULL);

    if (TTF_Init() < 0)
    {
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    /*Initialize the game hashtable*/
    hashtabsystem_init();

    game->running = true;
    game->mem = init_hashtab(512);
    game->get = &default_hashtab_get;
    game->unset = &default_hashtab_unset;
    game->set = &default_hashtab_set;
    game->load = &default_hashtab_loadresource;
    set_default_hashtab(game->mem);

    game->event = (SDL_Event*)malloc(sizeof(SDL_Event));
    game->active_ui = &title_menu_ui;

    /* Set top level list pointers */
    game->first_sprite = NULL;
    game->last_sprite = NULL;
    game->first_level = NULL;
    game->last_paused_time = 0;
    game->paused_time = 0;

    /* Initialize Fonts */
    if (init_fonts() < 0)
    {
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    /* Initialize Sound */
    if (init_sound() < 0)
    {
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    /* Seed random number generator */
    srand(time(NULL));

    /*Initialize UIs*/
    level_ui_load_resources(game);
    menu_ui_resource_load();

    /* Load JSON levels */
    load_levels();
}

/*
 * Cleanup -- Safely release resources for shutdown
 */
void cleanup()
{
    if (game->event)
    {
        free(game->event);
        game->event = NULL;
    }

    SDL_CloseAudio();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

/*
 * random_uint32 -- get random integer within a range
 */
uint32_t random_uint32(uint32_t low, uint32_t high)
{
    return (rand() % high-low+1)+low;
}
