/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

DBImpARCPoint.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

template <class Float>
class DBPointARCLabRecord
	{
	private:
		int ID;
		int IntID;
		Float CoordX,	CoordY;
		Float MinX,		MinY;
		Float MaxX,		MaxY;
		void Swap ()
			{ 
			DBByteOrderSwapWord (&ID);
			DBByteOrderSwapWord (&IntID);
			switch (sizeof (Float))
				{
				case sizeof (DBFloat4):
					DBByteOrderSwapWord (&CoordX);
					DBByteOrderSwapWord (&CoordY);
					DBByteOrderSwapWord (&MinX);
					DBByteOrderSwapWord (&MinY);
					DBByteOrderSwapWord (&MaxX);
					DBByteOrderSwapWord (&MaxY);
					break;
				case sizeof (DBFloat):
					DBByteOrderSwapLongWord (&CoordX);
					DBByteOrderSwapLongWord (&CoordY);
					DBByteOrderSwapLongWord (&MinX);
					DBByteOrderSwapLongWord (&MinY);
					DBByteOrderSwapLongWord (&MaxX);
					DBByteOrderSwapLongWord (&MaxY);
					break;
				default:
					fprintf (stderr,"Wrong Float Size in: DBPointARCLabRecord<Float> ::Swap ()\n");
					break;
				}
			}
		DBInt Read (FILE *file,int swap)
			{
			if (fread (this,sizeof (DBPointARCLabRecord),1,file) != (int) 1)
				{ perror ("File Reading Error in: DBPointARCLabRecord::Read ()"); return (DBFault); }
			if (swap) Swap ();
			return (DBSuccess);
			}
	public:
		DBInt LoadPoints (FILE *file,DBObjTable *points,DBRegion *extent)
			{
			DBInt swap = DBByteOrder (DBByteOrderLITTLE), pointNum = 0;;
			DBObjTableField *coordField = points->Field (DBrNCoord);
			DBObjRecord *record;
			DBCoordinate coord;
			
			if (coordField == (DBObjTableField *) NULL)
				{
				fprintf (stderr,"Corrupt Point Data Block in: DBPointARCFloatLabRecord::LoadPoints (DBObjTable *)\n");
				return (DBFault);
				}
			
			for (record = points->First ();record != (DBObjRecord *) NULL;record = (DBObjRecord *) points->Next ())
				{
				if (Read (file,swap) == DBFault) return (DBFault);
				DBPause ((++pointNum * 100)/ points->ItemNum ());
				coord.X = CoordX;
				coord.Y = CoordY;
				extent->Expand (coord);
				coordField->Coordinate (record,coord);
				}
			return (DBSuccess);
			}
	};

int DBImportARCPoint (DBObjData *vecData,const char *arcCov)

	{
	FILE *inFile;
	char fileName [DBDataFileNameLen];
	DBInt floatCov;
	short infoHeader [50];
	DBPointARCLabRecord<float>		floatRec;
	DBPointARCLabRecord<double>	doubleRec;
	DBObjTable *items = vecData->Table (DBrNItems);
	DBRegion dataExtent = vecData->Extent ();

	floatCov = false;
	sprintf (fileName,"%s/bnd",arcCov);
	if (access (fileName,R_OK)) floatCov = true;
	else
		{
		sprintf (fileName,"%s/bnd.adf",arcCov);
		if (access (fileName,R_OK)) floatCov = true;
		}
	
	sprintf (fileName,"%s/lab",arcCov);
	if (access (fileName,R_OK) == DBFault) sprintf (fileName,"%s/lab.adf",arcCov);	

	if ((inFile = fopen (fileName,"r")) == NULL)
		{ perror ("File Opening Error in: DBImportARCPoint ()"); return (DBFault); }
	if (fread (infoHeader,sizeof (short),50,inFile) != 50)
		{ perror ("File Reading Error in: DBImportARCPoint ()"); return (DBFault); }
		
	if (floatCov)	floatRec.LoadPoints  (inFile,items,&dataExtent);
	else				doubleRec.LoadPoints (inFile,items,&dataExtent);
	fclose (inFile);
	vecData->Extent (dataExtent);
   vecData->Projection (DBMathGuessProjection (dataExtent));
   vecData->Precision  (DBMathGuessPrecision  (dataExtent));
	return (DBSuccess);
	}
