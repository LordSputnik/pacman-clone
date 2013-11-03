#ifndef _PACMAN_AUDIO_H_
#define _PACMAN_AUDIO_H_

#include <stdint.h>

void InitSound(void);
void DestroySound(void);

void SetWakka(uint8_t on);
void UpdateWakka(void);

void PacmanDie(void);
uint8_t isPacmanDying(void);
void PacmanDead(void);

#endif // _PACMAN_AUDIO_H_
