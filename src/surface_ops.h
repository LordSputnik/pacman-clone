#ifndef _PACMAN_SURFACE_H_
#define _PACMAN_SURFACE_H_

#include <SDL/SDL.h>

SDL_Surface* flip_x (SDL_Surface* surface);
SDL_Surface* flip_y (SDL_Surface* surface);

SDL_Surface* scale (SDL_Surface* surface, float scale_x, float scale_y);

SDL_Surface* rotate (SDL_Surface* surface, int degrees);

SDL_Surface* rot_90 (SDL_Surface* surface);
SDL_Surface* rot_180 (SDL_Surface* surface);
SDL_Surface* rot_270 (SDL_Surface* surface);

#endif /* _PACMAN_SURFACE_H_ */
