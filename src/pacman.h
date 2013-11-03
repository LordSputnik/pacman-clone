#ifndef _PACMAN_PACMAN_H_
#define _PACMAN_PACMAN_H_

#include "defines.h"
#include "utils.h"
#include "game.h"
#include <SDL/SDL.h>

struct Anim;
struct Grid;

struct Pacman
{
    struct Anim* pacman_anim_;
    struct Anim* death_anim_;

    SDL_Rect draw_pos_;
    struct vec3f draw_pos_f_;

    struct vec3i pos_;
    struct vec3i last_dir_;
    struct vec3i dir_;
    struct vec3i next_dir_;

    float speed_;
    float rotation_;
    uint8_t dying;
};

void pacman_init(struct Pacman* player, struct Anim* anims);
void pacman_reset(struct Pacman* player);
void pacman_update(struct Pacman* player, struct Grid* grid);
void pacman_render(struct Pacman* player, SDL_Surface* screen);
void pacman_set_next_dir(struct Pacman* player, struct Grid* grid, int x, int y);
void pacman_change_direction(struct Pacman* player, struct Grid* grid);

#endif /* _PACMAN_PACMAN_H_ */
