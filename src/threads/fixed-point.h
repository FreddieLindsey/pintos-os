
//Converts integer n into fixed point p.q where f = 2^q
#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <inttypes.h>

#define int_to_fp(n, f) ((n) * (f))   
#define fp_to_int(x, f) ((x) / (f))
#define add_fp_fp(x, y) ((x) + (y))
#define add_fp_int(x, n, f) ((x) + ((n)*(f)))
#define sub_fp_fp(x, y) ((x) - (y))
#define sub_fp_int(x, n, f) ((x) - ((n)*(f)))
#define mul_fp_int(x, n) ((x) * (n))
#define mul_fp_fp(x, y, f) ((((int64_t)(x)) * (y))/(f))
#define div_fp_int(x, n) ((x) / (n))
#define div_fp_fp(x, y, f) ((((int64_t)(x)) * (f))/(y))


int fp_to_int_nearest(int x, int f) {
  if (x >= 0)
    return (x + f/2)/f;
  else
    return (x - f/2)/f;
}

#endif

