/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBVectorMisc.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

DBVectorIF::DBVectorIF (DBObjData *data)

	{
	DataPTR       = data;
	ItemTable     = DataPTR->Table (DBrNItems);
	SymbolTable   = DataPTR->Table (DBrNSymbols);

	SymbolFLD     = ItemTable->Field (DBrNSymbol);

	SymbolIDFLD   = SymbolTable->Field (DBrNSymbolID);
	ForegroundFLD = SymbolTable->Field (DBrNForeground);
	BackgroundFLD = SymbolTable->Field (DBrNBackground);
	StyleFLD      = SymbolTable->Field (DBrNStyle);
	}

DBObjRecord *DBVectorIF::Item (DBCoordinate coord) const

	{
	DBObjRecord *record, *retRecord = (DBObjRecord *) NULL;

	switch (DataPTR->Type ())
		{
		case DBTypeVectorPoint:
			{
			DBFloat dist, minDist = DBHugeVal;
			DBObjTableField *coordFLD  = ItemTable->Field (DBrNCoord);
			DBMathDistanceFunction distFunc =  DBMathGetDistanceFunction (DataPTR);
			
			for (record = FirstItem ();record != (DBObjRecord *) NULL;record = NextItem ())
				{
				dist = DBMathCoordinateDistance (distFunc,coord,coordFLD->Coordinate (record));
				if (dist < minDist) { minDist = dist; retRecord = record; }
				}
			} break;
		case DBTypeVectorLine:		break;
		case DBTypeVectorPolygon:	break;
		default: CMmsgPrint (CMmsgAppError, "Unknown Vector Data Type in: %s %d",__FILE__,__LINE__); break;
		}
	return (retRecord);
	}
