#ifndef _PACMAN_GAME_H_
#define _PACMAN_GAME_H_

#include <stdint.h>
#include "image.h"
#include "hiscore.h"

struct Grid;
struct Ghost;
struct vec3i;

void set_behaviour(char behaviour);
char get_ghost_behaviour();

float get_blinky_speed(struct Ghost* ghost, struct Grid* grid);
float get_pinky_speed();
float get_inky_speed();
float get_clyde_speed();
float get_ghost_speed(struct Ghost* ghost, struct Grid* grid);
float get_pacman_speed();

struct Pacman* get_pacman();
struct Grid* get_grid();

void kill_pacman();

void set_remaining_dots(uint8_t dot_count);
void dec_remaining_dots();
uint8_t get_remaining_dots();

void inc_level();

void init_game(struct Anim* anims, struct Image* images);
void init_level();

uint32_t get_score();
uint8_t get_game_over();

void spawn_bonus();
uint8_t is_bonus_spawned();
void eat_bonus();
void kill_bonus();
void eat_dot();
void eat_super_dot();

void set_frightened(char active);
uint8_t get_frightened_state();
void notify_ghost_eaten();

void update_game(struct Anim* anims, struct Image* images, uint32_t dt);
void render_game(struct Anim* anims, struct Image* images, SDL_Surface* screen);

#endif /*  _PACMAN_GAME_H_ */

