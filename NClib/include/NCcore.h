#ifndef _NCGcore_H
#define _NCGcore_H

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

typedef enum { NCGsucceeded =  0, NCGfailed    = -1, NCGundefined = -2 } NCGstate; 


#define NCGcoreEpsilon   0.000001
#define NCGcorePrecision 0.0001
#define NCGcoreTolerance 0.001

#define NCGcoreYesNoString(cond)     (cond ? "yes" : "no")
#define NCGcoreTrueFalseString(cond) (cond ? "true" : "false")

#if defined(__cplusplus)
}
#endif

#endif
