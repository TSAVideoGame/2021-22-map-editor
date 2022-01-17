#include "stm/stm.h"
#include <JEL/jel.h>
#include <signal.h> 
#include <unistd.h> 
#include "resm/resm.h"
#include <time.h>
#include <stdio.h>
#include "gfx/gfx.h"
#include "components/components.h"
#include "core/core.h"
#include "anim/anim.h"

#define ASCII_0 48

#define TILE_SIZE 32


/*
 * MUSEUM
 *
 * This is a museum level, whether
 * it's an actual level or procedurally
 * generated shouldn't matter
 */
static int test_map_meta[] = { 7, 7 };
static char* test_map =
"000 000 000 000 000 000 000 "
"000 100 100 100 100 100 000 "
"000 100 100 100 100 100 000 "
"000 200 200 200 200 200 000 "
"000 200 200 200 200 200 000 "
"000 200 200 200 200 200 000 "
"000 000 000 000 000 000 000 "
;
static int map_x, map_y;
static JEL_Entity* tiles;
static JEL_Entity player;
static struct { int x; int y; } camera;

static int museum_fn_create(struct STM_S *state)
{
  map_x = map_meta[0];
  map_y = map_meta[1];

  tiles = malloc(sizeof(JEL_Entity) * map_x * map_y);
 
  int spawn_x, spawn_y;

  for (int i = 0; i < map_x * map_y; ++i) {
    tiles[i] = JEL_entity_create();
    JEL_SET(tiles[i], Position, (i % map_x) * TILE_SIZE, (i / map_x) * TILE_SIZE);
    JEL_SET(tiles[i], Sprite, 0, TILE_SIZE, TILE_SIZE, map_tiles[i] * 32, 16, 32, 32, 0);
    /* Collision */
    int coltype = map_collisions[i];
    if (coltype) {
      void (*col_fn)(JEL_Entity, JEL_Entity);
      switch (coltype) {
        case 1:
          col_fn = tile_collision_fn;
          break;
        default:
          col_fn = dummy_collision_fn;
          break;
      }
      JEL_SET(tiles[i], AABB, TILE_SIZE, TILE_SIZE, col_fn);
    }
    /* Item */
    switch (map_items[i]) {
      case 1:
        spawn_x = i % map_y * TILE_SIZE;
        spawn_y = i / map_y * TILE_SIZE;
        break;
      default: break;
    }
    test_map -= (map_x * map_y - 1) * 4;

    player = JEL_entity_create();
    JEL_SET(player, Position, 256, 256);
    JEL_SET(player, Physics, 0, 0, 0, 0);
    JEL_SET(player, Sprite, 1, TILE_SIZE, TILE_SIZE, 0, 0, 16, 16, 0);
    JEL_SET(player, Animation, (struct JIN_Animd*)JIN_resm_get("player_animation"), 1, 0, 0, );

    return 0;
}

static int museum_fn_destroy(struct STM_S* state)
{
    for (int i = 0; i < map_x * map_y; ++i) {
        JEL_entity_destroy(tiles[i]);
    }

    free(tiles);

    JEL_entity_destroy(player);

    return 0;
}

int player_time(float mv) {
    float seconds = 0;
    void handleAlarm(int sig) { // Handles the signal alarm
        if (mv >= 5) {
            seconds = 0;
        }
        seconds++;
        //printf("\rElapsed time : %f", seconds);
        alarm(1); // Sends alarm signal after one second 
    }
    setbuf(stdout, NULL);

    //printf("\rElapsed time : %f", seconds);
    signal(SIGALRM, handle); // Assigns a handler for the alarm signal 
    alarm(1); // Sends alarm signal after one second 
    while (1); // Prevents the process from terminating 
    return seconds;
}

/*
 * Probably put player movement into a different function
 * Figure out a 'friction' for the player
 *
 * Equation is a - a(v^2 / vT^2)
 */
static int player_movement(void)
{
    float accel = .5;
    float max_vel = 5;

    struct Physics phys;
    JEL_GET(player, Physics, &phys);
    if (JIN_input.keys.a || JIN_input.keys.d) {

        if (JIN_input.keys.a) {
            JEL_SET_PROP(player, Sprite, dir, 1);
            phys.x_accel = -1 * accel + accel * ((phys.x_vel * phys.x_vel) / (max_vel * max_vel));
            phys.x_vel = -1 * player_time(max_vel) * phys.x_accel;
            JEL_SET_PROP(player, Physics, x_accel, phys.x_accel);
            JEL_SET_PROP(player, Physics, x_vel, phys.x_vel);
        }
        if (JIN_input.keys.d) {
            JEL_SET_PROP(player, Sprite, dir, 0);
            phys.x_accel = accel - accel * ((phys.x_vel * phys.x_vel) / (max_vel * max_vel));
            phys.x_vel = player_time(max_vel) * phys.x_accel;
            JEL_SET_PROP(player, Physics, x_accel, phys.x_accel);
            JEL_SET_PROP(player, Physics, x_vel, phys.x_vel);
        }

    }
    else {
        phys.x_accel = phys.x_vel / -2;
        JEL_SET_PROP(player, Physics, x_accel, phys.x_accel);
    }

    if (JIN_input.keys.w || JIN_input.keys.s) {
        if (JIN_input.keys.w) {
            phys.y_accel = -1 * accel + accel * ((phys.y_vel * phys.y_vel) / (max_vel * max_vel));
            phys.y_vel = player_time(max_vel) * phys.y_accel;
            JEL_SET_PROP(player, Physics, y_accel, phys.y_accel);
            JEL_SET_PROP(player, Physics, y_vel, phys.y_vel);
        }
        if (JIN_input.keys.s) {
            phys.y_accel = accel - accel * ((phys.y_vel * phys.y_vel) / (max_vel * max_vel));
            phys.y_vel = player_time(max_vel) * phys.y_accel;
            JEL_SET_PROP(player, Physics, y_accel, phys.y_accel);
            JEL_SET_PROP(player, Physics, y_vel, phys.y_vel);
        }
    }
    else {
        phys.y_accel = phys.y_vel / -2;
        JEL_SET_PROP(player, Physics, y_accel, phys.y_accel);
    }

    /* Animation */
    if (JIN_input.keys.w || JIN_input.keys.a || JIN_input.keys.s || JIN_input.keys.d) {
        JIN_anim_set(player, "RUN");
    }
    else {
        JIN_anim_set(player, "IDLE");
    }

    return 0;
}

static void update_camera(void)
{
  struct Position pos;
  JEL_GET(player, Position, &pos);
  camera.x = pos.x + TILE_SIZE / 2 - WINDOW_WIDTH / 2;
  camera.y = pos.y + TILE_SIZE / 2 - WINDOW_HEIGHT / 2;
}

static int museum_fn_update(struct STM_S *state)
{
    player_movement();
    JIN_anim_update();

    struct JEL_Query q;
    JEL_QUERY(q, Position, Physics);

    for (unsigned int i = 0; i < q.count; ++i) {
        struct PositionIt pos;
        struct PhysicsIt phys;
        JEL_IT(pos, q.tables[i], Position);
        JEL_IT(phys, q.tables[i], Physics);

        for (JEL_EntityInt j = 1; j < q.tables[i]->count; ++j) {
            /* Explicit Euler Integration ;) */
            phys.x_vel[j] += phys.x_accel[j];
            pos.x[j] += phys.x_vel[j];

            phys.y_vel[j] += phys.y_accel[j];
            pos.y[j] += phys.y_vel[j];
        }
    }


  JEL_query_destroy(&q);
  update_camera();

  if (JIN_input.keys.o == 1) {
    JIN_stm_queue("PAUSE", STM_PERSIST_PREV);
  }

    return 0;
}

static int museum_fn_draw(struct STM_S* state)
{
    JIN_gfx_sprite_draw(camera.x, camera.y);

    return 0;
}

int JIN_states_create_museum(struct STM_S* state)
{
    if (STM_s_create(state, 0, museum_fn_create, museum_fn_destroy, museum_fn_update, museum_fn_draw)) return -1;
    return 0;
}

