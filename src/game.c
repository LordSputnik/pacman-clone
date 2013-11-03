#include "game.h"

#include "pacman.h"
#include "ghost.h"
#include "blinky.h"
#include "pinky.h"
#include "inky.h"
#include "clyde.h"
#include "grid.h"
#include "utils.h"
#include "audio.h"
#include <math.h>

  struct Pacman pacman;
  struct Ghost blinky, pinky, inky, clyde;
  struct Grid my_grid;

char behaviour_;
uint8_t frightened_;
uint8_t ghost_eat_combo;
uint8_t pacman_dead;

uint8_t effective_level;
uint8_t level;
uint8_t stage;
int32_t stage_times[7];
uint32_t score;
static uint32_t high_score;
uint32_t level_time;
uint8_t remaining_dots;
uint8_t remaining_lives;

float pacman_speed[21];
float ghost_speed[21];
float tunnel_speed[21];
uint8_t elroy1_dots[21];
float elroy1_speed[21];
uint8_t elroy2_dots[21];
float elroy2_speed[21];

uint8_t level_bonus[21];
uint16_t bonus_points[21];

float pacman_fright_speed[21];
float ghost_fright_speed[21];
int32_t fright_time[21];
int32_t fright_timer;
uint8_t bonus_spawned;
int16_t bonus_timer;

uint8_t ghost_counter[5]; /* Pinky, Inky, Clyde, Global, Not Used. */
uint8_t* exit_house[3]; /* Pinky, Inky, Clyde */
uint8_t dot_limit[3][21];
uint8_t active_counter; /* 0 is inactive. */

uint32_t get_score()
{
  return score;
}


void set_behaviour(char behaviour)
{
  behaviour_ = behaviour;

  ghost_set_behaviour(&blinky,behaviour);
  ghost_set_behaviour(&pinky,behaviour);
  ghost_set_behaviour(&inky,behaviour);
  ghost_set_behaviour(&clyde,behaviour);
}

char get_ghost_behaviour()
{
  return behaviour_;
}

void inc_level()
{
  ++level;
}

struct Pacman* get_pacman()
{
  return &pacman;
}

struct Grid* get_grid()
{
  return &my_grid;
}

void notify_ghost_eaten()
{
  switch(ghost_eat_combo)
  {
  case 1:
    score += 200;
    break;
  case 2:
    score += 400;
    break;
  case 3:
    score += 800;
    break;
  case 4:
    score += 1600;
    break;
  }

  ++ghost_eat_combo;
}

float get_blinky_speed(struct Ghost* ghost, struct Grid* grid)
{
  if(grid->blocks[ghost->pos_.x][ghost->pos_.y] == TUNNEL)
  {
    return tunnel_speed[effective_level];
  }

  if(ghost->frightened_ == 1)
  {
    return ghost_fright_speed[effective_level];
  }

  if(remaining_dots < elroy2_dots[effective_level])
  {
    return elroy2_speed[effective_level];
  }
  else if(remaining_dots < elroy1_dots[effective_level])
  {
    return elroy1_speed[effective_level];
  }

  return ghost_speed[effective_level];
}

/* float get_pinky_speed() */
/* float get_inky_speed() */
/* float get_clyde_speed() */
float get_ghost_speed(struct Ghost* ghost, struct Grid* grid)
{
  if(grid->blocks[ghost->pos_.x][ghost->pos_.y] == TUNNEL)
  {
    return tunnel_speed[effective_level];
  }

  if(ghost->frightened_ == 1)
  {
    return ghost_fright_speed[effective_level];
  }

  return ghost_speed[effective_level];
}

float get_pacman_speed()
{
  if(frightened_ == 1)
  {
    return pacman_fright_speed[effective_level];
  }

  return pacman_speed[effective_level];
}

void eat_dot()
{
  --remaining_dots;
  score += 10;

  if(active_counter < 3)
  {
    ++ghost_counter[active_counter];

    if(ghost_counter[active_counter] >= dot_limit[active_counter][effective_level])
    {
      (*exit_house[active_counter]) = 1;
      active_counter++;
    }
  }
}

void eat_super_dot()
{
  --remaining_dots;
  set_frightened(1);
  score += 50;

  if(active_counter < 3)
  {
    ++ghost_counter[active_counter];

    if(ghost_counter[active_counter] >= dot_limit[active_counter][effective_level])
    {
      (*exit_house[active_counter]) = 1;
      active_counter++;
    }
  }
}

uint8_t get_remaining_dots()
{
  return remaining_dots;
}

void set_frightened(char active)
{
  frightened_ = active;
  ghost_eat_combo = 1;

  if(frightened_ == 1)
    fright_timer = fright_time[effective_level];

  ghost_set_frightened(&blinky,active);
  ghost_set_frightened(&pinky,active);
  ghost_set_frightened(&inky,active);
  ghost_set_frightened(&clyde,active);
}

uint8_t get_frightened_state()
{
  return frightened_;
}

void start_kill_pacman()
{
  if(pacman_dead == 1)
    return;

  pacman_dead = 1;

  stop_anim(pacman.pacman_anim_);
  play_anim(pacman.death_anim_);
  pacman.dying = 1;

  PacmanDie();
}

void end_kill_pacman()
{
  stop_anim(pacman.death_anim_);
  play_anim(pacman.pacman_anim_);

  PacmanDead();

  pacman_reset(&pacman);
  --remaining_lives;
}

uint8_t get_game_over()
{
  return (remaining_lives == 0);
}

void init_game(struct Anim* anims, struct Image* images)
{
  int i;
  grid_load("level.txt", &my_grid);

  high_score = get_high_score();

  pacman_init(&pacman,anims);
  blinky_init(&blinky,anims,images);
  pinky_init(&pinky,anims,images);
  inky_init(&inky,anims,images);
  clyde_init(&clyde,anims,images);

  exit_house[0] = &(pinky.exit_house);
  exit_house[1] = &(inky.exit_house);
  exit_house[2] = &(clyde.exit_house);

  {
    level_bonus[0] = CHERRY_IMG; bonus_points[0] = 100;
    level_bonus[1] = STRAWBERRY_IMG; bonus_points[1] = 300;
    level_bonus[2] = level_bonus[3] = ORANGE_IMG; bonus_points[2] = bonus_points[3] = 500;
    level_bonus[4] = level_bonus[5] = APPLE_IMG; bonus_points[4] = bonus_points[5] = 700;
    level_bonus[6] = level_bonus[7] = GRAPES_IMG; bonus_points[6] = bonus_points[7] = 1000;
    level_bonus[8] = level_bonus[9] = GAX_IMG; bonus_points[8] = bonus_points[9] = 2000;
    level_bonus[10] = level_bonus[11] = BELL_IMG; bonus_points[10] = bonus_points[11] = 3000;

    for(i = 12; i != 21; ++i)
    {
      level_bonus[i] = KEY_IMG; bonus_points[i] = 5000;
    }
  }

  { /* Set the speed of pacman. */
    pacman_speed[0] = 0.8f;
    pacman_speed[1] = pacman_speed[2] = pacman_speed[3] = 0.9f;

    for(i = 4; i != 20; ++i)
      pacman_speed[i] = 1.0f;

    pacman_speed[20] = 0.9f;
  }

  {
    ghost_speed[0] = 0.75f;
    ghost_speed[1] = ghost_speed[2] = ghost_speed[3] = 0.85f;

    for(i = 4; i != 21; ++i)
      ghost_speed[i] = 0.95f;
  }

  {
    tunnel_speed[0] = 0.40f;
    tunnel_speed[1] = tunnel_speed[2] = tunnel_speed[3] = 0.45f;

    for(i = 4; i != 21; ++i)
      tunnel_speed[i] = 0.5f;
  }

  {
    fright_time[0] = 6000; fright_time[1] = 5000;
    fright_time[2] = 4000; fright_time[3] = 3000;
    fright_time[4] = 2000; fright_time[5] = 5000;
    fright_time[6] = 2000; fright_time[7] = 2000;
    fright_time[8] = 1000; fright_time[9] = 5000;
    fright_time[10] = 2000; fright_time[11] = 1000;
    fright_time[12] = 1000; fright_time[13] = 3000;
    fright_time[14] = 1000; fright_time[15] = 1000;
    fright_time[17] = 1000;
    fright_time[16] = fright_time[18] = fright_time[19] = fright_time[20] = 0;
  }

  {
    pacman_fright_speed[0] = 0.9f;
    pacman_fright_speed[1] = pacman_fright_speed[2] = pacman_fright_speed[3] = 0.95f;

    for(i = 4; i != 21; ++i)
    {
      pacman_fright_speed[i] = 1.0f;
    }
  }

{
    ghost_fright_speed[0] = 0.5f;
    ghost_fright_speed[1] = ghost_fright_speed[2] = ghost_fright_speed[3] = 0.55f;

    for(i = 4; i != 21; ++i)
    {
      ghost_fright_speed[i] = 0.6f;
    }
  }

  {
    elroy1_speed[0] = 0.8f;
    elroy1_speed[1] = elroy1_speed[2] = elroy1_speed[3] = 0.9f;

    for(i = 4; i != 21; ++i)
    {
      elroy1_speed[i] = 1.0f;
    }
  }

  {
    elroy1_dots[0] = 20;
    elroy1_dots[1] = 30;
    elroy1_dots[2] = 40;
    elroy1_dots[3] = 40;
    elroy1_dots[4] = 40;
    elroy1_dots[5] = 50;
    elroy1_dots[6] = 50;
    elroy1_dots[7] = 50;
    elroy1_dots[8] = 60;
    elroy1_dots[9] = 60;
    elroy1_dots[10] = 60;
    elroy1_dots[11] = 80;
    elroy1_dots[12] = 80;
    elroy1_dots[13] = 80;
    elroy1_dots[14] = elroy1_dots[15] = elroy1_dots[16] = elroy1_dots[17] = 100;
    elroy1_dots[18] = elroy1_dots[19] = elroy1_dots[20] = 120;
  }

  {
    elroy2_speed[0] = 0.85f;
    elroy2_speed[1] = elroy2_speed[2] = elroy2_speed[3] = 0.95f;

    for(i = 4; i != 21; ++i)
    {
      elroy2_speed[i] = 1.05f;
    }
  }

  {
    elroy2_dots[0] = 10;
    elroy2_dots[1] = 15;
    elroy2_dots[2] = 20;
    elroy2_dots[3] = 20;
    elroy2_dots[4] = 20;
    elroy2_dots[5] = 25;
    elroy2_dots[6] = 25;
    elroy2_dots[7] = 25;
    elroy2_dots[8] = 30;
    elroy2_dots[9] = 30;
    elroy2_dots[10] = 30;
    elroy2_dots[11] = 40;
    elroy2_dots[12] = 40;
    elroy2_dots[13] = 40;
    elroy2_dots[14] = elroy2_dots[15] = elroy2_dots[16] = elroy2_dots[17] = 50;
    elroy2_dots[18] = elroy2_dots[19] = elroy2_dots[20] = 60;
  }

  {
    for(i = 0; i != 21; ++i)
    {
      dot_limit[0][i] = 0;
    }

    dot_limit[1][0] = 30;
    for(i = 1; i != 21; ++i)
    {
      dot_limit[1][i] = 0;
    }

    dot_limit[2][0] = 60;
    dot_limit[2][1] = 50;
    for(i = 2; i != 21; ++i)
    {
      dot_limit[2][i] = 0;
    }
  }

  level = 0;
  frightened_ = 0;
  remaining_lives = 3;
  bonus_spawned = 0;
  init_level();


/*uint8_t elroy1_dots[21];
float elroy1_speed[21];
uint8_t elroy2_dots[21];
float elroy2_speed[21];*/
}

void init_level()
{
  if(level < 21)
    effective_level = level;

  set_behaviour(1);
  pacman_dead = 0;
  frightened_ = 0;
  remaining_dots = 244;
  stage = 0;
  pinky.dot_counter = 0;
  inky.dot_counter = 0;
  clyde.dot_counter = 0;

  active_counter = 0;
  (*exit_house[0]) = 0;
  (*exit_house[1]) = 0;
  (*exit_house[2]) = 0;

  stage_times[1] = 20000; /* Chase 1 and 2 times are always 20s. */
  stage_times[3] = 20000;
  stage_times[4] = 5000; /* Scatter 3 time is always 5s. */
  printf("Level: %i Level Time: %i Stage: %i Stage Time: %i\n",effective_level,level_time,stage,stage_times[stage]);
  switch(effective_level)
  {
    case 0:
      stage_times[0] = 7000;
      stage_times[2] = 7000;
      stage_times[5] = 20000;
      stage_times[6] = 5000;
      break;
    case 1:
    case 2:
    case 3:
      stage_times[0] = 7000;
      stage_times[2] = 7000;
      stage_times[5] = 1033000;
      stage_times[6] = 17;
      break;
    default:
      stage_times[0] = 5000;
      stage_times[2] = 5000;
      stage_times[5] = 1037000;
      stage_times[6] = 17; /*  ~1/60 of a second. */
      break;
  }
}

void spawn_bonus()
{
  bonus_spawned = 1;
}

uint8_t is_bonus_spawned()
{
  return bonus_spawned;
}

void eat_bonus()
{
  score += bonus_points[effective_level];
  kill_bonus();
}

void kill_bonus()
{
  bonus_spawned = 0;
}

void update_game(struct Anim* anims, struct Image* images, uint32_t dt)
{
  if(pacman_dead == 1)
  {
    pacman_dead = isPacmanDying();
    if(pacman_dead == 0)
    {
      end_kill_pacman();
    }
    return;
  }

  level_time += dt;

  if(bonus_spawned == 1)
    bonus_timer -= dt;

  blinky_update(&blinky,&pacman,&my_grid);
  pinky_update(&pinky,&pacman,&my_grid);
  inky_update(&inky,&blinky, &pacman,&my_grid);
  clyde_update(&clyde,&pacman,&my_grid);
  pacman_update(&pacman,&my_grid);

  if(frightened_ == 1)
  {
    fright_timer -= dt;
    if(fright_timer < 0)
    {
      set_frightened(0);
    }
  }
  else
  {
    if(stage < 7)
    {
      stage_times[stage] -= dt;

      if(stage_times[stage] < 0)
      {

        if(behaviour_ == 1)
          set_behaviour(2);
        else
          set_behaviour(1);

        printf("Level: %i Level Time: %i Stage: %i Stage Time: %i\n",effective_level,level_time,stage,stage_times[stage]);
        printf("Behaviour: %i\n",behaviour_);

        ++stage;
      }
    }
  }

  if(get_remaining_dots() == 0)
  {
    inc_level();
    init_level();

    grid_reload(&my_grid);

    pacman_reset(&pacman);
    blinky_init(&blinky,anims,images);
    pinky_init(&pinky,anims,images);
    inky_init(&inky,anims,images);
    clyde_init(&clyde,anims,images);
  }
  else if(get_remaining_dots() == 170)
  {
    spawn_bonus();
    bonus_timer = (rand() % 1000) + 9000;
  }
  else if(get_remaining_dots() == 70)
  {
    spawn_bonus();
    bonus_timer = (rand() % 1000) + 9000;
  }


  if(bonus_timer <= 0 && bonus_spawned == 1)
  {
    kill_bonus();
  }
}

void score_render(struct Image* images, SDL_Surface* screen)
{
  /* Nasty, but probably necessary... */
  uint32_t temp = score;
  SDL_Rect rect;
  rect.x = 6*GRID_CELL_SIZE; rect.y = 1*GRID_CELL_SIZE;
  do
  {
    SDL_BlitSurface(images[(temp % 10) + ZERO_IMG].img, NULL, screen, &rect);
    temp /= 10;
    rect.x -= GRID_CELL_SIZE;
  } while ( temp != 0 );

  if(high_score > score)
  {
    temp = high_score;
  }
  else
  {
    temp = score;
  }
  rect.x = 16*GRID_CELL_SIZE; rect.y = 1*GRID_CELL_SIZE;
  do
  {
    SDL_BlitSurface(images[(temp % 10) + ZERO_IMG].img, NULL, screen, &rect);
    temp /= 10;
    rect.x -= GRID_CELL_SIZE;
  } while ( temp != 0 );
}

void bonus_render(struct Image* images, SDL_Surface* screen)
{
  SDL_Rect rect;
  int i = 0;
  int draw_count;
  rect.x = 25*GRID_CELL_SIZE + (GRID_CELL_SIZE/2); rect.y = 34*GRID_CELL_SIZE + (GRID_CELL_SIZE/4);


  if(level > 20)
  {
    for(; i != 6; ++i)
    {
      SDL_BlitSurface(images[KEY_IMG].img, NULL, screen, &rect);
      rect.x -= 2*GRID_CELL_SIZE;
    }
    return;
  }

  if(level < 7)
    draw_count = level;
  else
    draw_count = 6;

  for(i = 0; i <= draw_count; ++i)
  {
    SDL_BlitSurface(images[level_bonus[effective_level-draw_count+i]].img, NULL, screen, &rect);
    rect.x -= 2*GRID_CELL_SIZE;
  }

  if(bonus_spawned)
  {
    rect.x = 13*GRID_CELL_SIZE + (GRID_CELL_SIZE/2); rect.y = 20*GRID_CELL_SIZE;
    SDL_BlitSurface(images[level_bonus[effective_level]].img, NULL, screen, &rect);
  }
}

void life_render(struct Image* images, SDL_Surface* screen)
{
  SDL_Rect rect;
  int i = 1;
  rect.y = 34*GRID_CELL_SIZE + (GRID_CELL_SIZE/4);
  rect.x = 2*GRID_CELL_SIZE + (GRID_CELL_SIZE/2);

  for(;i < remaining_lives; ++i)
  {
    SDL_BlitSurface(images[LIFE_IMG].img, NULL, screen, &rect);
    rect.x += 2*GRID_CELL_SIZE;
  }
}

void render_game(struct Anim* anims, struct Image* images, SDL_Surface* screen)
{
  grid_render(&my_grid,images,anims,screen);

  score_render(images,screen);
  bonus_render(images,screen);
  life_render(images,screen);

  ghost_render(&blinky,screen);
  ghost_render(&pinky,screen);
  ghost_render(&inky,screen);
  ghost_render(&clyde,screen);
  pacman_render(&pacman,screen);


}
