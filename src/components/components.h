#ifndef JIN_COMPONENTS_H
#define JIN_COMPONENTS_H

/*
 * COMPONENTS
 *
 * Some components that are global or do not really go anywhere else
 * (at least for now)
 */

#include <JEL/jel.h>

struct Position {
  int x;
  int y;
};
JEL_COMPONENT(Position, x, y);

#endif
