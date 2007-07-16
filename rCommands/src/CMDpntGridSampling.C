/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDpntGridSampling.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, mode = 0, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *splName = (char *) NULL;
	DBObjData *grdData, *splData, *data;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-s","--sample"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing sampling coverage!\n"); return (CMfailed); }
			splName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			const char *modes [] = {"table", "attrib", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing mode!\n");         return (CMfailed); }
			if ((mode = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid mode!\n");         return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"     -s,--sample    [sampling network or grid]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--mode      [table|attrib]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if (splName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Sampling coverage is not specified\n"); return (CMfailed); }

	splData = new DBObjData ();
	if ((splData->Read (splName) == DBFault) || 
		 ((splData->Type () != DBTypeNetwork) && (splData->Type () != DBTypeVectorPoint)))
		{ delete splData; return (CMfailed); }

	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || (grdData->Type () != DBTypeGridContinuous))
		{ delete splData; delete grdData; return (CMfailed); }
		
	if (mode == 0)
		{
		if (title	== (char *) NULL)	title   = "Grid Sample";
		if (subject == (char *) NULL)	subject = grdData->Document (DBDocSubject);
		if (domain	== (char *) NULL)	domain  = splData->Document (DBDocGeoDomain);
		if (version == (char *) NULL) version = "0.01pre";	

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
		RGlibGridSampling (splData,grdData);
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ?
				splData->Write (argv [2]) : splData->Write (stdout);
		}

	delete grdData;
	delete splData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
