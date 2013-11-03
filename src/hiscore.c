#include "hiscore.h"
#include <stdio.h>

char loaded = 0;

static struct high_score_t high_score[10];

void load_high_scores()
{
  FILE* high_scores = fopen("hiscores","rb");

  if(high_scores == NULL)
  {
    int i = 0;
    for(;i != 10; ++i)
      high_score[i].score = 0;

    return;
  }

  if(fread(high_score, sizeof(struct high_score_t),10,high_scores) != 10)
  {
    if(feof(high_scores) == 0)
    {
      int i;
      puts("Error in file op.");
      for(i = 0; i != 10; ++i)
        high_score[i].score = 0;

      return;
    }
  }
  return;
}

uint32_t get_high_score()
{
  if(loaded == 0)
    load_high_scores();

  return high_score[0].score;
}

void set_high_score(uint32_t score)
{
  int i = 10;
  for(;i != 0; --i)
  {
    if(score >= high_score[i-1].score)
    {
      if(i != 10)
      {
        high_score[i].score = high_score[i-1].score;
        high_score[i].duration = high_score[i-1].duration;
        high_score[i].time = high_score[i-1].time;
      }
    }
    else
    {
      break;
    }
  }

  if(i != 10)
  {
    high_score[i].score = score;
    high_score[i].time = time(NULL);
  }
}

void save_high_scores()
{
  FILE* high_scores = fopen("hiscores","wb");

  if(high_scores != NULL)
    fwrite(high_score,sizeof(struct high_score_t),10,high_scores);

  fclose(high_scores);
  return;
}
