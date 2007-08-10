/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDnetBasinStats.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

#define CMDBasin	0
#define CMDHead	1
#define CMDDivide	2

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *networkName = (char *) NULL;
	DBInt statMode = CMDBasin;
	DBObjData *data, *netData, *grdData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-n","--network"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing network!\n");      return (CMfailed); }
			networkName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			int  statCodes [] = { CMDBasin, CMDHead, CMDDivide };
			const char *statStrs [] = {	"basin","head", "divide", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing mode!\n");         return (CMfailed); }
			if ((statMode = CMoptLookup (statStrs,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid mode!\n");		return (CMfailed); }
			statMode = statCodes [statMode];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output table>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -n,--network   [network coverage]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--mode      [basin|head|divide]\n");
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

	if (networkName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Network is not specified\n"); return (CMfailed); }

	netData = new DBObjData ();
	if ((netData->Read (networkName) == DBFault) || (netData->Type () != DBTypeNetwork))
		{ delete netData; return (CMfailed); }

	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || (grdData->Type () != DBTypeGridContinuous))
		{ delete netData; delete grdData; return (CMfailed); }

	if (title	== (char *) NULL)	title   = "Basin Statistics";
	if (subject == (char *) NULL)	subject = grdData->Document (DBDocSubject);
	if (domain	== (char *) NULL)	domain  = netData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = "0.01pre";	

	data = new DBObjData (title,DBTypeTable);
	data->Document (DBDocSubject,subject);
	data->Document (DBDocGeoDomain,domain);
	data->Document (DBDocVersion,version);

	switch (statMode)
		{
		default:
		case CMDBasin:		ret = RGlibNetworkBasinStats (netData,grdData,data);			break;
	 	case CMDHead:		ret = RGlibNetworkHeadStats (netData,grdData,data,false);	break;
	 	case CMDDivide:	ret = RGlibNetworkHeadStats (netData,grdData,data,true);	break;
		}
	if (ret == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data; delete netData; delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
