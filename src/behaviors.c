#include "main.h"

extern game_state *game;

int behavior_pursue(uint32_t lastticks,uint32_t nowticks,sprite_el *spr)
{
    sprite_el *player;
    velocity proposed_vel, distance_vel;
    point center_sprite, center_player;

    if( (player = game->get("lvl_player")))
    {
        center_player.x = (player->position.x + ( player->sprite_size.w/2));
        center_player.y = (player->position.y + ( player->sprite_size.h/2));
        center_sprite.x = (spr->position.x + ( spr->sprite_size.w/2));
        center_sprite.y = (spr->position.y + ( spr->sprite_size.h/2));
        distance_vel.x = proposed_vel.x = abs(center_player.x - center_sprite.x);
        distance_vel.y = proposed_vel.y = abs(center_player.y - center_sprite.y);

        if(center_player.y < center_sprite.y)
        {
            proposed_vel.y = proposed_vel.y * -1;
        }

        if(center_player.x < center_sprite.x)
        {
            proposed_vel.x = proposed_vel.x * -1;
        }

        if(proposed_vel.x > spr->vel_bound.upper_vel.x )
        {
            proposed_vel.x = spr->vel_bound.upper_vel.x;
        }
        else if(proposed_vel.x < spr->vel_bound.lower_vel.x )
        {
            proposed_vel.x = spr->vel_bound.lower_vel.x;
        }


        if(proposed_vel.y > spr->vel_bound.upper_vel.y )
        {
            proposed_vel.y = spr->vel_bound.upper_vel.y;
        }
        else if(proposed_vel.y < spr->vel_bound.lower_vel.y )
        {
            proposed_vel.y = spr->vel_bound.lower_vel.y;
        }
        else
        {
            proposed_vel.y *= 1;
        }

        if(abs(distance_vel.y) < 30 && abs(distance_vel.x) < 30)
        {
            proposed_vel.x = spr->vel.x;
            proposed_vel.y = spr->vel.y;
        }

        spr->vel.x = proposed_vel.x;
        spr->vel.y = proposed_vel.y;
    }
    return 0;
}
