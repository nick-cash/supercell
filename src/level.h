/*
 * Ludum Dare Jam 23 - 4/20/2012
 */
#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

/*
 * Includes
 */
#include "cJSON.h"
#include "sprite.h"

/*
 * Defines
 */
#define MAX_LEVEL_SIZE  5242880

/*
 * Enumerations/declarations
 */
struct level_st;
struct proto_sprite;

/*
 * Structs
 */
typedef struct level_st
{
    char*                name;
    char*                filename;
    uint32_t             time_when_loaded;
    uint32_t             time_level_over;
    uint32_t             num_enemies_killed_level_over;
    uint32_t             num_enemies_killed;
    uint32_t             last_oxygen;
    uint32_t             oxygen_rate;
    uint32_t             last_hunter;
    uint32_t             hunter_rate;
    struct level_st*     next;
    struct proto_sprite* first_proto;
} level_st;

typedef struct proto_sprite
{
    char*                id;
    struct proto_sprite* next;
    sprite               spr;
    sprite_type          type;
    point                start_position;
    point                velocity;
    point                spawn_vector;
    uint32_t             spawn_interval;
    uint32_t             last_spawn_time;
    uint32_t             start_spawn_time;
    uint32_t             number_to_spawn;
} proto_sprite;

/*
 * Externs
 */
extern void update_level(level_st*);
extern void load_levels();
extern level_st* remake_level(level_st* old);

#endif // LEVEL_H_INCLUDED
