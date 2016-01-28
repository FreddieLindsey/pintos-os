
//Converts integer n into fixed point p.q where f = 2^q
#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#define IntToFP(n, f) ((n) * (f))   
#define FPToInt(x, f) ((x) / (f))
#define FPToIntNearest(x, f) if x >= 0 \
				(((x) + (f)) /2) \
                             else  \
                                (((x) - (f)) /2)
#define AddFPFP(x, y) ((x) + (y))
#define AddFPInt(x, n, f) ((x) + ((n)*(f)))
#define SubFPFP(x, y) ((x) - (y))
#define SubFPInt(x, n, f) ((x) - ((n)*(f)))
#define MultiplyFPInt(x, n) ((x) * (n))
#define MultiplyFPFP(x, y, f) ((((int64_t)(x)) * (y))/(f))
#define DivideFPInt(x, n) ((x) / (n))
#define DivideFPFP(x, y, f) ((((int64_t)(x)) * (f))/(y))

#endif

