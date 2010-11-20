#ifndef _NCcore_H
#define _NCcore_H

#include<stdio.h>

#if defined(__cplusplus)
extern "C" {
#else

#ifndef bool 

typedef enum { false = 0, true = 1 } bool;

#define false   false
#define true    true

#endif

#endif

typedef enum { NCsucceeded =  0, NCfailed    = -1, NCundefined = -2 } NCstate; 


#define NCcoreEpsilon   0.000001
#define NCcorePrecision 0.0001
#define NCcoreTolerance 0.001

#define NCcoreYesNoString(cond)     (cond ? "yes" : "no")
#define NCcoreTrueFalseString(cond) (cond ? "true" : "false")

#if defined(__cplusplus)
}
#endif

#endif
