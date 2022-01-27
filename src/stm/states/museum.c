#include "stm/stm.h"
#include <JEL/jel.h>
#include "resm/resm.h"
#include "gfx/gfx.h"
#include "components/components.h"
#include "core/core.h"

#define ASCII_0 48

#define TILE_SIZE 32

/*
 * MUSEUM
 *
 * This is a museum level, whether
 * it's an actual level or procedurally
 * generated shouldn't matter
 *
 * Make sure you set these variables below before
 * you queue/push this state
 */
int   map_meta[2];
char *map_tiles;
char *map_items;
char *map_collisions;

static int map_x, map_y;
static JEL_Entity *tiles;
static JEL_Entity player;
static struct { int x; int y; } camera;

#include "maps/map.c"
static int museum_fn_create(struct STM_S *state)
{
  for (int i = 0; i < 2; ++i) {
    map_meta[i] = test_meta[i];
  }
  map_tiles = test_tiles;
  map_items = test_items;
  map_collisions = test_collisions;

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
    }
    JEL_Entity new_item;
    /* Item */
    switch (map_items[i]) {
      case 1: /* Player spawn */
        spawn_x = i % map_x * TILE_SIZE;
        spawn_y = i / map_x * TILE_SIZE;
        break;
      case 2: /* Artifact */
        new_item = JEL_entity_create();
        JEL_SET(new_item, Position, i % map_x * TILE_SIZE, i / map_x * TILE_SIZE);
        JEL_SET(new_item, Sprite, 1, TILE_SIZE, TILE_SIZE, 160, 16, 32, 32, 0);
        break;
      case 3: /* Guard */
        new_item = JEL_entity_create();
        JEL_SET(new_item, Position, i % map_x * TILE_SIZE, i / map_x * TILE_SIZE);
        JEL_SET(new_item, Sprite, 1, TILE_SIZE, TILE_SIZE, 32, 0, 16, 16, 0);
        break;
      case 4: /* Guard */
        new_item = JEL_entity_create();
        JEL_SET(new_item, Position, i % map_x * TILE_SIZE, i / map_x * TILE_SIZE);
        JEL_SET(new_item, Sprite, 1, TILE_SIZE, TILE_SIZE, 32, 0, 16, 16, 0);
        break;
      default: break;
    }
  }

  player = JEL_entity_create();
  JEL_SET(player, Position, spawn_x, spawn_y);

  return 0;
}

static int museum_fn_destroy(struct STM_S *state)
{
  for (int i = 0; i < map_x * map_y; ++i) {
    JEL_entity_destroy(tiles[i]);
  }

  free(tiles);

  JEL_entity_destroy(player);


  /* Not ideal but it works for now */
  struct JEL_Query q;
  JEL_QUERY(q, Sprite);
  for (unsigned int t = 0; t < q.count; ++t) {
    struct JEL_EntityCIt e;
    JEL_IT(e, q.tables[t], JEL_EntityC);

    for (unsigned int i = 1; q.tables[t]->count > 1;) {
      JEL_entity_destroy(e.entity[i]);
    }
  }
  JEL_query_destroy(&q);

  return 0;
}

/*
void handleAlarm(int sig) { // Handles the signal alarm
    if (mv >= 5) {
        seconds = 0;
    }
    seconds++;
    //printf("\rElapsed time : %f", seconds);
    alarm(1); // Sends alarm signal after one second 
}
int player_time(float mv) {
    float seconds = 0;
    setbuf(stdout, NULL);

    //printf("\rElapsed time : %f", seconds);
    signal(SIGALRM, handle); // Assigns a handler for the alarm signal 
    alarm(1); // Sends alarm signal after one second 
    while (1); // Prevents the process from terminating 
    return seconds;

}
*/

static void update_camera(void)
{
  if (JIN_input.keys.w) {
    camera.y -= 4; 
  }
  if (JIN_input.keys.a) {
    camera.x -= 4;
  }
  if (JIN_input.keys.s) {
    camera.y += 4; 
  }
  if (JIN_input.keys.d) {
    camera.x += 4;
  }
}

static int museum_fn_update(struct STM_S *state)
{
  update_camera();

  return 0;
}

static int museum_fn_draw(struct STM_S *state)
{
  JIN_gfx_sprite_draw(camera.x, camera.y);

  return 0;
}

int JIN_states_create_museum(struct STM_S *state)
{
  if (STM_s_create(state, 0, museum_fn_create, museum_fn_destroy, museum_fn_update, museum_fn_draw)) return -1;
  return 0;
}

