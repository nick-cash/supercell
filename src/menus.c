#include "main.h"


int pause_ui_input();
int pause_ui_render();
int title_menu_ui_input();
int title_menu_ui_render();
bool menu_in_rect(SDL_Rect position, SDL_Rect rect);
int inst_screen_ui_input(int (*active_ui)(int ui_action));

/**
 * UI handler for the initial screen.
 * @param int ui_action Action to perform
 * @return 1 to break the main application loop, 0 to proceed normally,
 */
int pause_ui(int ui_action)
{
    int return_value;
    switch(ui_action)
    {
        case UI_INPUT:
            return_value = pause_ui_input();
            break;
        case UI_RENDER:
        default:
            return_value = pause_ui_render();
    }
    return return_value;
}

/*Handles pause UI's input.*/
int pause_ui_input()
{
    SDL_Surface *pause_menu_title = game->get("pause_menu_title");
    SDL_Surface *menu_resume_text = game->get("menu_resume_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    SDL_Surface *menu_help_text = game->get("menu_help_text");
    uint16_t    *pause_position = game->get("pause_position");
    uint32_t    *lastticks;
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);
    SDL_Rect    resume_target, exit_target, help_target;

    if((*pause_position) > 2 || (*pause_position) < 0)
    {
        (*pause_position) = 0;
    }

    /*Enabling mouse support will require knowing
    where the menu items are rendered*/
    current_y += (pause_menu_title->h * 3);

    resume_target.x = (WINDOW_WIDTH/2) - (menu_resume_text->w/2) - 20;
    resume_target.y = current_y;
    resume_target.w = menu_resume_text->w+40;
    resume_target.h = menu_resume_text->h;

    /*Must match the rendering code below to be useful.*/
    current_y += (menu_resume_text->h * 2);

    exit_target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2) - 20;
    exit_target.y = current_y;
    exit_target.w = menu_exit_text->w+40;
    exit_target.h = menu_exit_text->h;

    current_y += (menu_exit_text->h * 2);

    help_target.x = (WINDOW_WIDTH/2) - (menu_help_text->w/2) - 20;
    help_target.y = current_y;
    help_target.w = menu_help_text->w+40;
    help_target.h = menu_help_text->h;
    //inst_pause_menu_ui
    switch (game->event->type)
    {
        case SDL_MOUSEMOTION:
            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},resume_target))
            {
                (*pause_position) = 0;
            }

            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},exit_target))
            {
                (*pause_position) = 1;
            }

            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},help_target))
            {
                (*pause_position) = 2;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},resume_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                //If lbl_lastticks is present, lets update it to right now.
                game->paused_time += (SDL_GetTicks() - game->last_paused_time);
                lastticks = game->get("lvl_lastticks");
                if(lastticks)
                {
                    (*lastticks) = SDL_GetTicks() - game->paused_time;
                }
                //level_st* level = game->get("current_level");
                //level->time_when_loaded += (*lastticks - level->time_when_loaded);
                play_sound("unpause-sound");
                game->active_ui = &level_ui;
            }

            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},exit_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                return 1;
            }

            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},help_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                game->active_ui = &inst_pause_menu_ui;
            }
            break;
        case SDL_KEYUP:
            switch(game->event->key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    if((*pause_position) > 0)
                    {
                        (*pause_position) -= 1;
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if((*pause_position) < 2)
                    {
                        (*pause_position) += 1;
                    }
                    break;

                case SDLK_KP_ENTER:
                case SDLK_RETURN:
                case SDLK_SPACE:
                case SDLK_z:
                    if((*pause_position) == 0)
                    {
                        //If lbl_lastticks is present, lets update it to right now.
                        game->paused_time += (SDL_GetTicks() - game->last_paused_time);
                        lastticks = game->get("lvl_lastticks");
                        if(lastticks)
                        {
                            (*lastticks) = SDL_GetTicks() - game->paused_time;
                        }

                        //level_st* level = game->get("current_level");
                        //level->time_when_loaded += (*lastticks - level->time_when_loaded);

                        play_sound("unpause-sound");
                        game->active_ui = &level_ui;
                    }
                    else if ((*pause_position) == 0)
                    {
                        return 1;
                    }
                    else
                    {
                        game->active_ui = &inst_pause_menu_ui;
                    }
                default:
                    break;
            }

    }
    return 0;
}

/* Renders the pause menu. */
int pause_ui_render()
{
    SDL_Surface *pause_menu_title = game->get("pause_menu_title");
    SDL_Surface *menu_resume_text = game->get("menu_resume_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    SDL_Surface *menu_selector = game->get("menu_selector");
    SDL_Surface *background = game->get("menu_background");
    SDL_Surface *menu_help_text = game->get("menu_help_text");
    uint16_t    *pause_position = game->get("pause_position");
    SDL_Rect    target;
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);

    /*Draw background*/
    SDL_BlitSurface(background, NULL, game->screen, NULL);

    /*Place the menu title - centered.*/
    target.x = (WINDOW_WIDTH/2) - (pause_menu_title->w/2);
    target.y = current_y;
    SDL_BlitSurface(pause_menu_title, NULL, game->screen, &target);

    /*Update current_y...*/
    current_y += (pause_menu_title->h * 3);

    /*Place the resume option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_resume_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_resume_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*pause_position) == 0)
    {
        target.x -= 48;
        target.y = current_y + (menu_resume_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_resume_text->h * 2);

    /*Place the exit game option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_exit_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*pause_position) == 1)
    {
        target.x -= 48;
        target.y = current_y + (menu_exit_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_exit_text->h * 2);

    /*Place the exit game option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_help_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_help_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*pause_position) == 2)
    {
        target.x -= 48;
        target.y = current_y + (menu_help_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }
    return 0;
}

/**
 * Does initialization for both pause_ui and title_ui
 */
int menu_ui_resource_load()
{
    /*Naming conventoin for images:
        pause_ for pause-specific items
        menu_ for shared items and entry text
        title_menu for title_menu-specific items. This can be revisited*/
    SDL_Surface *pause_menu_title;
    SDL_Surface *menu_resume_text;
    SDL_Surface *menu_exit_text;
    SDL_Surface *menu_start_text;
    SDL_Surface *menu_help_text;
    SDL_Surface *menu_restart_text;
    SDL_Surface *dead_menu_title;
    SDL_Surface *menu_restart_game_text;
    SDL_Surface *win_menu_title;
    uint16_t    *pause_position, *title_menu_position, *dead_menu_position, *win_menu_position;
    uint16_t    *menu_start_y;
    TTF_Font* menufont;
    SDL_Color menutextcolor = {255,240,240};

    menufont = game->get(MENU_FONT);

    /*Pre-render the menu entries.*/
    /*This is fine with a limited number of entries.*/
    /*But we'd want a better way if we did anything fancy.*/
    dead_menu_title = TTF_RenderText_Blended(menufont, "-- Game Over --",menutextcolor);
    pause_menu_title = TTF_RenderText_Blended(menufont, "-- Paused --",menutextcolor);
    menu_restart_text = TTF_RenderText_Blended(menufont, "Restart Level",menutextcolor);
    menu_resume_text = TTF_RenderText_Blended(menufont, "Resume Game",menutextcolor);
    menu_exit_text = TTF_RenderText_Blended(menufont, "Exit Game",menutextcolor);
    menu_start_text = TTF_RenderText_Blended(menufont, "Start Game",menutextcolor);
    menu_help_text = TTF_RenderText_Blended(menufont, "Instructions",menutextcolor);
    win_menu_title = TTF_RenderText_Blended(menufont, "-- You Win! --",menutextcolor);
    menu_restart_game_text = TTF_RenderText_Blended(menufont, "Restart Game",menutextcolor);

    game->set("dead_menu_title", dead_menu_title);
    game->set("menu_restart_text", menu_restart_text);
    game->set("pause_menu_title", pause_menu_title);
    game->set("menu_resume_text", menu_resume_text);
    game->set("menu_exit_text", menu_exit_text);
    game->set("menu_start_text", menu_start_text);
    game->set("menu_help_text",menu_help_text);
    game->set("win_menu_title", win_menu_title);
    game->set("menu_restart_game_text", menu_restart_game_text);

    /*Pre-allocate the position variables.*/
    pause_position = malloc(sizeof(uint16_t));
    (*pause_position) = 0;
    game->set("pause_position", pause_position);

    title_menu_position = malloc(sizeof(uint16_t));
    (*title_menu_position) = 0;
    game->set("title_menu_position", title_menu_position);

    dead_menu_position = malloc(sizeof(uint16_t));
    (*dead_menu_position) = 0;
    game->set("dead_menu_position", dead_menu_position);

    win_menu_position = malloc(sizeof(uint16_t));
    (*win_menu_position) = 0;
    game->set("win_menu_position", win_menu_position);


    /*A setting:*/
    menu_start_y = malloc(sizeof(uint16_t));
    (*menu_start_y) = 290;
    game->set("menu_start_y", menu_start_y);

    /*Load the menu_background*/
    if (!load_image("menu.jpg","menu_background"))
    {
        printf("Failed to load menu.jpg!\n");
    }

    /*Load the menu_background*/
    if (!load_image("selector.png","menu_selector"))
    {
        printf("Failed to load selector.png!\n");
    }
    //SDL_Surface *inst_background = game->get("instruction_background");
    if(!load_image("instructions.jpg","instruction_background"))
    {
        printf("Failed to load instructions.jpg!\n");
    }

    return 1;
}

/*Handles input for the instruction screens.  Have the UI functions for
 the pause menu and title menu specify which active_ui to return to.*/
int inst_screen_ui_input(int (*active_ui)(int ui_action))
{
    SDL_Rect back_target;
    back_target.x = 593;
    back_target.y = 541;
    back_target.w = 182;
    back_target.h = 50;
    switch (game->event->type)
    {
        case SDL_MOUSEBUTTONUP:

            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},back_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
               game->active_ui = active_ui;
            }

            break;
        case SDL_KEYUP:
            switch(game->event->key.keysym.sym)
            {
                case SDLK_KP_ENTER:
                case SDLK_RETURN:
                case SDLK_SPACE:
                case SDLK_z:
                    game->active_ui = active_ui;
                    break;
                default:
                    break;
            }

    }
    return 0;
}

int instruction_screen_render()
{
    SDL_Surface *inst_background = game->get("instruction_background");
    SDL_BlitSurface(inst_background, NULL, game->screen, NULL);
    return 0;
}

int inst_pause_menu_ui(int ui_action)
{
    int return_value;
    switch(ui_action)
    {
        case UI_INPUT:
            return_value = inst_screen_ui_input(pause_ui);
            break;
        case UI_RENDER:
        default:
            return_value = instruction_screen_render();
    }
    return return_value;
    return 0;
}

int inst_title_menu_ui(int ui_action)
{
    int return_value;
    switch(ui_action)
    {
        case UI_INPUT:
            return_value = inst_screen_ui_input(title_menu_ui);
            break;
        case UI_RENDER:
        default:
            return_value = instruction_screen_render();
    }
    return return_value;
    return 0;
}

/*
 * Title Menu UI
 */
/**
 * UI handler for the initial screen.
 * @param int ui_action Action to perform
 * @return 1 to break the main application loop, 0 to proceed normally,
 */
int title_menu_ui(int ui_action)
{
    int return_value;
    switch(ui_action)
    {
        case UI_INPUT:
            return_value = title_menu_ui_input();
            break;
        case UI_RENDER:
        default:
            return_value = title_menu_ui_render();
    }
    return return_value;
}

/**
 * For use here in menus.c - Checks to see if a point is within the given
 * rectangle.  Does not use the w or h params of position's rect.
 * @return bool true on it is, false on it is not.
 */
bool menu_in_rect(SDL_Rect position, SDL_Rect rect)
{
    if(position.x <= rect.x+rect.w &&
        position.x >= rect.x &&
        position.y <= rect.y+rect.h &&
        position.y >= rect.y)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*Handles title menu's input.*/
int title_menu_ui_input()
{
    SDL_Surface *menu_start_text = game->get("menu_start_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    SDL_Surface *menu_help_text = game->get("menu_help_text");
    uint16_t    *title_menu_position = game->get("title_menu_position");
    uint32_t    *lastticks;
    SDL_Rect    start_target, exit_target, help_target;
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);

    /*Targets are used for mouse events.*/
    if((*title_menu_position) > 2 || (*title_menu_position) < 0)
    {
        (*title_menu_position) = 0;
    }

    /*Enabling mouse support will require knowing
    where the menu items are rendered*/
    start_target.x = (WINDOW_WIDTH/2) - (menu_start_text->w/2) - 20;
    start_target.y = current_y;
    start_target.w = menu_start_text->w+40;
    start_target.h = menu_start_text->h;

    /*Must match the rendering code below to be useful.*/
    current_y += (menu_start_text->h * 2);

    exit_target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2) - 20;
    exit_target.y = current_y;
    exit_target.w = menu_exit_text->w+40;
    exit_target.h = menu_exit_text->h;

    current_y += (menu_exit_text->h * 2);

    help_target.x = (WINDOW_WIDTH/2) - (menu_help_text->w/2) - 20;
    help_target.y = current_y;
    help_target.w = menu_help_text->w+40;
    help_target.h = menu_help_text->h;

    switch (game->event->type)
    {
        case SDL_MOUSEMOTION:
            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},start_target))
            {
                (*title_menu_position) = 0;
            }

            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},exit_target))
            {
                (*title_menu_position) = 1;
            }

            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},help_target))
            {
                (*title_menu_position) = 2;
            }

            break;
        case SDL_MOUSEBUTTONUP:
            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},start_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                //If lbl_lastticks is present, lets update it to right now.
                lastticks = game->get("lvl_lastticks");
                game->paused_time = 0;  //Game start should reset these.
                if(lastticks)
                {
                    (*lastticks) = SDL_GetTicks();
                }
                game->active_ui = &level_ui;
            }

            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},exit_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                return 1;
            }

            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},help_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                game->active_ui = &inst_title_menu_ui;
            }
            break;
        case SDL_KEYUP:
            switch(game->event->key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    if((*title_menu_position) > 0)
                    {
                        (*title_menu_position) -= 1;
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if((*title_menu_position) < 2)
                    {
                        (*title_menu_position) += 1;
                    }
                    break;

                case SDLK_KP_ENTER:
                case SDLK_RETURN:
                case SDLK_SPACE:
                case SDLK_z:
                    if((*title_menu_position) == 0)
                    {
                        //If lbl_lastticks is present, lets update it to right now.
                        lastticks = game->get("lvl_lastticks");
                        if(lastticks)
                        {
                            (*lastticks) = SDL_GetTicks() - game->paused_time;
                        }
                        game->active_ui = &level_ui;
                    }
                    else if((*title_menu_position) == 1)
                    {
                        return 1;
                    }
                    else
                    {
                        game->active_ui = &inst_title_menu_ui;
                    }
                default:
                    break;
            }

    }
    return 0;
}




/* Renders the pause menu. */
int title_menu_ui_render()
{
    SDL_Surface *menu_start_text = game->get("menu_start_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    SDL_Surface *menu_selector = game->get("menu_selector");
    SDL_Surface *background = game->get("menu_background");
    SDL_Surface *menu_help_text = game->get("menu_help_text");

    uint16_t    *title_menu_position = game->get("title_menu_position");
    SDL_Rect    target;
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);

    /*Draw background*/
    SDL_BlitSurface(background, NULL, game->screen, NULL);

    /*Place the start option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_start_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_start_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*title_menu_position) == 0)
    {
        target.x -= 48;
        target.y = current_y + (menu_start_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_start_text->h * 2);

    /*Place the exit game option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_exit_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*title_menu_position) == 1)
    {
        target.x -= 48;
        target.y = current_y + (menu_exit_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_exit_text->h * 2);

    /*Place the instruction option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_help_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_help_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*title_menu_position) == 2)
    {
        target.x -= 48;
        target.y = current_y + (menu_help_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }
    return 0;
}


int necromancy_ui_input();
int necromancy_ui_render();

/*
 * Necromancy UI
 */

int necromancy_ui(int ui_action)
{
    int return_value;
    switch(ui_action)
    {
        case UI_INPUT:
            return_value = necromancy_ui_input();
            break;
        case UI_RENDER:
        default:
            return_value = necromancy_ui_render();
    }
    return return_value;
}



/*Handles pause UI's input.*/
int necromancy_ui_input()
{
    SDL_Surface *pause_menu_title = game->get("pause_menu_title");
    SDL_Surface *menu_restart_text = game->get("menu_restart_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    uint16_t    *dead_menu_position = game->get("dead_menu_position");
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);
    SDL_Rect    restart_target, exit_target;
    level_st    *level = game->get("current_level");

    if((*dead_menu_position) > 1 || (*dead_menu_position) < 0)
    {
        (*dead_menu_position) = 0;
    }

    /*Enabling mouse support will require knowing
    where the menu items are rendered*/
    current_y += (pause_menu_title->h * 3);

    restart_target.x = (WINDOW_WIDTH/2) - (menu_restart_text->w/2) - 20;
    restart_target.y = current_y;
    restart_target.w = menu_restart_text->w+40;
    restart_target.h = menu_restart_text->h;

    /*Must match the rendering code below to be useful.*/
    current_y += (menu_restart_text->h * 2);

    exit_target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2) - 20;
    exit_target.y = current_y;
    exit_target.w = menu_exit_text->w+40;
    exit_target.h = menu_exit_text->h;

    current_y += (menu_exit_text->h * 2);

    //inst_pause_menu_ui
    switch (game->event->type)
    {
        case SDL_MOUSEMOTION:
            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},restart_target))
            {
                (*dead_menu_position) = 0;
            }

            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},exit_target))
            {
                (*dead_menu_position) = 1;
            }

            break;

        case SDL_MOUSEBUTTONUP:
            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},restart_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                remake_level(level);
                game->active_ui = &level_ui;
                break;
                //Retsrt level
            }

            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},exit_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                return 1;
            }
            break;
        case SDL_KEYUP:
            switch(game->event->key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    if((*dead_menu_position) > 0)
                    {
                        (*dead_menu_position) -= 1;
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if((*dead_menu_position) < 1)
                    {
                        (*dead_menu_position) += 1;
                    }
                    break;

                case SDLK_KP_ENTER:
                case SDLK_RETURN:
                case SDLK_SPACE:
                case SDLK_z:
                    if((*dead_menu_position) == 0)
                    {
                        remake_level(level);
                        game->active_ui = &level_ui;
                        //remake level, set active UI.
                    }
                    else if ((*dead_menu_position) == 1)
                    {
                        return 1;
                    }
                default:
                    break;
            }

    }
    return 0;
}

/* Renders the pause menu. */
int necromancy_ui_render()
{
    SDL_Surface *dead_menu_title = game->get("dead_menu_title");
    SDL_Surface *menu_restart_text = game->get("menu_restart_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    SDL_Surface *menu_selector = game->get("menu_selector");
    SDL_Surface *background = game->get("menu_background");;
    uint16_t    *dead_menu_position = game->get("dead_menu_position");
    SDL_Rect    target;
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);

    /*Draw background*/
    SDL_BlitSurface(background, NULL, game->screen, NULL);

    /*Place the menu title - centered.*/
    target.x = (WINDOW_WIDTH/2) - (dead_menu_title->w/2);
    target.y = current_y;
    SDL_BlitSurface(dead_menu_title, NULL, game->screen, &target);

    /*Update current_y...*/
    current_y += (dead_menu_title->h * 3);

    /*Place the resume option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_restart_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_restart_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*dead_menu_position) == 0)
    {
        target.x -= 48;
        target.y = current_y + (menu_restart_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_restart_text->h * 2);

    /*Place the exit game option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_exit_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*dead_menu_position) == 1)
    {
        target.x -= 48;
        target.y = current_y + (menu_exit_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_exit_text->h * 2);

    return 0;
}





/*Winning screen*/

int sheen_ui_input();
int sheen_ui_render();

/*
 * Necromancy UI
 */

int sheen_ui(int ui_action)
{
    int return_value;
    switch(ui_action)
    {
        case UI_INPUT:
            return_value = sheen_ui_input();
            break;
        case UI_RENDER:
        default:
            return_value = sheen_ui_render();
    }
    return return_value;
}



/*Handles pause UI's input.*/
int sheen_ui_input()
{
    SDL_Surface *pause_menu_title = game->get("pause_menu_title");
    SDL_Surface *menu_restart_text = game->get("menu_restart_game_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    uint16_t    *win_menu_position = game->get("win_menu_position");
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);
    SDL_Rect    restart_target, exit_target;

    if((*win_menu_position) > 1 || (*win_menu_position) < 0)
    {
        (*win_menu_position) = 0;
    }

    /*Enabling mouse support will require knowing
    where the menu items are rendered*/
    current_y += (pause_menu_title->h * 3);

    restart_target.x = (WINDOW_WIDTH/2) - (menu_restart_text->w/2) - 20;
    restart_target.y = current_y;
    restart_target.w = menu_restart_text->w+40;
    restart_target.h = menu_restart_text->h;

    /*Must match the rendering code below to be useful.*/
    current_y += (menu_restart_text->h * 2);

    exit_target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2) - 20;
    exit_target.y = current_y;
    exit_target.w = menu_exit_text->w+40;
    exit_target.h = menu_exit_text->h;

    current_y += (menu_exit_text->h * 2);

    //inst_pause_menu_ui
    switch (game->event->type)
    {
        case SDL_MOUSEMOTION:
            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},restart_target))
            {
                (*win_menu_position) = 0;
            }

            if(menu_in_rect((SDL_Rect){game->event->motion.x,game->event->motion.y,0,0},exit_target))
            {
                (*win_menu_position) = 1;
            }

            break;

        case SDL_MOUSEBUTTONUP:
            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},restart_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                game->set("current_level", game->first_level);
                remake_level(game->first_level);
                game->active_ui = &level_ui;
                break;
                //Retsrt level
            }

            if(menu_in_rect((SDL_Rect){game->event->button.x,game->event->button.y,0,0},exit_target)
                &&
               game->event->button.button == SDL_BUTTON_LEFT)
            {
                return 1;
            }
            break;
        case SDL_KEYUP:
            switch(game->event->key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    if((*win_menu_position) > 0)
                    {
                        (*win_menu_position) -= 1;
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if((*win_menu_position) < 1)
                    {
                        (*win_menu_position) += 1;
                    }
                    break;

                case SDLK_KP_ENTER:
                case SDLK_RETURN:
                case SDLK_SPACE:
                case SDLK_z:
                    if((*win_menu_position) == 0)
                    {
                        game->set("current_level", game->first_level);
                        remake_level(game->first_level);
                        game->active_ui = &level_ui;
                        break;
                        //remake level, set active UI.
                    }
                    else if ((*win_menu_position) == 1)
                    {
                        return 1;
                    }
                default:
                    break;
            }

    }
    return 0;
}

/* Renders the pause menu. */
int sheen_ui_render()
{
    SDL_Surface *dead_menu_title = game->get("win_menu_title");
    SDL_Surface *menu_restart_text = game->get("menu_restart_game_text");
    SDL_Surface *menu_exit_text = game->get("menu_exit_text");
    SDL_Surface *menu_selector = game->get("menu_selector");
    SDL_Surface *background = game->get("menu_background");;
    uint16_t    *win_menu_position = game->get("win_menu_position");
    SDL_Rect    target;
    uint16_t    *menu_start_y = game->get("menu_start_y");
    uint16_t    current_y = (*menu_start_y);

    /*Draw background*/
    SDL_BlitSurface(background, NULL, game->screen, NULL);

    /*Place the menu title - centered.*/
    target.x = (WINDOW_WIDTH/2) - (dead_menu_title->w/2);
    target.y = current_y;
    SDL_BlitSurface(dead_menu_title, NULL, game->screen, &target);

    /*Update current_y...*/
    current_y += (dead_menu_title->h * 3);

    /*Place the resume option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_restart_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_restart_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*win_menu_position) == 0)
    {
        target.x -= 48;
        target.y = current_y + (menu_restart_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_restart_text->h * 2);

    /*Place the exit game option.*/
    target.x = (WINDOW_WIDTH/2) - (menu_exit_text->w/2);
    target.y = current_y;
    SDL_BlitSurface(menu_exit_text, NULL, game->screen, &target);

    /*if Resume is the current option, show an indicator by it.*/
    if((*win_menu_position) == 1)
    {
        target.x -= 48;
        target.y = current_y + (menu_exit_text->h/2) - (menu_selector->h/2);
        SDL_BlitSurface(menu_selector, NULL, game->screen, &target);
    }

    current_y += (menu_exit_text->h * 2);

    return 0;
}
