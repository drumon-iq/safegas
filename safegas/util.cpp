#include "util.h"

unsigned long duration = 0;
unsigned long timer = 0;
unsigned long old_millis = 0;
bool doAlert = false;

void set_duration_min (int mins)
{
  duration = mins * 60 * 1000;
}
