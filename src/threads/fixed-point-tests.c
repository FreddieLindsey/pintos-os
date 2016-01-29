#include "fixed-point.h"
#include <assert.h>
#include <stdio.h>

int main() {

  //testing fixed-point with 17.14
  int p = 17;
  int q = 14;
  int f = 1 << q;
  
  int n = 7;
  int x = (21.3323 * f);
  int y = (30.5544 * f);
  
  assert(INT_TO_FP(n, f) == 114688);
  assert(FP_TO_INT(81920 , f) == 5);
  assert(fp_to_int_nearest(x, f) == 21);
  assert(fp_to_int_nearest(-y, f) == -31);
  assert(ADD_FP_FP(x, y) == (int)(51.8867 * f));
  assert(ADD_FP_INT(x, n, f) == 464196);
  assert(SUB_FP_FP(y, x) == 151095); 
  assert(SUB_FP_INT(x, n, f) == 234820);
  assert(MUL_FP_INT(x, n) == 2446556);
  assert(MUL_FP_FP(x, y, f) == 10679001);
  assert(DIV_FP_INT(x, n) == 49929);
  assert(DIV_FP_FP(x, y, f) == 11438);

}
