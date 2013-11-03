#ifndef _PACMAN_DEFINES_H_
#define _PACMAN_DEFINES_H_

#include <stdint.h>

#define IMG_DIR "images/"
#define ANIM_DIR "anims/"
#define GRID_DIR "levels/"

#define MAX_IMAGE_COUNT 100
#define MAX_ANIM_COUNT 20

#define GRID_CELL_SIZE 20

#define SCREEN_X 560
#define SCREEN_Y 720

#define GRID_X 28
#define GRID_Y 36

#define MOVE_SPEED 3

#define true 1
#define false 0

#define EYES_IMAGE 9

#define BLINKY_ANIM 1
#define PINKY_ANIM 5
#define INKY_ANIM 9
#define CLYDE_ANIM 13
#define WIBBLY_ANIM 17
static const uint8_t DEATH_ANIM = 18;

#define ZERO_IMG 16
#define CHERRY_IMG 26
#define STRAWBERRY_IMG 27
#define ORANGE_IMG 28
#define APPLE_IMG 29
#define GRAPES_IMG 30
#define GAX_IMG 31
#define BELL_IMG 32
#define KEY_IMG 33
#define LIFE_IMG 34

#endif /* _PACMAN_DEFINES_H_ */
