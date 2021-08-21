#include "main.h"

/**
 * Draw a sprite to a surface
 * @param SDL_Surface *screen Surface to draw to.
 * @param sprite *active Sprite in question.
 * @param point *viewport Viewport starting coordinates.
 */
void draw_sprite(SDL_Surface *screen, sprite *active, point *viewport)
{
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    src_rect.w = active->sprite_size.w;
    src_rect.h = active->sprite_size.h;
    src_rect.x = active->sprite_size.x+(active->frame*active->sprite_size.w);
    src_rect.y = active->sprite_size.y;

    dst_rect.w = active->sprite_size.w;
    dst_rect.h = active->sprite_size.h;
    dst_rect.x = (int16_t)round(active->position.x - viewport->x);
    dst_rect.y = (int16_t)round(active->position.y - viewport->y);

    SDL_BlitSurface(active->spritesheet, &src_rect, screen, &dst_rect);
}
