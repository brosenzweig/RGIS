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
	int argPos, argNum = argc, ret, mode = 0, netMode = 0, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *splName = (char *) NULL;
	DBObjData *grdData, *splData, *data;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-s","--sample"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing sampling coverage!"); return (CMfailed); }
			splName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			const char *modes [] = {"table", "attrib", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing mode!");         return (CMfailed); }
			if ((mode = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid mode!");         return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--netmode"))
			{
			const char *modes [] = {"from", "to", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing mode!");         return (CMfailed); }
			if ((netMode = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid mode!");         return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output table>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -s,--sample    [sampling points or network]");
			CMmsgPrint (CMmsgInfo,"     -m,--mode      [table|attrib]");
			CMmsgPrint (CMmsgInfo,"     -n,--netmode   [from|to]");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if (splName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Sampling coverage is not specified"); return (CMfailed); }

	splData = new DBObjData ();
	if ((splData->Read (splName) == DBFault) ||
		 ((splData->Type () != DBTypeNetwork) && (splData->Type () != DBTypeVectorPoint)))
		{ delete splData; return (CMfailed); }

	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || ((grdData->Type () & DBTypeGrid) != DBTypeGrid))
		{ delete splData; delete grdData; return (CMfailed); }

	if (mode == 0)
		{
		if (title	== (char *) NULL)   title = (char *) "Grid Sample";
		if (subject == (char *) NULL) subject = grdData->Document (DBDocSubject);
		if (domain	== (char *) NULL)  domain = splData->Document (DBDocGeoDomain);
		if (version == (char *) NULL) version = (char *) "0.01pre";

		data = new DBObjData (title,DBTypeTable);
		data->Document (DBDocGeoDomain,domain);
		data->Document (DBDocSubject,subject);
		data->Document (DBDocVersion,version);
		if ((ret = RGlibGridSampling (splData,grdData,data)) == DBSuccess)
			ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);
		delete data;
		}
	else
		{
		if (title	!= (char *) NULL)	splData->Name (title);
		if (subject != (char *) NULL)	splData->Document (DBDocSubject,subject);
		if (domain	!= (char *) NULL)	splData->Document (DBDocGeoDomain,domain);
		if (version != (char *) NULL) splData->Document (DBDocVersion,version);
		RGlibGridSampling (splData,grdData,netMode);
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ?
				splData->Write (argv [2]) : splData->Write (stdout);
		}

	delete grdData;
	delete splData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
