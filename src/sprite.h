/*
 * Ludum Dare Jam 23 - 4/20/2012
 */
#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

/*
 * Twiddly Bits
 */
#define SPEED_POWERUP   (1 << 0)

/*
 * Typedefs, Enums, Structs
 */
typedef enum {false, true} bool;

typedef enum
{
  SPRITE_UNKNOWN, SPRITE_PLAYER, SPRITE_PLAYER_BULLET,
  SPRITE_ENEMY, SPRITE_ENEMY_BULLET, SPRITE_NANOBOT1, SPRITE_NANOBOT2,
  SPRITE_CROSS, SPRITE_BLUE_BLOB, SPRITE_RED_BLOB, SPRITE_SPEED_POWERUP,
  SPRITE_OXYGEN, SPRITE_PURPLE_BLOB, SPRITE_ORANGE_BLOB, SPRITE_GREEN_BLOB,
  SPRITE_BLACK_BLOB
} sprite_type;

typedef struct {
	float x;
    float y;
} point;

typedef struct {
    point upper_vel;
    point lower_vel;
} velocity_bounds;

typedef point velocity;

typedef struct sprite_el
{
    struct sprite_el*   next;
    struct sprite_el*   prev;
    sprite_type         type;
    uint8_t             powerups;
    bool                firing;
    uint32_t            firing_delay;
    uint32_t            last_shot_time;
    point               position;    //Position on overall field of action.
    velocity            vel;         //Velocity;
    uint32_t            num_frames;  //Number of animated frames
    uint32_t            frame;       //Which sprite frame to use.
                                     //Right now: sprite rect is at X size * frame
    uint32_t            frame_delay; //Time between frame updates
    uint32_t            last_frame_time; //When the last frame was upated, used with delay
    uint32_t            spritesheet_revert_time; // When to go back to normal
    velocity_bounds     vel_bound;   //How fast is this thing actually allowed to go.
    int                 (*behavior)(uint32_t ,uint32_t nowticks,struct sprite_el* spr);
    SDL_Rect            sprite_size;
    SDL_Surface         *spritesheet;
    int32_t             health;
    int32_t             damage_power;
    uint32_t            ttl;         //Time to Live!  But why!?
    uint32_t            timespawn;   //Time of birth!
} sprite_el;

typedef struct sprite_el sprite;

typedef struct sprite_collision
{
    struct sprite_collision*    next;
    sprite*                     hitter;
    sprite*                     hittee;
} sprite_collision;

/*
 * Functions / Externs
 */
extern sprite_el* make_sprite(sprite_type, point, velocity);
extern void make_blank_sprite(sprite_el*);
extern void draw_sprite(SDL_Surface *, sprite *, point *);
extern void draw_all_sprites();
extern void update_all_sprites();
extern char *sprite_type_to_string(sprite_type);
extern sprite_type string_to_sprite_type(char *);
extern void check_sprite_collisions();
extern void clear_sprites();

#endif /* SPRITE_H_INCLUDED */
