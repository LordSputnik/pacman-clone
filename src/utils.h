#ifndef _PACMAN_UTILS_H_
#define _PACMAN_UTILS_H_

#include <stdint.h>

struct vec3i
{
  int16_t x;
  int16_t y;
};

struct vec3f
{
  float x;
  float y;
};

int PrintSDLError();
int PrintILError();

int max(int a, int b);
int min(int a, int b);

#ifdef SHOW_LEAK

void* Malloc(size_t i);
void Free(void* ptr);

#else

#define Malloc malloc
#define Free free

#endif

#endif /* _PACMAN_UTILS_H_ */
