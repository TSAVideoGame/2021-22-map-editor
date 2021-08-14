#include "states.h"
#include "../resm/resm.h"
#include "../globals.h"
#include "snd/snd.h"

static int test_fn_create(struct STM_State *state)
{
  JIN_resm_add(&JIN_resm, "L", "res/L.wav", JIN_RES_BGM); 
  
  return 0;
}

static int test_fn_destroy(struct STM_State *state)
{
  return 0;
}

static int test_fn_update(struct STM_State *state)
{
  JIN_sndbgm_set(JIN_resm_get(&JIN_resm, "L"));
  JIN_sndbgm_play();

  return 0;
}

static int test_fn_draw(struct STM_State *state)
{
  return 0;
}

int JIN_states_test_create(struct STM_State *state)
{
  if (STM_state_create(state, "test", 0, 0, test_fn_create, test_fn_destroy, test_fn_update, test_fn_draw)) return -1;

  return 0;
}
