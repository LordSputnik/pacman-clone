#include "ghost.h"

#include "grid.h"
#include "image.h"
#include "game.h"

uint32_t square_distance_to_target(struct Ghost * ghost, struct vec3i test_point, struct vec3i target);
void get_dist_to_target(uint32_t* dist, struct Ghost* blinky, struct Grid* grid);

void ghost_move_out_of_home(struct Ghost* ghost)
{
  if((ghost->pos_.x == 13) || (ghost->pos_.x == 14))
  {
    if(ghost->pos_.y == 14) /* Blinky out of home, change behaviour. */
    {
      ghost->dir_.x = -1; ghost->dir_.y = 0;
      ghost->waypoint_.x = ghost->pos_.x - 1;
      ghost->waypoint_.y = 14;

      ghost->in_house = 0;
      ghost->behaviour_ = get_ghost_behaviour();
    }
    else
    {
      ghost->render_dir_ = 0;
      ghost->dir_.x = 0;  ghost->dir_.y = -1;
    }
  }
  else
  {
    if((ghost->pos_.x == 11) || (ghost->pos_.x == 12))
    {
      ghost->render_dir_ = 3;
      ghost->dir_.x = 1;
    }
    else if((ghost->pos_.x == 15) || (ghost->pos_.x == 16))
    {
      ghost->render_dir_ = 1;
      ghost->dir_.x = -1;
    }
    ghost->dir_.y = 0;
  }
}

void ghost_choose_direction(struct Ghost* ghost)
{
  switch(ghost->in_house)
  {
    case 1:
      if(ghost->exit_house == 1)
      {
        ghost_move_out_of_home(ghost);
      }
      else
      {
        if((ghost->pos_.y == 16) || (ghost->pos_.y == 18))
        {
          ghost->dir_.y = -ghost->dir_.y;
          if(ghost->dir_.y == 1)
            ghost->render_dir_ = 2;
          else
            ghost->render_dir_ = 0;
        }
        else
          printf("Ghost Pos Y: %i",ghost->pos_.y);
      }
      break;
    default:
      ghost->dir_.x = ghost->waypoint_.x - ghost->pos_.x;
      ghost->dir_.y = ghost->waypoint_.y - ghost->pos_.y;
      break;
  }
}

void ghost_choose_waypoint(struct Ghost* ghost, struct Grid* grid)
{
  uint8_t min = 0;
  uint32_t dist[4];

  get_dist_to_target(dist,ghost,grid);

  if(dist[1] < dist[min])
    min = 1;

  if(dist[2] < dist[min])
    min = 2;

  if(dist[3] < dist[min])
    min = 3;

  if(ghost->in_house == 0)
    ghost->render_dir_ = min;

  switch(min)
  {
    case 0: /* Go dist[0] */
      /* printf("Blinky Waypoint Up"); */
      ghost->waypoint_.y -= 1;
      break;
    case 1: /* Go dist[1] */
      /* printf("Blinky Waypoint Left"); */
      ghost->waypoint_.x -= 1;
      break;
    case 2: /* Go dist[2] */
      /* printf("Blinky Waypoint Down"); */
      ghost->waypoint_.y += 1;
      break;
    case 3: /* Go dist[3] */
      /* printf("Blinky Waypoint Right"); */
      ghost->waypoint_.x += 1;
      break;
  }
  /* printf(" alternative distances: U: %u L: %u D: %u R: %u\n",dist[0],dist[1],dist[2],dist[3]); */
}

void ghost_render(struct Ghost* ghost, SDL_Surface* screen)
{
  if(ghost->frightened_ == 1 && ghost->eaten == 0)
  {
    SDL_BlitSurface ( ghost->wibbly_anim_->currentFrame, NULL, screen, &ghost->draw_pos_ );
  }
  else if(ghost->eaten)
  {
    SDL_BlitSurface ( ghost->eyes[ghost->render_dir_]->img, NULL, screen, &ghost->draw_pos_ );
  }
  else
  {
    SDL_BlitSurface ( ghost->ghost_anim_[ghost->render_dir_]->currentFrame, NULL, screen, &ghost->draw_pos_ );
  }

  PrintSDLError();
}

void ghost_set_frightened(struct Ghost* ghost, char active)
{
  ghost->frightened_ = active;

  if(active == 1 && ghost->eaten == 0) /* Reverse direction. */
    ghost->reverse_next_tile_ = 1;
}

void ghost_set_behaviour(struct Ghost* ghost, char behaviour)
{
  ghost->behaviour_ = behaviour;

  if(ghost->eaten == 0 && ghost->in_house == 0) /* Reverse direction. */
    ghost->reverse_next_tile_ = 1;
}

void get_dist_to_target(uint32_t* dist, struct Ghost* ghost, struct Grid* grid)
{
  struct vec3i test_point;
  test_point.x = ghost->waypoint_.x;
  test_point.y = ghost->waypoint_.y - 1;

  if(( check_bounds(test_point) == 0 ) || (check_collisions_ghost(grid,test_point.x,test_point.y) == true) || (ghost->dir_.y == 1))
    dist[0] = UINT_MAX;
  else if((ghost->in_house == 0) && (ghost->eaten == 0) && (grid->blocks[test_point.x][test_point.y] == GATE))
    dist[0] = UINT_MAX;
  else
    dist[0] = square_distance_to_target(ghost,test_point,ghost->target_);

  test_point.x = ghost->waypoint_.x - 1;
  test_point.y = ghost->waypoint_.y;

  if(( check_bounds(test_point) == 0 ) || (check_collisions_ghost(grid, test_point.x, test_point.y) == true) || (ghost->dir_.x == 1))
    dist[1] = UINT_MAX;
  else if((ghost->in_house == 0) && (ghost->eaten == 0) && (grid->blocks[test_point.x][test_point.y] == GATE))
    dist[1] = UINT_MAX;
  else
    dist[1] = square_distance_to_target(ghost,test_point,ghost->target_);

  test_point.x = ghost->waypoint_.x;
  test_point.y = ghost->waypoint_.y + 1;

  if(( check_bounds(test_point) == 0 ) || (check_collisions_ghost(grid,test_point.x,test_point.y) == true) || (ghost->dir_.y == -1))
    dist[2] = UINT_MAX;
  else if((ghost->in_house == 0) && (ghost->eaten == 0) && (grid->blocks[test_point.x][test_point.y] == GATE))
    dist[2] = UINT_MAX;
  else
    dist[2] = square_distance_to_target(ghost,test_point,ghost->target_);

  test_point.x = ghost->waypoint_.x + 1;
  test_point.y = ghost->waypoint_.y;

  if(( check_bounds(test_point) == 0 ) || (check_collisions_ghost(grid, test_point.x, test_point.y) == true) || (ghost->dir_.x == -1))
    dist[3] = UINT_MAX;
  else if((ghost->in_house == 0) && (ghost->eaten == 0) && (grid->blocks[test_point.x][test_point.y] == GATE))
    dist[3] = UINT_MAX;
  else
    dist[3] = square_distance_to_target(ghost,test_point,ghost->target_);
}

uint32_t square_distance_to_target(struct Ghost * ghost,struct vec3i test_point, struct vec3i target)
{
  if(ghost->frightened_ == 0 || ghost->eaten == 1)
  {
    int16_t x = target.x - test_point.x;
    int16_t y = target.y - test_point.y;

    return x*x + y*y;
  }
  else
  {
    return rand();
  }
}
