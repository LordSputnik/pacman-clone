#include "surface_ops.h"

void copy_alpha_mask_sd(SDL_Surface* src, SDL_Surface* dest)
{
    dest->format->Amask = src->format->Amask;
    dest->format->Ashift = src->format->Ashift;
}

SDL_Surface* flip_y (SDL_Surface* surface)
{
    if( SDL_LockSurface(surface) == -1)
        return NULL;
    else
    {
      unsigned char* data = surface->pixels;
      unsigned char buffer;

      int width = surface->w;
      int height = surface->h;
      int Bpp = surface->format->BytesPerPixel;
      int x,y,i;
      int row_index;
      int src_index, dest_index;

      for(y = 0; y != height; y++)
      {
          row_index = y*width*Bpp;
          for(x = 0; x != width/2; x++)
          {
              src_index = row_index + x * Bpp;
              dest_index = row_index + ( width * Bpp ) - (x+1) * Bpp;

              for(i = 0; i != Bpp; i++)
              {
                  buffer = data[src_index+i];
                  data[src_index+i] = data[dest_index+i];
                  data[dest_index+i] = buffer;
              }
          }
      }
      SDL_UnlockSurface(surface);
      return surface;
    }
}

SDL_Surface* flip_x (SDL_Surface* surface)
{
    if( SDL_LockSurface(surface) == -1)
        return NULL;
    else
    {
      unsigned char* data = surface->pixels;
      unsigned char buffer;
      int x,y,i, width, height, Bpp;
      int column_index;
      int src_index, dest_index;

      width = surface->w;
      height = surface->h;
      Bpp = surface->format->BytesPerPixel;

      for(x = 0; x != width; x++)
      {
          column_index = x * Bpp;
          for(y = 0; y != height/2; y++)
          {
              src_index = column_index + (y * width * Bpp);
              dest_index = column_index + ((height - (y+1)) * width * Bpp);

              for(i = 0; i != Bpp; i++)
              {
                  buffer = data[src_index+i];
                  data[src_index+i] = data[dest_index+i];
                  data[dest_index+i] = buffer;
              }
          }
      }

      SDL_UnlockSurface(surface);
      return surface;
    }
}


SDL_Surface* scale (SDL_Surface* surface, float scale_x, float scale_y)
{
    float ul, ll, ur, lr;
    float frac_x, frac_y;
    float src_x, src_y;
    int isrc_x, isrc_y, isrc_x_1, isrc_y_1, ul_off, ll_off, ur_off, lr_off;
    int x,y,c;
    unsigned char* surface_data, *result_data;
    SDL_Surface* result = NULL;
    surface_data = result_data = NULL;
    printf("Scale: %f %f\n",scale_x,scale_y);

    result = SDL_CreateRGBSurface(SDL_SWSURFACE, (int)(scale_x*surface->w), (int)(scale_y*surface->h), surface->format->BitsPerPixel, 0, 0, 0, surface->format->Amask);
    copy_alpha_mask_sd(surface,result);

    SDL_LockSurface(surface);
    SDL_LockSurface(result);

    
    for(y = 0; y != result->h; ++y)
    {
        for(x = 0; x != result->w; ++x)
        {
            src_x = (float)(x)/scale_x;
            src_y = (float)(y)/scale_y;

            isrc_x = (int)src_x;
            isrc_y = (int)src_y;

            frac_x = (float)(isrc_x) - src_x;
            frac_y = (float)(isrc_y) - src_y;

            if( isrc_x < (surface->w - 1) )
                isrc_x_1 = isrc_x + 1;
            else
                isrc_x_1 = isrc_x;

            if( isrc_y < (surface->h - 1) )
                isrc_y_1 = isrc_y + 1;
            else
                isrc_y_1 = isrc_y;

            ul = (1.0f - frac_x) * (1.0f - frac_y);
            ll = (1.0f - frac_x) * frac_y;
            ur = frac_x * (1.0f - frac_y);
            lr = frac_x * frac_y;

            surface_data = (unsigned char*)(surface->pixels);
            result_data = (unsigned char*)(result->pixels);
            for(c = 0; c != result->format->BytesPerPixel; ++c)
            {
                ul_off = isrc_y * surface->pitch + isrc_x * surface->format->BytesPerPixel + c;
                ll_off = isrc_y_1 * surface->pitch + isrc_x * surface->format->BytesPerPixel + c;
                ur_off = isrc_y * surface->pitch + isrc_x_1 * surface->format->BytesPerPixel + c;
                lr_off = isrc_y_1 * surface->pitch + isrc_x_1 * surface->format->BytesPerPixel + c;

                result_data [
                y * result->pitch + x * result->format->BytesPerPixel + c
                ]  =
                (unsigned char) ( ul * surface_data[ul_off] + ll * surface_data[ll_off]
                                 + ur * surface_data[ur_off] + lr * surface_data[lr_off] );
            }
        }
    }
    SDL_UnlockSurface(result);
    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);

    return result;
}


SDL_Surface* rotate (SDL_Surface* surface, int degrees)
{
    switch(degrees)
    {
        case 90:
            return rot_90(surface);
        case 180:
            return rot_180(surface);
        case 270:
            return rot_270(surface);
    }
    return NULL;
}

SDL_Surface* rot_90 (SDL_Surface* surface)
{
    if( SDL_LockSurface(surface) == -1)
        return NULL;
    else
    {
      unsigned char* data = NULL;
      unsigned char* buffer = NULL;
      int width, height, Bpp;
      int x,y,i;
      int new_x, new_y;
      int src_index, dest_index;

      SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->h, surface->w, surface->format->BitsPerPixel,
                                    0, 0, 0, surface->format->Amask);
      copy_alpha_mask_sd(surface,result);

      SDL_LockSurface(result);

      width = surface->w;
      height = surface->h;
      Bpp = surface->format->BytesPerPixel;

      data = (unsigned char*)(surface->pixels);
      buffer = (unsigned char*)(result->pixels);

      for(y = 0; y != height; y++)
      {
          new_x = height - (y + 1);
          for(x = 0; x != width; x++)
          {
              new_y = x;
              src_index = (y * width * Bpp) + (x * Bpp);
              dest_index = (new_y * width * Bpp) + (new_x * Bpp);
              for(i = 0; i != Bpp; i++)
              {
                  buffer[dest_index+i] = data[src_index+i];
              }
          }
      }

      SDL_UnlockSurface(result);
      SDL_UnlockSurface(surface);
      SDL_FreeSurface(surface);

      return result;
    }
}

SDL_Surface* rot_180 (SDL_Surface* surface)
{
    if( SDL_LockSurface(surface) == -1)
        return NULL;
    else
    {
      int width, height, Bpp;
      int x,y,i;
      int new_x, new_y;
      int src_index, dest_index;
      unsigned char* data = NULL;
      unsigned char* buffer = NULL;
      SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel,
                                    0, 0, 0, surface->format->Amask);
      copy_alpha_mask_sd(surface,result);

      SDL_LockSurface(result);

      width = surface->w;
      height = surface->h;
      Bpp = surface->format->BytesPerPixel;

      data = (uint8_t*)(surface->pixels);
      buffer = (uint8_t*)(result->pixels);

      for(y = 0; y != height; y++)
      {
          new_y = height - (y + 1);
          for(x = 0; x != width; x++)
          {
              new_x = width - (x + 1);
              src_index = (y * width * Bpp) + (x * Bpp);
              dest_index = (new_y * width * Bpp) + (new_x * Bpp);
              for(i = 0; i != Bpp; i++)
              {
                  buffer[dest_index+i] = data[src_index+i];
              }
          }
      }

      SDL_UnlockSurface(result);
      SDL_UnlockSurface(surface);
      SDL_FreeSurface(surface);
      return result;
    }
}
SDL_Surface* rot_270 (SDL_Surface* surface)
{
    if( SDL_LockSurface(surface) == -1)
        return NULL;
    else
    {
      int width, height, Bpp;
      int x,y,i;
      int new_x, new_y;
      int src_index, dest_index;
      unsigned char* data = NULL;
      unsigned char* buffer = NULL;
      SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->h, surface->w, surface->format->BitsPerPixel,
                                    0, 0, 0, surface->format->Amask);
      copy_alpha_mask_sd(surface,result);

      SDL_LockSurface(result);

      width = surface->w;
      height = surface->h;
      Bpp = surface->format->BytesPerPixel;

      data = (uint8_t*)surface->pixels;
      buffer = (uint8_t*)result->pixels;

      for(y = 0; y != height; y++)
      {
          new_x = y;
          for(x = 0; x != width; x++)
          {
              new_y = width - (x + 1);
              src_index = (y * width * Bpp) + (x * Bpp);
              dest_index = (new_y * width * Bpp) + (new_x * Bpp);

              for(i = 0; i != Bpp; i++)
              {
                  buffer[dest_index+i] = data[src_index+i];
              }
          }
      }

      SDL_UnlockSurface(result);
      SDL_UnlockSurface(surface);
      SDL_FreeSurface(surface);
      return result;
    }
}
