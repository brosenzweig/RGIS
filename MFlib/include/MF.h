/******************************************************************************

GHAAS Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2003, University of New Hampshire

MF.h

balazs.fekete@unh.edu

*******************************************************************************/

#ifndef MF_H_INCLUDED
#define MF_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

enum { MFStop     = false,
       MFContinue = true,
       MFState    = false,
       MFFlux     = true,
       MFBoundary = false,
       MFInitial  = true,
       MFUnset    = -2 };

#define MFDefaultMissingFloat -9999.0
#define MFDefaultMissingByte  -99
#define MFDefaultMissingInt   -9999
#define MFPrecision 0.0001
#define MFTolerance 0.001

enum { MFInput = 1, MFRoute = 2, MFOutput = 3, MFByte = 4, MFShort = 5, MFInt = 6, MFFloat = 7, MFDouble = 8 };

enum { MFTimeStepDay    = (0x01 << 0x00),
       MFTimeStepMonth  = (0x01 << 0x01),
       MFTimeStepYear   = (0x01 << 0x02)};

#define MFDateClimatologyStr "XXXX"
#define MFNoUnit " "

enum { MFDateStringLength = 24, MFNameLength = 64 };

void MFSwapHalfWord (void *);
void MFSwapWord     (void *);
void MFSwapLongWord (void *);

typedef struct MFDataStream_s {
	int   Type;
	union {
		FILE  *File;
		int    Int;
		double Float;
		} Handle;
	} MFDataStream_t;

typedef struct MFVarHeader_s {
	short  Swap,	DataType;
	int    ItemNum;
	union {
		int    Int;
		double Float;
		} Missing;
	char   Date [MFDateStringLength];
	} MFVarHeader_t;

enum { MFConst, MFFile, MFPipe, MFhttp };

typedef struct MFVariable_s {
	MFVarHeader_t Header;
	int    ID;
	char   Name [MFNameLength];
	char   Unit [MFNameLength];
	bool   Flux, Initial, Set, Route;
	short  TStep;
	void  *Data;

	void (*Func) (int);
	char  *InPath, *OutPath;
	int    NStep;
	MFDataStream_t *InStream, *OutStream;
	} MFVariable_t;

typedef void (*MFFunction) (int);

MFDataStream_t *MFDataStreamOpen  (const char *,const char *);
int MFDataStreamClose (MFDataStream_t *);
int MFDataStreamRead  (MFVariable_t *);
int MFDataStreamWrite (MFVariable_t *, const char *);

int    MFVarGetID (char *,char *,int,bool,bool);
MFVariable_t *MFVarGetByID   (int) ;
MFVariable_t *MFVarGetByName (const char *) ;
void   MFVarSetInt (int,int,int);
int    MFVarGetInt (int,int,int);
void   MFVarSetFloat (int,int,double);
double MFVarGetFloat (int,int,double);
int    MFVarGetTStep (int);
bool   MFVarTestMissingVal (int, int);
void   MFVarSetMissingVal  (int, int);
char  *MFVarTypeString (int);
int    MFOptionParse (int, char *[]);
const char *MFOptionGet (const char *);
void   MFOptionPrintList ();
void   MFOptionMessage (const char *, const char *, const char *[]);


int    MFModelRun (int,char *[],int,int (*) ());
int    MFModelAddFunction (MFFunction);
float  MFModelGetXCoord    (int);
float  MFModelGetYCoord    (int);
float  MFModelGetLongitude (int);
float  MFModelGetLatitude  (int);
int    MFModelGetDownLink  (int, size_t);
float  MFModelGetArea      (int);
float  MFModelGetLength    (int);
float  MFModelGet_dt       ();
void   MFDefEntering       (char *);
void   MFDefLeaving        (char *);

int    MFVarItemSize (int);
bool   MFVarReadHeader  (MFVarHeader_t *,FILE *);
bool   MFVarWriteHeader (MFVarHeader_t *,FILE *);

typedef struct MFObject_s {
	int   ID;
	short DLinkNum, ULinkNum;
	float XCoord, YCoord, Lon, Lat;
	float Area, Length;
	size_t *DLinks, *ULinks;
	} MFObject_t;

typedef struct MFDomain_s {
	short Swap, Type;
	int ObjNum;
	MFObject_t *Objects;
	} MFDomain_t;

MFDomain_t *MFDomainGet (FILE *);
int  MFDomainWrite (MFDomain_t *,FILE *);
void MFDomainFree  (MFDomain_t *);

bool  MFDateCompare (char *,char *);
char *MFDateGetCurrent ();
int   MFDateGetDayOfYear    ();
int   MFDateGetDayOfMonth   ();
int   MFDateGetMonthLength  ();
int   MFDateGetCurrentDay   ();
int   MFDateGetCurrentMonth ();
int   MFDateGetCurrentYear  ();

float MFModelGetArea (int );

char *MFDateAdvance ();
bool  MFDateSetStart   (char *);
bool  MFDateSetEnd     (char *);
bool  MFDateSetCurrent (char *);
void  MFDateSetTimeStep (int);
int   MFDateGetTimeStep ();
char *MFDateTimeStepString (int);
char *MFDateTimeStepUnit   (int);
int   MFDateTimeStepLength ();
void  MFDateRewind ();

float MFRungeKutta (float,float,float,float (*deltaFunc) (float,float));

#if defined(__cplusplus)
}
#endif

#endif /* MF_H_INCLUDED */
