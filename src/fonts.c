/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

/*
 * Includes
 */
#include "main.h"

/*
 * Functions
 */

/**
 * Loads all fonts used in the game. Pre-renders all glyphs for use in dialogues.
 * NOTE: If we need to load more fonts, we should refactor this...
 * @return 0 on success, non-zero on error
 */
Sint8 init_fonts()
{
    TTF_Font* default_font;
    TTF_Font* menu_font;
    TTF_Font* time_font;
    SDL_Surface* glyph = NULL;
    char* buf = NULL;
    Uint16 ch;
    SDL_Color white = {255,255,255};

    default_font = TTF_OpenFontRW(loadresource(DEFAULT_FONT), 1, 18);
    menu_font = TTF_OpenFontRW(loadresource(DEFAULT_FONT), 1, 32);
    time_font = TTF_OpenFontRW(loadresource(DEFAULT_FONT), 1, 24);

    if (!default_font)
    {
        return -1;
    }

    if (!menu_font)
    {
        return -1;
    }

    if (!time_font)
    {
        return -1;
    }

    game->set(DEFAULT_FONT, default_font);
    game->set(MENU_FONT, menu_font);
    game->set(TIME_FONT, time_font);

    /*
     * Pre-render all printable characters. 32 = space, 126 = ~
     */
    buf = malloc(strlen(DEFAULT_FONT)+3);

    for (ch = 32; ch <= 126; ++ch)
    {
        glyph = TTF_RenderGlyph_Blended(default_font, ch, white);

        if (!glyph)
        {
            return -2;
        }

        sprintf(buf, "%s_%c", DEFAULT_FONT, ch);
        game->set(buf, glyph);
    }

    free(buf);

    return 0;
}
