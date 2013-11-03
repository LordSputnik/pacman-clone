#include "inky.h"

#include "grid.h"
#include "image.h"
#include "pacman.h"

void inky_init(struct Ghost* inky, struct Anim* anims, struct Image* images)
{
  inky->behaviour_ = 1;

  inky->draw_pos_.x = 12* GRID_CELL_SIZE; inky->draw_pos_.y = 17 * GRID_CELL_SIZE;
  inky->draw_pos_f_.x = (float)(12*GRID_CELL_SIZE); inky->draw_pos_f_.y = (float)(17*GRID_CELL_SIZE);

  inky->dir_.x = 0; inky->dir_.y = -1;
  inky->pos_.x = 12; inky->pos_.y = 17;
  inky->target_.x = GRID_X-1; inky->target_.y = GRID_Y-2;

  inky->speed_ = 2.0f;

  inky->ghost_anim_[0] = &anims[INKY_ANIM];
  inky->ghost_anim_[1] = &anims[INKY_ANIM+1];
  inky->ghost_anim_[2] = &anims[INKY_ANIM+2];
  inky->ghost_anim_[3] = &anims[INKY_ANIM+3];
  inky->eyes[0] = &images[EYES_IMAGE];
  inky->eyes[1] = &images[EYES_IMAGE+1];
  inky->eyes[2] = &images[EYES_IMAGE+2];
  inky->eyes[3] = &images[EYES_IMAGE+3];

  inky->wibbly_anim_ = &anims[WIBBLY_ANIM];
  inky->reverse_next_tile_ = 0;
  inky->frightened_ = 0;
  inky->eaten = 0;
  inky->in_house = 1;
  inky->dot_counter = 0;
}

void inky_update(struct Ghost* inky, struct Ghost* blinky, struct Pacman* player, struct Grid* grid)
{
  inky->speed_ = 3.0f * get_ghost_speed(inky,grid);

  if(inky->pos_.x == player->pos_.x && inky->pos_.y == player->pos_.y)
  {
    if(inky->frightened_ == 1)
    {
      inky->eaten = 1;
      inky->frightened_ = 0;
      notify_ghost_eaten();
    }
    else if(inky->eaten == 0)
    {
      start_kill_pacman();
    }
  }

  if(inky->eaten)
  {
    if(inky->pos_.x == 15 && inky->pos_.y == 17)
    {
      inky->eaten = 0;
      inky->in_house = 1;
      inky->speed_ = 3.0f*get_ghost_speed(inky,grid);
    }
    else
    {
      inky->target_.x = 15; inky->target_.y = 17; /* Pinky's home square (yes, Pinky, not Blinky). */
      inky->speed_ = 5.0f;
    }
  }
  else
  {
    inky_set_target(inky,blinky,player);
  }

  inky->draw_pos_f_.x += inky->speed_ * inky->dir_.x;
  inky->draw_pos_f_.y += inky->speed_ * inky->dir_.y;

  inky->draw_pos_.x = (int16_t)(inky->draw_pos_f_.x);
  inky->draw_pos_.y = (int16_t)(inky->draw_pos_f_.y);

  /* Only proceed if Clyde has hit the next square. */
  if((inky->dir_.x != 1) || ( inky->draw_pos_.x < (inky->pos_.x + inky->dir_.x) * GRID_CELL_SIZE ))
    if((inky->dir_.x != -1) || ( inky->draw_pos_.x > (inky->pos_.x + inky->dir_.x) * GRID_CELL_SIZE ))
      if((inky->dir_.y != 1) || ( inky->draw_pos_.y < (inky->pos_.y + inky->dir_.y) * GRID_CELL_SIZE ))
        if((inky->dir_.y != -1) || ( inky->draw_pos_.y > (inky->pos_.y + inky->dir_.y) * GRID_CELL_SIZE ))
          return;

  inky->pos_.x += inky->dir_.x;
  inky->pos_.y += inky->dir_.y;
  inky->draw_pos_f_.x = (float)(inky->pos_.x*GRID_CELL_SIZE);
  inky->draw_pos_f_.y = (float)(inky->pos_.y*GRID_CELL_SIZE);

  if(inky->pos_.x == GRID_X && inky->pos_.y == 17)
  {
    /* Wrap around */
    inky->pos_.x = 0;
    inky->draw_pos_.x = 1-GRID_CELL_SIZE;
    inky->draw_pos_f_.x = (float)(1-GRID_CELL_SIZE);
    inky->waypoint_.x = 2;
    inky->waypoint_.y = 17;
  }
  else if(inky->pos_.x == -1 && inky->pos_.y == 17)
  {
    /* Wrap around */
    inky->pos_.x = GRID_X-1;
    inky->draw_pos_.x = (GRID_CELL_SIZE * GRID_X) - 1;
    inky->draw_pos_f_.x = (float)((GRID_CELL_SIZE * GRID_X) -1);
    inky->waypoint_.x = GRID_X-3;
    inky->waypoint_.y = 17;
  }
  else if(!(inky->pos_.x == GRID_X-1 && inky->pos_.y == 17) && !(inky->pos_.x == 0 && inky->pos_.y == 17))
  {
    /* Just reached a new square. */
    if((inky->reverse_next_tile_ == 1) && (inky->in_house == 0))
    {
      inky->dir_.x = -inky->dir_.x;
      inky->dir_.y = -inky->dir_.y;
      inky->waypoint_.x = inky->pos_.x + inky->dir_.x;
      inky->waypoint_.y = inky->pos_.y + inky->dir_.y;
      inky->reverse_next_tile_ = 0;
    }
    else
      ghost_choose_direction(inky);

    ghost_choose_waypoint(inky,grid);
  }
}

void inky_set_target(struct Ghost* inky, struct Ghost* blinky, struct Pacman* player)
{
  switch(inky->behaviour_)
  {
    case 1:
      inky->target_.x = 2; inky->target_.y = 0;
      break;
    case 2:
    {
      struct vec3i fulcrum, diff;
      fulcrum.x = player->pos_.x + (player->dir_.x * 2);
      fulcrum.y = player->pos_.y + (player->dir_.y * 2);

      diff.x = fulcrum.x - blinky->pos_.x;
      diff.y = fulcrum.y - blinky->pos_.y;

      inky->target_.x = diff.x + fulcrum.x;
      inky->target_.y = diff.y + fulcrum.y;
      break;
    }
  }
}
