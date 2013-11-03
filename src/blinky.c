#include "blinky.h"

#include "grid.h"
#include "image.h"
#include "pacman.h"

void blinky_init(struct Ghost* blinky, struct Anim* anims, struct Image* images)
{
  blinky->behaviour_ = 1;

  blinky->draw_pos_.x = 13 * GRID_CELL_SIZE; blinky->draw_pos_.y = 14 * GRID_CELL_SIZE;
  blinky->draw_pos_f_.x = (float)(13*GRID_CELL_SIZE); blinky->draw_pos_f_.y = (float)(14*GRID_CELL_SIZE);

  blinky->pos_.x = 13; blinky->pos_.y = 14;

  blinky->dir_.x = -1; blinky->dir_.y = 0;

  blinky->waypoint_.x = 12; blinky->waypoint_.y = 13;

  blinky->target_.x = GRID_X-3;
  blinky->target_.y = 0;

  blinky->speed_ = 2.0f;

  blinky->ghost_anim_[0] = &anims[BLINKY_ANIM];
  blinky->ghost_anim_[1] = &anims[BLINKY_ANIM+1];
  blinky->ghost_anim_[2] = &anims[BLINKY_ANIM+2];
  blinky->ghost_anim_[3] = &anims[BLINKY_ANIM+3];
  blinky->eyes[0] = &images[EYES_IMAGE];
  blinky->eyes[1] = &images[EYES_IMAGE+1];
  blinky->eyes[2] = &images[EYES_IMAGE+2];
  blinky->eyes[3] = &images[EYES_IMAGE+3];

  blinky->wibbly_anim_ = &anims[WIBBLY_ANIM];
  blinky->in_house = 0;
  blinky->reverse_next_tile_ = 0;
  blinky->frightened_ = 0;
  blinky->eaten = 0;
  blinky->render_dir_ = 0;
  blinky->exit_house = 1;
}

void blinky_update(struct Ghost* blinky, struct Pacman* player, struct Grid* grid)
{
  blinky->speed_ = 3.0f * get_blinky_speed(blinky,grid);

  if(blinky->pos_.x == player->pos_.x && blinky->pos_.y == player->pos_.y)
  {
    if(blinky->frightened_ == 1)
    {
      blinky->eaten = 1;
      blinky->frightened_ = 0;
      notify_ghost_eaten();
    }
    else if(blinky->eaten == 0)
    {
      start_kill_pacman();
    }
  }

  if(blinky->eaten)
  {
    if(blinky->pos_.x == 14 && blinky->pos_.y == 17)
    {
      blinky->eaten = 0;
      blinky->in_house = 1;
      blinky->speed_ = 3.0f*get_ghost_speed(blinky,grid);
    }
    else
    {
      blinky->target_.x = 14; blinky->target_.y = 17; /* Pinky's home square (yes, Pinky, not Blinky). */
      blinky->speed_ = 5.0f;
    }
  }
  else
  {
    blinky_set_target(blinky,player);
  }

  blinky->draw_pos_f_.x += blinky->speed_ * blinky->dir_.x;
  blinky->draw_pos_f_.y += blinky->speed_ * blinky->dir_.y;

  blinky->draw_pos_.x = (int16_t)(blinky->draw_pos_f_.x);
  blinky->draw_pos_.y = (int16_t)(blinky->draw_pos_f_.y);

  /* Only proceed if Blinky has hit the next square. */
  if((blinky->dir_.x != 1) || ( blinky->draw_pos_.x < (blinky->pos_.x + blinky->dir_.x) * GRID_CELL_SIZE ))
    if((blinky->dir_.x != -1) || ( blinky->draw_pos_.x > (blinky->pos_.x + blinky->dir_.x) * GRID_CELL_SIZE ))
      if((blinky->dir_.y != 1) || ( blinky->draw_pos_.y < (blinky->pos_.y + blinky->dir_.y) * GRID_CELL_SIZE ))
        if((blinky->dir_.y != -1) || ( blinky->draw_pos_.y > (blinky->pos_.y + blinky->dir_.y) * GRID_CELL_SIZE ))
          return;

  blinky->pos_.x += blinky->dir_.x;
  blinky->pos_.y += blinky->dir_.y;

  blinky->draw_pos_f_.x = (float)(blinky->pos_.x*GRID_CELL_SIZE);
  blinky->draw_pos_f_.y = (float)(blinky->pos_.y*GRID_CELL_SIZE);

  if(blinky->pos_.x == GRID_X && blinky->pos_.y == 17)
  {
    /* Wrap around */
    blinky->pos_.x = 0;
    blinky->draw_pos_.x = 1-GRID_CELL_SIZE;
    blinky->draw_pos_f_.x = (float)(1-GRID_CELL_SIZE);
    blinky->waypoint_.x = 2;
    blinky->waypoint_.y = 17;
  }
  else if(blinky->pos_.x == -1 && blinky->pos_.y == 17)
  {
    /* Wrap around */
    blinky->pos_.x = GRID_X-1;
    blinky->draw_pos_.x = (GRID_CELL_SIZE * GRID_X) - 1;
    blinky->draw_pos_f_.x = (float)((GRID_CELL_SIZE * GRID_X) -1);
    blinky->waypoint_.x = GRID_X-3;
    blinky->waypoint_.y = 17;
  }
  else if(!(blinky->pos_.x == GRID_X-1 && blinky->pos_.y == 17) && !(blinky->pos_.x == 0 && blinky->pos_.y == 17))
  {
    /* Just reached a new square. */
    if((blinky->reverse_next_tile_ == 1) && (blinky->in_house == 0))
    {
      blinky->dir_.x = -blinky->dir_.x;
      blinky->dir_.y = -blinky->dir_.y;
      blinky->waypoint_.x = blinky->pos_.x + blinky->dir_.x;
      blinky->waypoint_.y = blinky->pos_.y + blinky->dir_.y;
      blinky->reverse_next_tile_ = 0;
    }
    else
      ghost_choose_direction(blinky);

    ghost_choose_waypoint(blinky,grid);
  }
}

void blinky_set_target(struct Ghost* blinky, struct Pacman* player)
{
  switch(blinky->behaviour_)
  {
    case 1:
      blinky->target_.x = GRID_X-3; blinky->target_.y = 0;
      break;
    case 2:
      blinky->target_.x = player->pos_.x; blinky->target_.y = player->pos_.y;
      break;
  }
}

