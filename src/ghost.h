#ifndef _PACMAN_GHOST_H_
#define _PACMAN_GHOST_H_

#include "defines.h"
#include "utils.h"
#include <SDL/SDL.h>
#include <stdint.h>
#include <limits.h>

struct Anim;
struct Grid;

struct Ghost
{
  struct Anim* current_anim_;
  struct Anim* ghost_anim_[4];
  struct Image* eyes[4];
  struct Anim* wibbly_anim_;

  struct vec3f draw_pos_f_;
  SDL_Rect draw_pos_;

  struct vec3i pos_;
  struct vec3i dir_;
  struct vec3i target_;
  struct vec3i waypoint_;

  float speed_;

  char behaviour_; /*0 - IN HOUSE, 1 - CHASE, 2 - SCATTER, 3 - FLEE*/
  char in_house;
  char frightened_;

  uint8_t dot_counter;
  uint8_t exit_house;

  uint8_t render_dir_;

  char reverse_next_tile_;
  char eaten;
};

void ghost_move_out_of_home(struct Ghost* ghost);
void ghost_choose_direction(struct Ghost* ghost);
void ghost_choose_waypoint(struct Ghost* ghost, struct Grid* grid);
void ghost_render(struct Ghost* ghost, SDL_Surface* screen);
void ghost_set_behaviour(struct Ghost* ghost, char behaviour);
void ghost_set_frightened(struct Ghost* ghost, char active);

#endif /*  _PACMAN_GHOST_H_ */
