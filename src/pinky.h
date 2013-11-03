#ifndef _PACMAN_PINKY_H_
#define _PACMAN_PINKY_H_

struct Anim;
struct Grid;
struct Pacman;

#include "ghost.h"

void pinky_init(struct Ghost* pinky, struct Anim* anims, struct Image* images);
void pinky_update(struct Ghost* pinky, struct Pacman* player, struct Grid* grid);
void pinky_set_target(struct Ghost* pinky, struct Pacman* player);

#endif /*  _PACMAN_PINKY_H_ */
