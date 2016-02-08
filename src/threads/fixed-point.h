//Converts integer n into fixed point p.q where f = 2^q
#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <inttypes.h>

#define INT_TO_FP(n, f) ((n) * (f))
#define FP_TO_INT(x, f) ((x) / (f))
#define ADD_FP_FP(x, y) ((x) + (y))
#define ADD_FP_INT(x, n, f) ((x) + ((n)*(f)))
#define SUB_FP_FP(x, y) ((x) - (y))
#define SUB_FP_INT(x, n, f) ((x) - ((n)*(f)))
#define MUL_FP_INT(x, n) ((x) * (n))
#define MUL_FP_FP(x, y, f) ((((int64_t)(x)) * (y))/(f))
#define DIV_FP_INT(x, n) ((x) / (n))
#define DIV_FP_FP(x, y, f) ((((int64_t)(x)) * (f))/(y))
/* This is the f value in fixed_point arithmetic */
#define FIXED_BASE (2 << 17)

int fp_to_int_nearest(int x, int f) {
  if (x >= 0)
    return (x + f/2)/f;
  else
    return (x - f/2)/f;
}

#endif
