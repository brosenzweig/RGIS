#ifndef _NCGcm_H
#define _NCGcm_H

#include<NCstring.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define NCGcmArgTest(arg,shortArg,longArg) ((strcmp(arg,shortArg)==0)||(strcmp(arg,longArg)==0))
#define NCGcmArgCheck(arg,argPos,argNum) ((argPos >= argNum)||(arg[argPos][0] == '-'))

char *NCGcmProgName     (char *);
// Returns the program name protion of the argv[0] by locating the last '/' character preceeding
// the program name.
void  NCGcmArgShiftLeft	(int argPos,     char **argv, int argc);
// Shifts all the argument starting from argPos + 1 to argc - 1 and moves the content of argv[argPos] to argv[argc -1] 
int   NCGcmStringLookup (char **list, char *string,  bool matchlength);
// Finds the first occurance of the matching "string" from list.

#if defined(__cplusplus)
}
#endif

#endif /* _NCGcm_H */
