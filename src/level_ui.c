/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

/*
 * Includes
 */
#include "main.h"
#include "sprite.h"

const float speed_base_ms = 100.0; //Speeds are pixels per / 100 millesconds.

int level_ui_render();
int level_ui_input();

/**
 * UI handler for the initial screen.
 * @param int ui_action Action to perform
 * @return 1 to break the main application loop, 0 to proceed normally,
 */
int level_ui(int ui_action)
{
    int return_value;
    switch(ui_action)
    {
        case UI_INPUT:
            return_value = level_ui_input();
            break;
        case UI_RENDER:
        default:
            return_value = level_ui_render();
    }
    return return_value;
}

/* Handle level rendering */
int level_ui_render()
{
    level_st* level = game->get("current_level");
    SDL_Surface *background;
    point *viewport_position;
    sprite *player = game->get("lvl_player");
    SDL_Rect rect;
    SDL_Color hp;
    uint32_t initx, inity;

    /*Do updates*/
    update_level(game->get("current_level"));

    /*Retrieve resources from hashtable.*/
    background = game->get("background1");

    /*Render the background.*/
    viewport_position = game->get("lvl_viewport_position");

    initx = ((int32_t)round(viewport_position->x))%WINDOW_WIDTH;
    inity = ((int32_t)round(viewport_position->y))%WINDOW_HEIGHT;
    rect.x = initx;
    rect.y = inity;
    SDL_BlitSurface(background, NULL, game->screen, &rect);
    rect.x = initx-WINDOW_WIDTH;
    rect.y = inity;
    SDL_BlitSurface(background, NULL, game->screen, &rect);
    rect.x = initx-WINDOW_WIDTH;
    rect.y = inity-WINDOW_HEIGHT;
    SDL_BlitSurface(background, NULL, game->screen, &rect);
    rect.x = initx;
    rect.y = inity-WINDOW_HEIGHT;
    SDL_BlitSurface(background, NULL, game->screen, &rect);

    draw_all_sprites();

    /*Draw the player's health.*/
    if(player)
    {
        /*Hard code these for now!*/
        if(player->health > 0)
        {
            /*Draw health bar container.*/
            rect.x = 0;
            rect.y = 0;
            rect.w = 4+( 2 * ((player->health > 100) ? player->health : 100));
            rect.h = 16;
            hp.b = hp.g = hp.r = 0;
            SDL_FillRect(game->screen, &rect, SDL_MapRGB(game->screen->format, hp.r, hp.g, hp.b));
            rect.w = player->health*2;
            rect.h -= 4;
            rect.x = 2;
            rect.y = 2;
            hp.r = 70;
            hp.g = 130;
            hp.b = 180;
            SDL_FillRect(game->screen, &rect, SDL_MapRGB(game->screen->format, hp.r, hp.g, hp.b));
        }
    }

    /*Display enemies killed*/
    {
        SDL_Surface* enemies_killed;
        SDL_Color textcolor = {255,240,240,0};
        TTF_Font* font = game->get(DEFAULT_FONT);
        SDL_Rect target;
        char str[64];

        str[0] = '\0';
        sprintf(str, "Enemies Killed: %d", level->num_enemies_killed);

        if (level->num_enemies_killed_level_over != 0)
        {
            char temp[16];

            temp[0] = '\0';
            sprintf(temp, " / %4d", level->num_enemies_killed_level_over);

            strcat(str, temp);
        }

        enemies_killed = TTF_RenderText_Blended(font, str, textcolor);
        target.x = WINDOW_WIDTH - enemies_killed->w - 5;
        target.y = 10;

        SDL_BlitSurface(enemies_killed, NULL, game->screen, &target);
        SDL_FreeSurface(enemies_killed);
    }

    /* Display time left if applicable */
    if (level->time_level_over > 0)
    {
        SDL_Surface* time;
        SDL_Color textcolor = {255,240,240};
        TTF_Font* font = game->get(TIME_FONT);
        SDL_Rect target;
        char str[32];

        str[0] = '\0';

        sprintf(str, "Survive for %d seconds", (level->time_level_over - ((SDL_GetTicks() - game->paused_time) - level->time_when_loaded))/1000);

        time = TTF_RenderText_Blended(font, str, textcolor);
        target.x = WINDOW_WIDTH/2 - time->w/2;
        target.y = 10;

        SDL_BlitSurface(time, NULL, game->screen, &target);
        SDL_FreeSurface(time);
    }

    return 0;
}

/* Handle level UI input events */
int level_ui_input()
{
    sprite *player = game->get("lvl_player");
//    level_st* level = game->get("current_level");

    switch (game->event->type)
    {
        case SDL_MOUSEBUTTONDOWN:
            if (game->event->button.button == SDL_BUTTON_LEFT)
            {
                player->firing = true;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            player->firing = false;
            break;

        case SDL_KEYDOWN:
            switch(game->event->key.keysym.sym)
            {
                case SDLK_a:
                case SDLK_LEFT:
                    if (player->powerups & SPEED_POWERUP)
                    {
                        player->vel.x = -30;
                    }
                    else
                    {
                        player->vel.x = -20;
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if (player->powerups & SPEED_POWERUP)
                    {
                        player->vel.y = 30;
                    }
                    else
                    {
                        player->vel.y = 20;
                    }
                    break;
                case SDLK_w:
                case SDLK_UP:
                    if (player->powerups & SPEED_POWERUP)
                    {
                        player->vel.y = -30;
                    }
                    else
                    {
                        player->vel.y = -20;
                    }
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    if (player->powerups & SPEED_POWERUP)
                    {
                        player->vel.x = 30;
                    }
                    else
                    {
                        player->vel.x = 20;
                    }
                    break;

                default:
                    break;
            }
            break;

        case SDL_KEYUP:
            switch(game->event->key.keysym.sym)
            {
                case SDLK_a:
                case SDLK_d:
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    {
                        uint8_t* keystate = SDL_GetKeyState(NULL);

                        if (!keystate[SDLK_a] &&
                            !keystate[SDLK_d] &&
                            !keystate[SDLK_LEFT] &&
                            !keystate[SDLK_RIGHT])
                        {
                            player->vel.x = 0;
                        }
                    }
                    break;

                case SDLK_w:
                case SDLK_s:
                case SDLK_DOWN:
                case SDLK_UP:
                    {
                        uint8_t* keystate = SDL_GetKeyState(NULL);

                        if (!keystate[SDLK_w] &&
                            !keystate[SDLK_s] &&
                            !keystate[SDLK_DOWN] &&
                            !keystate[SDLK_UP])
                        {
                            player->vel.y = 0;
                        }
                    }
                    break;

                case SDLK_ESCAPE:
                    play_sound("pause-sound");
                    game->last_paused_time = SDL_GetTicks();
                    game->active_ui = &pause_ui;
                    player->vel.x = 0;
                    player->vel.y = 0;
                    break;

/* remove these before release */
/*
                case SDLK_r:
                    remake_level(level);
                    break;

                case SDLK_KP_PLUS:
                    game->set("current_level", level->next);
                    clear_sprites();
                    break;

                case SDLK_1:
                    game->active_ui = &sheen_ui;
                    break;
*/
                default:
                    break;
            }
            break;
    }
    return 0;
}

/**
 * Level UI Resource loader
 */
int level_ui_load_resources()
{
    velocity *viewport_velocity;
    point *viewport_position;
    sprite_el *prepopulated_sprite;
    velocity_bounds *default_vel_bound;

    default_vel_bound = malloc(sizeof(velocity_bounds));

    default_vel_bound->upper_vel.x = 20;
    default_vel_bound->upper_vel.y = 18;

    default_vel_bound->lower_vel.y = -18;
    default_vel_bound->lower_vel.x = -20;

    game->set("default_velocity_bound", default_vel_bound);

    /*Enable KEYDOWN/KEYUP repeat*/
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    viewport_velocity = malloc(sizeof(velocity));
    viewport_position = malloc(sizeof(point));

    viewport_position->x = 5.0f;
    viewport_position->y = 5.0f;

    viewport_velocity->x = 4.5f;
    viewport_velocity->y = 2.5f;

    /* Load images and make basic sprites */
    if (!load_image("bg1.jpg","background1"))
    {
        printf("Failed to load bg1.jpg!\n");
    }

    /* Sprites */
    if (!load_image("nanobots.png","enemy_set1"))
    {
        printf("Failed to load nanobots.png!\n");
    }

    /*nanobot1*/
    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("enemy_set1");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,80,80};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 200;
    prepopulated_sprite->type = SPRITE_NANOBOT1;
    prepopulated_sprite->behavior = &behavior_pursue;
    prepopulated_sprite->health = 2;
    prepopulated_sprite->damage_power = 4;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_NANOBOT1),prepopulated_sprite);

    /*nanobot2*/
    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("enemy_set1");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,80,80,80};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 250;
    prepopulated_sprite->firing_delay = 250;
    prepopulated_sprite->type = SPRITE_NANOBOT2;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 4;
    prepopulated_sprite->damage_power = 7;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_NANOBOT2),prepopulated_sprite);

    /*player*/
    if (!load_image("images/NormalFull.png","maindude"))
    {
        printf("Failed to load images\\supercellnew.png!\n");
    }

    if (!load_image("images\\StressFull.png","maindude-duress"))
    {
        printf("Failed to load images\\supercell_duress.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("maindude");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,100,100};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 400;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_PLAYER;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 100;
    prepopulated_sprite->damage_power = 2;
    prepopulated_sprite->ttl = 0;
    game->set(sprite_type_to_string(SPRITE_PLAYER),prepopulated_sprite);

    /*enemy*/
    if (!load_image("celltest.png","blob"))
    {
        printf("Failed to load celltest.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("blob");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,100,60};
    prepopulated_sprite->num_frames = 2;
    prepopulated_sprite->frame_delay = 250;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_ENEMY;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 2;
    prepopulated_sprite->damage_power = 3;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_ENEMY),prepopulated_sprite);

    /*cross*/
    if (!load_image("enemy1.png","cross"))
    {
        printf("Failed to load enemy1.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("cross");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,130,130};
    prepopulated_sprite->num_frames = 2;
    prepopulated_sprite->frame_delay = 500;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_CROSS;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 10;
    prepopulated_sprite->damage_power = 3;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_CROSS),prepopulated_sprite);

    /*blue blob*/
    if (!load_image("images\\blob_enemy.png","blue-blob"))
    {
        printf("Failed to load enemy1new.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("blue-blob");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,80,80};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 400;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_BLUE_BLOB;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 4;
    prepopulated_sprite->damage_power = 3;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_BLUE_BLOB),prepopulated_sprite);

    /*red blob*/
    if (!load_image("enemyballnew.png","red-blob"))
    {
        printf("Failed to load enemyballnew.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("red-blob");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,40,40};
    prepopulated_sprite->num_frames = 6;
    prepopulated_sprite->frame_delay = 200;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_RED_BLOB;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 4;
    prepopulated_sprite->damage_power = 3;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_RED_BLOB),prepopulated_sprite);

    /*player_bullet*/
    if (!load_image("player-basic-bullet.png","player-bullet"))
    {
        printf("Failed to load player-basic-bullet.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("player-bullet");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,20,20};
    prepopulated_sprite->num_frames = 1;
    prepopulated_sprite->frame_delay = 0;
    prepopulated_sprite->firing_delay = 0;
    prepopulated_sprite->type = SPRITE_PLAYER_BULLET;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 1;
    prepopulated_sprite->damage_power = 2;
    prepopulated_sprite->ttl = 3400;
    game->set(sprite_type_to_string(SPRITE_PLAYER_BULLET),prepopulated_sprite);

    /*speed powerup*/
    if (!load_image("images\\Speed.png","speed-powerup"))
    {
        printf("Failed to load speed-powerup.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("speed-powerup");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,30,30};
    prepopulated_sprite->num_frames = 2;
    prepopulated_sprite->frame_delay = 500;
    prepopulated_sprite->firing_delay = 0;
    prepopulated_sprite->type = SPRITE_SPEED_POWERUP;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->ttl = 6000;
    prepopulated_sprite->health = 1;
    game->set(sprite_type_to_string(SPRITE_SPEED_POWERUP),prepopulated_sprite);

    /*oxygen*/
    if (!load_image("oxygenbubble.png","oxygen"))
    {
        printf("Failed to load oxygenbubble.png!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("oxygen");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,25,25};
    prepopulated_sprite->num_frames = 1;
    prepopulated_sprite->frame_delay = 11000;
    prepopulated_sprite->firing_delay = 0;
    prepopulated_sprite->type = SPRITE_OXYGEN;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->damage_power = -3;
    prepopulated_sprite->ttl = 10000;
    prepopulated_sprite->health = 1;
    game->set(sprite_type_to_string(SPRITE_OXYGEN),prepopulated_sprite);

    /*purple blob*/
    if (!load_image("images\\blob_enemy2.png","purple-blob"))
    {
        printf("Failed to load purple blob!!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("purple-blob");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,80,80};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 400;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_PURPLE_BLOB;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 4;
    prepopulated_sprite->damage_power = 4;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_PURPLE_BLOB),prepopulated_sprite);

    /*orange blob*/
    if (!load_image("images\\blob_enemy3.png","orange-blob"))
    {
        printf("Failed to load orange blob!!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("orange-blob");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,80,80};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 400;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_ORANGE_BLOB;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 2;
    prepopulated_sprite->damage_power = 4;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_ORANGE_BLOB),prepopulated_sprite);

    /*green blob*/
    if (!load_image("images\\blob_enemy4.png","green-blob"))
    {
        printf("Failed to load green blob!!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("green-blob");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,80,80};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 400;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_GREEN_BLOB;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 2;
    prepopulated_sprite->damage_power = 2;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_GREEN_BLOB),prepopulated_sprite);

    /*black blob*/
    if (!load_image("images\\blob_enemy5.png","black-blob"))
    {
        printf("Failed to load black blob!!\n");
    }

    prepopulated_sprite = (sprite_el*)malloc(sizeof(sprite_el));
    memcpy(&prepopulated_sprite->vel_bound, default_vel_bound, sizeof(velocity_bounds));
    prepopulated_sprite->spritesheet = game->get("black-blob");
    prepopulated_sprite->sprite_size = (SDL_Rect){0,0,80,80};
    prepopulated_sprite->num_frames = 3;
    prepopulated_sprite->frame_delay = 400;
    prepopulated_sprite->firing_delay = 200;
    prepopulated_sprite->type = SPRITE_BLACK_BLOB;
    prepopulated_sprite->behavior = NULL;
    prepopulated_sprite->health = 6;
    prepopulated_sprite->damage_power = 6;
    prepopulated_sprite->ttl = 10000;
    game->set(sprite_type_to_string(SPRITE_BLACK_BLOB),prepopulated_sprite);

    /*Set level ui defaults*/
    game->set("lvl_viewport_velocity", viewport_velocity);
    game->set("lvl_viewport_position", viewport_position);

    return 1; //Maybe we should do real error checking at some point.
              // ^ Nope, Chuck Testa.
}
