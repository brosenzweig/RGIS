/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdAppendLayers.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, data, dataNum = 0, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char **dataList = (char **) NULL;
	DBObjData *grdData, *appData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--append"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing append grid!");  return (CMfailed); }
			if ((dataList = (char **) realloc (dataList,(dataNum + 1) * sizeof (char *))) == (char **) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (DBFault); }
			dataList [dataNum++] = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!");        return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!");      return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!");       return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!");      return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--append    [append grid]");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]");
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

	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || ((grdData->Type () & DBTypeGrid) != DBTypeGrid))
		{ delete grdData; return (CMfailed); }

	if (title   != (char *) NULL) grdData->Name (title);
	if (subject != (char *) NULL) grdData->Document (DBDocSubject,subject);
	if (domain  != (char *) NULL) grdData->Document (DBDocGeoDomain,domain);
	if (version != (char *) NULL) grdData->Document (DBDocVersion,version);

	for (data = 0;data < dataNum;++data)
		{
		appData = new DBObjData ();
		if (appData->Read (dataList [data]) == DBFault) { delete grdData; delete appData; return (CMfailed); }
		if (DBGridAppend (grdData,appData)  == DBFault) { delete grdData; delete appData; return (CMfailed); }
		delete appData;
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? grdData->Write (argv [2]) : grdData->Write (stdout);

	delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
