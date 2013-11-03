#ifndef _PACMAN_BLINKY_H_
#define _PACMAN_BLINKY_H_

#include "defines.h"
#include "utils.h"
#include "game.h"

struct Anim;
struct Grid;
struct Pacman;

#include "ghost.h"

void blinky_init(struct Ghost* blinky, struct Anim* anims, struct Image* images);
void blinky_update(struct Ghost* blinky, struct Pacman* player, struct Grid* grid);
void blinky_set_target(struct Ghost* blinky, struct Pacman* player);

#endif /* _PACMAN_BLINKY_H_ */
