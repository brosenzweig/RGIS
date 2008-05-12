#ifndef _CM_H
#define _CM_H

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#else
#include <stdbool.h>	
#endif

typedef enum { CMsucceeded = 0,  CMfailed  = -1 } CMreturn;

#define CMargTest(arg,shortArg,longArg) ((strcmp(arg,shortArg)==0)||(strcmp(arg,longArg)==0))

typedef enum { CMmsgSysError = 0, CMmsgAppError = 1, CMmsgUsrError = 2, CMmsgDebug = 3, CMmsgWarning = 4, CMmsgInfo = 5} CMmsgType;

const char *CMprgName (const char *);
int   CMargShiftLeft  (int, char **, int);
int   CMoptLookup     (const char **, const char *, bool);
void  CMoptPrintList  (CMmsgType, const char *, const char *[]);

bool CMmsgSetStreamFile (CMmsgType, const char *);
void CMmsgSetStream     (CMmsgType, FILE *);
void CMmsgSetStatus     (CMmsgType, bool);
int  CMmsgPrint         (CMmsgType, const char *, ...);
void CMmsgIndent        (CMmsgType, bool);
void CMmsgCloseStream   (CMmsgType);
void CMmsgCloseAllStreams ();

char *CMstrAppend    (char *, const char *, const char *);
char *CMstrDuplicate (const char *);

char *CMbufGetLine      (char *, int *, FILE *);
char *CMbufTrim         (char *);
char *CMbufStripChar    (char *, char);
#define CMbufStripDQuotes(buffer) (CMbufStripChar (buffer,'\"'))
#define CMbufStripSQuotes(buffer) (CMbufStripChar (buffer,'\''))

#define CMmathEpsilon 0.000001
bool    CMmathEqualValues (double,double);
#define CMmathMinimum(a,b) (((a) < (b)) ? (a) : (b))
#define CMmathMaximum(a,b) (((a) > (b)) ? (a) : (b))
#define CMyesNoString(cond) (cond ? "yes" : "no")


#if defined(__cplusplus)
}
#endif

#endif
