#ifndef _NCGtime_H
#define _NCGtime_H

#include<NC.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include<udunits.h>

typedef enum { NCGtimeYear   = 0,
					NCGtimeMonth  = 1,
					NCGtimeDay    = 2,
					NCGtimeHour   = 3,
	            NCGtimeMinute = 4,
					NCGtimeSecond = 5 } NCGtimeStep;

bool     NCGtimeLeapYear    (int);
size_t   NCGtimeMonthLength (int, size_t);
size_t   NCGtimeDayOfYear   (int, size_t, size_t);
NCGstate NCGtimeParse (const char *, NCGtimeStep, utUnit *, double *);
NCGstate NCGtimePrint (NCGtimeStep, utUnit *, double, char *);
const char *NCGtimeStepString (NCGtimeStep);

#if defined(__cplusplus)
}
#endif

#endif /* _NCGtime_H */
