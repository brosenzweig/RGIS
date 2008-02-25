#ifndef _CM_H
#define _CM_H

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#else
#include <stdbool.h>	
#endif

enum { CMsucceeded = 0,  CMfailed  = -1 } CMreturn;

#define CMargTest(arg,shortArg,longArg) ((strcmp(arg,shortArg)==0)||(strcmp(arg,longArg)==0))

typedef enum { CMmsgSysError, CMmsgAppError, CMmsgUsrError, CMmsgDebug, CMmsgWarning, CMmsgInfo } CMmsgType;

const char *CMprgName (const char *);
int   CMargShiftLeft  (int, char **, int);
int   CMoptLookup     (const char **, const char *, bool);
void  CMoptPrintList  (CMmsgType, const char *, const char *[]);

bool CMmsgSetStreamFile (CMmsgType, const char *);
void CMmsgSetStream     (CMmsgType, FILE *);
void CMmsgSetStatus     (CMmsgType, bool);
int  CMmsgPrint         (CMmsgType, const char *, ...);
void CMmsgCloseStream   (CMmsgType);
void CMmsgCloseAllStreams ();

char *CMstrAppend    (char *, const char *, const char *);
char *CMstrDuplicate (const char *);

char *CMbufGetLine      (char *, int *, FILE *);
char *CMbufTrim         (char *);
char *CMbufStripChar    (char *, char);
#define CMbufStripDQuotes(buffer) (CMbufStripChar (buffer,'\"'))
#define CMbufStripSQuotes(buffer) (CMbufStripChar (buffer,'\''))

#if defined(__cplusplus)
}
#endif

#endif
