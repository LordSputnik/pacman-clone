#ifndef _PACMAN_INKY_H_
#define _PACMAN_INKY_H_

struct Anim;
struct Grid;
struct Pacman;

#include "ghost.h"

void inky_init(struct Ghost* inky, struct Anim* anims, struct Image* images);
void inky_update(struct Ghost* inky, struct Ghost* blinky, struct Pacman* player, struct Grid* grid);
void inky_set_target(struct Ghost* inky, struct Ghost* blinky, struct Pacman* player);

#endif /*  _PACMAN_INKY_H_ */
