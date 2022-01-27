#include "resm.h"
#include <string.h>
#include <stdlib.h>
#include "core/logger/logger.h"

/* Graphics */
#include "gfx/gfx.h"

#define GROWTH_FACTOR 2
#define INITIAL_COUNT 8

/*
 * RESM_allocate
 *
 * @desc
 *   Allocates more memory for a RESM_M
 * @param resm
 *   RESM_M to allocate memory for
 * @param count
 *   How many items to allocate
 * @return
 *    0 on success
 *    1 on already has enough memory
 *   -1 on out of memory
 */
#define ALLOC_VAR(type, name) \
  { \
    type name; \
    if (!(name = malloc(count * sizeof(*name)))) ERR_EXIT(-1, "Allocate resm: Out of memory"); \
    memcpy(name, resm->name, resm->count * sizeof(*name)); \
    free(resm->name); \
    resm->name = name; \
  }
static int RESM_allocate(struct RESM_M *resm, unsigned int count)
{
  if (resm->allocated >= count) return 1;

  ALLOC_VAR(void **,         resources);
  ALLOC_VAR(char **,         names);
  ALLOC_VAR(enum RESM_T *,   types);

  resm->allocated = count;

  return 0;
}

/*
 * RESM_create
 *
 * @desc
 *   Creates a RESM_M
 * @param resm
 * @return
 */
int RESM_create(struct RESM_M *resm)
{
  if (!(resm->resources = malloc(INITIAL_COUNT * sizeof(void *       )))) ERR_EXIT(-1, "Create resm: Out of memory");
  if (!(resm->names     = malloc(INITIAL_COUNT * sizeof(char *       )))) ERR_EXIT(-1, "Create resm: Out of memory");
  if (!(resm->types     = malloc(INITIAL_COUNT * sizeof(enum RESM_T)))) ERR_EXIT(-1, "Create resm: Out of memory");

  resm->count = 0;
  resm->allocated = INITIAL_COUNT;

  return 0;
}

/*
 * RESM_destroy
 *
 * @desc
 *   Destroys a RESM_M and all of its
 *   resources
 * @param resm
 *   RESM_M to destroy
 */
void RESM_destroy(struct RESM_M *resm)
{
  for (unsigned int i = 0; i < resm->count; ++i) {
    free(resm->names[i]);

    switch (resm->types[i]) {
      case RESM_SHADER:
        JIN_shader_destory(resm->resources[i]);
        break;
      case RESM_PNG:
        JIN_texture_destroy(resm->resources[i]);
        break;
      case RESM_MODEL:
        JIN_model_destory(resm->resources[i]);
        break;
    }

    free(resm->resources[i]);
  }

  free(resm->resources);
  free(resm->names);
  free(resm->types);
}

/*
 * RESM_add
 *
 * @desc
 *   Adds a resource to RESM_M
 * @param resm
 *   RESM_M to add to
 * @param name
 *   What the resources will be named
 * @param fpath
 *   File location of the resources
 * @param type
 *   Type of the resource (used to figure out
 *   what function to call)
 * @return
 *    0 on success
 *   -1 on failure
 */
#define RES_MALLOC(type) if (!(resm->resources[resm->count] = malloc(sizeof(type)))) return -1;
int RESM_add(struct RESM_M *resm, const char *name, const char *fpath, enum RESM_T type)
{
  if (resm->allocated <= resm->count) {
    if (RESM_allocate(resm, resm->count * GROWTH_FACTOR)) ERR_EXIT(-1, "Could not allocate resm");
  }

  size_t name_size = strlen(name) + 1;
  if (!(resm->names[resm->count] = malloc(name_size * sizeof(char)))) return -1;
  for (size_t i = 0; i < name_size; ++i) resm->names[resm->count][i] = name[i];

  resm->types[resm->count] = type;

  switch (type) {
    case RESM_SHADER:
      RES_MALLOC(unsigned int);
      JIN_shader_create(resm->resources[resm->count], fpath);
      break;
    case RESM_PNG:
      RES_MALLOC(unsigned int);
      JIN_texture_create(resm->resources[resm->count], fpath);
      break;
    case RESM_MODEL:
      RES_MALLOC(struct JIN_Model);
      JIN_model_create(resm->resources[resm->count], fpath);
      break;
  }

  ++resm->count;

  return 0;
}

/*
 * RESM_get
 *
 * @desc
 *   TODO: Use map instead of linear search
 *   Get a resource given its name.
 * @param resm
 *   RESM_M to search
 * @param name
 *   Name of the resource to get
 * @return
 *   Pointer to the resource
 *   NULL if not found
 */
void * RESM_get(struct RESM_M *resm, const char *name)
{
  for (unsigned int i = 0; i < resm->count; ++i) {
    if (!strcmp(resm->names[i], name)) {
      return resm->resources[i];
    }
  }

  return NULL;
}

/*
 * JIN Wrappers
 *
 * They're just wrappers but use
 * the global JIN_resm RESM_M
 */
#include "core/globals.h"
int JIN_resm_add(const char *name, const char *fpath, enum RESM_T type)
{
  return RESM_add(&JIN_resm, name, fpath, type);
}

void * JIN_resm_get(const char *name)
{
  return RESM_get(&JIN_resm, name);
}
