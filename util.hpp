//
//
// util

#pragma once

#include <math.h>
  
#ifndef M_PI
#define M_PI  (3.14159265)
#endif

static constexpr float TAU = 2 * M_PI;
static constexpr float M_2PI = 2 * M_PI;
static constexpr double M_3PI_2 = (3.0 * M_PI) / 2.0;

inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}