/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

/*
 * Includes
 */
#include "main.h"
#include "level.h"

/*
 * Function Prototypes
 */
level_st* make_level(char*);
level_st* load_level(char*, bool);

/*
 * Functions
 */

/*
 * load_levels -- Load all levels from their files
 */
void load_levels()
{
    level_st* level;

    if ((level = load_level("gen_s4_0.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->num_enemies_killed_level_over = 50;
    level->oxygen_rate = 750;

    if ((level = load_level("gen_s4_1.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->time_level_over = 120000;
    level->oxygen_rate = 840;

    if ((level = load_level("gen_s4_2.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->num_enemies_killed_level_over = 100;
    level->oxygen_rate = 750;

    if ((level = load_level("gen_s4_3.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->time_level_over = 150000;
    level->oxygen_rate = 850;

    if ((level = load_level("gen_s4_4.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->num_enemies_killed_level_over = 200;
    level->oxygen_rate = 600;

    if ((level = load_level("gen_s4_5.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->time_level_over = 180000;
    level->oxygen_rate = 850;

    if ((level = load_level("gen_s4_6.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->num_enemies_killed_level_over = 300;
    level->oxygen_rate = 500;

    if ((level = load_level("gen_s4_7.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->time_level_over = 180000;
    level->oxygen_rate = 900;

    if ((level = load_level("gen_s4_8.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->num_enemies_killed_level_over = 350;
    level->oxygen_rate = 400;

    if ((level = load_level("gen_s4_9.json", true)) == NULL)
    {
        printf("Level failed to load. Possible JSON Error:\n\n%s", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }
    level->time_level_over = 120000;
    level->oxygen_rate = 1200;

    game->set("current_level", game->first_level);
}

/*
 * update_level -- Spawn enemies, play sounds, etc
 */
void update_level(level_st* level)
{
    proto_sprite* proto;
    uint32_t current_time = SDL_GetTicks() - game->paused_time;
    point* viewport_position = game->get("lvl_viewport_position");
    velocity *viewport_velocity = game->get("lvl_viewport_velocity");
    uint32_t oxy_vp_x; //Viewport position to spawn at
    uint32_t oxy_vp_y; //
    float oxy_x, oxy_y;
    point viewport_middle;
    point player_middle;
    velocity oxyvel;

    sprite *player = game->get("lvl_player");

    if (level->time_when_loaded == 0)
    {
        level->time_when_loaded = SDL_GetTicks() - game->paused_time;
    }

    /* start our music if it isn't already playing */
    play_sound("main-song");

    /* Check to see if the level is over */
    if (level->num_enemies_killed_level_over != 0)
    {
        if (level->num_enemies_killed >= level->num_enemies_killed_level_over)
        {
            if (level->next)
            {
                 //If the level's been previously loaded.  We should reload it.
                if(level->next->time_when_loaded != 0)
                {
                    remake_level(level->next);
                }

                game->set("current_level", level->next);
                clear_sprites();
            }
            else
            {
                /* Win? */
                game->active_ui = &sheen_ui;
            }
        }
    }
    else if ((current_time - level->time_when_loaded) >= level->time_level_over)
    {
        if (level->next)
        {
            //If the level's been previously loaded.  We should reload it.
            if(level->next->time_when_loaded != 0)
            {
                remake_level(level->next);
            }
            game->set("current_level", level->next);
            clear_sprites();
        }
        else
        {
            /* Win? */
            game->active_ui = &sheen_ui;
        }
    }

    /* Ominously spawn and chase down the player at ridiculous speed if they try escaping.
     */
    if (player && (current_time - level->last_hunter) > (level->hunter_rate))
    {
        viewport_middle.x = viewport_position->x + (float)(WINDOW_WIDTH/2);
        viewport_middle.y = viewport_position->y + (float)(WINDOW_HEIGHT/2);
        float okaytop = -250+viewport_position->y;
        float okaybottom = 250+viewport_position->y+(float)WINDOW_HEIGHT;
        float okayleft = -250+viewport_position->x;
        float okayright = 250+viewport_position->x+(float)WINDOW_WIDTH;
        player_middle.x = player->position.x+(player->sprite_size.w/2);
        player_middle.y = player->position.y+(player->sprite_size.h/2);

        if( !(player_middle.y <= okaybottom && player_middle.y >= okaytop &&
            player_middle.x <= okayright  && player_middle.x >= okayleft) )
        {
            sprite *evil;
            evil = make_sprite(SPRITE_NANOBOT1, viewport_middle, (velocity){5,5});
            evil->vel_bound.lower_vel.x = -46+viewport_velocity->x;
            evil->vel_bound.lower_vel.y = -46+viewport_velocity->y;
            evil->vel_bound.upper_vel.x = 46+viewport_velocity->x;
            evil->vel_bound.upper_vel.y = 46+viewport_velocity->y;
            evil->health = 8;
            evil->behavior = &behavior_pursue;
            level->last_hunter = current_time;
        }
    }

    /* Spawn oxygen if we need to. */
    /* They are spawned away from the player. */
    if(player && (current_time - level->last_oxygen) > level->oxygen_rate)
    {
        /*Attempt to spawn an oxygen bubble.*/
        oxy_vp_x = random_uint32(0, (WINDOW_WIDTH-25));
        oxy_vp_y = random_uint32(0, (WINDOW_HEIGHT-25));
        /*Map these to game coordinates.*/
        oxy_x = (float)oxy_vp_x + viewport_position->x;
        oxy_y = (float)oxy_vp_y + viewport_position->y;
        player_middle.x = player->position.x+(player->sprite_size.w/2);
        player_middle.y = player->position.x+(player->sprite_size.w/2);

        if(abs( oxy_x - player_middle.x) > (WINDOW_WIDTH/4) &&
           abs( oxy_y - player_middle.y) > (WINDOW_HEIGHT/4)
           )
        {
            oxyvel.x = viewport_velocity->x + (random_uint32(1,6));
            oxyvel.y = viewport_velocity->y + (random_uint32(1,6));
            make_sprite(SPRITE_OXYGEN, (point){oxy_x,oxy_y}, oxyvel);
            level->last_oxygen = current_time;
        }
    }

    /* Spawn sprites if we need to */
    for (proto = level->first_proto; proto != NULL; proto = proto->next)
    {
        if ((proto->number_to_spawn > 0) &&
            (current_time >= (proto->start_spawn_time + level->time_when_loaded)))
        {
            if ((current_time >= (proto->last_spawn_time + proto->spawn_interval)) ||
                (proto->last_spawn_time == 0))
            {
                point position;
                sprite* spr;

                proto->start_position.x += proto->spawn_vector.x;
                proto->start_position.y += proto->spawn_vector.y;

                position = proto->start_position;
                position.x += viewport_position->x;
                position.y += viewport_position->y;

                proto->last_spawn_time = current_time;
                proto->number_to_spawn--;

                spr = make_sprite(proto->type, position, proto->velocity);

                if (proto->spr.firing_delay != 0)
                {
                    spr->firing_delay = proto->spr.firing_delay;
                }

                if (proto->spr.num_frames != 0)
                {
                    spr->num_frames = proto->spr.num_frames;
                }

                if (proto->spr.frame_delay != 0)
                {
                    spr->frame_delay = proto->spr.frame_delay;
                }

                if (proto->spr.health != 0)
                {
                    spr->health = proto->spr.health;
                }

                if (proto->spr.damage_power != 0)
                {
                    spr->damage_power = proto->spr.damage_power;
                }
            }
        }
    }

    /* Update all sprite positions/animations/etc */
    update_all_sprites();

    /* Check for and handle collisions */
    check_sprite_collisions();
}

/*
 * load_level -- Parse a JSON level file into an actual structure
 */
level_st* load_level(char* filename, bool link)
{
    FILE* file;
    level_st* level;
    char* filetarget;
    char* temp = (char*)malloc(MAX_LEVEL_SIZE+1);

    filetarget = malloc((strlen("resources\\levels\\") + strlen(filename) + 1) * sizeof(char));
    filetarget[0] = '\0';

    strcat(filetarget, "resources\\levels\\");
    strcat(filetarget, filename);

    file = fopen(filetarget, "r");

    if (file == NULL)
    {
        return NULL;
    }
    else
    {
        uint32_t bytes_read = fread(temp, 1, MAX_LEVEL_SIZE, file);

        temp[bytes_read] = '\0';

        fclose(file);
    }

    free(filetarget);

    level = make_level(temp);
    level->filename = strdup(filename);

    if (link)
    {
        if (game->first_level == NULL)
        {
            game->first_level = level;
        }
        else
        {
            level_st* level_ptr;

            /* baby jesus cries sometimes */
            for (level_ptr = game->first_level;
                 level_ptr != NULL;
                 level_ptr = level_ptr->next)
            {
                if (level_ptr->next == NULL)
                {
                    break;
                }
            }

            level_ptr->next = level;
        }
    }

    free(temp);

    return level;
}

level_st* make_level(char* level_json)
{
    level_st* level = NULL;
    cJSON *root = cJSON_Parse(level_json);
    cJSON *proto_sprites, *ptr;

    if (root == NULL)
    {
        return NULL;
    }

    level = (level_st*)malloc(sizeof(level_st));

    if (level == NULL)
    {
        return NULL;
    }

    level->oxygen_rate = 750;
    level->last_oxygen = 0;
    level->last_hunter = 0;
    level->hunter_rate = 500;
    level->next = NULL;
    level->first_proto = NULL;
    level->time_when_loaded = 0;
    level->time_level_over = 0;
    level->num_enemies_killed_level_over = 0;
    level->num_enemies_killed = 0;

    cJSON* name = cJSON_GetObjectItem(root,"name");
    level->name = strdup(name->valuestring);

    /* Iterate over all prototypes and store them */
    proto_sprites = cJSON_GetObjectItem(root,"proto sprites");

    for (ptr = proto_sprites->child; ptr != NULL; ptr = ptr->next)
    {
        proto_sprite* proto = (proto_sprite*)malloc(sizeof(proto_sprite));

        if (!proto)
        {
            exit(EXIT_FAILURE);
        }

        proto->next = NULL;
        proto->type = SPRITE_UNKNOWN;
        proto->start_position = (point){0,0};
        proto->velocity = (point){0,0};
        proto->spawn_vector = (point){0,0};
        proto->spawn_interval = 0;
        proto->last_spawn_time = 0;
        proto->start_spawn_time = 0;
        proto->number_to_spawn = 0;
        proto->id = strdup(ptr->string);

        make_blank_sprite(&proto->spr);

        if (level->first_proto == NULL)
        {
            level->first_proto = proto;
        }
        else
        {
            proto_sprite* proto_ptr;

            /* baby jesus cries many times*/
            for (proto_ptr = level->first_proto;
                 proto_ptr != NULL;
                 proto_ptr = proto_ptr->next)
            {
                if (proto_ptr->next == NULL)
                {
                    break;
                }
            }

            proto_ptr->next = proto;
        }

        /* Set Data */
        cJSON* data;

        for (data = ptr->child; data != NULL; data = data->next )
        {
            switch(data->string[0])
            {
                case 'd':
                    if (!strcmp("damage power",data->string))
                    {
                        proto->spr.damage_power = data->valueint;
                    }
                    break;

                case 'f':
                    if (!strcmp("frame delay",data->string))
                    {
                        proto->spr.frame_delay = data->valueint;
                    }
                    else if (!strcmp("firing delay",data->string) ||
                             !strcmp("firing_delay",data->string))
                    {
                        proto->spr.firing_delay = data->valueint;
                    }
                    break;

                case 'h':
                    if (!strcmp("health",data->string))
                    {
                        proto->spr.health = data->valueint;
                    }
                    break;

                case 'n':
                    if (!strcmp("number to spawn",data->string))
                    {
                        proto->number_to_spawn = data->valueint;
                    }
                    else if (!strcmp("number of frames",data->string) ||
                             !strcmp("num_frames",data->string))
                    {
                        proto->spr.num_frames = data->valueint;
                    }
                    break;

                case 's':
                    if (!strcmp("start position",data->string))
                    {
                        point pos = (point){0,0};
                        cJSON* child_data;

                        for (child_data = data->child;
                             child_data != NULL;
                             child_data = child_data->next)
                        {
                            if (!strcmp("x",child_data->string))
                            {
                                pos.x = child_data->valuedouble;
                            }
                            else if (!strcmp("y",child_data->string))
                            {
                                pos.y = child_data->valuedouble;
                            }
                        }

                        proto->start_position = pos;
                    }
                    else if (!strcmp("spawn vector",data->string))
                    {
                        point vec = (point){0,0};
                        cJSON* child_data;

                        for (child_data = data->child;
                             child_data != NULL;
                             child_data = child_data->next)
                        {
                            if (!strcmp("x",child_data->string))
                            {
                                vec.x = child_data->valuedouble;
                            }
                            else if (!strcmp("y",child_data->string))
                            {
                                vec.y = child_data->valuedouble;
                            }
                        }

                        proto->spawn_vector = vec;
                    }
                    else if (!strcmp("spawn interval",data->string))
                    {
                        proto->spawn_interval = data->valueint;
                    }
                    else if (!strcmp("start spawn time",data->string))
                    {
                        proto->start_spawn_time = data->valueint;
                    }
                    break;

                case 't':
                    if (!strcmp("type",data->string))
                    {
                        proto->type = string_to_sprite_type(data->valuestring);
                    }
                    break;

                case 'v':
                    if (!strcmp("velocity",data->string))
                    {
                        point velo = (point){0,0};
                        cJSON* child_data;

                        for (child_data = data->child;
                             child_data != NULL;
                             child_data = child_data->next)
                        {
                            if (!strcmp("x",child_data->string))
                            {
                                velo.x = child_data->valuedouble;
                            }
                            else if (!strcmp("y",child_data->string))
                            {
                                velo.y = child_data->valuedouble;
                            }
                        }

                        proto->velocity = velo;
                    }
                    break;
            }
        }
    }

    cJSON_Delete(root);

    return level;
}

/*
 * 1 + 1 = new level???
 */
level_st* remake_level(level_st* old)
{
    level_st* new_level;

    new_level = load_level(old->filename, false);
    new_level->num_enemies_killed_level_over = old->num_enemies_killed_level_over;
    new_level->time_level_over = old->time_level_over;
    new_level->oxygen_rate = old->oxygen_rate;

    if (game->first_level == old)
    {
        game->first_level = new_level;
    }
    else
    {
        level_st* level_ptr;

        /* baby jesus cries was murdered this night :( */
        for (level_ptr = game->first_level;
             level_ptr != NULL;
             level_ptr = level_ptr->next)
        {
            if (level_ptr->next == old)
            {
                break;
            }
        }

        level_ptr->next = new_level;
    }

    new_level->next = old->next;

    /* if we are remaking the current level, set the ptr in the hashtab */
    if (game->get("current_level") == old)
    {
        clear_sprites();
        game->set("current_level", new_level);
    }

    /* destroy all proto's and then free */
    proto_sprite* proto, *proto_next;

    for (proto = old->first_proto; proto != NULL; proto = proto_next)
    {
        proto_next = proto->next;

        free(proto->id);
        free(proto);
    }

    free(old->filename);
    free(old->name);
    free(old);

    return new_level;
}
