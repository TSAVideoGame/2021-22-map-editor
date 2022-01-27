#include "resm/resm.h"
#include "core/logger/logger.h"

/*
 * CORE INIT RES
 *
 * A list of resources to initialize
 */

#define INIT_RESOURCE_LIST \

static int init_resources(void)
{
  #define X(name, path, type) if (JIN_resm_add(name, path, type)) ERR_EXIT(-1, "Can't create resource at " #path ". Res type is " #type);
  INIT_RESOURCE_LIST
  #undef X
  return 0;
}
