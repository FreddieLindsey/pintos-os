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
  
  assert(int_to_fp(n, f) == 114688);
  assert(fp_to_int(81920 , f) == 5);
  assert(fp_to_int_nearest(x, f) == 21);
  assert(fp_to_int_nearest(-y, f) == -31);
  assert(add_fp_fp(x, y) == (int)(51.8867 * f));
  assert(add_fp_int(x, n, f) == 464196);
  assert(sub_fp_fp(y, x) == 151095); 
  assert(sub_fp_int(x, n, f) == 234820);
  assert(mul_fp_int(x, n) == 2446556);
  assert(mul_fp_fp(x, y, f) == 10679001);
  assert(div_fp_int(x, n) == 49929);
  assert(div_fp_fp(x, y, f) == 11438);

}
