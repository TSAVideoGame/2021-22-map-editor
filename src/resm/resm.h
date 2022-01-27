#ifndef RESM_H
#define RESM_H

/*
 * RESOURCE MANAGER
 *
 * A resource is a 'thing' the game uses like
 * an image, a sound, or a script (they are
 * all external files)
 *
 * Resources are shared across the game, so
 * they will be accesed by a unsigned long int
 * id and a type
 *
 * TYPES:
 * enum   RESM_T | Types of resources
 * struct RESM_M | The actual resource manager
 *
 */

enum RESM_T {
  /* Graphics */
  RESM_SHADER, /* OpenGL shader program */
  RESM_PNG,    /* OpenGL texture */
  RESM_MODEL,  /* OpenGL VAO and VBO */
};

struct RESM_M {
  unsigned int allocated;
  unsigned int count;
  void       **resources;
  char       **names;
  enum RESM_T *types;
};

/*
 * Resource Manager Functions
 *
 * create  | Create a resource manager
 * destroy | Destroy a resource manager
 * add     | Add a resource
 * get     | Get a resource
 */
int    RESM_create (struct RESM_M *resm);
void   RESM_destroy(struct RESM_M *resm);
int    RESM_add    (struct RESM_M *resm, const char *name, const char *fpath, enum RESM_T type);
void * RESM_get    (struct RESM_M *resm, const char *name);

/*
 * JIN Functions
 */
int    JIN_resm_add(const char *name, const char *fpath, enum RESM_T type);
void * JIN_resm_get(const char *name);

#endif
