#include "image.h"
#include "utils.h"


#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "game.h"
#include "pacman.h"
#include "blinky.h"
#include "grid.h"
#include "audio.h"

void initialize();
void handle_input(struct Grid* grid, struct Pacman* player, uint8_t* done);
void load_images(struct Image* images, struct Anim* anims, uint8_t* imgCount, uint8_t* animCount);

int main ( int argc, char* argv[] )
{

  SDL_Surface* screen = NULL;

  struct Image images [ MAX_IMAGE_COUNT ];
  struct Anim  anims  [ MAX_ANIM_COUNT ];
  uint8_t img_count, anim_count;
  uint8_t done = 0;
  char frightened_ = 0;
  uint8_t up_flash = 0;
  SDL_Rect rect;

  srand(0);

  initialize();

  InitSound();

  screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, 0, SDL_SWSURFACE | SDL_DOUBLEBUF);
  SDL_WM_SetCaption("PACMAN","PACMAN");

  img_count = anim_count = 0;

puts("Seg fault.");
  load_images(images,anims,&img_count,&anim_count);
  puts(" >>>> Initialized!");

  init_game(anims,images);

  while(done == 0)
  {
    SDL_Delay(20);
    up_flash++;

    update_game(anims,images,20);
    done = get_game_over();

    if(done == 1)
      break;

    UpdateWakka();

    if(frightened_ != get_frightened_state())
    {
      frightened_ = get_frightened_state();
      if(frightened_ == 1)
        play_anims(anims,WIBBLY_ANIM);
      else
        stop_anims(anims,WIBBLY_ANIM);
    }

    tick_anims(anims,anim_count);

    render_game(anims,images,screen);


    rect.x = GRID_CELL_SIZE*3; rect.y = 0;
    if(up_flash > 15)
    {
      SDL_BlitSurface(images[13].img, NULL, screen, &rect);
      if(up_flash == 30)
        up_flash = 0;
    }

    rect.x = GRID_CELL_SIZE*9; rect.y = 0;
    SDL_BlitSurface(images[14].img, NULL, screen, &rect);

    SDL_Flip(screen);
    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGBA( screen->format, 0, 0, 0, SDL_ALPHA_OPAQUE ) );

    handle_input(get_grid(),get_pacman(),&done);
  }

  set_high_score(get_score());
  save_high_scores();

  DestroySound();

  return EXIT_SUCCESS;
}

void initialize()
{
    int error = 0;

    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO); /* 0 success SDL_GetError */

    IMG_Init(IMG_INIT_PNG);

    if( Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG )
    {
        printf("SDL_Mixer Error! Error information: %s\n", Mix_GetError());
    }

    error = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); /* -1 error */
    if( error == -1 )
    {
        printf("SDL_Mixer Error! Error information: %s\n", Mix_GetError());
    }
   	PrintSDLError();
}

void handle_input(struct Grid* grid, struct Pacman* player, uint8_t* done)
{
    SDL_Event event;
    while( SDL_PollEvent ( &event ) )
    {
        switch( event.type ) /* Process the appropriate event type */
        {
            case SDL_QUIT:  /* Handle a KEYDOWN event */
                (*done) = 1;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        (*done) = 1;
                        break;
                    case SDLK_r:
                        rotate_anim(player->pacman_anim_,180.0f);
                        break;
                    case SDLK_p:
                        player->pacman_anim_->frametime = 500;
                        break;
                    default:
                      break;
                    }
                break;
        }
    }

    {
      uint8_t* keystate = SDL_GetKeyState(NULL);
      if( keystate[SDLK_UP] )
      {
        pacman_set_next_dir(player, grid, 0, -1);
      }
      else if( keystate[SDLK_DOWN] )
      {
        pacman_set_next_dir(player, grid, 0, 1);
      }
      if( keystate[SDLK_LEFT] )
      {
        pacman_set_next_dir(player, grid, -1, 0);
      }
      else if( keystate[SDLK_RIGHT])
      {
        pacman_set_next_dir(player, grid, 1, 0);
      }
    }
}

void load_images(struct Image* images, struct Anim* anims, uint8_t* imgCount, uint8_t* animCount)
{
    load_image("images/Dot20px.png", images, imgCount); /* 0 */
    load_image("images/SuperDot20px.png", images, imgCount); /* 1 */
    load_image("images/Straight20px.png", images, imgCount); /*  2 */
        copy_image(images, imgCount, 2); /*  3 */
        rotate_image(images,3,90);
    load_image("images/Corner20px.png", images, imgCount); /* 4 */
        copy_image(images, imgCount, 4); /* 5 */
        flipY_image(images, 5);

        copy_image(images, imgCount, 5); /* 6 */
        flipX_image(images, 6);

        copy_image(images, imgCount, 6); /* 7 */
        flipY_image(images, 7);
    load_image("images/StraightBW20px.png", images, imgCount); /*  8 */

    load_image("images/EU20px.png",images, imgCount);
    load_image("images/EL20px.png",images, imgCount);
    load_image("images/ED20px.png",images, imgCount);
    load_image("images/ER20px.png",images, imgCount);

    load_image("images/1UP.png",images, imgCount);
    load_image("images/HighScore.png",images, imgCount);
    load_image("images/Ready.png",images,imgCount);
    load_image("images/0.png",images, imgCount);
    load_image("images/1.png",images, imgCount);
    load_image("images/2.png",images, imgCount);
    load_image("images/3.png",images, imgCount);
    load_image("images/4.png",images, imgCount);
    load_image("images/5.png",images, imgCount);
    load_image("images/6.png",images, imgCount);
    load_image("images/7.png",images, imgCount);
    load_image("images/8.png",images, imgCount);
    load_image("images/9.png",images, imgCount);

    load_image("images/Cherries.png",images, imgCount);
    load_image("images/Strawberry.png",images, imgCount);
    load_image("images/Orange.png",images, imgCount);
    load_image("images/Apple.png",images, imgCount);
    load_image("images/Grapes.png",images, imgCount);
    load_image("images/Gax.png",images, imgCount);
    load_image("images/Bell.png",images, imgCount);
    load_image("images/Key.png",images, imgCount);

    load_image("anims/Pacman/P520px.png",images, imgCount);
    flipY_image(images,LIFE_IMG);

    load_anim("pacman.ani",anims,animCount);
        play_anims(anims,0);

  {
    load_anim("BU.ani",anims,animCount);
      play_anims(anims,BLINKY_ANIM);
    load_anim("BL.ani",anims,animCount);
      play_anims(anims,BLINKY_ANIM+1);
    load_anim("BD.ani",anims,animCount);
      play_anims(anims,BLINKY_ANIM+2);
    load_anim("BR.ani",anims,animCount);
      play_anims(anims,BLINKY_ANIM+3);
  }

  {
    load_anim("PU.ani",anims,animCount);
      play_anims(anims,PINKY_ANIM);
    load_anim("PL.ani",anims,animCount);
      play_anims(anims,PINKY_ANIM+1);
    load_anim("PD.ani",anims,animCount);
      play_anims(anims,PINKY_ANIM+2);
    load_anim("PR.ani",anims,animCount);
      play_anims(anims,PINKY_ANIM+3);
  }


  {
    load_anim("IU.ani",anims,animCount);
      play_anims(anims,INKY_ANIM);
    load_anim("IL.ani",anims,animCount);
      play_anims(anims,INKY_ANIM+1);
    load_anim("ID.ani",anims,animCount);
      play_anims(anims,INKY_ANIM+2);
    load_anim("IR.ani",anims,animCount);
      play_anims(anims,INKY_ANIM+3);
  }


  {
    load_anim("CU.ani",anims,animCount);
          play_anims(anims,CLYDE_ANIM);
    load_anim("CL.ani",anims,animCount);
          play_anims(anims,CLYDE_ANIM+1);
    load_anim("CD.ani",anims,animCount);
          play_anims(anims,CLYDE_ANIM+2);
    load_anim("CR.ani",anims,animCount);
          play_anims(anims,CLYDE_ANIM+3);
  }

  load_anim("wibbly.ani",anims,animCount);
  load_anim("pacman_die.ani",anims,animCount);
}
