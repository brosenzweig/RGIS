/******************************************************************************

GHAAS RiverGIS Plot Utility V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

rgisPlot.H

balazs.fekete@unh.edu

*******************************************************************************/

#include<DB.H>
#include<cpgplot.h>

class RGPColorMapEntry
	{
	public:
		DBShort Red;
		DBShort Green;
		DBShort Blue;
	};

#define RGPBufferSIZE 0x100

void RGPPrintMessage (DBInt,DBInt *,const char *);
DBInt RGPPrintError (DBInt, DBInt, const char *);
void  RGPInitPenColors ();
DBInt RGPSetColorMap (RGPColorMapEntry *,DBInt);

DBInt RGPDrawVecPoint (DBInt , DBInt *, DBObjData *);
DBInt RGPDrawVecLine (DBInt , DBInt *, DBObjData *);
DBInt RGPDrawGridContinuous (DBInt, DBInt *, DBObjData *);
DBInt RGPDrawNetwork (DBInt, DBInt *, DBObjData *);
