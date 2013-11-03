#include "utils.h"
#include <SDL/SDL.h>

int PrintSDLError()
{
	char* error = SDL_GetError();
	if( strlen(error) > 0 )
	{
		printf("SDL has encountered an error: %s\n", error);
		SDL_ClearError();
		return 1;
	}
	return 0;
}

#ifdef SHOW_LEAK
void* Malloc(size_t i)
{
    static int alloc = 0;
    if(i > 0)
    {
        void * ptr = malloc(i);
        printf(">>>>> Heap allocations: %i, ptr = %p\n", ++alloc, ptr);
        return ptr;
    }
    else
    {
        printf(">>>>> Heap allocations: %i\n", alloc);
        return NULL;
    }
}

void Free(void* ptr)
{
    static int dealloc = 0;
    if(ptr == NULL)
    {
        printf("<<<<< Heap deallocations: %i\n", dealloc);
    }
    else
    {
        printf("<<<<< Heap deallocations: %i, ptr = %p\n", ++dealloc, ptr);
        free(ptr);
    }
    return;
}

#endif

#ifndef max
  int max(int a, int b)
  {
      return (a < b ? b : a);
  }
#endif

#ifndef min
  int min(int a, int b)
  {
      return (a < b ? a : b);
  }
#endif