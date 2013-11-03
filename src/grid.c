#include "grid.h"
#include <stdio.h>
#include "image.h"

int grid_load(const char* grid_name, struct Grid* grid)
{
  FILE* grid_file = NULL;
  size_t path_size = strlen(grid_name)+strlen(GRID_DIR);
  char* file_path = Malloc((path_size+1) * sizeof(char));

  puts("Loading Grid!");

  strcpy(file_path, GRID_DIR);
  strcat(file_path, grid_name);

  grid_file = fopen(file_path,"r");

  if(grid_file == NULL)
  {
    printf("Could not open level: %s!\n", file_path);
    return -1;
    Free(file_path);
  }
  else
  {
    char buffer;
    int x, y;
    for(y = 0; y != 3; y++)
    {
      for(x = 0; x != GRID_X; x++)
      {
        grid->blocks[x][y] = BLANK;
      }
    }

    for(y = 3; y != GRID_Y; y++)
    {
      for(x = 0; x != GRID_X; x++)
      {
        do
        {
          buffer = fgetc (grid_file);
        } while( buffer == '\n' );

        switch(buffer)
        {
        case '.':
          grid->blocks[x][y] = DOT;
          break;
        case 'o':
          grid->blocks[x][y] = SUPER_DOT;
          break;
        case 'w':
        case 'x':
          grid->blocks[x][y] = H_LINE;
          break;
        case 'a':
        case 'd':
          grid->blocks[x][y] = V_LINE;
          break;
        case 'q':
          grid->blocks[x][y] = CORNER_UL;
          break;
        case 'e':
          grid->blocks[x][y] = CORNER_UR;
          break;
        case 'c':
          grid->blocks[x][y] = CORNER_DR;
          break;
        case 'z':
          grid->blocks[x][y] = CORNER_DL;
          break;
        case '-':
          grid->blocks[x][y] = GATE;
          break;
        case 's':
          grid->blocks[x][y] = FORBIDDEN;
          break;
        case '=':
          grid->blocks[x][y] = TUNNEL;
          break;
        default:
          grid->blocks[x][y] = BLANK;
          break;
        }
      }
    }

    fclose(grid_file);
    Free(file_path);

    return 0;
  }
}

void grid_reload(struct Grid* grid)
{
  int x,y;
  for(y = 3; y != GRID_Y; y++)
  {
    for(x = 0; x != GRID_X; x++)
    {
      if(grid->blocks[x][y] == EMPTIED)
        grid->blocks[x][y] = DOT;
      if(grid->blocks[x][y] == SUPER_EMPTIED)
        grid->blocks[x][y] = SUPER_DOT;
    }
  }
}

void grid_render(struct Grid* grid, struct Image* images, struct Anim* anims, SDL_Surface* screen)
{
  SDL_Rect blitloc;
  int x, y;
  blitloc.y = 0;
  for(y = 0; y != GRID_Y; y++)
  {
    blitloc.x = 0;
    for(x = 0; x != GRID_X; x++)
    {
      switch(grid->blocks[x][y])
      {
        case DOT:
          SDL_BlitSurface ( images[0].img, NULL, screen, &(blitloc) );
          break;
        case SUPER_DOT:
          SDL_BlitSurface ( images[1].img, NULL, screen, &(blitloc) );
          break;
        case H_LINE:
          SDL_BlitSurface ( images[2].img, NULL, screen, &(blitloc) );
          break;
        case V_LINE:
          SDL_BlitSurface ( images[3].img, NULL, screen, &(blitloc) );
          break;
        case CORNER_UL:
          SDL_BlitSurface ( images[4].img, NULL, screen, &(blitloc) );
          break;
        case CORNER_UR:
          SDL_BlitSurface ( images[5].img, NULL, screen, &(blitloc) );
          break;
        case CORNER_DR:
          SDL_BlitSurface ( images[6].img, NULL, screen, &(blitloc) );
          break;
        case CORNER_DL:
          SDL_BlitSurface ( images[7].img, NULL, screen, &(blitloc) );
          break;
        case GATE:
          SDL_BlitSurface ( images[8].img, NULL, screen, &(blitloc) );
          break;
        default:
          break;
      }
      blitloc.x += GRID_CELL_SIZE;
    }
    blitloc.y += GRID_CELL_SIZE;
  }
}

int check_collisions_ghost(struct Grid* grid, int x, int y)
{
  switch( grid->blocks[x][y] )
  {
    case BLANK:
    case EMPTIED:
    case SUPER_EMPTIED:
    case DOT:
    case SUPER_DOT:
    case GATE:
    case TUNNEL:
      return 0;
    default:
      return 1;
  }
}

int check_collisions(struct Grid* grid, int x, int y)
{
  switch( grid->blocks[x][y] )
  {
    case BLANK:
    case EMPTIED:
    case SUPER_EMPTIED:
    case DOT:
    case SUPER_DOT:
    case TUNNEL:
        return 0;
    default:
        return 1;
  }
}

uint8_t check_bounds(struct vec3i pos)
{
  if(pos.x >= GRID_X)
    return 0;
  else if(pos.x < 0)
    return 0;

  if(pos.y >= GRID_Y)
    return 0;
  else if(pos.y < 0)
    return 0;

  return 1;
}
