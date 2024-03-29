/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDnetCreatePnts.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title      = (char *) NULL,   *subject = (char *) NULL;
	char *domain     = (char *) NULL,   *version = (char *) NULL;
	DBObjData  *netData, *pntData;
	float area = 5000.0, tolerance = 10.0;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--area"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing area value!"); return (CMfailed); }
			if (sscanf (argv [argPos],"%f", &area) != 1)
				{ CMmsgPrint (CMmsgUsrError, "Invalid area [%s]!", argv [argPos]); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--tolerance"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing tolerance value!"); return (CMfailed); }
			if (sscanf (argv [argPos],"%f", &tolerance) != 1)
				{ CMmsgPrint (CMmsgUsrError, "Invalid tolerance [%s]!", argv [argPos]); return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input network> <output grid>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--area        [area]");
			CMmsgPrint (CMmsgInfo,"     -r,--tolerance   [tolerance]");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]");
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

	netData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? netData->Read (argv [1]) : netData->Read (stdin);
	if ((ret == DBFault) || (netData->Type () != DBTypeNetwork))
		{
		CMmsgPrint (CMmsgUsrError,"Non-network coverage!");
		delete netData;
		return (CMfailed);
		}

	if (title	== (char *) NULL)   title = (char *) "Subbasin ponts";
	if (subject == (char *) NULL) subject = (char *) "SubbasinPoints";
	if (domain	== (char *) NULL)  domain = netData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";

	pntData = new DBObjData (title,DBTypeVectorPoint);
	pntData->Document (DBDocSubject,subject);
	pntData->Document (DBDocGeoDomain,domain);
	pntData->Document (DBDocVersion,version);

	pntData->LinkedData (netData);
	if (((ret = RGlibNetworkStations (netData,area,tolerance,pntData)) == DBSuccess) &&
	    ((ret = RGlibPointSTNCharacteristics (pntData)) == DBSuccess))
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? pntData->Write (argv [2]) : pntData->Write (stdout);

	delete netData; delete pntData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
