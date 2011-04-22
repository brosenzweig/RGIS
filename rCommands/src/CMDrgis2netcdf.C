/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDrgis2netcdf.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	DBInt argPos, argNum = argc, ret;
	DBObjData *grdData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input data> <output netcdf>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum < 2) { CMmsgPrint (CMmsgUsrError,"Missing output netcdf!"); return (CMfailed); }
	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }

	grdData = new DBObjData ();
	ret = (argNum > 2) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if (ret == DBFault) { delete grdData; return (CMfailed); }
	
	ret = DBExportNetCDF (grdData,argNum > 2 ? argv [2] : argv [1]);
	delete grdData;
	return (ret);
	}
