#ifndef _PACMAN_GRID_H_
#define _PACMAN_GRID_H_

#include "defines.h"
#include "utils.h"
#include <SDL/SDL.h>

struct Image;
struct Anim;

enum BlockType
{
    BLANK = 0,
    EMPTIED,
    SUPER_EMPTIED,
    H_LINE,
    V_LINE,
    CORNER_UL,
    CORNER_UR,
    CORNER_DR,
    CORNER_DL,
    GATE,
    DOT,
    SUPER_DOT,
    TUNNEL,
    FORBIDDEN
};

struct Grid
{
  enum BlockType blocks[GRID_X][GRID_Y];
};

int grid_load(const char* grid_name, struct Grid* grid);
void grid_reload(struct Grid* grid);
void grid_render(struct Grid* grid, struct Image* images, struct Anim* anims, SDL_Surface* screen);
int check_collisions_ghost(struct Grid* grid, int x, int y);
int check_collisions(struct Grid* grid, int x, int y);
uint8_t check_bounds(struct vec3i pos);

#endif /* _PACMAN_GRID_H_ */
