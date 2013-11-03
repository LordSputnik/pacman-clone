#include <stdint.h>
#include <time.h>

struct high_score_t
{
  uint32_t score;
  uint32_t duration;
  time_t time;
};

void load_high_scores();
uint32_t get_high_score();
void set_high_score(uint32_t score);
void save_high_scores();
