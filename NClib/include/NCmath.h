#ifndef _NCmath_H
#define _NCmath_H

#include<NC.h>
#include<math.h>
#include<ctype.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FLOAT_NOVALUE NAN
#define INT_NOVALUE 0x7fffffffL

bool   NCmathIsNumber(char *expr);
// takes given string and verifies if it's a double.
bool   NCmathEqualValues (double, double);
// Compares two floats for equality.

void   NCmathSetSphereRadius (double);
void   NCmathSetEpsilon (double);
double NCmathCoordinateDistance (NCprojection,     const NCcoordinate_t *, const NCcoordinate_t *);
double NCmathRectangleArea      (NCprojection,     const NCcoordinate_t *, const NCcoordinate_t *);
double NCmathVectorByVector     (const NCcoordinate_t *, const NCcoordinate_t *, const NCcoordinate_t *);
double NCmathVectorXVector      (const NCcoordinate_t *, const NCcoordinate_t *, const NCcoordinate_t *);
void   NCmathGauss (double [],int,int);
double NCmathStudentsT (double);

#if defined(__cplusplus)
}
#endif

#endif /* _NCmath_H */
