#include "audio.h"
#include <SDL/SDL_mixer.h>

Mix_Chunk * wakka = NULL;
int wakka_channel = -1;
uint8_t wakka_on = 0;
Mix_Chunk * siren = NULL;
Mix_Chunk * die = NULL;
int death_channel = -1;

void SetWakka(uint8_t on)
{
  wakka_on = on;
}

void UpdateWakka()
{
  if(wakka_on)
  {
    if(wakka_channel == -1 || Mix_Playing(wakka_channel) == 0)
      wakka_channel = Mix_PlayChannel(-1,wakka,0);
  }
}

void PacmanDie()
{
  Mix_HaltChannel(-1);
  death_channel  = Mix_PlayChannel(-1,die,0);
}

uint8_t isPacmanDying()
{
  return (uint8_t)(Mix_Playing(death_channel));
}

void PacmanDead()
{
  Mix_PlayChannel(-1,siren,-1);
}

void InitSound()
{
  Mix_Init(0);
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_U8, 1, 1024);

  siren = Mix_LoadWAV("audio/siren.wav");
  siren->volume = 64;

  wakka = Mix_LoadWAV("audio/wakka.wav");
  wakka->volume = 64;

  die = Mix_LoadWAV("audio/die.wav");

  Mix_PlayChannel(-1,siren,-1);
}

void DestroySound()
{
  Mix_CloseAudio();
}
