/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDpntStationDist.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *tmplName = (char *) NULL;
	int mode = true;
	DBFloat factor = 1.0;
	DBObjData *data, *pntData, *tmplData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-e","--template"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing template coverage!\n"); return (CMfailed); }
			tmplName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--factor"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing factor!\n");       return (CMfailed); }
			if (sscanf (argv [argPos],"%lf",&factor) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Factor scanning error!\n");	return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			int  modCodes [] = { true, false };
			const char *values [] = { "dist", "ids", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing mode!\n");         return (CMfailed); }
			if ((mode = CMoptLookup (values,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid value!\n");	return (CMfailed); }
			mode = modCodes [mode];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input point> <output grid>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -e,--template  [grid or network coverage]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--mode      [dist|ids]\n");
			CMmsgPrint (CMmsgInfo,"     -f,--factor    [value]\n");
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

	if (tmplName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Template coverage is not specified\n"); return (CMfailed); }

	tmplData = new DBObjData ();
	if ((tmplData->Read (tmplName) == DBFault)     ||
		 (tmplData->Type () == DBTypeVectorPoint)   ||
		 (tmplData->Type () == DBTypeVectorLine)    ||
		 (tmplData->Type () == DBTypeVectorPolygon) ||
		 (tmplData->Type () == DBTypeTable)) { delete tmplData; return (CMfailed); }

	pntData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? pntData->Read (argv [1]) : pntData->Read (stdin);
	if ((ret == DBFault) || (pntData->Type () != DBTypeVectorPoint))
		{ delete pntData, delete tmplData; return (CMfailed); }
		
	if (title	== (char *) NULL) title   = "Distance to Station";
	if (subject == (char *) NULL) subject = pntData->Document (DBDocSubject);
	if (domain	== (char *) NULL)	domain  = tmplData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = "0.01pre";	

	if (tmplData->Type () == DBTypeNetwork)
		data = DBNetworkToGrid (tmplData, mode ? DBTypeGridContinuous : DBTypeGridDiscrete);
	else
		data = DBGridToGrid    (tmplData, mode ? DBTypeGridContinuous : DBTypeGridDiscrete);
	delete tmplData;

	data->Name (title);
	data->Document (DBDocSubject,subject);
	data->Document (DBDocGeoDomain, domain);
	data->Document (DBDocVersion, version);

	if ((ret = DBPointToGrid (pntData,data,factor)) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	if (verbose) RGlibPauseClose ();
	delete pntData; delete data;
	return (ret);
	}
