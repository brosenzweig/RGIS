/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdSeasonMean.C

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
	DBInt stepNum = 0, offset = 0, doSum = false;
	DBObjData *tsData, *data;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--aggregate"))
			{
			int sumCodes  [] = { false, true	};
			const char *sumStrs [] = { "avg", "sum", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing aggregation method!\n"); return (CMfailed); }
			if ((doSum = CMoptLookup (sumStrs,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid aggregation method!\n"); return (CMfailed); }
			doSum = sumCodes [doSum];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-o","--offset"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing offset!\n");       return (CMfailed); }
			if (sscanf (argv [argPos],"%d",&offset) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid number of steps!\n");	return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-e","--step"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing number of steps!\n"); return (CMfailed); }
			if (sscanf (argv [argPos],"%d",&stepNum) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid number of steps!\n"); return (CMfailed); }
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
			CMmsgPrint (CMmsgSysError,"%s [options] <input grid> <output grid>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgSysError,"     -a,--aggregate [avg|sum]\n");
			CMmsgPrint (CMmsgSysError,"     -o,--offset    [offset]\n");
			CMmsgPrint (CMmsgSysError,"     -e,--step      [number of steps]\n");
			CMmsgPrint (CMmsgSysError,"     -t,--title     [dataset title]\n");
			CMmsgPrint (CMmsgSysError,"     -u,--subject   [subject]\n");
			CMmsgPrint (CMmsgSysError,"     -d,--domain    [domain]\n");
			CMmsgPrint (CMmsgSysError,"     -v,--version   [version]\n");
			CMmsgPrint (CMmsgSysError,"     -V,--verbose\n");
			CMmsgPrint (CMmsgSysError,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	tsData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? tsData->Read (argv [1]) : tsData->Read (stdin);
	if ((ret == DBFault) || (tsData->Type () != DBTypeGridContinuous))
		{ delete tsData; return (CMfailed); }

	if (title	== (char *) NULL)	title   = tsData->Name ();
	if (subject == (char *) NULL)	subject = tsData->Document (DBDocSubject);
	if (domain	== (char *) NULL)	domain  = tsData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = tsData->Document (DBDocVersion);	

	data = DBGridToGrid (tsData);
	data->Name (title);
	data->Document (DBDocSubject,subject);
	data->Document (DBDocGeoDomain,domain);
	data->Document (DBDocVersion,version);	

	if ((ret = RGlibSeasonMean (tsData, data, stepNum, offset, doSum)) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete tsData;
	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
