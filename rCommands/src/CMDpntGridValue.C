/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDpntGridSampling.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret;
   DBCoordinate coord;
	DBObjData *data;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-c","--coordinates"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing sampling coordinates!"); return (CMfailed); }
			if (sscanf (argv [argPos],"%lf,%lf", &(coord.X),&(coord.Y)) != 2)
            { CMmsgPrint (CMmsgUsrError,"Invalid sampling coordinates!"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output table>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -c,--coordinates");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }

	data = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin);
	
   switch (data->Type ())
      {
      case DBTypeGridContinuous:
         {
         DBInt   layerID;
         DBFloat value;
         DBGridIF *gridIF = new DBGridIF (data);
         DBObjRecord *layerRec;
         
         for (layerID = 0; layerID < gridIF->LayerNum (); ++layerID)
            {
            layerRec = gridIF->Layer (layerID);
            if (gridIF->Value(layerRec,coord,&value))
               printf ("%s: %f\n",layerRec->Name(),value);
            else
               printf ("%s: nodata",layerRec->Name());
            }
         delete gridIF;
         } break;
      case DBTypeGridDiscrete:
         break;
      case DBTypeNetwork:
         break;
      default:
         break;
      }
	delete data;
	return (ret);
	}

