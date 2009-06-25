/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

CMDrgis2asciigrid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
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
	DBGridIO *gridIO;

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
				{ CMmsgPrint (CMmsgUsrError,"Missing layerName!\n");    return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"%s [options] <dm file> <rgis file>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--all\n");
			CMmsgPrint (CMmsgInfo,"     -l,--layer [layername]\n");
			CMmsgPrint (CMmsgInfo,"     -i,--list\n");
			CMmsgPrint (CMmsgInfo,"     -n,--num\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if (((doList || doNum) && (doAll || (layerName != (char *) NULL))) ||
		 (doAll & (layerName != (char *) NULL)))
		{ CMmsgPrint (CMmsgUsrError,"Conflicting options!\n"); return (CMfailed); }

	data = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin);
	if ((ret == DBFault) || ((data->Type () & DBTypeGrid) != DBTypeGrid))
		{ delete data; return (CMfailed); }

	gridIO = new DBGridIO (data);
	if ((out = argNum > 2 ? fopen (argv [2],"w") : stdout) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Output file opening error!\n"); return (CMfailed); }

	if (doNum) fprintf (out,"%d\n",gridIO->LayerNum ());
	if (doList)
		for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
			{
			layerRec = gridIO->Layer (layerID);
			fprintf (out,"%s\n",layerRec->Name ());
			}
	if (doAll)
		for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
			{
			layerRec = gridIO->Layer (layerID);
			DBExportARCGridLayer (data,layerRec,out);
			}
	else if (layerName != (char *) NULL)
		{
		if ((layerRec = gridIO->Layer (layerName)) == (DBObjRecord *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Wrong layername\n"); }
		else	DBExportARCGridLayer (data,layerRec,out);
		}

	if (argNum > 2) fclose (out);

	delete gridIO;
	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
