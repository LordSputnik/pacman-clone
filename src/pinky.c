#include "pinky.h"

#include "grid.h"
#include "image.h"
#include "pacman.h"

void pinky_set_target(struct Ghost* pinky, struct Pacman* player);

void pinky_init(struct Ghost* pinky, struct Anim* anims, struct Image* images)
{
  pinky->behaviour_ = 1;

  pinky->draw_pos_.x = 14 * GRID_CELL_SIZE; pinky->draw_pos_.y = 17 * GRID_CELL_SIZE;
  pinky->draw_pos_f_.x = (float)(14*GRID_CELL_SIZE); pinky->draw_pos_f_.y = (float)(17*GRID_CELL_SIZE);

  pinky->dir_.x = 0; pinky->dir_.y = 1;
  pinky->pos_.x = 14; pinky->pos_.y = 17;
  pinky->target_.x = 2; pinky->target_.y = 0;

  pinky->speed_ = 2.0f;

  pinky->ghost_anim_[0] = &anims[PINKY_ANIM];
  pinky->ghost_anim_[1] = &anims[PINKY_ANIM+1];
  pinky->ghost_anim_[2] = &anims[PINKY_ANIM+2];
  pinky->ghost_anim_[3] = &anims[PINKY_ANIM+3];
  pinky->eyes[0] = &images[EYES_IMAGE];
  pinky->eyes[1] = &images[EYES_IMAGE+1];
  pinky->eyes[2] = &images[EYES_IMAGE+2];
  pinky->eyes[3] = &images[EYES_IMAGE+3];

  pinky->wibbly_anim_ = &anims[WIBBLY_ANIM];
  pinky->reverse_next_tile_ = 0;
  pinky->frightened_ = 0;
  pinky->eaten = 0;
  pinky->render_dir_ = 1;
  pinky->in_house = 1;
  pinky->dot_counter = 0;
}

void pinky_update(struct Ghost* pinky, struct Pacman* player, struct Grid* grid)
{
  pinky->speed_ = 3.0f * get_ghost_speed(pinky,grid);

  if(pinky->pos_.x == player->pos_.x && pinky->pos_.y == player->pos_.y)
  {
    if(pinky->frightened_ == 1)
    {
      pinky->eaten = 1;
      pinky->frightened_ = 0;
      notify_ghost_eaten();
    }
    else if(pinky->eaten == 0)
    {
      start_kill_pacman();
    }
  }

  if(pinky->eaten)
  {
    if(pinky->pos_.x == 15 && pinky->pos_.y == 17)
    {
      pinky->eaten = 0;
      pinky->in_house = 1;
      pinky->speed_ = 3.0f*get_ghost_speed(pinky,grid);
    }
    else
    {
      pinky->target_.x = 15; pinky->target_.y = 17; /* Pinky's home square (yes, Pinky, not Blinky). */
      pinky->speed_ = 5.0f;
    }
  }
  else
  {
    pinky_set_target(pinky,player);
  }

  pinky->draw_pos_f_.x += pinky->speed_ * pinky->dir_.x;
  pinky->draw_pos_f_.y += pinky->speed_ * pinky->dir_.y;

  pinky->draw_pos_.x = (int16_t)(pinky->draw_pos_f_.x);
  pinky->draw_pos_.y = (int16_t)(pinky->draw_pos_f_.y);

  /* Only proceed if Clyde has hit the next square. */
  if((pinky->dir_.x != 1) || ( pinky->draw_pos_.x < (pinky->pos_.x + pinky->dir_.x) * GRID_CELL_SIZE ))
    if((pinky->dir_.x != -1) || ( pinky->draw_pos_.x > (pinky->pos_.x + pinky->dir_.x) * GRID_CELL_SIZE ))
      if((pinky->dir_.y != 1) || ( pinky->draw_pos_.y < (pinky->pos_.y + pinky->dir_.y) * GRID_CELL_SIZE ))
        if((pinky->dir_.y != -1) || ( pinky->draw_pos_.y > (pinky->pos_.y + pinky->dir_.y) * GRID_CELL_SIZE ))
          return;

  pinky->pos_.x += pinky->dir_.x;
  pinky->pos_.y += pinky->dir_.y;
  pinky->draw_pos_f_.x = (float)(pinky->pos_.x*GRID_CELL_SIZE);
  pinky->draw_pos_f_.y = (float)(pinky->pos_.y*GRID_CELL_SIZE);

  if(pinky->pos_.x == GRID_X && pinky->pos_.y == 17)
  {
    /* Wrap around */
    pinky->pos_.x = 0;
    pinky->draw_pos_.x = 1-GRID_CELL_SIZE;
    pinky->draw_pos_f_.x = (float)(1-GRID_CELL_SIZE);
    pinky->waypoint_.x = 2;
    pinky->waypoint_.y = 17;
  }
  else if(pinky->pos_.x == -1 && pinky->pos_.y == 17)
  {
    /* Wrap around */
    pinky->pos_.x = GRID_X-1;
    pinky->draw_pos_.x = (GRID_CELL_SIZE * GRID_X) - 1;
    pinky->draw_pos_f_.x = (float)((GRID_CELL_SIZE * GRID_X) -1);
    pinky->waypoint_.x = GRID_X-3;
    pinky->waypoint_.y = 17;
  }
  else if(!(pinky->pos_.x == GRID_X-1 && pinky->pos_.y == 17) && !(pinky->pos_.x == 0 && pinky->pos_.y == 17))
  {
    /* Just reached a new square. */
    if((pinky->reverse_next_tile_ == 1) && (pinky->in_house == 0))
    {
      pinky->dir_.x = -pinky->dir_.x;
      pinky->dir_.y = -pinky->dir_.y;
      pinky->waypoint_.x = pinky->pos_.x + pinky->dir_.x;
      pinky->waypoint_.y = pinky->pos_.y + pinky->dir_.y;
      pinky->reverse_next_tile_ = 0;
    }
    else
      ghost_choose_direction(pinky);

    ghost_choose_waypoint(pinky,grid);
  }
}

void pinky_set_target(struct Ghost* pinky, struct Pacman* player)
{
  switch(pinky->behaviour_)
  {
    case 1:
      pinky->target_.x = 2; pinky->target_.y = 0;
      break;
    case 2:
    {
      pinky->target_.x = player->pos_.x + (player->dir_.x*4);
      pinky->target_.y = player->pos_.y + (player->dir_.y*4);
      break;
    }
  }
}
