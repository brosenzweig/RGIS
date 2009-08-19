/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2002, University of New Hampshire

DB.H

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <cm.h>

#define DBNull            0x0L
#define DBSuccess         0
#define DBFault          -1
#define DBSet             1
#define DBClear           0
#define DBNone           -1
#define DBForward         true
#define DBBackward        false
#define DBStringLength    0x20
#define DBDataFileNameLen 0x100

#define DBSystemNameLen   0x20
#define DBLoginNameLen    0x20
#define DBUserNameLen     0x40
#define DBModulNameLen    0x50
#define DBDefaultSymbol   1
#define DBHugeVal         1.797693134862315708e308
#define DBDefaultMissingFloatVal  -9999.00
#define DBDefaultMissingIntVal    -9999
#define DBDefaultMissingByteVal    255
#define DBASCIISeparator           '\t'

#define DBProjectionSpherical      0x00
#define DBProjectionCartesian      0x01

#define DBByteOrderBIG             0x00
#define DBByteOrderLITTLE          0x01

#define DBTypeVector               ((DBInt) 0x0100L)
#define DBTypeVectorPoint          ((DBInt) (DBTypeVector | 0x010L))
#define DBTypeVectorLine           ((DBInt) (DBTypeVector | 0x020L))
#define DBTypeVectorPolygon        ((DBInt) (DBTypeVector | 0x040L))
#define DBTypeGrid                 ((DBInt) 0x0200L)
#define DBTypeGridContinuous       ((DBInt) (DBTypeGrid   | 0x010L))
#define DBTypeGridDiscrete         ((DBInt) (DBTypeGrid   | 0x020L))
#define DBTypeNetwork              ((DBInt) 0x0400L)
#define DBTypeTable                ((DBInt) 0x0800L)

#define DBDataFlagOwner                   0x10000000L
#define DBDataFlagChanged                 0x20000000L
#define DBDataFlagDisplay                 0x40000000L
#define DBDataFlagLinked                  0x80000000L

#define DBDataFlagSelectMode              0x08000000L

#define DBDataFlagUserModeFlags           0x07000000L
#define DBDataFlagUserModeQuery           0x00000000L
#define DBDataFlagUserModeSelect          0x01000000L
#define DBDataFlagUserModeIdentify        0x02000000L
#define DBDataFlagUserModeAdd             0x03000000L
#define DBDataFlagUserModeMove            0x04000000L
#define DBDataFlagUserModeFlip            0x05000000L
#define DBDataFlagUserModeDelete          0x06000000L
#define DBDataFlagUserModeRotate          0x07000000L

#define DBDataFlagHideIdle                0x00800000L

#define DBDataFlagDispModeFlags           0x00700000L

#define DBDataFlagDispModeVecAnnotNone    0x00000000L
#define DBDataFlagDispModeVecAnnotate     0x00100000L
#define DBDataFlagDispModeVecAnnotSelect  0x00200000L

#define DBDataFlagDispModeContShadeSets   0x00700000L

#define DBDataFlagDispModeContStandard    0x00000000L
#define DBDataFlagDispModeContGreyScale   0x00100000L
#define DBDataFlagDispModeContBlueScale   0x00200000L
#define DBDataFlagDispModeContBlueRed     0x00300000L
#define DBDataFlagDispModeContElevation   0x00400000L

#define DBDataFlagDispModeNetColors       0x00300000L
#define DBDataFlagDispModeNetColorUniform 0x00000000L
#define DBDataFlagDispModeNetColorBasin   0x00100000L
#define DBDataFlagDispModeNetColorSymbol  0x00200000L
#define DBDataFlagDispModeNetDirection    0x00400000L

#define DBDataFlagGroupControlMode        0x00080000L

#define DBObjectFlagSortReversed          ((DBInt) (0x01 << 0x00))
#define DBObjectFlagProcessed             ((DBInt) (0x01 << 0x01))
#define DBObjectFlagIdle                  ((DBInt) (0x01 << 0x02))
#define DBObjectFlagSelected              ((DBInt) (0x01 << 0x03))
#define DBObjectFlagLocked                ((DBInt) (0x01 << 0x04))
#define DBObjectFlagChanged               ((DBInt) (0x01 << 0x04))

#define DBDataLISTFlagSmartSort           0x10000000L

char *DBDataTypeString (int);

#define DBVarUnitNoUnitClass              0x00

#define DBNetDirE  ((DBInt) (0x01 << 0x00))
#define DBNetDirSE ((DBInt) (0x01 << 0x01))
#define DBNetDirS  ((DBInt) (0x01 << 0x02))
#define DBNetDirSW ((DBInt) (0x01 << 0x03))
#define DBNetDirW  ((DBInt) (0x01 << 0x04))
#define DBNetDirNW ((DBInt) (0x01 << 0x05))
#define DBNetDirN  ((DBInt) (0x01 << 0x06))
#define DBNetDirNE ((DBInt) (0x01 << 0x07))

#define GHAASSubjAny                    ((char *) NULL)
#define GHAASSubjNetwork                ((char *) "STNetwork")
#define GHAASSubjRunoff                 ((char *) "Runoff")
#define GHAASSubjDischarge              ((char *) "Discharge")
#define GHAASSubjFlowHeight             ((char *) "Flow Height")
#define GHAASSubjFlowWidth              ((char *) "Flow Width")
#define GHAASSubjFlowVelocity           ((char *) "Flow Velocity")
#define GHAASSubjFlowSurfSlope          ((char *) "Surface Slope")
#define GHAASSubjRiverStorage           ((char *) "River Storage")
#define GHAASSubjFloodStorage           ((char *) "Flood Storage")
#define GHAASSubjFloodArea              ((char *) "Flood Area")
#define GHAASSubjOverlandStorage        ((char *) "OLand Storage")
#define GHAASSubjOverlandVelocity       ((char *) "OLand Velocity")
#define GHAASSubjPrecip                 ((char *) "Precipitation")
#define GHAASSubjAirTemp                ((char *) "Air Temperature")
#define GHAASSubjHumidity               ((char *) "Humidity")
#define GHAASSubjVaporPres              ((char *) "Vapor Pressure")
#define GHAASSubjWindSpeed              ((char *) "Wind Speed")
#define GHAASSubjSolarRad               ((char *) "Solar Radiation")
#define GHAASSubjStations               ((char *) "Stations")
#define GHAASSubjReservoirs             ((char *) "Reservoirs")
#define GHAASSubjElevation              ((char *) "Elevation")
#define GHAASSubjPits                   ((char *) "STNPits")

/***********************************************************************************************************/

typedef unsigned char   DBByte;
typedef int             DBInt;
typedef unsigned        DBUnsigned;
typedef short           DBShort;
typedef unsigned short	DBUShort;
typedef double          DBFloat;
typedef float           DBFloat4;
typedef long long       DBAddress;

typedef int (*DBSortProcedure) (const void *,const void *);

DBInt DBByteOrder (void);
DBInt DBByteOrder (DBInt);
void  DBByteOrderSwapHalfWord (void *);
void  DBByteOrderSwapWord     (void *);
void  DBByteOrderSwapLongWord (void *);

class DBVarString
	{
	private:
		DBShort   LengthVAR;
		DBAddress StringPTR;
	public:
		DBVarString () { StringPTR = (DBAddress) NULL; LengthVAR = 0; }
		DBVarString (const char *string) { StringPTR = (DBAddress) NULL; VString (string); }
		~DBVarString () { if (StringPTR != (DBAddress) NULL) free ((char *) NULL + StringPTR);  }
		DBVarString (const DBVarString &string)
			{
			if ((StringPTR = (DBAddress) ((char *) malloc (string.VStrLength () + 1) - (char *) NULL)) == (DBAddress) NULL)
				perror ("Memory Allocation Error in: DBVarString::String ()");
			else { LengthVAR = string.VStrLength (); strcpy ((char *) NULL + StringPTR,string.VString ()); }
			}
		char *VString () const { return ((char *) NULL + StringPTR); }
		DBInt VString (const char *string,DBInt len)
			{
			for (;len > 0;--len) if (string [len - 1] != ' ') break;
			if ((StringPTR = StringPTR == (DBAddress) NULL ? (DBAddress) ((char *) malloc (len + 1) - (char *) NULL) :
								 (DBAddress) ((char *) realloc ((char *) NULL + StringPTR,len + 1) - (char *) NULL)) == (DBAddress) NULL)
				{ perror ("Memory Allocation Error in: DBVarString::String ()"); return (DBFault); }
			strncpy ((char *) NULL + StringPTR,string,len); ((char *) NULL + StringPTR) [len] = '\0'; LengthVAR = len;
			return (DBSuccess);
			}
		DBInt VString (const char *string) { return (VString (string,strlen (string))); }
		DBInt VStrLength () const { return (LengthVAR); }
		void Swap () { DBByteOrderSwapHalfWord (&LengthVAR); }
		int Read  (FILE *,int);
		int Write (FILE *);
	};

class DBCoordinate
	{
	private:
	public:
		DBFloat X;
		DBFloat Y;
		DBCoordinate () { X = 0.0;Y = 0.0; }
		DBCoordinate (DBFloat x,DBFloat y) { X = x;Y = y; }
		DBCoordinate (const DBCoordinate &coord) { X = coord.X;Y = coord.Y; }
		friend DBCoordinate operator+ (DBCoordinate coord0,DBCoordinate coord1)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X + coord1.X; retCoord.Y = coord0.Y + coord1.Y;
			return (retCoord);
			}
		friend DBCoordinate operator+ (DBCoordinate coord0,DBFloat delta)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X + delta; retCoord.Y = coord0.Y + delta;
			return (retCoord);
			}
		friend DBCoordinate operator- (DBCoordinate coord0,DBCoordinate coord1)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X - coord1.X; retCoord.Y = coord0.Y - coord1.Y;
			return (retCoord);
			}
		friend DBCoordinate operator- (DBCoordinate coord0,DBFloat delta)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X - delta; retCoord.Y = coord0.Y - delta;
			return (retCoord);
			}
		friend DBCoordinate operator* (DBCoordinate coord0,DBCoordinate coord1)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X * coord1.X; retCoord.Y = coord0.Y * coord1.Y;
			return (retCoord);
			}
		friend DBCoordinate operator* (DBCoordinate coord0,DBFloat scalar)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X * scalar; retCoord.Y = coord0.Y * scalar;
			return (retCoord);
			}
		friend DBCoordinate operator/ (DBCoordinate coord0,DBCoordinate coord1)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X / coord1.X; retCoord.Y = coord0.Y / coord1.Y;
			return (retCoord);
			}
		friend DBCoordinate operator/ (DBCoordinate coord0,DBFloat scalar)
			{
			DBCoordinate retCoord;
			retCoord.X = coord0.X / scalar; retCoord.Y = coord0.Y / scalar;
			return (retCoord);
			}
		void Swap () { DBByteOrderSwapLongWord (&X); DBByteOrderSwapLongWord (&Y);}
	};

class DBRegion
	{
	private:
	public:
		DBCoordinate UpperRight;
		DBCoordinate LowerLeft;
		void Initialize ()
			{
			UpperRight.X = -DBHugeVal;	UpperRight.Y = -DBHugeVal;
			LowerLeft.X  =  DBHugeVal;	LowerLeft.Y  =  DBHugeVal;
			};
		DBRegion () {	Initialize (); }

		friend DBRegion operator+ (DBRegion region,DBFloat scalar)
			{
			DBRegion retRegion;
			retRegion.LowerLeft.X  = region.LowerLeft.X	+ scalar;
			retRegion.LowerLeft.Y  = region.LowerLeft.Y	+ scalar;
			retRegion.UpperRight.X = region.UpperRight.X + scalar;
			retRegion.UpperRight.Y = region.UpperRight.Y + scalar;
			return (retRegion);
			}

		friend DBRegion operator+ (DBRegion region,DBCoordinate coord)
			{
			DBRegion retRegion;
			retRegion.LowerLeft.X  = region.LowerLeft.X	+ coord.X;
			retRegion.LowerLeft.Y  = region.LowerLeft.Y	+ coord.Y;
			retRegion.UpperRight.X = region.UpperRight.X + coord.X;
			retRegion.UpperRight.Y = region.UpperRight.Y + coord.Y;
			return (retRegion);
			}

		friend DBRegion operator- (DBRegion region,DBFloat scalar)
			{
			DBRegion retRegion;
			retRegion.LowerLeft.X  = region.LowerLeft.X	- scalar;
			retRegion.LowerLeft.Y  = region.LowerLeft.Y	- scalar;
			retRegion.UpperRight.X = region.UpperRight.X - scalar;
			retRegion.UpperRight.Y = region.UpperRight.Y - scalar;
			return (retRegion);
			}

		friend DBRegion operator- (DBRegion region,DBCoordinate coord)
			{
			DBRegion retRegion;
			retRegion.LowerLeft.X  = region.LowerLeft.X	- coord.X;
			retRegion.LowerLeft.Y  = region.LowerLeft.Y	- coord.Y;
			retRegion.UpperRight.X = region.UpperRight.X - coord.X;
			retRegion.UpperRight.Y = region.UpperRight.Y - coord.Y;
			return (retRegion);
			}

		void Expand (DBCoordinate coord)
			{
			if (LowerLeft.X  > coord.X) LowerLeft.X  = coord.X;
			if (LowerLeft.Y  > coord.Y) LowerLeft.Y  = coord.Y;
			if (UpperRight.X < coord.X) UpperRight.X = coord.X;
			if (UpperRight.Y < coord.Y) UpperRight.Y = coord.Y;
			}
		void Expand (DBRegion region) { Expand (region.LowerLeft); Expand (region.UpperRight); }
		void Shrink (DBRegion region)
			{
			Expand (region);
			if (LowerLeft.X  < region.LowerLeft.X)  LowerLeft.X  = region.LowerLeft.X;
			if (LowerLeft.Y  < region.LowerLeft.Y)  LowerLeft.Y  = region.LowerLeft.Y;
			if (UpperRight.X > region.UpperRight.X) UpperRight.X = region.UpperRight.X;
			if (UpperRight.Y > region.UpperRight.Y) UpperRight.Y = region.UpperRight.Y;
			}
		int InRegion (DBCoordinate coord) const
			{
			if (LowerLeft.X   > coord.X) return (false);
			if (LowerLeft.Y   > coord.Y) return (false);
			if (UpperRight.X  < coord.X) return (false);
			if (UpperRight.Y  < coord.Y) return (false);
			return (true);
			}
		int InRegion (DBRegion region) const
			{
			if (LowerLeft.X  > region.UpperRight.X)	return (false);
			if (LowerLeft.Y  > region.UpperRight.Y)	return (false);
			if (UpperRight.X < region.LowerLeft.X) 	return (false);
			if (UpperRight.Y < region.LowerLeft.Y)		return (false);
			return (true);
			}
		void Swap () { UpperRight.Swap (); LowerLeft.Swap (); }
	};

class DBPosition
	{
	private:
	public:
		DBUShort Row;
		DBUShort Col;
		DBPosition () { Row = 0; Col = 0; }
		DBPosition (const DBPosition &pos) { Row = pos.Row; Col = pos.Col; }
		friend DBPosition operator+ (DBPosition pos0,DBPosition pos1)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col + pos1.Col; retPos.Row = pos0.Row + pos1.Row;
			return (retPos);
			}
		friend DBPosition operator+ (DBPosition pos0,DBShort delta)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col + delta; retPos.Row = pos0.Row + delta;
			return (retPos);
			}
		friend DBPosition operator- (DBPosition pos0,DBPosition pos1)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col - pos1.Col; retPos.Row = pos0.Row - pos1.Row;
			return (retPos);
			}
		friend DBPosition operator- (DBPosition pos0,DBShort delta)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col - delta; retPos.Row = pos0.Row - delta;
			return (retPos);
			}
		friend DBPosition operator* (DBPosition pos0,DBPosition pos1)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col * pos1.Col; retPos.Row = pos0.Row * pos1.Row;
			return (retPos);
			}
		friend DBPosition operator* (DBPosition pos0,DBShort scalar)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col * scalar; retPos.Row = pos0.Row * scalar;
			return (retPos);
			}
		friend DBPosition operator/ (DBPosition pos0,DBPosition pos1)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col / pos1.Col; retPos.Row = pos0.Row / pos1.Row;
			return (retPos);
			}
		friend DBPosition operator/ (DBPosition pos0,DBShort scalar)
			{
			DBPosition retPos;
			retPos.Col = pos0.Col / scalar; retPos.Row = pos0.Row / scalar;
			return (retPos);
			}
		void Swap () { DBByteOrderSwapHalfWord (&Row); DBByteOrderSwapHalfWord (&Col);}
	};

#define DBTimeStepMinute	(0x01)
#define DBTimeStepHour		(0x01 << 0x01)
#define DBTimeStepDay		(0x01 << 0x02)
#define DBTimeStepWeek		(0x01 << 0x03)
#define DBTimeStepMonth		(0x01 << 0x04)
#define DBTimeStepYear		(0x01 << 0x05)

class DBDate
	{
	private:
		DBShort  MonthsVAR;
		DBUShort MinutesVAR;
		DBShort  FlagsVAR;
	public:
		DBInt	Flags () const { return (FlagsVAR); }
		void Set (const char *);
		void Set (DBInt year)
			{ Set (year,DBDefaultMissingIntVal); }
		void Set (DBInt year,DBInt month)
			{ Set (year,month,DBDefaultMissingIntVal); }
		void Set (DBInt year,DBInt month,DBInt day)
			{ Set (year,month,day,DBDefaultMissingIntVal); }
		void Set (DBInt year,DBInt month,DBInt day,DBInt hour)
			{ Set (year,month,day,hour,DBDefaultMissingIntVal); }
		void Set (DBInt year,DBInt month,DBInt day,DBInt hour,DBInt min);
		char *Get () const;
		void Get (DBInt *year,DBInt *month)
			{ *year = Year (); *month = Month (); }
		void Get (DBInt *year,DBInt *month,DBInt *day) const
			{ *year = Year (); *month = Month (); *day = Day (); }
		void Get (DBInt *year,DBInt *month,DBInt *day,DBInt *hour) const
			{ *year = Year (); *month = Month (); *day = Day (); *hour = Hour (); }
		void Get (DBInt *year,DBInt *month,DBInt *day,DBInt *hour,DBInt *min) const
			{ *year = Year (); *month = Month (); *day = Day (); *hour = Hour (); *min = Minute ();}
		void Year (DBInt year)
			{
			DBInt month = Month (),day = Day (), hour = Hour (), min = Minute ();
			Set (year,month,day,hour,min);
			}
		DBInt Year () const;
		void Month (DBInt month)
			{
			DBInt year = Year (),day = Day (), hour = Hour (), min = Minute ();
			Set (year,month,day,hour,min);
			}
		DBInt Month () const;
		void Day (DBInt day)
			{
			DBInt year = Year (), month = Month (), hour = Hour (), min = Minute ();
			Set (year,month,day,hour,min);
			}
		DBInt Day () const;
		void Hour (DBInt hour)
			{
			DBInt year = Year (), month = Month (), day = Day (), min = Minute ();
			Set (year,month,day,hour,min);
			}
		DBInt Hour () const;
		void Minute (DBInt min)
			{
			DBInt year = Year (), month = Month (), day = Day (), hour = Hour ();
			Set (year,month,day,hour,min);
			}
		DBInt Minute () const;

		DBInt YearsAD ()	const;
		DBInt MonthsAD () const;
		DBInt DaysAD ()	const;
		DBInt HoursAD ()	const;
		DBInt MinutesAD ()const;
		DBInt Week () const;
		DBFloat DecimalYear  () const;
		DBFloat DecimalMonth () const;
		void Swap ()
			{
			DBByteOrderSwapHalfWord (&MonthsVAR);
			DBByteOrderSwapHalfWord (&MinutesVAR);
			DBByteOrderSwapHalfWord (&FlagsVAR);
			}
		friend DBInt operator- (DBDate date0,DBDate date1)
			{ return (date0.MinutesAD () - date1.MinutesAD ()); }
		friend DBDate operator+ (DBDate,DBDate);
		friend bool operator <  (DBDate,DBDate);
		friend bool operator <= (DBDate,DBDate);
		friend bool operator == (DBDate,DBDate);
		friend bool operator != (DBDate,DBDate);
		friend bool operator >= (DBDate,DBDate);
		friend bool operator >  (DBDate,DBDate);
	};

class DBObservation

	{
	private:
	public:
		DBDate Date;
		DBFloat Value;
	void swap ()
		{
		Date.Swap ();
		DBByteOrderSwapLongWord (&Value);
		}
	DBObservation ()
		{
		Date.Set (DBDefaultMissingIntVal,DBDefaultMissingIntVal,DBDefaultMissingIntVal,DBDefaultMissingIntVal,DBDefaultMissingIntVal);
		Value = DBDefaultMissingFloatVal;
		}
	DBObservation (const DBObservation &obs)
		{ Date = obs.Date; Value = obs.Value; }
	DBObservation (DBDate date,DBFloat value)
		{ Date = date; Value = value; }
	};

class DBObsRange

	{
	private:
	public:
		DBObservation Min;
		DBObservation Max;
		void Initialize ()
			{
			Min.Value = DBHugeVal;	Min.Date.Set ( 2730,1,1);
			Max.Value = -DBHugeVal; Max.Date.Set (-2730,1,1);
			}
		void Expand (DBObservation obs)
			{
			Min.Value = Min.Value < obs.Value ? Min.Value : obs.Value;
			Min.Date = Min.Date.MinutesAD () < obs.Date.MinutesAD () ? Min.Date : obs.Date;
			Max.Value = Max.Value > obs.Value ? Max.Value : obs.Value;
			Max.Date = Max.Date.MinutesAD () > obs.Date.MinutesAD () ? Max.Date : obs.Date;
			}
		void Expand (DBObsRange obsRange)
			{ Expand (obsRange.Min); Expand (obsRange.Max); }
		DBFloat MinValue () const { return (Min.Value); }
		DBFloat MaxValue () const { return (Max.Value); }
		DBDate  Begin ()    const { return (Min.Date); }
		DBDate  End ()      const { return (Max.Date); }
	};

/***********************************************************************************************************/


class DBObjectHeader
	{
	private:
		DBInt RowIdVAR;
		DBInt ListPosVAR;
	public:
		DBObjectHeader () { RowIdVAR = ListPosVAR = DBFault; }
		DBObjectHeader (const DBObjectHeader &objHeader)
			{ RowIdVAR = objHeader.RowIdVAR; ListPosVAR = objHeader.ListPosVAR; }
		DBInt	RowID (void)		const	{ return (RowIdVAR); }
		void	_RowID (DBInt id)			{ RowIdVAR = id; }
		DBInt	ListPos (void)		const	{ return (ListPosVAR); }
		void	ListPos (DBInt listPos)	{ ListPosVAR = listPos; }

	};

class DBObject : public DBObjectHeader
	{
	private:
		DBInt	SizeVAR,		FlagsVAR;
		DBVarString	NameSTR;
		void Swap ()
			{
			DBByteOrderSwapWord (&SizeVAR);
			DBByteOrderSwapWord (&FlagsVAR);
			}
	public:
		DBObject () { NameSTR.VString (""); FlagsVAR = 0x0; SizeVAR = sizeof (DBObject); }
		DBObject (const char *name)
			{ NameSTR.VString (name); FlagsVAR = 0x0; SizeVAR = sizeof (DBObject); }
		DBObject (const char *name,DBUnsigned size)
			{ NameSTR.VString (name); FlagsVAR = 0x0; SizeVAR = size; }
		DBObject (const DBObject &obj) : DBObjectHeader (obj)
			{
			NameSTR.VString (obj.Name ());
			SizeVAR  = obj.SizeVAR;
			FlagsVAR = obj.FlagsVAR;
			}
		char *Name (void)	const		{ return (NameSTR.VString ()); }
		void Name (const char *name)				{ NameSTR.VString  (name); }
		void Name (const char *name,DBInt len) { NameSTR.VString  (name,len); }
		DBUnsigned Size (void)	const	{ return (SizeVAR); }
		DBUnsigned Flags (void) const	{ return (FlagsVAR); }
		void Flags (DBUnsigned flags)	{ FlagsVAR = flags; }
		void Flags (DBUnsigned flags,DBInt set) { FlagsVAR = set ? FlagsVAR | flags : FlagsVAR & (~flags); }
		int Read  (FILE *,int);
		int Write (FILE *);
	};

class DBObjRecord : public DBObject
	{
	private:
		DBUnsigned LengthVAR;
		DBUnsigned ItemSizeVAR;
		DBAddress DataPTR;
		void Swap ()
			{
			DBByteOrderSwapWord (&LengthVAR);
			DBByteOrderSwapWord (&ItemSizeVAR);
			}
	public:
		DBObjRecord () : DBObject ("",sizeof (DBObjRecord))
			{ LengthVAR = 0; ItemSizeVAR = 0; DataPTR = (DBAddress) NULL; }
		DBObjRecord (const char *name,DBUnsigned length,DBInt itemSize) : DBObject (name,sizeof (DBObjRecord))
			{
			DataPTR = length > 0 ? (DBAddress) ((char *) malloc (length) - (char *) NULL) : (DBAddress) NULL;
			LengthVAR = DataPTR == (DBAddress) NULL ? 0 : length;
			ItemSizeVAR = itemSize;
			}
		DBObjRecord (const char *name,DBUnsigned length) : DBObject (name,sizeof (DBObjRecord))
			{
			DataPTR = length > 0 ? (DBAddress) ((char *) malloc (length) - (char *) NULL) : (DBAddress) NULL;
			LengthVAR = DataPTR == (DBAddress) NULL ? 0 : length;
			ItemSizeVAR = 0;
			}
		DBObjRecord (DBObjRecord &record) : DBObject (record)
			{
			LengthVAR = record.LengthVAR;
			ItemSizeVAR = record.ItemSizeVAR;
			DataPTR = (DBAddress) ((char *) malloc (LengthVAR) - (char *) NULL);
			memcpy ((char *) NULL + DataPTR,(char *) NULL + record.DataPTR,record.LengthVAR);
			}
		~DBObjRecord () { if (DataPTR != (DBAddress) NULL) free ((char *) NULL + DataPTR); }
		void *Data () const { return ((char *) NULL + DataPTR); }
		DBInt Length () const { return (LengthVAR); }
		void Realloc (DBInt length)
			{
			if (length > 0)
				{
				DataPTR	 = (DBAddress) ((char *) realloc ((char *) NULL + DataPTR,length) - (char *) NULL);
				LengthVAR = DataPTR == (DBAddress) NULL ? 0 : length;
				}
			else	{ free ((char *) NULL + DataPTR); DataPTR = (DBAddress) NULL; LengthVAR = 0; }
			}
		int Read  (FILE *,int);
		int Write (FILE *);
	};

int _DBObjectLISTNameSort  (const DBObject **,const DBObject **);
int _DBObjectLISTNameReversedSort  (const DBObject **,const DBObject **);
template <class Object>

class DBObjectLIST : public DBObject
	{
	private:
		DBInt NumVAR;
		DBInt CurrentVAR;
		DBInt IndexVAR;
		DBInt DummyVAR;
		DBAddress ItemsPTR;
		DBAddress ListPTR;
		void Swap ()
			{
			DBByteOrderSwapWord (&NumVAR);
			DBByteOrderSwapWord (&CurrentVAR);
			DBByteOrderSwapWord (&IndexVAR);
			}
	public:
		DBObjectLIST (const char *name) : DBObject (name,sizeof (DBObjectLIST))
			{ ItemsPTR = ListPTR = (DBAddress) NULL; NumVAR = CurrentVAR = 0; }
		DBObjectLIST (const char *name,DBUnsigned size) : DBObject (name,size)
			{ ItemsPTR = ListPTR = (DBAddress) NULL; NumVAR = CurrentVAR = 0; }
		DBObjectLIST (const DBObjectLIST &objList) : DBObject (objList)
			{
			DBInt i;
			NumVAR = objList.NumVAR;
			CurrentVAR = objList.CurrentVAR;
			IndexVAR = objList.IndexVAR;
			DummyVAR = objList.DummyVAR;

			ItemsPTR = (DBAddress) (((char *) malloc ((NumVAR + 1) * sizeof (Object *))) - ((char *) NULL));
			ListPTR =  (DBAddress) (((char *) malloc ((NumVAR + 1) * sizeof (Object *))) - ((char *) NULL));

			ListPTR =  (DBAddress) (((char *) malloc ((NumVAR + 1) * sizeof (Object *))) - ((char *) NULL));
			for (i = 0;i < NumVAR;++i)
				{
				((Object **) ((char *) NULL + ItemsPTR)) [i] =
				((Object **) ((char *) NULL + ListPTR))  [i] = new Object (*(((Object **) ((char *) NULL + objList.ItemsPTR)) [i]));
				((Object **) ((char *) NULL + ListPTR))  [i]->ListPos (i);
				}
			}
		~DBObjectLIST () { DeleteAll (); }

		Object *Item ()
			{
			return (CurrentVAR != NumVAR ? ((Object **) ((char *) NULL + ItemsPTR)) [CurrentVAR] : (Object *) NULL); }
		Object *Item (DBInt rowID,DBInt setCurrent)
			{
			if ((rowID < 0) || (rowID > NumVAR - 1)) return ((Object *) NULL);
			if (setCurrent) CurrentVAR = (DBUnsigned) rowID;
			return (((Object **) ((char *) NULL + ItemsPTR)) [rowID]);
			}
		Object *Item (DBInt rowID) { return (Item (rowID,false)); }
		Object *Item (const char *name,DBInt setCurrent)
			{
			DBInt i;
			if (name == (char *) NULL) return ((Object *) NULL);
			for (i = 0;i < NumVAR; ++i) if (strcmp (((Object **) ((char *) NULL + ItemsPTR)) [i]->Name (),name) == 0) break;
			if (i < (DBInt) NumVAR)
				{ if (setCurrent) CurrentVAR = (DBUnsigned) i; return (((Object **) ((char *) NULL + ItemsPTR)) [i]); }

			if (setCurrent) CurrentVAR = NumVAR;
			return ((Object *) NULL);
			}
		Object *Item (const char *name) { return (Item (name,false)); }
		DBInt ItemNum () { return (NumVAR);}
		void Add (Object *object)
			{
			if (ItemsPTR == (DBAddress) NULL)
				{
				ItemsPTR = (DBAddress) ((char *) malloc ((NumVAR + 1) * sizeof (Object *)) - (char *) NULL);
				ListPTR = (DBAddress)  ((char *) malloc ((NumVAR + 1) * sizeof (Object *)) - (char *) NULL);
				}
			else
				{
				ItemsPTR = (DBAddress) ((char *) realloc ((char *) NULL + ItemsPTR,(NumVAR + 1) * sizeof (Object *)) - (char *) NULL);
				ListPTR = (DBAddress)  ((char *) realloc ((char *) NULL + ListPTR ,(NumVAR + 1) * sizeof (Object *)) - (char *) NULL);
				}
			if ((ItemsPTR != (DBAddress) NULL) && (ListPTR != (DBAddress) NULL))
				{
				CurrentVAR = NumVAR;
				((Object **) ((char *) NULL + ItemsPTR)) [CurrentVAR] = ((Object **) ((char *) NULL + ListPTR)) [CurrentVAR] = object;
				((DBObject *) object)->_RowID (NumVAR);
				((DBObject *) object)->ListPos (NumVAR);
				NumVAR = NumVAR + 1;
				}
			else	NumVAR = CurrentVAR = 0;
			Flags (DBObjectFlagIdle,(NumVAR > 0) ? DBClear : DBSet);
			}

		void Remove (Object *object)
			{
			DBInt i, rowID;

			if (object == (Object *) NULL) return;
			rowID = object->RowID ();
			if ((rowID < 0) || (rowID > (DBInt) NumVAR - 1)) return;

			for (i = rowID;i < (DBInt) NumVAR - 1;++i)
				{
				((Object **) ((char *) NULL + ItemsPTR)) [i] = ((Object **) ((char *) NULL + ItemsPTR)) [i + 1];
				((Object **) ((char *) NULL + ItemsPTR)) [i]->_RowID (i);
				}
			for (i = 0;i < (DBInt) NumVAR - 1; ++i)
				if (((Object **) ((char *) NULL + ItemsPTR)) [i]->ListPos () > object->ListPos ())
					{
					((Object **) ((char *) NULL + ItemsPTR)) [i]->ListPos (((Object **) ((char *) NULL + ItemsPTR)) [i]->ListPos () - 1);
					((Object **) ((char *) NULL + ListPTR)) [((Object **) ((char *) NULL + ItemsPTR)) [i]->ListPos ()] = ((Object **) ((char *) NULL + ItemsPTR)) [i];
					}

			if (--NumVAR > 0)
				{
				ItemsPTR = (DBAddress) ((char *) realloc ((char *) NULL + ItemsPTR,NumVAR * sizeof (Object *)) - (char *) NULL);
				ListPTR  = (DBAddress) ((char *) realloc ((char *) NULL +  ListPTR,NumVAR * sizeof (Object *)) - (char *) NULL);
				}
			else
				{
				free ((char *) NULL + ItemsPTR);
				free ((char *) NULL + ListPTR);
				ItemsPTR = ListPTR = (DBAddress) NULL;
				}
			if ((ItemsPTR == (DBAddress) NULL) || (ListPTR == (DBAddress) NULL)) CurrentVAR = NumVAR = 0;
			else CurrentVAR = CurrentVAR < NumVAR ? CurrentVAR : NumVAR - 1;
			}
		void Remove	(const DBInt rowID)		{ Remove (Item (rowID)); }
		void Remove (const char *objName)	{ Remove (Item (objName)); }
		void Remove ()	{ Remove (Item ()); }
		void RemoveAll ()
			{
			if (NumVAR > 0)
				{
				free ((char *) NULL + ItemsPTR);
				free ((char *) NULL + ListPTR);
				}
			ItemsPTR = ListPTR = (DBAddress) NULL;
			CurrentVAR = NumVAR = 0;
			}
		void Delete (Object *object)	{	Remove (object);	delete object;	}
		void Delete	(const DBInt rowID)		{ Delete (Item (rowID)); }
		void Delete (const char *objName)	{ Delete (Item (objName)); }
		void DeleteAll ()
			{
			DBInt i;
			for (i = 0;i < (DBInt) NumVAR;++i) delete ((Object **) ((char *) NULL + ItemsPTR)) [i];
			RemoveAll ();
			}
		Object *First (DBInt *indexVAR,DBInt startVal)
			{
			*indexVAR = startVal < NumVAR ? startVal : NumVAR;
			return (*indexVAR < NumVAR ? ((Object **) ((char *) NULL + ListPTR)) [*indexVAR] : (Object *) NULL);
			}
		Object *First (DBInt *indexVAR)	{ return (First (indexVAR,0)); }
		Object *First ()	{ return (First (&IndexVAR)); }
		Object *First (DBInt startVal)	{ return (First (&IndexVAR,startVal)); }
		Object *Last  ()  { return (First (&IndexVAR,NumVAR - 1)); }
		Object *Last  (DBInt *indexVAR)  { return (First (indexVAR,NumVAR - 1)); }
		Object *Next (DBInt *indexVAR,DBInt dir)
			{
			if (dir)
					{ if ((++(*indexVAR)) < NumVAR) return (((Object **) ((char *) NULL + ListPTR)) [*indexVAR]); }
			else	{ if ((--(*indexVAR)) >= 0)     return (((Object **) ((char *) NULL + ListPTR)) [*indexVAR]); }
			return ((Object *) NULL);
			}
		Object *Next (DBInt *indexVAR) { return (Next (indexVAR,DBForward)); }
		Object *Next () { return (Next (&IndexVAR,DBForward)); }
		Object *Next (DBInt dir) { return (Next (&IndexVAR,dir)); }
		int Read (FILE *file,int swap)
			{
			int i;

			if (DBObject::Read (file,swap) != DBSuccess) return (DBFault);

			if (fread ((char *) this + sizeof (DBObject),sizeof (DBObjectLIST) - sizeof (DBObject) - 2 * sizeof (DBAddress),1,file) != 1)
				{ perror ("File Reading Error in: DBObjectLIST::Read ()"); return (DBFault); }
			if (swap) Swap ();
			if (NumVAR > 0)
				{
				if ((ItemsPTR = (DBAddress) ((char *) malloc (NumVAR * sizeof (Object *)) - (char *) NULL)) == (DBAddress) NULL)
					{ perror ("Memory Allocation Error in:DBObjectLIST<object>::Read ()"); return (DBFault); }
				if ((ListPTR  = (DBAddress) ((char *) malloc (NumVAR * sizeof (Object *)) - (char *) NULL)) == (DBAddress) NULL)
					{ perror ("Memory Allocation Error in:DBObjectLIST<object>::Read ()"); return (DBFault); }
				for (i = 0 ;i < (DBInt) NumVAR;++i)
					{
					((Object **) ((char *) NULL + ItemsPTR)) [i] = ((Object **) ((char *) NULL + ListPTR)) [i] = new Object ();
					((Object **) ((char *) NULL + ItemsPTR)) [i]->_RowID (i);
					((Object **) ((char *) NULL + ItemsPTR)) [i]->ListPos (i);
					}
				}
			else ItemsPTR = ListPTR = (DBAddress) NULL;
			return (DBSuccess);
			}
		int ReadItem (FILE *file,DBInt id, int swap)
			{ return (((Object **) ((char *) NULL + ItemsPTR)) [id]->Read (file,swap)); }
		int Write (FILE *file)
			{
			if (DBObject::Write (file) != DBSuccess) return (DBFault);

			if (fwrite ((char *) this + sizeof (DBObject),sizeof (DBObjectLIST) - sizeof (DBObject) - 2 * sizeof (DBAddress),1,file) != 1)
				{ perror ("File Writing Error in: DBObjectLIST::Write ()"); return (DBFault); }
			return (DBSuccess);
			}
		int WriteItem (FILE *file,DBInt id)
			{ return (((Object **) ((char *) NULL + ItemsPTR)) [id]->Write (file)); }
		void ItemSort ()
			{
			DBInt i;
			for (i = 0 ;i < (DBInt) NumVAR;++i)
				((Object **) ((char *) NULL + ListPTR)) [(((Object **) ((char *) NULL + ItemsPTR)) [i])->ListPos ()] = ((Object **) ((char *) NULL + ItemsPTR)) [i];
			for (i = 0 ;i < (DBInt) NumVAR;++i)
				{
				((Object **)  ((char *) NULL + ItemsPTR)) [i] = ((Object **) ((char *) NULL + ListPTR)) [i];
				(((Object **) ((char *) NULL + ItemsPTR)) [i])->_RowID (i);
				(((Object **) ((char *) NULL + ItemsPTR)) [i])->ListPos (i);
				}
			}
		void ItemSort (int (*compFunc) (const Object **,const Object **))
			{ ListSort (compFunc); ItemSort (); }
		void ListSort (int (*compFunc) (const Object **,const Object **))
			{
			DBInt i;

			qsort ((Object **) ((char *) NULL + ListPTR),NumVAR,sizeof (Object *),(int (*)(const void *, const void *)) compFunc);
			for (i = 0;i < NumVAR;++i) (((Object **) ((char *) NULL + ListPTR)) [i])->ListPos (i);
			}
		void ListSort (DBInt reversed)
			{
			if (reversed)
					ListSort ((int (*) (const Object **,const Object **)) _DBObjectLISTNameReversedSort);
			else	ListSort ((int (*) (const Object **,const Object **)) _DBObjectLISTNameSort);
			}
		void ListSort () { ListSort (false); }
		void ListReset ()
			{
			DBInt i;
			for (i = 0 ;i < (DBInt) NumVAR;++i)
				{
				((Object **) ((char *) NULL + ListPTR)) [i] = ((Object **) ((char *) NULL + ItemsPTR)) [i];
				(((Object **)((char *) NULL + ListPTR)) [i])->ListPos (i);
				}
			}
	};

#define DBVariableString		0x01
#define DBVariableInt			0x02
#define DBVariableFloat			0x03
#define DBVariableDate			0x04

#define DBTableFieldString		DBVariableString
#define DBTableFieldInt			DBVariableInt
#define DBTableFieldFloat		DBVariableFloat
#define DBTableFieldDate		DBVariableDate
#define DBTableFieldCoord		0x05
#define DBTableFieldRegion		0x06
#define DBTableFieldPosition	0x07
#define DBTableFieldTableRec	0x08
#define DBTableFieldDataRec	0x09

template <class Numeric>
class DBProperties
	{
	private:
		Numeric NoDataVAR;
		Numeric MinVAR;
		Numeric MaxVAR;
	public:
		void NoData (Numeric VAR) { NoDataVAR = VAR; }
		Numeric NoData ()  const { return (NoDataVAR); }
		void Minimum (Numeric VAR) { MinVAR = VAR; }
		Numeric Minimum () const { return (MinVAR); }
		void Maximum (Numeric VAR) { MaxVAR = VAR; }
		Numeric Maximum () const { return (MaxVAR); }
		void Swap ()
			{
			switch (sizeof (Numeric))
				{
				case 2:
					DBByteOrderSwapHalfWord (&NoDataVAR);
					DBByteOrderSwapHalfWord (&MinVAR);
					DBByteOrderSwapHalfWord (&MaxVAR);
					break;
				case 4:
					DBByteOrderSwapWord (&NoDataVAR);
					DBByteOrderSwapWord (&MinVAR);
					DBByteOrderSwapWord (&MaxVAR);
					break;
				case 8:
					DBByteOrderSwapLongWord (&NoDataVAR);
					DBByteOrderSwapLongWord (&MinVAR);
					DBByteOrderSwapLongWord (&MaxVAR);
					break;
				case sizeof (DBDate):
					((DBDate *) (&NoDataVAR))->Swap ();
					((DBDate *) (&MinVAR))->Swap ();
					((DBDate *) (&MaxVAR))->Swap ();
				default:	break;
				}
			}
	};

#define DBHiddenField ((char *) "")

class DBTableFieldDefinition
	{
	private:
		char *NameSTR;
		DBInt TypeVAR;
		char  *FormatSTR;
		DBUnsigned LengthVAR;
		DBByte RequiredVAR;
	public:
		DBTableFieldDefinition (){ NameSTR = (char *) NULL; TypeVAR = DBFault; FormatSTR = (char *) "%8x"; LengthVAR = 0; RequiredVAR = false;}
		DBTableFieldDefinition (char *name,DBInt type,char *format, DBUnsigned length)
			{ NameSTR = name; TypeVAR = type; FormatSTR = format; LengthVAR = length; RequiredVAR = false;}
		DBTableFieldDefinition (char *name,DBInt type,char *format, DBUnsigned length,DBInt required)
			{ NameSTR = name; TypeVAR = type; FormatSTR = format; LengthVAR = length; RequiredVAR = required; }
		char *Name () { return (NameSTR); }
		DBInt Type () const { return (TypeVAR); }
		const char *Format () const { return (FormatSTR); }
		DBUnsigned Length () const { return (LengthVAR); }
		DBInt Required () const { return (RequiredVAR); }
	};

class DBObjTableField : public DBObject
	{
	private:
		DBByte	TypeVAR;
		DBByte	UnitTypeVAR;
		DBByte	UnitVAR;
		DBByte	RequiredVAR;

		DBShort	StartByteVAR;
		DBUShort	LengthVAR;
		char FormatSTR [DBStringLength];
		union	{
				DBProperties<DBByte>		BytePROP;
				DBProperties<DBShort>	ShortPROP;
				DBProperties<DBInt>		IntPROP;
				DBProperties<DBFloat4>	Float4PROP;
				DBProperties<DBFloat>	FloatPROP;
				DBProperties<DBDate>		DatePROP;
				DBInt							RecordPROP;
				};
		void Initialize (DBInt,const char *, DBUnsigned,DBInt);
		void Swap ();
	public:
		DBObjTableField () : DBObject ("",sizeof (DBObjTableField))
			{ Initialize (DBFault,DBHiddenField,1,false); Type (DBTableFieldString); }
		DBObjTableField (const char *name,DBInt size) : DBObject (name,size)
			{ Initialize (DBFault,DBHiddenField,1,false); Type (DBTableFieldString); }
		DBObjTableField (const char *name,DBInt type,const char *format, DBUnsigned length)
		: DBObject (name,sizeof (DBObjTableField)) { Initialize (type,format, length,false); }
		DBObjTableField (const char *name,DBInt type,const char *format, DBUnsigned length,DBInt required)
		: DBObject (name,sizeof (DBObjTableField)) { Initialize (type,format, length,required); }
		DBObjTableField (DBObjTableField &);
		~DBObjTableField () { }

		void Format (const char *format) { strncpy (FormatSTR,format,sizeof (FormatSTR) - 1); }
		const char *Format () const { return (FormatSTR); }
		void  StartByte (DBInt startByte) { StartByteVAR = startByte; }
		DBInt StartByte () const { return (StartByteVAR); }
		DBInt FormatWidth () const;
		void  FormatWidth (DBInt width);
		DBInt FormatDecimals () const;
		void  FormatDecimals (DBInt);
		DBInt Type ()		 const { return (TypeVAR); }
		void Type (DBInt);
		DBInt Length ()	 const { return (LengthVAR); }
		void Length (DBInt);
		DBInt Required ()	 const { return (RequiredVAR); }
		void Required (DBInt required) { RequiredVAR = required; }

		void	RecordProp (DBInt prop) { RecordPROP = prop; }
		DBInt RecordProp ()				{ return (RecordPROP); }

		void				String	 	(DBObjRecord *, const char *);
		char 				*String 		(const DBObjRecord *) const;
		void  			Int 			(DBObjRecord *, DBInt );
		DBInt 			Int 			(const DBObjRecord *) const;
		DBInt				IntNoData	(void) const
			{
			switch (LengthVAR)
				{
				case sizeof (DBByte):	return (BytePROP.NoData ());
				case sizeof (DBShort):	return (ShortPROP.NoData ());
				default:						return (IntPROP.NoData ());
				}
			}
		void				IntNoData	(DBInt noData)
			{
			switch (LengthVAR)
				{
				case sizeof (DBByte):	BytePROP.NoData  ((DBByte) noData);	break;
				case sizeof (DBShort):	ShortPROP.NoData ((DBShort)noData);	break;
				default: 					IntPROP.NoData (noData);				break;
				}
			}
		void				Float 		(DBObjRecord *, DBFloat);
		DBFloat			Float 		(const DBObjRecord *) const;
		DBFloat			FloatNoData	(void) const
			{ switch (LengthVAR) { case sizeof (DBFloat4): return ((DBFloat) Float4PROP.NoData ()); default: return (FloatPROP.NoData ()); } }
		void			FloatNoData	(DBFloat noData)
			{ switch (LengthVAR) { case sizeof (DBFloat4): Float4PROP.NoData ((DBFloat4) noData); break; default: FloatPROP.NoData (noData); break; } }
		void				Date  		(DBObjRecord *, DBDate);
		DBDate			Date 			(const DBObjRecord *) const;
		DBDate			DateNoData	(void) const { DBDate date; date.Set (""); return (date); }
		void				Record 		(DBObjRecord *, DBObjRecord *);
		DBObjRecord		*Record		(const DBObjRecord *) const;
		void				Coordinate 	(DBObjRecord *, DBCoordinate);
		DBCoordinate	Coordinate	(const DBObjRecord *) const;
		void				Region		(DBObjRecord *, DBRegion);
		DBRegion			Region		(const DBObjRecord *) const;
		void				Position		(DBObjRecord *, DBPosition);
		DBPosition		Position		(const DBObjRecord *) const;
		int Read	 (FILE *,int);
		int Write (FILE *);
	};

class DBObjTable : public DBObjectLIST<DBObjRecord>
	{
	private:
		DBInt RecordLengthVAR;
		DBObjectLIST<DBObjTableField> *FieldPTR;
		DBObjectLIST<DBObjRecord> *MethodPTR;
		const char *RecordName (DBInt id)
			{ static char string [DBStringLength]; sprintf (string,"Record:%d",id + 1); return (string); }
	public:
		DBObjTable () : DBObjectLIST<DBObjRecord> ("",sizeof (DBObjTable))
			{
			FieldPTR = new DBObjectLIST<DBObjTableField> ("Table Fields",sizeof (DBObjectLIST<DBObjTableField>));
			MethodPTR = new DBObjectLIST<DBObjRecord> ("Method List",sizeof (DBObjectLIST<DBObjRecord>));
			RecordLengthVAR = 0;
			};
		DBObjTable (const char *name) : DBObjectLIST<DBObjRecord> (name,sizeof (DBObjTable))
			{
			FieldPTR = new DBObjectLIST<DBObjTableField> ("Table Fields",sizeof (DBObjectLIST<DBObjTableField>));
			MethodPTR = new DBObjectLIST<DBObjRecord> ("Method List",sizeof (DBObjectLIST<DBObjRecord>));
			RecordLengthVAR = 0;
			};
		DBObjTable (const char *name,DBTableFieldDefinition *fieldDefs) : DBObjectLIST<DBObjRecord> (name,sizeof (DBObjTable))
			{
			DBInt i = 0;
			FieldPTR = new DBObjectLIST<DBObjTableField> ("Table Fields",sizeof (DBObjectLIST<DBObjTableField>));
			MethodPTR = new DBObjectLIST<DBObjRecord> ("Method List",sizeof (DBObjectLIST<DBObjRecord>));
			RecordLengthVAR = 0;
			while (fieldDefs [i++].Name () != NULL)
				AddField (new DBObjTableField (fieldDefs [i - 1].Name (),fieldDefs [i - 1].Type (),fieldDefs [i - 1].Format (),fieldDefs [i - 1].Length (),fieldDefs [i - 1].Required ()));
			};
		DBObjTable (const char *name,DBUnsigned size) : DBObjectLIST<DBObjRecord> (name,size)
			{ FieldPTR = new DBObjectLIST<DBObjTableField> ("Table Fields",sizeof (DBObjectLIST<DBObjTableField>)); RecordLengthVAR = 0; };
		DBObjTable (DBObjTable &);
		~DBObjTable () { DeleteAll (); delete FieldPTR; delete MethodPTR; }
		void AddField (DBObjTableField *);
		void RedefineField (DBObjTableField *,DBObjTableField *);
		void DeleteField (DBObjTableField *);
		void DeleteAllFields ();
		DBObjRecord *Add (DBObjRecord *newRec) { DBObjectLIST<DBObjRecord>::Add (newRec); return (newRec);}
		DBObjRecord *Add ()
			{
			DBObjTableField *field;
			DBObjRecord *newRec = new DBObjRecord (RecordName (ItemNum ()),RecordLengthVAR);
			DBObjectLIST<DBObjRecord>::Add (newRec);
			for (field = FieldPTR->First ();field != (DBObjTableField *) NULL;field = FieldPTR->Next ())
				switch (field->Type ())
					{
					case DBTableFieldInt: 	field->Int   (newRec,field->IntNoData ());	break;
					case DBTableFieldFloat:	field->Float (newRec,field->FloatNoData ());	break;
					}

			return (newRec);
			}
		DBObjRecord *Add (const char *name)
			{
			DBObjTableField *field;
			DBObjRecord *newRec = new DBObjRecord (name,RecordLength ());
			DBObjectLIST<DBObjRecord>::Add (newRec);
			for (field = FieldPTR->First ();field != (DBObjTableField *) NULL;field = FieldPTR->Next ())
				switch (field->Type ())
					{
					case DBTableFieldInt: 	field->Int   (newRec,field->IntNoData ());	break;
					case DBTableFieldFloat:	field->Float (newRec,field->FloatNoData ());	break;
					}
			return (newRec);
			}
		DBInt FieldNum () const { return (FieldPTR->ItemNum ()); }
		DBObjTableField *Field () { return (FieldPTR->Item ()); }
		DBObjTableField *Field (const DBInt fieldID)	  { return  (FieldPTR->Item (fieldID)); }
		DBObjTableField *Field (const char *fieldName) { return (FieldPTR->Item (fieldName)); }
		DBObjectLIST<DBObjTableField> *Fields () const { return (FieldPTR); }
		DBInt RecordLength () const { return (RecordLengthVAR); }
		void ListSort (int (*compFunc) (const DBObjRecord **,const DBObjRecord **))
			{ DBObjectLIST<DBObjRecord>::ListSort (compFunc); }
		void ListSort (DBObjectLIST<DBObjTableField> *);
		void ListSort (DBObjTableField *field)
			{
			DBObjectLIST<DBObjTableField> *fields = new DBObjectLIST<DBObjTableField> ("Field List");
			fields->Add (new DBObjTableField (*field));
			ListSort (fields);
			delete fields;
			}
		void ListSort () { DBObjectLIST<DBObjRecord>::ListSort (); }
		void ListSort (DBInt reversed) { DBObjectLIST<DBObjRecord>::ListSort (reversed); }
		int Read	 (FILE *,int);
		int Write (FILE *);
	};

DBInt DBTableFieldIsString	(const DBObject *);
DBInt DBTableFieldIsInteger(const DBObject *);
DBInt DBTableFieldIsFloat	(const DBObject *);
DBInt DBTableFieldIsCategory(const DBObject *);
DBInt DBTableFieldIsSimple (const DBObject *);
DBInt DBTableFieldIsNumeric(const DBObject *);
DBInt DBTableFieldIsAxisVal(const DBObject *);
DBInt DBTableFieldIsDate 	(const DBObject *);
DBInt DBTableFieldIsVisible(const DBObject *);
DBInt DBTableFieldIsOptional(const DBObject *);
DBInt DBTableFieldMatch (const DBObjTableField *,const DBObjRecord *,const DBObjTableField *,const DBObjRecord *);

/***********************************************************************************************************/

class DBDataHeader

	{
	private:
		DBShort 		ByteOrderVAR;
		DBShort 		TypeVAR;
		DBShort		MajorVAR;
		DBShort		MinorVAR;
		DBShort		ProjectionVAR;
		DBShort		PrecisionVAR;
		DBShort		SpatialUnitVAR;
		DBShort		ValueUnitVAR;
		DBShort		MinScaleVAR;
		DBShort		MaxScaleVAR;
		DBInt			DummyVAR;
		DBRegion 	ExtentVAR;
		DBDate		LastModVAR;
		void Swap ();
	public:
		DBInt Type () const	{ return (TypeVAR); }
		void  Type (DBInt type)	{ TypeVAR = type; }
		DBInt Projection () const { return ((DBInt) ProjectionVAR); }
		void  Projection (DBInt projection) { ProjectionVAR = (DBShort) projection; }
		DBInt Precision () const { return ((DBInt) PrecisionVAR); }
		void  Precision (DBInt value) { PrecisionVAR = (DBShort) value; }
		void  Precision (DBFloat value) { DBFloat prec; prec = rint ((double) log10 (value)); Precision ((DBInt) prec); }
		DBInt SpatialUnit () const { return ((DBInt) SpatialUnitVAR); }
		void  SpatialUnit (DBInt value) { SpatialUnitVAR = (DBShort) value; }
		DBInt ValueUnit () const { return ((DBInt) ValueUnitVAR); }
		void  ValueUnit (DBInt value) { ValueUnitVAR = (DBShort) value; }
		DBInt MaxScale () const { return ((DBInt) MaxScaleVAR); }
		void  MaxScale (DBInt value) { MaxScaleVAR = (DBShort) value; }
		DBInt MinScale () const { return ((DBInt) MinScaleVAR); }
		void  MinScale (DBInt value) { MinScaleVAR = (DBShort) value; }
		DBRegion Extent () const { return (ExtentVAR); }
		void Extent (DBRegion extent) { ExtentVAR = extent; }
		DBDate LastModification ();
		DBDataHeader ();
		DBDataHeader (const DBDataHeader &);
		int Read  (FILE *file);
		int Write (FILE *file);
	};

#define DBrNCoord               ((char *) "Coord")
#define DBrNVertexes            ((char *) "Vertexes")
#define DBrNVertexNum           ((char *) "VertexNum")
#define DBrNRegion              ((char *) "Region")
#define DBrNSymbol              ((char *) "SymbolFLD")
#define DBrNSymbolID            ((char *) "SymbolID")
#define DBrNForeground          ((char *) "Foreground")
#define DBrNBackground          ((char *) "Background")
#define DBrNStyle               ((char *) "Style")
#define DBrNLinkNum             ((char *) "LinkNum")
#define DBrNLeftPoly            ((char *) "LeftPolygon")
#define DBrNRightPoly           ((char *) "RightPolygon")
#define DBrNNextLine            ((char *) "NextLine")
#define DBrNPrevLine            ((char *) "PrevLine")
#define DBrNFromNode            ((char *) "FromNode")
#define DBrNToNode              ((char *) "ToNode")
#define DBrNPerimeter           ((char *) "Perimeter")
#define DBrNFirstLine           ((char *) "FirstLine")
#define DBrNLineNum             ((char *) "LineNum")
#define DBrNArea                ((char *) "Area")

#define DBrNMouthPos            ((char *) "MouthPos")
#define DBrNColor               ((char *) "Color")
#define DBrNBasinOrder          ((char *) "BasinOrder")
#define DBrNBasinArea           ((char *) "BasinArea")
#define DBrNBasinLength         ((char *) "BasinLength")
#define DBrNLookupGrid          ((char *) "NetLookupGrid")

#define DBrNPosition            ((char *) "Position")
#define DBrNToCell              ((char *) "ToCell")
#define DBrNFromCell            ((char *) "FromCell")
#define DBrNOrder               ((char *) "Order")
#define DBrNMagnitude           ((char *) "Magnitude")
#define DBrNBasin               ((char *) "BasinID")
#define DBrNBasinCells          ((char *) "BasinCells")
#define DBrNTravel              ((char *) "Travel")
#define DBrNUpCellPos           ((char *) "MostUpCellPos")
#define DBrNCellArea            ((char *) "CellArea")
#define DBrNCellLength          ((char *) "CellLength")
#define DBrNSubbasinArea        ((char *) "SubbasinArea")
#define DBrNSubbasinLength      ((char *) "SubbasinLength")
#define DBrNDistToMouth         ((char *) "DistToMouth")
#define DBrNDistToOcean         ((char *) "DistToOcean")

#define DBrNRowNum              ((char *) "RowNum")
#define DBrNColNum              ((char *) "ColNum")
#define DBrNCellWidth           ((char *) "CellWidth")
#define DBrNCellHeight          ((char *) "CellHeight")
#define DBrNValueType           ((char *) "ValueType")
#define DBrNValueSize           ((char *) "ValueSize")
#define DBrNLayer               ((char *) "Layer")
#define DBrNMinimum             ((char *) "Minimum")
#define DBrNMaximum             ((char *) "Maximum")
#define DBrNAverage             ((char *) "Average")
#define DBrNStdDev              ((char *) "StdDev")
#define DBrNMissingValue        ((char *) "MissingValue")
#define DBrNGridValue			((char *) "GridValue")
#define DBrNGridArea            ((char *) "GridArea")
#define DBrNGridPercent         ((char *) "GridPercent")

#define DBrNItems               ((char *) "DBItems")
#define DBrNSelection           ((char *) "DBSelection")

#define DBrNNodes               ((char *) "DBNodes")
#define DBrNLineList            ((char *) "DBLineList")
#define DBrNContours            ((char *) "DBContours")
#define DBrNContourList			((char *) "DBContourList")
#define DBrNSymbols             ((char *) "DBSymbols")
#define DBrNCells               ((char *) "DBCells")
#define DBrNLayers              ((char *) "DBLayers")
#define DBrNGroups              ((char *) "DBGroups")

#define DBrNRelateData			((char *) "DBRelateData")
#define DBrNRelateField         ((char *) "DBRelateField")
#define DBrNRelateJoinField     ((char *) "DBJoinField")
#define DBrNRelations           ((char *) "DBRelations")

#define DBrNCategoryID          ((char *) "CategoryID")
#define DBrNCategory            ((char *) "Category")
#define DBrNPercent             ((char *) "Percent")

#define DBDocSubject            ((char *) "Subject")
#define DBDocGeoDomain 			((char *) "GeoDomain")
#define DBDocVersion 			((char *) "Version")
#define DBDocCitationRef 		((char *) "Citation Reference")
#define DBDocCitationInst 		((char *) "Citation Institue")
#define DBDocSourceInst 		((char *) "Source Institue")
#define DBDocSourcePerson	 	((char *) "Source Person")
#define DBDocOwnerPerson 		((char *) "Contact Person")
#define DBDocComment            ((char *) "Comment")

#define DBDataNameLen			0x040
#define DBDataVersionLen		0x008
#define DBDataGeoDomainLen		0x020
#define DBDataSubjectLen		0x020
#define DBDataCitationRefLen	0x100
#define DBDataCitationInstLen	0x040
#define DBDataSourceInstLen	0x040
#define DBDataSourcePersonLen	0x020
#define DBDataCommentLen		0x100

class DBObjData : public DBObject,public DBDataHeader
	{
	private:
		char FileNameSTR [DBDataFileNameLen];
		DBObjData						*LinkedDataPTR;
		DBObjectLIST<DBObjTable>	*TablesPTR;
		DBObjectLIST<DBObjRecord>	*ArraysPTR;
		DBObjectLIST<DBObjRecord> 	*DocsPTR;
		DBObjectLIST<DBObject>		*DispPTR;
		void Initialize ();
		int BuildFields ();
		DBDataHeader _Header () const { return (*((DBDataHeader *) (((char *) this) + sizeof (DBObject)))); }
		int _Read	 (FILE *file,int swap);
		int _Write (FILE *);
	public:
		DBObjData () : DBObject ("Noname Data",sizeof (DBObjData))		{ Initialize (); }
		DBObjData (const char *name,DBInt type) : DBObject (name,sizeof (DBObjData)) { Initialize (); Type (type); }
		DBObjData (DBObjData &);
		~DBObjData () { delete TablesPTR; delete ArraysPTR; /* delete DocsPTR; */ }
		void Type (DBInt type);
		DBInt Type () const	{ return (DBDataHeader::Type ()); }
		void FileName (const char *fileName) { strncpy (FileNameSTR,fileName,sizeof (FileNameSTR) - 1); }
		char *FileName () { return (FileNameSTR); }
		void Document (const char *,const char *);
		char *Document (const char *);
		DBRegion Extent (DBObjRecord *);
		DBRegion Extent () const { return (DBDataHeader::Extent ()); }
		void Extent (DBRegion extent) { DBDataHeader::Extent (extent); }
		void RecalcExtent ();
		DBInt SelectObject (DBObjRecord *,DBInt);
		void LinkedData (DBObjData *data) { LinkedDataPTR = data; }
		DBObjData *LinkedData () { return (LinkedDataPTR); }
		DBObjTable  *Table (const char *objName) { return (TablesPTR->Item (objName)); }
		DBObjectLIST<DBObjTable>	*Tables ()		{ return (TablesPTR); }
		DBObjectLIST<DBObjRecord>	*Arrays  ()		{ return (ArraysPTR); }
		DBObjectLIST<DBObject>		*Displays  ()	{ return (DispPTR); }
		DBObject *Display (char *name) { return (DispPTR->Item (name)); }
		void DispAdd (DBObject *dispObj) { DispPTR->Add (dispObj); }
		void DispRemove (DBObject *dispObj) { DispPTR->Remove (dispObj); }
		int Read	 (const char *fileName);
		int Read  (FILE *file);
		int Write (const char *fileName);
		int Write (FILE *);
	};

class DBObjMetaEntry : public DBObject

	{
	private:
		DBInt TypeVAR;
		DBInt DummyVAR;
		char FileNameSTR	[DBDataFileNameLen];
		char SubjectSTR 	[DBStringLength];
		char GeoDomainSTR [DBStringLength];
		void Initialize ()
			{
			TypeVAR			= DBFault;
			strcpy (FileNameSTR,	"");
			strcpy (SubjectSTR,	"");
			strcpy (GeoDomainSTR,"");
			}
		void Swap ()
			{
 			DBByteOrderSwapWord (&TypeVAR);
 			DBByteOrderSwapWord (&DummyVAR);
 			}
	public:
		DBObjMetaEntry () : DBObject ("",sizeof (DBObjMetaEntry)){ Initialize (); }
		DBObjMetaEntry (DBObjData *data) : DBObject (data->Name (),sizeof (DBObjMetaEntry))
			{
			Type (data->Type ());
			strcpy (FileNameSTR,data->FileName ());
			strcpy (SubjectSTR,data->Document (DBDocSubject));
			strcpy (GeoDomainSTR,data->Document (DBDocGeoDomain));
			}
		DBInt Type () const { return (TypeVAR); }
		void Type (DBInt type) { TypeVAR = type; }
		char *FileName () { return (FileNameSTR); }
		void FileName (const char *fileName) { strncpy (FileNameSTR,fileName,sizeof (FileNameSTR) - 1); }
		char *Subject () { return (SubjectSTR); }
		void Subject (const char *subject) { strncpy (SubjectSTR,subject,DBStringLength - 1); }
		char *GeoDomain () { return (GeoDomainSTR); }
		void GeoDomain (const char *geoDomain) { strncpy (GeoDomainSTR,geoDomain,DBStringLength - 1); }
		int Read (FILE *,int);
		int Write(FILE *);
	};

class DBObjSubject : public DBObject

	{
	private:
		DBInt UnitCodeVAR;
		DBInt OccuranceVAR;
		void Initialize () { UnitCodeVAR = DBFault; OccuranceVAR = 0; }
		void Swap ()
			{
			DBByteOrderSwapWord (&UnitCodeVAR);
			DBByteOrderSwapWord (&OccuranceVAR);
			};
	public:
		DBObjSubject () : DBObject ("",sizeof (DBObjSubject)){ Initialize (); }
		DBObjSubject (const char *name) : DBObject (name,sizeof (DBObjSubject)) { Initialize (); }
		DBInt Occurance () { return (OccuranceVAR); }
		void OccuranceInc () { OccuranceVAR++; }
		void OccuranceDec () { OccuranceVAR--; }
		int Read  (FILE *,int);
		int Write (FILE *);
	};

class DBObjGeoDomain : public DBObject

	{
	private:
		DBRegion Region;
		DBInt OccuranceVAR;
		DBInt DummyVAR;
		void Initialize () { OccuranceVAR = 0; }
		void Swap ()
			{
			Region.Swap ();
			DBByteOrderSwapWord (&OccuranceVAR);
			DBByteOrderSwapWord (&DummyVAR);
			};
	public:
		DBObjGeoDomain () : DBObject ("",sizeof (DBObjGeoDomain)) { Initialize (); }
		DBObjGeoDomain (const char *name) : DBObject (name,sizeof (DBObjGeoDomain)) { Initialize (); }
		DBInt Occurance () { return (OccuranceVAR); }
		void OccuranceInc () { OccuranceVAR++; }
		void OccuranceDec () { OccuranceVAR--; }
		int Read  (FILE *,int);
		int Write (FILE *);
	};

class DBObjProject : public DBObject

	{
	private:
		char ModulNameSTR [DBStringLength];
		char FileNameSTR	[DBDataFileNameLen];
	public:
		DBObjProject () { strcpy (ModulNameSTR,""); strcpy (FileNameSTR,""); }
		DBObjProject (const char *modulName,const char *fileName)
			{	ModulName (modulName);	FileName  (fileName); }
		char *ModulName () { return (ModulNameSTR); }
		void ModulName (const char *modulName) { strncpy (ModulNameSTR,modulName,sizeof (ModulNameSTR) - 1); }
		char *FileName () { return (FileNameSTR); }
		void FileName (const char *fileName)	{ strncpy (FileNameSTR,fileName,sizeof (FileNameSTR) - 1); }
		int Read  (FILE *,int);
		int Write (FILE *);
	};

class DBDataset
	{
	private:
		char	MetaFileNameSTR [DBDataFileNameLen];
		DBRegion	ExtentVAR;
		DBObjectLIST<DBObjMetaEntry>	*MetaLIST;
		DBObjectLIST<DBObjSubject>		*SubjectLIST;
		DBObjectLIST<DBObjGeoDomain>	*GeoDomainLIST;
		DBObjectLIST<DBObjData> 		*DataLIST;
		int Read (const char *);
		int Write(const char *);
	public:
		DBDataset (char *, char *);
		~DBDataset ();

		void Data (DBObjData *data);
		DBObjData *Data () { return ((DBObjData *) DataLIST->Item ());}
		DBObjData *Data (DBInt rowID) { return ((DBObjData *) DataLIST->Item (rowID));}
		DBObjData *Data (DBInt rowID,DBInt setCurrent) { return ((DBObjData *) DataLIST->Item (rowID,setCurrent));}
		DBObjData *Data (const char *dataName) { return ((DBObjData *) DataLIST->Item (dataName));}
		DBObjData *Data (const char *dataName,DBInt setCurrent) { return ((DBObjData *) DataLIST->Item (dataName,setCurrent));}
		DBObjData *FirstData ()	{ return (DataLIST->First ()); }
		DBObjData *NextData ()	{ return (DataLIST->Next ()); }
		void RemoveData 	(DBObjData *);
		void DeleteData 	(DBObjData *data) { RemoveData (data); delete data; }
		void Meta			(DBObjData *);
		DBObjMetaEntry *Meta (const char *dataName) { return ((DBObjMetaEntry *) MetaLIST->Item (dataName)); }
		void RemoveMetaEntry (DBObjMetaEntry *);
		void DeleteMetaEntry (DBObjMetaEntry *metaEntry)	{ RemoveMetaEntry (metaEntry); delete metaEntry;}
		DBObjectLIST<DBObjMetaEntry>	*MetaList ()		{ return (MetaLIST); }
		DBObjectLIST<DBObjSubject>		*SubjectList ()	{ return (SubjectLIST); }
		DBObjectLIST<DBObjGeoDomain>	*GeoDomainList (){ return (GeoDomainLIST); }
		DBObjectLIST<DBObjData>			*DataList ()		{ return (DataLIST); }
		DBRegion Extent () { return (ExtentVAR); }
	};

#define DBMathOperandConst  0x0010
#define DBMathOperandVar    0x0020
#define DBMathOperandExpr   0x0030
#define DBMathOperandCond   0x0040

#define DBMathOperatorArgs  0x0800
#define DBMathOperatorCond  0x0400
#define DBMathOperatorFunc  0x0200
#define DBMathOperatorOr    0x0100
#define DBMathOperatorAnd   0x0080
#define DBMathOperatorGrt   0x0040
#define DBMathOperatorEqu   0x0020
#define DBMathOperatorSml   0x0010
#define DBMathOperatorAdd   0x0006
#define DBMathOperatorSub   0x0005
#define DBMathOperatorMul   0x0004
#define DBMathOperatorMod   0x0003
#define DBMathOperatorDiv   0x0002
#define DBMathOperatorExp   0x0001

class DBMathExpression;

class DBMathOperand

	{
	private:
		DBInt OprTypeVAR; /* Constant, Variable, Function, Expression or Condition */
		DBInt VarTypeVAR; /* String, Integer, Float */
		union
			{
			char *String;
			int   Int;
			float Float;
			DBMathExpression *ExpPTR;
			DBObject *ObjPTR;
			DBObjTableField *FldPTR;
			} Var;
	public:
		DBMathOperand (DBMathExpression *expr)
			{ OprTypeVAR = DBMathOperandCond; VarTypeVAR = DBVariableInt; Var.ExpPTR = expr; }
		DBMathOperand (char *);
		DBInt   Expand (DBObjectLIST<DBObject> *);
		DBInt   Configure (DBObjectLIST<DBObjTableField> *);
		char   *String (DBObjRecord *);
		DBInt   Int    (DBObjRecord *);
		DBFloat Float  (DBObjRecord *);
	};

class DBMathExpression

	{
	private:
		DBInt OperVAR;
		DBInt TypeVAR;
		DBMathOperand *LeftPTR;
		DBMathOperand *RightPTR;
	public:
		DBMathExpression (char *, char *, DBInt);
		DBMathExpression (char *, char *);
		DBMathExpression (char *, DBMathExpression *);
		DBMathExpression (char *, int, void *);
		DBInt Expand (DBObjectLIST<DBObject> *);
		DBInt Configure (DBObjectLIST<DBObjTableField> *);
		char *String (DBObjRecord *rec,DBInt);
		char *String (DBObjRecord *rec);
		DBInt   Int    (DBObjRecord *rec,DBInt);
		DBInt   Int    (DBObjRecord *rec);
		DBFloat Float  (DBObjRecord *rec,DBInt);
		DBFloat Float  (DBObjRecord *rec);
	};

DBInt DBSocketConnectServer(char *,DBInt);
DBInt DBSocketClientInitMsg (DBInt,char *);

void  DBSocketClose   (DBInt);
DBInt DBSocketStartServer  (DBInt,char *);
DBInt DBSocketGetPort (DBInt);
char *DBSocketGetHostName (DBInt);
DBInt DBSocketAcceptClient (DBInt);
DBInt DBSocketServerInitMsg (DBInt, char **);

DBInt DBSocketRead  (DBInt,void *,DBInt);
DBInt DBSocketWrite (DBInt,void *,DBInt);

void DBPauseSetFunction (int (*) (int));
int DBPause (int);

#define DBMathMax(a,b) ((a) >= (b) ? (a) : (b))
#define DBMathMin(a,b) ((a) <= (b) ? (a) : (b))
#define DBMathRegressionMin 4

void    DBMathSetGlobeRadius  (DBFloat);
char   *DBMathIntAutoFormat   (DBInt);
char   *DBMathFloatAutoFormat (DBFloat);
DBInt   DBMathGuessProjection (DBRegion);
DBInt   DBMathGuessPrecision  (DBRegion);
DBInt	DBMathTestCoordinateInRange (DBCoordinate *,DBRegion *);
DBInt	DBMathTestGeoDataBoundary   (DBObjData *,DBRegion *);

typedef DBFloat (*DBMathDistanceFunction) (DBCoordinate,DBCoordinate);

DBFloat DBMathCartesianDistance (DBCoordinate,DBCoordinate);
DBFloat DBMathSphericalDistance (DBCoordinate,DBCoordinate);

DBMathDistanceFunction DBMathGetDistanceFunction (DBInt);
DBMathDistanceFunction DBMathGetDistanceFunction (DBObjData *);
DBFloat DBMathCoordinateDistance (DBMathDistanceFunction,DBCoordinate,DBCoordinate);
DBFloat DBMathCoordinateDistance (DBInt,DBCoordinate,DBCoordinate);
DBFloat DBMathCoordinateDistance (DBObjData *,DBCoordinate,DBCoordinate);
DBFloat DBMathRectangleArea (DBInt,DBCoordinate, DBCoordinate);
DBFloat DBMathRectangleArea (DBInt,DBRegion);
DBFloat DBMathRectangleArea (DBInt,DBCoordinate, DBCoordinate);
DBFloat DBMathRectangleArea (DBObjData *data,DBRegion);
DBInt   DBMathCoordinateXCompare (const DBCoordinate *,const DBCoordinate *);
DBInt   DBMathCoordinateXCompare (const DBCoordinate *,const DBCoordinate *);
DBInt   DBMathObservationTimeCompare (const DBObservation *,const DBObservation *);
DBInt   DBMathObservationValueCompare (const DBObservation *,const DBObservation *);

DBFloat DBMathVectorByVector (DBCoordinate *,DBCoordinate *,DBCoordinate *);
DBFloat DBMathVectorXVector (DBCoordinate *,DBCoordinate *,DBCoordinate *);
void    DBMathGauss ();
DBFloat DBMathStudentsT(DBFloat);


DBInt DBMiscTestWhiteChar (char);
void  DBMiscTrimWhiteSpaces (char *);
void  DBMiscTrimBrackets (char *);
DBInt DBMiscTrimQuotes (char *);

DBInt DBDateIsLeapYear (DBInt);
DBInt DBDateIsLeapYear (DBDate);

DBInt DBFileSize (const char *);

char      *DBInfoFileName       (const char *,DBInt);
char      *DBInfoFileName       (const char *,const char *);
DBInt      DBInfoGetFields      (DBObjTable *,const char *);
DBInt      DBInfoGetTable       ( DBObjTable *,const char *);
DBInt      DBImportARCVector    (DBObjData *,const char *,const char *,const char *);
DBInt      DBImportASCIINet     (DBObjData *,const char *);
DBInt      DBImportDMGrid       (DBObjData *,FILE *);
DBInt      DBImportDMGrid       (DBObjData *,const char *);
DBInt      DBImportNetCDF       (DBObjData *,const char *);

DBInt      DBExportASCIITable   (DBObjTable *,const char *);
DBInt      DBExportASCIITable   (DBObjTable *,FILE *);
DBInt      DBExportARCGenPoint  (DBObjData *,char *);
DBInt      DBExportARCTableData (DBObjData *,char *,char *);
DBInt      DBExportARCTableDef  (DBObjData *,char *,char *);
DBInt      DBExportARCGridLayer (DBObjData *,DBObjRecord *,FILE *);
DBInt      DBExportARCGridLayer (DBObjData *,DBObjRecord *,const char *);
DBInt      DBExportDMGrid (DBObjData *,FILE *);
DBInt      DBExportDMGrid (DBObjData *,const char *);
DBInt      DBExportNetCDF (DBObjData *,const char *);


DBInt      DBPointInRange (DBObjData *,DBInt,DBRegion *);
DBInt      DBPointAddPoint (DBObjData *,DBCoordinate *,DBInt,DBInt);
DBInt      DBVectorPointFindPoint (DBObjData *,DBCoordinate *);

DBInt      DBVectorArcAdd (DBObjData *,DBCoordinate *,DBInt,DBInt,DBInt);
void       DBVectorArcFlip (DBObjData *,DBInt);
DBInt      DBVectorArcFindNearest (DBObjData *,DBCoordinate *,DBRegion *);
DBInt      DBVectorArcTestRange (DBObjData *,DBInt,DBRegion *);

DBFloat    DBVectorPolyArea (DBObjData *,DBInt);
DBInt      DBVectorPolyFindPolygon (DBObjData *,DBCoordinate *,DBRegion *);
DBInt      DBVectorPolyQuadColor (DBObjData *);
DBInt      DBVectorPolyTestRange (DBObjData *,DBInt,DBRegion *);

void       DBGridOperation (DBObjData *,DBObjData *,DBInt,DBInt);
void       DBGridOperation (DBObjData *,DBFloat,DBInt);
void       DBGridOperationAbs (DBObjData *);
DBObjData *DBNetworkToGrid (DBObjData *,DBInt);
DBObjData *DBGridToGrid (DBObjData *,DBInt,DBInt,DBInt);
DBObjData *DBGridToGrid (DBObjData *,DBInt);
DBObjData *DBGridToGrid (DBObjData *);
DBObjData *DBGridCreate (char *,DBRegion,DBCoordinate,DBInt);
DBObjData *DBGridCreate (char *,DBRegion,DBCoordinate);
DBInt      DBGridAppend (DBObjData *,DBObjData *);
DBInt      DBPointToGrid (DBObjData *,DBObjData *, DBFloat);
DBInt      DBPointToGrid (DBObjData *,DBObjData *,DBObjData *);

DBInt      DBNumberOfDays (DBInt, DBInt);
