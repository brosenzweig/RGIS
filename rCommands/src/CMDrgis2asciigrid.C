/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDrgis2asciigrid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	FILE *out;
	int argPos, argNum = argc, ret, verbose = false;
	char *layerName = (char *) NULL;
	int doList = false, doNum = false, doAll = false;
	DBInt layerID;
	DBObjData *data;
	DBObjRecord *layerRec;
	DBGridIF *gridIF;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--all"))
			{ argNum = CMargShiftLeft (argPos,argv,argNum); doAll  = true; continue; }
		if (CMargTest (argv [argPos],"-i","--list"))
			{ argNum = CMargShiftLeft (argPos,argv,argNum); doList = true; continue; }
		if (CMargTest (argv [argPos],"-n","--num"))
			{ argNum = CMargShiftLeft (argPos,argv,argNum); doNum  = true; continue; }
		if (CMargTest (argv [argPos],"-l","--layer"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing layerName!");    return (CMfailed); }
			layerName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <dm file> <rgis file>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--all");
			CMmsgPrint (CMmsgInfo,"     -l,--layer [layername]");
			CMmsgPrint (CMmsgInfo,"     -i,--list");
			CMmsgPrint (CMmsgInfo,"     -n,--num");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if (((doList || doNum) && (doAll || (layerName != (char *) NULL))) ||
		 (doAll & (layerName != (char *) NULL)))
		{ CMmsgPrint (CMmsgUsrError,"Conflicting options!"); return (CMfailed); }

	data = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin);
	if ((ret == DBFault) || ((data->Type () & DBTypeGrid) != DBTypeGrid))
		{ delete data; return (CMfailed); }

	gridIF = new DBGridIF (data);
	if ((out = argNum > 2 ? fopen (argv [2],"w") : stdout) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Output file opening error!"); return (CMfailed); }

	if (doNum) fprintf (out,"%d",gridIF->LayerNum ());
	if (doList)
		for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
			{
			layerRec = gridIF->Layer (layerID);
			fprintf (out,"%s",layerRec->Name ());
			}
	if (doAll)
		for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
			{
			layerRec = gridIF->Layer (layerID);
			DBExportARCGridLayer (data,layerRec,out);
			}
	else if (layerName != (char *) NULL)
		{
		if ((layerRec = gridIF->Layer (layerName)) == (DBObjRecord *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Wrong layername"); }
		else	DBExportARCGridLayer (data,layerRec,out);
		}

	if (argNum > 2) fclose (out);

	delete gridIF;
	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
