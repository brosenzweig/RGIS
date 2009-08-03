#ifndef _CM_H
#define _CM_H

#include <stdio.h>
#include <pthread.h>

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

typedef struct CMthreadTask_s {
	size_t Id;
	bool   Completed, Locked;
	size_t Dependence;
} CMthreadTask_t, *CMthreadTask_p;

typedef void (*CMthreadUserFunc) (void *, size_t);

typedef struct CMthreadJob_s {
	CMthreadTask_p   Tasks;
	size_t           TaskNum;
	size_t           LastId;
	CMthreadUserFunc UserFunc;
	void            *UserData;
} CMthreadJob_t, *CMthreadJob_p;

CMthreadJob_p CMthreadJobCreate        (size_t taskNum, CMthreadUserFunc, void *);
void          CMthreadJobDestroy       (CMthreadJob_p);
CMreturn      CMthreadJobTaskDependece (CMthreadJob_p, size_t, size_t);

typedef struct CMthreadData_s {
	size_t          Id;
	pthread_t       Thread;
	pthread_mutex_t Mutex;
	pthread_cond_t  Signal;
	size_t          CompletedTasks;
	void           *TeamPtr;
} CMthreadData_t,  *CMthreadData_p;

typedef struct CMthreadTeam_s {
	CMthreadData_p  Threads;
	size_t          ThreadNum;
	pthread_mutex_t Mutex;
	pthread_cond_t  Signal;
	CMthreadJob_p   Job;
} CMthreadTeam_t, *CMthreadTeam_p;

CMthreadTeam_p CMthreadTeamCreate     (size_t threadNum);
void           CMthreadTeamDestroy    (CMthreadTeam_p);
void           CMthreadTeamJobExecute (CMthreadTeam_p, CMthreadJob_p);

#if defined(__cplusplus)
}
#endif

#endif
