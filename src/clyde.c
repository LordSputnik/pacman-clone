#include "clyde.h"

#include "grid.h"
#include "image.h"
#include "pacman.h"

void clyde_init(struct Ghost* clyde, struct Anim* anims, struct Image* images)
{
  clyde->behaviour_ = 1;

  clyde->draw_pos_.x = 15 * GRID_CELL_SIZE; clyde->draw_pos_.y = 17 * GRID_CELL_SIZE;
  clyde->draw_pos_f_.x = (float)(15*GRID_CELL_SIZE); clyde->draw_pos_f_.y = (float)(17*GRID_CELL_SIZE);

  clyde->dir_.x = 0; clyde->dir_.y = -1;
  clyde->pos_.x = 15; clyde->pos_.y = 17;
  clyde->target_.x = 0; clyde->target_.y = GRID_Y-2;

  clyde->speed_ = 2.0f;

  clyde->ghost_anim_[0] = &anims[CLYDE_ANIM];
  clyde->ghost_anim_[1] = &anims[CLYDE_ANIM+1];
  clyde->ghost_anim_[2] = &anims[CLYDE_ANIM+2];
  clyde->ghost_anim_[3] = &anims[CLYDE_ANIM+3];
  clyde->eyes[0] = &images[EYES_IMAGE];
  clyde->eyes[1] = &images[EYES_IMAGE+1];
  clyde->eyes[2] = &images[EYES_IMAGE+2];
  clyde->eyes[3] = &images[EYES_IMAGE+3];

  clyde->wibbly_anim_ = &anims[WIBBLY_ANIM];
  clyde->reverse_next_tile_ = 0;
  clyde->frightened_ = 0;
  clyde->eaten = 0;
  clyde->render_dir_ = 1;
  clyde->in_house = 1;
  clyde->dot_counter = 0;
}

void clyde_update(struct Ghost* clyde, struct Pacman* player, struct Grid* grid)
{
  clyde->speed_ = 3.0f * get_ghost_speed(clyde,grid);

  if(clyde->pos_.x == player->pos_.x && clyde->pos_.y == player->pos_.y)
  {
    if(clyde->frightened_ == 1)
    {
      clyde->eaten = 1;
      clyde->frightened_ = 0;
      notify_ghost_eaten();
    }
    else if(clyde->eaten == 0)
    {
      start_kill_pacman();
    }
  }

  if(clyde->eaten)
  {
    if(clyde->pos_.x == 15 && clyde->pos_.y == 17)
    {
      clyde->eaten = 0;
      clyde->in_house = 1;
      clyde->speed_ = 3.0f*get_ghost_speed(clyde,grid);
    }
    else
    {
      clyde->target_.x = 15; clyde->target_.y = 17; /* Pinky's home square (yes, Pinky, not Blinky). */
      clyde->speed_ = 5.0f;
    }
  }
  else
  {
    clyde_set_target(clyde,player);
  }

  clyde->draw_pos_f_.x += clyde->speed_ * clyde->dir_.x;
  clyde->draw_pos_f_.y += clyde->speed_ * clyde->dir_.y;

  clyde->draw_pos_.x = (int16_t)(clyde->draw_pos_f_.x);
  clyde->draw_pos_.y = (int16_t)(clyde->draw_pos_f_.y);

  /* Only proceed if Clyde has hit the next square. */
  if((clyde->dir_.x != 1) || ( clyde->draw_pos_.x < (clyde->pos_.x + clyde->dir_.x) * GRID_CELL_SIZE ))
    if((clyde->dir_.x != -1) || ( clyde->draw_pos_.x > (clyde->pos_.x + clyde->dir_.x) * GRID_CELL_SIZE ))
      if((clyde->dir_.y != 1) || ( clyde->draw_pos_.y < (clyde->pos_.y + clyde->dir_.y) * GRID_CELL_SIZE ))
        if((clyde->dir_.y != -1) || ( clyde->draw_pos_.y > (clyde->pos_.y + clyde->dir_.y) * GRID_CELL_SIZE ))
          return;

  clyde->pos_.x += clyde->dir_.x;
  clyde->pos_.y += clyde->dir_.y;
  clyde->draw_pos_f_.x = (float)(clyde->pos_.x*GRID_CELL_SIZE);
  clyde->draw_pos_f_.y = (float)(clyde->pos_.y*GRID_CELL_SIZE);

  if(clyde->pos_.x == GRID_X && clyde->pos_.y == 17)
  {
    /* Wrap around */
    clyde->pos_.x = 0;
    clyde->draw_pos_.x = 1-GRID_CELL_SIZE;
    clyde->draw_pos_f_.x = (float)(1-GRID_CELL_SIZE);
    clyde->waypoint_.x = 2;
    clyde->waypoint_.y = 17;
  }
  else if(clyde->pos_.x == -1 && clyde->pos_.y == 17)
  {
    /* Wrap around */
    clyde->pos_.x = GRID_X-1;
    clyde->draw_pos_.x = (GRID_CELL_SIZE * GRID_X) - 1;
    clyde->draw_pos_f_.x = (float)((GRID_CELL_SIZE * GRID_X) -1);
    clyde->waypoint_.x = GRID_X-3;
    clyde->waypoint_.y = 17;
  }
  else if(!(clyde->pos_.x == GRID_X-1 && clyde->pos_.y == 17) && !(clyde->pos_.x == 0 && clyde->pos_.y == 17))
  {
    /* Just reached a new square. */
    if((clyde->reverse_next_tile_ == 1) && (clyde->in_house == 0))
    {
      clyde->dir_.x = -clyde->dir_.x;
      clyde->dir_.y = -clyde->dir_.y;
      clyde->waypoint_.x = clyde->pos_.x + clyde->dir_.x;
      clyde->waypoint_.y = clyde->pos_.y + clyde->dir_.y;
      clyde->reverse_next_tile_ = 0;
    }
    else
      ghost_choose_direction(clyde);

    ghost_choose_waypoint(clyde,grid);
  }
}

void clyde_set_target(struct Ghost* clyde, struct Pacman* player)
{
  switch(clyde->behaviour_)
  {
    case 1:
      clyde->target_.x = 0; clyde->target_.y = GRID_Y-2;
      break;
    case 2:
    {
      int32_t x, y, dist;
      x = player->pos_.x - clyde->pos_.x;
      y = player->pos_.y - clyde->pos_.y;
      dist = x*x + y*y;

      if(dist < 49) /* Less than 7 tile distance between clyde and pacman. */
      {
        clyde->target_.x = 0; clyde->target_.y = GRID_Y-2;
      }
      else
      {
        clyde->target_.x = player->pos_.x; clyde->target_.y = player->pos_.y;
      }
      break;
    }
  }
}
