/*
 * Ludum Dare Jam 23 - 4/20/2012
 */
#include "main.h"
#include "level.h"
#include "sprite.h"

/*
 * Our global sprite list lives here
 */
sprite_el* first_sprite;

/*
 * Function prototypes
 */
static void kill_sprite(sprite*);
static bool sprite_exists(sprite*);

/*
 * Functions
 */
void update_all_sprites()
{
    sprite* spr, *nextspr;
    point* viewport_velocity = game->get("lvl_viewport_velocity");
    point* viewport_position = game->get("lvl_viewport_position");
    uint32_t time_since_last_render, nowticks;
    uint32_t* lastticks;
    float vel_time;

    /* Grab last ticks */
    lastticks = game->get("lvl_lastticks");
    if(lastticks == NULL)
    {
        lastticks = malloc(sizeof(uint32_t));
        (*lastticks) = SDL_GetTicks() - game->paused_time;
    }
    nowticks = SDL_GetTicks() - game->paused_time;

    nextspr = game->first_sprite;
    while(nextspr)
    {
        spr = nextspr;
        nextspr = spr->next;
        if(spr->type != SPRITE_PLAYER &&
           (nowticks - spr->timespawn) > spr->ttl )
        {
            kill_sprite(spr);
        }
    }

    time_since_last_render = nowticks - (*lastticks);

    /*Let's do behaviors before we set lastticks to equal nowticks.*/
    for (spr = game->first_sprite; spr != NULL; spr = spr->next)
    {
        /*Perform behaviors*/
        if(spr->behavior != NULL)
        {
            spr->behavior((*lastticks), nowticks, spr);
        }
    }

    (*lastticks) = nowticks;
    game->set("lvl_lastticks", lastticks);

    /* update viewport */
    viewport_position->x = viewport_position->x + (viewport_velocity->x*((float)(time_since_last_render)) /(speed_base_ms));
    viewport_position->y = viewport_position->y + (viewport_velocity->y*((float)(time_since_last_render)) /(speed_base_ms));

    vel_time = (float)time_since_last_render /(speed_base_ms);

    /* Run updates */
    for (spr = game->first_sprite; spr != NULL; spr = spr->next)
    {
        /* handle projectiles */
        if ((spr->firing == true) && (spr->firing_delay > 0) &&
            (nowticks >= (spr->last_shot_time+spr->firing_delay)))
        {
            sprite_type type = (spr->type == SPRITE_PLAYER ? SPRITE_PLAYER_BULLET : SPRITE_ENEMY_BULLET);
            point pos = spr->position;
            point velo;

            pos.x += spr->sprite_size.w/2;
            pos.y += spr->sprite_size.h/2;

            if (spr->type == SPRITE_PLAYER)
            {
                int32_t mx, my;
                point mouse_pos;
                point* viewport_position = game->get("lvl_viewport_position");

                SDL_GetMouseState(&mx, &my);

                mouse_pos.x = mx + viewport_position->x;
                mouse_pos.y = my + viewport_position->y;

                velo.x = (mouse_pos.x - pos.x)/10;
                velo.y = (mouse_pos.y - pos.y)/10;
            }

            spr->last_shot_time = nowticks;

            make_sprite(type, pos, velo);

            if (spr->type == SPRITE_PLAYER)
            {
                //play_sound("player-shoot-sound");
            }
        }

        if (spr->type == SPRITE_PLAYER || spr->type == SPRITE_PLAYER_BULLET)
        {
            /*Viewport velocity is add to player velocity*/
            spr->position.x = spr->position.x + (spr->vel.x /*+ viewport_velocity->x)*/ * vel_time);
            spr->position.y = spr->position.y + (spr->vel.y /*+ viewport_velocity->y)*/ * vel_time);
        }
        else
        {
            spr->position.x += (spr->vel.x*vel_time);
            spr->position.y += (spr->vel.y*vel_time);
        }

        /* Update Frames */
        if (spr->spritesheet_revert_time != 0)
        {
            if (--spr->spritesheet_revert_time == 0)
            {
                spr->spritesheet = game->get("maindude");
                spr->num_frames = 3;

                if (spr->frame > 3)
                {
                    spr->frame = 0;
                }
            }
        }

        if (spr->num_frames > 1)
        {
            if (nowticks >= (spr->frame_delay + spr->last_frame_time))
            {
                spr->last_frame_time = nowticks;

                if (++spr->frame >= spr->num_frames)
                {
                    spr->frame = 0;
                }
            }
        }
    }
}

/*
 * This should be run outside of sprites being updated.
 */
void check_sprite_collisions()
{
    sprite_collision* first_collision = NULL;
    sprite_collision* last_collision = NULL;
    sprite_collision* collision = NULL;
    int kill_hitter, kill_hittee;
    sprite* spr, *collision_spr;

    for (spr = game->first_sprite; spr != NULL; spr = spr->next)
    {
        float left1, right1, bot1, top1;
        float off1_x, off1_y;

        off1_x = (spr->sprite_size.w * 0.05) >= 1 ? (spr->sprite_size.w * 0.05) : 1;
        off1_y = (spr->sprite_size.h * 0.05) >= 1 ? (spr->sprite_size.h * 0.05) : 1;

        left1  = spr->position.x + off1_x;
        top1   = spr->position.y + off1_y;
        right1 = spr->position.x + spr->sprite_size.w - off1_x;
        bot1   = spr->position.y + spr->sprite_size.h - off1_y;

        for (collision_spr = game->first_sprite;
             collision_spr != NULL;
             collision_spr = collision_spr->next)
        {
            if ((spr != collision_spr) &&
                (((spr->type == SPRITE_PLAYER || spr->type == SPRITE_PLAYER_BULLET) &&
                  (collision_spr->type != SPRITE_PLAYER && collision_spr->type != SPRITE_PLAYER_BULLET)) ||
                 ((spr->type != SPRITE_PLAYER && spr->type != SPRITE_PLAYER_BULLET) &&
                  (collision_spr->type == SPRITE_PLAYER || collision_spr->type == SPRITE_PLAYER_BULLET))))

            {
                float left2, right2, bot2, top2;
                float off2_x, off2_y;

                off2_x = (collision_spr->sprite_size.w * 0.05) >= 1 ? (collision_spr->sprite_size.w * 0.05) : 1;
                off2_y = (collision_spr->sprite_size.h * 0.05) >= 1 ? (collision_spr->sprite_size.h * 0.05) : 1;

                left2  = collision_spr->position.x + off2_x;
                top2   = collision_spr->position.y + off2_y;
                right2 = collision_spr->position.x + collision_spr->sprite_size.w - off2_x;
                bot2   = collision_spr->position.y + collision_spr->sprite_size.h - off2_y;

                if (((left2 > left1 && left2 < right1) || (left1 > left2 && left1 < right2)) &&
                    ((top2 > top1 && top2 < bot1) || (top1 > top2 && top1 < bot2)))
                {
                    collision = (sprite_collision*)malloc(sizeof(sprite_collision));
                    collision->hitter = spr;
                    collision->hittee = collision_spr;
                    collision->next = NULL;

                    /* Link that list */
                    if (first_collision == NULL)
                    {
                        first_collision = collision;
                        last_collision = collision;
                    }
                    else
                    {
                        last_collision->next = collision;
                        last_collision = collision;
                    }
                }
            }
        }
    }

    /* handle collisions */
    for (collision = first_collision; collision != NULL; )
    {
        sprite_collision* col_ptr = collision;

        /* handle collision stuff here -- make sure sprites exist! */
        if (sprite_exists(col_ptr->hitter) && sprite_exists(col_ptr->hittee))
        {
            /* handle powerups */
            if ((col_ptr->hitter->type == SPRITE_PLAYER && (col_ptr->hittee->type == SPRITE_SPEED_POWERUP)) ||
                (col_ptr->hittee->type == SPRITE_PLAYER && (col_ptr->hitter->type == SPRITE_SPEED_POWERUP)))
            {
                if (col_ptr->hitter->type == SPRITE_PLAYER)
                {
                    col_ptr->hitter->powerups |= SPEED_POWERUP;
                    col_ptr->hitter->health += 10;
                    kill_sprite(col_ptr->hittee);
                }
                else
                {
                    col_ptr->hittee->powerups |= SPEED_POWERUP;
                    col_ptr->hittee->health += 10;
                    kill_sprite(col_ptr->hitter);
                }
            }
            else
            {
                level_st* level = game->get("current_level");
                kill_hittee = 0;
                kill_hitter = 0;

                /*They damage each other.*/
                col_ptr->hittee->health -= col_ptr->hitter->damage_power;
                col_ptr->hitter->health -= col_ptr->hittee->damage_power;

                if (col_ptr->hittee->type == SPRITE_PLAYER &&
                    col_ptr->hittee->spritesheet == game->get("maindude-duress"))
                {
                    col_ptr->hitter->health -= col_ptr->hittee->damage_power;
                }

                if (col_ptr->hitter->type == SPRITE_PLAYER &&
                    col_ptr->hitter->spritesheet == game->get("maindude-duress"))
                {
                    col_ptr->hittee->health -= col_ptr->hitter->damage_power;
                }

                if(col_ptr->hittee->health > 250 && col_ptr->hittee->type == SPRITE_PLAYER)
                {
                    col_ptr->hittee->health = 250;
                }

                if(col_ptr->hitter->health > 250 && col_ptr->hitter->type == SPRITE_PLAYER)
                {
                    col_ptr->hitter->health = 250;
                }

                if( col_ptr->hittee->health < 0 && col_ptr->hittee->type != SPRITE_PLAYER )
                {
                    kill_hittee = 1;
                    //kill_sprite(col_ptr->hittee);

                    if (col_ptr->hittee->type == SPRITE_OXYGEN)
                    {
                        play_sound("bubble-sound");
                    }

                    if (col_ptr->hittee->type != SPRITE_OXYGEN &&
                        col_ptr->hittee->type != SPRITE_PLAYER_BULLET)
                    {
                        level->num_enemies_killed++;
                    }
                }

                if( col_ptr->hitter->health < 0 && col_ptr->hitter->type != SPRITE_PLAYER )
                {
                    kill_hitter = 1;
                    //kill_sprite(col_ptr->hitter);

                    if (col_ptr->hitter->type == SPRITE_OXYGEN)
                    {
                        play_sound("bubble-sound");
                    }

                    if (col_ptr->hitter->type != SPRITE_OXYGEN &&
                        col_ptr->hitter->type != SPRITE_PLAYER_BULLET)
                    {
                        level->num_enemies_killed++;
                    }
                }

                if (col_ptr->hittee->type == SPRITE_PLAYER && col_ptr->hitter->type != SPRITE_OXYGEN)
                {
                    col_ptr->hittee->spritesheet = game->get("maindude-duress");
                    col_ptr->hitter->num_frames = 6;
                    col_ptr->hittee->spritesheet_revert_time = 100;
                }

                if (col_ptr->hitter->type == SPRITE_PLAYER && col_ptr->hittee->type != SPRITE_OXYGEN)
                {
                    col_ptr->hitter->spritesheet = game->get("maindude-duress");
                    col_ptr->hitter->num_frames = 6;
                    col_ptr->hitter->spritesheet_revert_time = 100;
                }

                if ( (col_ptr->hittee->health < 0 && col_ptr->hittee->type == SPRITE_PLAYER) ||
                     (col_ptr->hitter->health < 0 && col_ptr->hitter->type == SPRITE_PLAYER))
                {
                    //Game over?
//                    play_sound("player-death-sound");
                    game->active_ui = &necromancy_ui;
                }

                if(kill_hittee == 1)
                {
                    kill_sprite(col_ptr->hittee);
                }

                if(kill_hitter == 1)
                {
                    kill_sprite(col_ptr->hitter);
                }
            }
        }

        /* move the list before we dealloc */
        collision = collision->next;

        /* realease dem memz */
        free(col_ptr);
    }
}

void draw_all_sprites()
{
    sprite* spr;
    point*  viewport_position = game->get("lvl_viewport_position");

    for (spr = game->first_sprite; spr != NULL; spr = spr->next)
    {
        draw_sprite(game->screen, spr, viewport_position);
    }
}

sprite_type string_to_sprite_type(char *string)
{
    if (!strcmp("player",string))
    {
        return SPRITE_PLAYER;
    }
    else if (!strcmp("nanobot1",string))
    {
        return SPRITE_NANOBOT1;
    }
    else if (!strcmp("nanobot2",string))
    {
        return SPRITE_NANOBOT2;
    }
    else if (!strcmp("enemy",string))
    {
        return SPRITE_ENEMY;
    }
    else if (!strcmp("cross",string))
    {
        return SPRITE_CROSS;
    }
    else if (!strcmp("player-bullet",string))
    {
        return SPRITE_PLAYER_BULLET;
    }
    else if (!strcmp("blue-blob",string))
    {
        return SPRITE_BLUE_BLOB;
    }
    else if (!strcmp("red-blob",string))
    {
        return SPRITE_RED_BLOB;
    }
    else if (!strcmp("speed-powerup",string))
    {
        return SPRITE_SPEED_POWERUP;
    }
    else if (!strcmp("sprite_oxygen",string))
    {
        return SPRITE_OXYGEN;
    }
    else if (!strcmp("purple-blob",string))
    {
        return SPRITE_PURPLE_BLOB;
    }
    else if (!strcmp("orange-blob",string))
    {
        return SPRITE_ORANGE_BLOB;
    }
    else if (!strcmp("green-blob",string))
    {
        return SPRITE_GREEN_BLOB;
    }
    else if (!strcmp("black-blob",string))
    {
        return SPRITE_BLACK_BLOB;
    }

    return SPRITE_UNKNOWN;
}

char *sprite_type_to_string(sprite_type type)
{
    switch(type)
    {
        default:
        case SPRITE_UNKNOWN:
            return "sprite_unkown";
        case SPRITE_PLAYER:
            return "sprite_player";
        case SPRITE_ENEMY:
            return "sprite_enemy";
        case SPRITE_NANOBOT1:
            return "sprite_nanobot1";
        case SPRITE_NANOBOT2:
            return "sprite_nanobot2";
        case SPRITE_CROSS:
            return "sprite_cross";
        case SPRITE_PLAYER_BULLET:
            return "sprite_player_bullet";
        case SPRITE_BLUE_BLOB:
            return "sprite_blue_blob";
        case SPRITE_RED_BLOB:
            return "sprite_red_blob";
        case SPRITE_SPEED_POWERUP:
            return "sprite_speed_powerup";
        case SPRITE_OXYGEN:
            return "sprite_oxygen";
        case SPRITE_PURPLE_BLOB:
            return "sprite_purple_blob";
        case SPRITE_GREEN_BLOB:
            return "sprite_green_blob";
        case SPRITE_ORANGE_BLOB:
            return "sprite_orange_blob";
        case SPRITE_BLACK_BLOB:
            return "sprite_black_blob";
    }
}

/* Schrodinger sprite? */
bool sprite_exists(sprite* spr)
{
    sprite* sptr;

    for (sptr = game->first_sprite; sptr != NULL; sptr = sptr->next)
    {
        if (spr == sptr)
        {
            return true;
        }
    }

    return false;
}

/*
 * Apoptosis?
 */
void kill_sprite(sprite* spr)
{
    if (spr)
    {
        if ( spr->prev == NULL )
        {
            game->first_sprite = spr->next;

            if (game->first_sprite)
                game->first_sprite->prev = NULL;
        }
        else
        {
            spr->prev->next = spr->next;
        }

        if (spr->next == NULL)
        {
            game->last_sprite = spr->prev;

            if (game->last_sprite)
                game->last_sprite->next = NULL;
        }
        else
        {
             spr->next->prev = spr->prev;
        }

        /* dealloc */
        free(spr);
        spr = NULL;
    }
}

/*
 * Generate a sprite
 */
sprite_el* make_sprite(sprite_type type,
                       point pos,
                       velocity velo)
{
    sprite_el* base_sprite = game->get(sprite_type_to_string(type));
    sprite_el* new_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    velocity_bounds *def_bound = game->get("default_velocity_bound");

    if (new_sprite == NULL)
    {
        return NULL;
    }

    if (base_sprite == NULL)
    {
        new_sprite->powerups = 0;
        new_sprite->next = NULL;
        new_sprite->prev = NULL;
        new_sprite->last_frame_time = 0;
        new_sprite->frame = 0;
	    new_sprite->firing = false;
        new_sprite->num_frames = 2;
        new_sprite->frame_delay = 400;
	    new_sprite->firing_delay = 200;
        new_sprite->position = pos;
        new_sprite->vel = velo;
        new_sprite->sprite_size = (SDL_Rect){0,0,130,130};
        new_sprite->spritesheet = game->get("cross");
        new_sprite->type = type;
        new_sprite->last_shot_time = 0;
        new_sprite->health = 1;
        new_sprite->damage_power = 1;
        new_sprite->ttl = 5000;
        new_sprite->spritesheet_revert_time = 0;
        memcpy(&new_sprite->vel_bound, def_bound, sizeof(velocity_bounds));
    }
    else
    {
        memcpy(new_sprite, base_sprite, sizeof(sprite_el));
        new_sprite->powerups = 0;
        new_sprite->position = pos;
        new_sprite->vel = velo;
        new_sprite->next = NULL;
        new_sprite->prev = NULL;
        new_sprite->last_frame_time = 0;
        new_sprite->last_shot_time = 0;
        new_sprite->spritesheet_revert_time = 0;
        new_sprite->frame = 0;
	    new_sprite->firing = false;
    }
    new_sprite->timespawn = SDL_GetTicks() - game->paused_time;

    /* Link it, link it good */
    if (game->first_sprite == NULL)
    {
        game->first_sprite = new_sprite;
        game->last_sprite = new_sprite;
    }
    else
    {
        game->last_sprite->next = new_sprite;
        new_sprite->prev = game->last_sprite;

        game->last_sprite = new_sprite;
    }

    if (new_sprite->type == SPRITE_PLAYER)
    {
        game->set("lvl_player", new_sprite);
    }

    return new_sprite;
}

/* DOES NOT LINKAGE */
void make_blank_sprite(sprite* spr)
{
    spr->next = NULL;
    spr->prev = NULL;
    spr->type = SPRITE_UNKNOWN;
    spr->firing = false;
    spr->firing_delay = 0;
    spr->last_shot_time = 0;
    spr->position = (point){0,0};
    spr->vel = (point){0,0};
    spr->num_frames = 0;
    spr->frame = 0;
    spr->frame_delay = 0;
    spr->last_frame_time = 0;
    spr->sprite_size = (SDL_Rect){0,0,0,0};
    spr->spritesheet = NULL;
    spr->spritesheet_revert_time = 0;
    spr->health = 0;
    spr->damage_power = 0;
}

/* DIE SPRITES DIE */
void clear_sprites()
{
    sprite* spr;
    sprite* spr_next;

    for(spr = game->first_sprite; spr != NULL; spr = spr_next)
    {
        spr_next = spr->next;

        /* dealloc/unlink */
        free(spr);
    }

    game->first_sprite = NULL;
    game->last_sprite = NULL;
}
