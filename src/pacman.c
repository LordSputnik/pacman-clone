#include "pacman.h"

#include "grid.h"
#include "image.h"
#include "game.h"
#include "audio.h"

void pacman_init(struct Pacman* player, struct Anim* anims)
{
    player->pacman_anim_ = &anims[0];
    player->death_anim_ = &anims[DEATH_ANIM];

    player->draw_pos_.x = 14*GRID_CELL_SIZE; player->draw_pos_.y = 26*GRID_CELL_SIZE;
    player->draw_pos_f_.x = (float)(14*GRID_CELL_SIZE); player->draw_pos_f_.y = (float)(26*GRID_CELL_SIZE);

    player->pos_.x = 14; player->pos_.y = 26;
    player->dir_.x = -1; player->dir_.y = 0;
    player->last_dir_ = player->dir_;
    player->next_dir_.x = -1; player->next_dir_.y = 0;

    flipY_anim(player->pacman_anim_);
    player->dying = 0;
}

void pacman_reset(struct Pacman* player)
{
    player->draw_pos_.x = 14*GRID_CELL_SIZE; player->draw_pos_.y = 26*GRID_CELL_SIZE;
    player->draw_pos_f_.x = (float)(14*GRID_CELL_SIZE); player->draw_pos_f_.y = (float)(26*GRID_CELL_SIZE);

    player->pos_.x = 14; player->pos_.y = 26;
    player->dir_.x = -1; player->dir_.y = 0;

    if(player->last_dir_.x == 1)
    {
      rotate_anim(player->pacman_anim_,180.0f);
      flipX_anim(player->pacman_anim_);
    }
    else if(player->last_dir_.y == 1)
    {
      rotate_anim(player->pacman_anim_,90.0f);
      flipX_anim(player->pacman_anim_);
    }
    else if(player->last_dir_.y == -1)
    {
      rotate_anim(player->pacman_anim_,-90.0f);
      flipX_anim(player->pacman_anim_);
    }

    player->last_dir_ = player->dir_;
    player->next_dir_.x = -1; player->next_dir_.y = 0;
    player->dying = 0;
}

void pacman_update(struct Pacman* player, struct Grid* grid)
{
  uint8_t has_eaten = 0;

  if(player->dying == 1)
  {
    SetWakka(0);
    return;
  }

  player->speed_ = 3.0f * get_pacman_speed();
  player->draw_pos_f_.x += player->speed_ * player->dir_.x;
  player->draw_pos_f_.y += player->speed_ * player->dir_.y;

  player->draw_pos_.x = (int)(player->draw_pos_f_.x);
  player->draw_pos_.y = (int)(player->draw_pos_f_.y);

  /* Only proceed if Pacman has hit the next square. */
  if((player->dir_.x != 1) || ( player->draw_pos_.x < (player->pos_.x + player->dir_.x) * GRID_CELL_SIZE ))
    if((player->dir_.x != -1) || ( player->draw_pos_.x > (player->pos_.x + player->dir_.x) * GRID_CELL_SIZE ))
      if((player->dir_.y != 1) || ( player->draw_pos_.y < (player->pos_.y + player->dir_.y) * GRID_CELL_SIZE ))
        if((player->dir_.y != -1) || ( player->draw_pos_.y > (player->pos_.y + player->dir_.y) * GRID_CELL_SIZE ))
          if(player->dir_.x != 0 || player->dir_.y != 0)
            return;

  player->pos_.x += player->dir_.x;
  player->pos_.y += player->dir_.y;

  if(is_bonus_spawned() == 1)
  {
    if((player->pos_.x == 13 && player->pos_.y == 20) || (player->pos_.x == 14 && player->pos_.y == 20))
    {
      eat_bonus();
      has_eaten = 1;
    }
  }

  if(player->pos_.x == GRID_X && player->pos_.y == 17)
  {
    /* Wrap around */
    player->pos_.x = 0;
    player->draw_pos_.x = 1-GRID_CELL_SIZE;
    player->draw_pos_f_.x = (float)(1-GRID_CELL_SIZE);
  }
  else if(player->pos_.x == -1 &&player->pos_.y == 17)
  {
    /* Wrap around */
    player->pos_.x = GRID_X-1;
    player->draw_pos_.x = (GRID_CELL_SIZE * GRID_X) - 1;
    player->draw_pos_f_.x = (float)((GRID_CELL_SIZE * GRID_X) - 1);
  }
  else
  {
    if(grid->blocks[player->pos_.x][player->pos_.y] == DOT)
    {
      grid->blocks[player->pos_.x][player->pos_.y] = EMPTIED;
      eat_dot();
      has_eaten = 1;
    }
    else if(grid->blocks[player->pos_.x][player->pos_.y] == SUPER_DOT)
    {
      grid->blocks[player->pos_.x][player->pos_.y] = SUPER_EMPTIED;
      eat_super_dot();
      has_eaten = 1;
    }

    if(has_eaten == 1)
    {
      SetWakka(1);
    }
    else
    {
      SetWakka(0);
    }

    pacman_change_direction(player,grid);

  }
}

void pacman_render(struct Pacman* player, SDL_Surface* screen)
{
  if(player->dying == 1)
  {
    SDL_BlitSurface ( player->death_anim_->currentFrame, NULL, screen, &(player->draw_pos_) );
  }
  else
  {
    SDL_BlitSurface ( player->pacman_anim_->currentFrame, NULL, screen, &(player->draw_pos_) );
  }


  PrintSDLError();
}

void pacman_set_next_dir(struct Pacman* player, struct Grid* grid, int x, int y)
{
  player->next_dir_.x = x;
  player->next_dir_.y = y;

  if(player->next_dir_.x == -player->dir_.x)
    pacman_change_direction(player,grid);
  else if(player->next_dir_.y == -player->dir_.y)
    pacman_change_direction(player,grid);
}

void pacman_change_direction(struct Pacman* player, struct Grid* grid)
{
struct vec3i target;
    /* Check whether we can turn in the next_dir_. If not, try to keep going. If not, stop. */
    target = player->pos_;
    target.x += player->next_dir_.x;
    target.y += player->next_dir_.y;

    if((check_bounds(target) == 1) && (check_collisions(grid, target.x, target.y) == 0))
    {
      if(player->next_dir_.x == 1)
      {
        if(player->last_dir_.x == -1)
        {
          flipX_anim(player->pacman_anim_);
          rotate_anim(player->pacman_anim_,180.0f);
        }
        else if(player->last_dir_.y == 1)
        {
          rotate_anim(player->pacman_anim_,-90.0f);
        }
        else if(player->last_dir_.y == -1)
        {
          rotate_anim(player->pacman_anim_,90.0f);
        }
      }
      else if(player->next_dir_.x == -1)
      {
        if(player->last_dir_.x == 1)
        {
          rotate_anim(player->pacman_anim_,180.0f);
          flipX_anim(player->pacman_anim_);
        }
        else if(player->last_dir_.y == 1)
        {
          rotate_anim(player->pacman_anim_,90.0f);
          flipX_anim(player->pacman_anim_);
        }
        else if(player->last_dir_.y == -1)
        {
          rotate_anim(player->pacman_anim_,-90.0f);
          flipX_anim(player->pacman_anim_);
        }
      }
      else if(player->next_dir_.y == 1)
      {
        if(player->last_dir_.x == -1)
        {
          flipX_anim(player->pacman_anim_);
          rotate_anim(player->pacman_anim_,-90.0f);
        }
        else if(player->last_dir_.x == 1)
          rotate_anim(player->pacman_anim_,90.0f);
        else if(player->last_dir_.y == -1)
          rotate_anim(player->pacman_anim_,180.0f);
      }
      else if(player->next_dir_.y == -1)
      {
        if(player->last_dir_.x == -1)
        {
          flipX_anim(player->pacman_anim_);
          rotate_anim(player->pacman_anim_,90.0f);
        }
        else if(player->last_dir_.x == 1)
          rotate_anim(player->pacman_anim_,-90.0f);
        else if(player->last_dir_.y == 1)
          rotate_anim(player->pacman_anim_,180.0f);
      }
      player->last_dir_ = player->dir_ = player->next_dir_;
    }
    else
    {
      target = player->pos_;
      target.x += player->dir_.x;
      target.y += player->dir_.y;
      if(!(player->pos_.x == GRID_X-1 && player->pos_.y == 17) && !(player->pos_.x == 0 && player->pos_.y == 17))
      {
        if((check_bounds(target) == 0) || (check_collisions(grid, target.x, target.y) == 1))
        {
          player->last_dir_ = player->dir_;
          player->dir_.x = player->dir_.y = 0; /* Stop. */
        }
      }
    }
}
