#ifndef _PACMAN_CLYDE_H_
#define _PACMAN_CLYDE_H_

#include "defines.h"
#include "utils.h"
#include "game.h"

struct Anim;
struct Grid;
struct Pacman;

#include "ghost.h"

void clyde_init(struct Ghost* clyde, struct Anim* anims, struct Image* images);
void clyde_update(struct Ghost* clyde, struct Pacman* player, struct Grid* grid);
void clyde_set_target(struct Ghost* clyde, struct Pacman* player);

#endif
