#ifndef _NCGmath_H
#define _NCGmath_H

#include<NC.h>
#include<math.h>
#include<ctype.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FLOAT_NOVALUE NAN
#define INT_NOVALUE 0x7fffffffL

bool   NCGmathIsNumber(char *expr);
// takes given string and verifies if it's a double.
bool   NCGmathEqualValues (double, double);
// Compares two floats for equality.

void   NCGmathSetSphereRadius (double);
void   NCGmathSetEpsilon (double);
double NCGmathCoordinateDistance (NCGprojection,     const NCGcoordinate_t *, const NCGcoordinate_t *);
double NCGmathRectangleArea      (NCGprojection,     const NCGcoordinate_t *, const NCGcoordinate_t *);
double NCGmathVectorByVector     (const NCGcoordinate_t *, const NCGcoordinate_t *, const NCGcoordinate_t *);
double NCGmathVectorXVector      (const NCGcoordinate_t *, const NCGcoordinate_t *, const NCGcoordinate_t *);
void   NCGmathGauss (double [],int,int);
double NCGmathStudentsT (double);

#if defined(__cplusplus)
}
#endif

#endif /* _NCGmath_H */
