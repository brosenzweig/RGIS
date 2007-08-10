/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDgrdExtractLayers.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<DB.H>
#include<DBio.H>
#include<RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *firstLayer = (char *) NULL, *lastLayer = (char *) NULL;
	DBObjData *grdData;
	DBObjRecord *layerRec;
	DBGridIO *gridIO;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-f","--first"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing first layer!\n");  return (CMfailed); }
			firstLayer = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-l","--last"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing last layer!\n");   return (CMfailed); }
			lastLayer = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!\n");        return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!\n");      return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!\n");       return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!\n");      return (CMfailed); }
			version  = argv [argPos];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>\n",CMprgName (argv [0]));
			CMmsgPrint (CMmsgInfo,"     -f,--first     [layername]\n");
			CMmsgPrint (CMmsgInfo,"     -l,--last      [layername]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]\n");
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

	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || (grdData->Type () & DBTypeGrid != DBTypeGrid))
		{ delete grdData; return (CMfailed); }

	gridIO = new DBGridIO (grdData);

	if (firstLayer != (char *) NULL)
		gridIO->DeleteLayers ((gridIO->Layer (0))->Name (),firstLayer);
	if (lastLayer  != (char *) NULL)
		{
		if ((layerRec = gridIO->Layer (lastLayer)) == (DBObjRecord *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Invalid last layer!\n");  delete gridIO; delete grdData; return (CMfailed); }
		if (layerRec->RowID () < gridIO->LayerNum () - 1)
			{
			layerRec = gridIO->Layer (layerRec->RowID () + 1);
			gridIO->DeleteLayers (layerRec->Name (),(gridIO->Layer (gridIO->LayerNum () - 1))->Name ());
			gridIO->DeleteLayer (gridIO->Layer (gridIO->LayerNum () - 1)->Name ());
			}
		}
	delete gridIO;

	if (title   != (char *) NULL) grdData->Name (title);
	if (subject != (char *) NULL) grdData->Document (DBDocSubject,subject);
	if (domain  != (char *) NULL) grdData->Document (DBDocGeoDomain,domain);
	if (version != (char *) NULL) grdData->Document (DBDocVersion,version);

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? grdData->Write (argv [2]) : grdData->Write (stdout);

	delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
