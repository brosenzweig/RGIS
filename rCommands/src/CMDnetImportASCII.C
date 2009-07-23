/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

CMDnetImportASCII.C

balazs.fekete@unh.edu

*******************************************************************************/
#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL;
	char *domain = (char *) NULL,		*version = (char *) NULL;
	DBObjData *netData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!\n");        return (CMfailed); }
			title = argv [argPos];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <ascii grid> <output network>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]\n");
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

	netData = new DBObjData ("",DBTypeNetwork);
	if (title	== (char *) NULL)   title = (char *) "Inverse Accumulated Grid";
	if (domain	== (char *) NULL)  domain = (char *) "Unknown";
	if (version == (char *) NULL) version = (char *) "0.01pre";

	netData->Name (title);
	netData->Document (DBDocSubject,"STNetwork");
	netData->Document (DBDocGeoDomain,domain);
	netData->Document (DBDocVersion,version);

	if ((ret = DBImportASCIINet (netData,argv [1])) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? netData->Write (argv [2]) : netData->Write (stdout);

	delete netData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
