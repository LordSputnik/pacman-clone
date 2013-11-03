#ifndef _PACMAN_IMAGE_H_
#define _PACMAN_IMAGE_H_

#include "defines.h"
#include "utils.h"
#include "surface_ops.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdint.h>

struct Image
{
    int width;
    int height;
    int depth;

    SDL_Surface* img;
};

struct Anim
{
    uint8_t id;
    uint16_t default_frametime;
    uint16_t frametime;
    uint8_t loop;
    uint8_t numFrames;

    uint16_t width;
    uint16_t height;
    uint16_t depth;

    float scale;
    uint8_t scaleDone;
    float* rotate;
    uint8_t rotDone;

    SDL_Surface** sdl_frames;

    uint8_t frameIndex;
    uint32_t tickTime;
    uint8_t playing;
    SDL_Surface* currentFrame;
};

int load_image      (const char* imgName, struct Image* imglist, uint8_t* imgCount);
int load_anim       (const char* animName, struct Anim* animlist, uint8_t* animCount);


void flipX_image(struct Image* imglist, uint8_t imgID);
void flipY_image(struct Image* imglist, uint8_t imgID);
void rotate_image(struct Image* imglist, uint8_t imgID, int16_t degrees);
void scale_image(struct Image* imglist, uint8_t imgID, float scaleFactor);
int copy_image(struct Image* imglist, uint8_t* imgCount, uint8_t imgSrcID);

void play_anims      (struct Anim* animlist, uint8_t animID);
void play_anim     (struct Anim* currentAnim);
void stop_anims      (struct Anim* animlist, uint8_t animID);
void stop_anim      (struct Anim* currentAnim);
void pause_anims    (struct Anim* animlist, uint8_t animID);
void pause_anim     (struct Anim* currentAnim);
void set_frametime  (struct Anim* curAnim, uint16_t newtime);

void rotate_anims   (struct Anim* animlist, uint8_t animID, float degrees);
void rotate_anim    (struct Anim* curAnim, float degrees);
void flipX_anim     (struct Anim* curAnim);
void flipY_anim     (struct Anim* curAnim);
void scale_anim     (struct Anim* animlist, uint8_t animID, float scale_factor);
void tick_anims     (struct Anim* animlist, uint8_t animCount);

int free_images(struct Image* imglist, uint8_t imgCount);
int free_anims(struct Anim* animlist, uint8_t animCount);

#endif /* _PACMAN_IMAGE_H_ */
