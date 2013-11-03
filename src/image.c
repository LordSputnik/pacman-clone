#include "image.h"
#include <stdio.h>

int load_image(const char* imgName, struct Image* imglist, uint8_t* imgCount)
{
  struct Image * curImg = imglist + (*imgCount);

  SDL_Surface* temp = IMG_Load(imgName);
  printf("Temp: %i Img Name: %s\n",temp,imgName);
  puts("Seg fault.");
  curImg->img = SDL_DisplayFormatAlpha(temp);
  SDL_FreeSurface(temp);

  SDL_SetAlpha(curImg->img, SDL_SRCALPHA | SDL_RLEACCEL, SDL_ALPHA_TRANSPARENT);

  curImg->width = curImg->img->w;
  curImg->height = curImg->img->h;
  curImg->depth = curImg->img->format->BytesPerPixel;

  printf("Image %i (%s) Loaded! %i x %i x %i bytes.\n",(*imgCount),imgName,curImg->width,curImg->height,curImg->depth);

  return (*imgCount)++;
}

void flipX_image(struct Image* imglist, uint8_t imgID)
{
    struct Image* curImg = imglist+imgID;

    flip_x(curImg->img);
}

void flipY_image(struct Image* imglist, uint8_t imgID)
{
    struct Image* curImg = imglist+imgID;

    flip_y(curImg->img);
}

void rotate_image(struct Image* imglist, uint8_t imgID, int16_t degrees)
{
    struct Image* curImg = imglist+imgID;

    curImg->img = rotate(curImg->img, degrees);

    curImg->width = curImg->img->w;
    curImg->height = curImg->img->h;
}

void scale_image(struct Image* imglist, uint8_t imgID, float scaleFactor)
{
    struct Image* curImg = imglist+imgID;

    curImg->img = scale(curImg->img,scaleFactor,scaleFactor);

    curImg->width = curImg->img->w;
    curImg->height = curImg->img->h;
}

int copy_image     (struct Image* imglist, uint8_t* imgCount, uint8_t imgSrcID)
{
    struct Image * curImg = imglist + (*imgCount);

    memcpy(curImg, &imglist[imgSrcID], sizeof(imglist[imgSrcID])); /* Get height, width and depth - NOT SURFACE. */

    curImg->img = SDL_ConvertSurface(imglist[imgSrcID].img, imglist[imgSrcID].img->format, imglist[imgSrcID].img->flags);

    printf("Image Copied to %i! %i x %i x %i bytes.\n",(*imgCount),curImg->width,curImg->height,curImg->depth);

    return (*imgCount)++;
}

int load_anim(const char* animName, struct Anim* animlist, uint8_t* animCount)
{
    struct Anim* curAnim = animlist + (*animCount);
    int filePathSize = strlen(animName) + strlen(ANIM_DIR);
    char buffer[100];
    char* value = NULL;
    FILE* animFile = NULL;

    char* filePath = Malloc((filePathSize+1)*sizeof(char));
    printf("File path size: %u\n",filePathSize);
    strcpy(filePath,ANIM_DIR);
    strcat(filePath,animName);

    animFile = fopen ( filePath, "r" );

    if(animFile == NULL)
    {
        printf("Could not find animation file: %s!\n", filePath);
        Free(filePath);
        return -1;
    }
    else
    {
      uint8_t i;
      uint16_t int_buf;

      fgets (buffer, 10, animFile);
      value = strtok(buffer," \n");
      sscanf(value,"%hu", &int_buf);
      curAnim->numFrames = (uint8_t)(int_buf);

      fgets (buffer, 10, animFile);
      value = strtok(buffer," \n");
      sscanf(value,"%hu", &(curAnim->frametime));
      curAnim->default_frametime = curAnim->frametime;

      fgets (buffer, 10, animFile);
      value = strtok(buffer," \n");
      sscanf(value,"%hu", &int_buf);
      curAnim->loop = (uint8_t)(int_buf);

      printf("Animation %s has %i frames, with a frametime of %ims. ", animName, curAnim->numFrames, curAnim->frametime);
      if(curAnim->loop == 0)
          puts("It won't loop.");
      else
          puts("It will loop.");

      curAnim->sdl_frames = (SDL_Surface**)Malloc(curAnim->numFrames*sizeof(SDL_Surface*));

      for( i = 0; i != curAnim->numFrames; ++i)
      {
        SDL_Surface* temp = NULL;
        fgets (buffer, 100, animFile);
        value = strtok(buffer," \n");

        printf("Loading image: %s\n",value);

        temp = IMG_Load(value);
        curAnim->sdl_frames[i] = SDL_DisplayFormatAlpha(temp);
        SDL_FreeSurface(temp);

        SDL_SetAlpha(curAnim->sdl_frames[i], SDL_SRCALPHA | SDL_RLEACCEL, SDL_ALPHA_TRANSPARENT);
      }
      fclose ( animFile );
      Free(filePath);
      curAnim->playing = 0;

      curAnim->width = curAnim->sdl_frames[0]->w;
      curAnim->height = curAnim->sdl_frames[0]->h;
      curAnim->depth = curAnim->sdl_frames[0]->format->BytesPerPixel;

      curAnim->rotDone = curAnim->numFrames;
      curAnim->scaleDone = curAnim->numFrames;
      curAnim->scale = 0.0f;
      curAnim->frameIndex = 0;

      curAnim->rotate = Malloc(curAnim->numFrames*sizeof(float));

      for(i = 0; i != curAnim->numFrames; ++i)
      {
          curAnim->rotate[i] = 0.0f;
      }


      return curAnim->id = (*animCount)++;
    }
}

void play_anims(struct Anim* animlist, uint8_t animID)
{
    struct Anim* curAnim = &(animlist[animID]);

    if(curAnim->playing == 2) /* If animation was paused, simply change the value of playing. */
    {
        curAnim->playing = 1;
    }
    else if(curAnim->playing == 0)
    {
        curAnim->currentFrame = curAnim->sdl_frames[0];
        curAnim->playing = 1;
        curAnim->frameIndex = 0;
        curAnim->tickTime = SDL_GetTicks() + curAnim->frametime;
    }
}

void play_anim(struct Anim* currentAnim)
{
  if(currentAnim->playing == 2) /* If animation was paused, simply change the value of playing. */
  {
      currentAnim->playing = 1;
  }
  else if(currentAnim->playing == 0)
  {
      currentAnim->currentFrame = currentAnim->sdl_frames[0];
      currentAnim->playing = 1;
      currentAnim->frameIndex = 0;
      currentAnim->tickTime = SDL_GetTicks() + currentAnim->frametime;
  }
}

void stop_anims(struct Anim* animlist, uint8_t animID)
{
    struct Anim* currentAnim = &(animlist[animID]);

    currentAnim->playing = 0;
}

void stop_anim      (struct Anim* currentAnim)
{
  currentAnim->playing = 0;
}

void pause_anims    (struct Anim* animlist, uint8_t animID)
{
    struct Anim* currentAnim = &(animlist[animID]);

    pause_anim(currentAnim);
}

void pause_anim     (struct Anim* currentAnim)
{
    currentAnim->playing = 2;
}

void set_frametime(struct Anim* curAnim, uint16_t newtime)
{
    if(newtime == 0)
        curAnim->frametime = curAnim->default_frametime;
    else
        curAnim->frametime = newtime;
}

void scale_anim    (struct Anim* animlist, uint8_t animID, float scale_factor)
{
    struct Anim* curAnim = &(animlist[animID]);
    if(scale_factor != 0.0f)
    {
        int i;
        curAnim->scale = scale_factor;
        curAnim->width = (unsigned)(scale_factor*(float)curAnim->width);
        curAnim->height = (unsigned)(scale_factor*(float)curAnim->height);
        curAnim->scaleDone = 0;

        for(i = 0; i != curAnim->numFrames; ++i)
        {
            curAnim->sdl_frames[i] = scale(curAnim->sdl_frames[i],scale_factor,scale_factor);
        }
        curAnim->currentFrame = curAnim->sdl_frames[curAnim->frameIndex];
    }


}

void flipX_anim     (struct Anim* curAnim)
{
    int i;
    for(i = 0; i != curAnim->numFrames; ++i)
    {
        curAnim->sdl_frames[i] = flip_x(curAnim->sdl_frames[i]);
    }
    curAnim->currentFrame = curAnim->sdl_frames[curAnim->frameIndex];
}

void flipY_anim    (struct Anim* curAnim)
{
    int i;
    for(i = 0; i != curAnim->numFrames; ++i)
    {
        curAnim->sdl_frames[i] = flip_y(curAnim->sdl_frames[i]);
    }
    curAnim->currentFrame = curAnim->sdl_frames[curAnim->frameIndex];
}

void rotate_anims     (struct Anim* animlist, uint8_t animID, float degrees)
{
    struct Anim* curAnim = &(animlist[animID]);
    rotate_anim(curAnim,degrees);
}

void rotate_anim     (struct Anim* curAnim, float degrees)
{
    while(degrees >= 360.0f)
        degrees -= 360.0f;
    while(degrees < 0.0f)
        degrees += 360.0f;

    if(degrees != 0.0f)
    {
        int i;
        for(i = 0; i != curAnim->numFrames; ++i)
        {
            curAnim->sdl_frames[i] = rotate(curAnim->sdl_frames[i],(int)degrees);
        }
    }
    curAnim->currentFrame = curAnim->sdl_frames[curAnim->frameIndex];
}


void tick_anims(struct Anim* animlist, uint8_t animCount)
{
    unsigned currentTime = SDL_GetTicks();
    int i;
    struct Anim* curAnim = animlist;
    for(i = 0; i != animCount; i++, curAnim++)
    {
        if( curAnim->playing != 1 ) /* Skip this animation if it's paused or stopped. */
            continue;

        if( curAnim->tickTime < currentTime )
        {
            unsigned frameIndex = ++curAnim->frameIndex;

            if(frameIndex == curAnim->numFrames)
            {
                if(curAnim->loop == 1)
                {
                    curAnim->frameIndex = frameIndex = 0;
                }
                else
                {
                    stop_anims(animlist,i);
                    continue;
                }
            }

            curAnim->currentFrame = curAnim->sdl_frames[frameIndex];

            curAnim->tickTime = SDL_GetTicks() + curAnim->frametime;
        }
    }
}

int free_images(struct Image* imglist, uint8_t imgCount)
{
    int i;
    for(i = 0; i < imgCount; i++)
    {
        SDL_FreeSurface(imglist[i].img);
    }

    return 0;
}

int free_anims(struct Anim* animlist, uint8_t animCount)
{
    uint8_t i, j;
    struct Anim* curAnim = &(animlist[0]);
    for(i = 0; i < animCount; i++, curAnim++)
    {
        SDL_FreeSurface(curAnim->currentFrame);
        for(j = 0; j != curAnim->numFrames; j++)
        SDL_FreeSurface(curAnim->sdl_frames[i]);
        Free(curAnim->rotate);
    }

    return 0;
}
