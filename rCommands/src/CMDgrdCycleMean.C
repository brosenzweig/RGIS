/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdCycleMean.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	int shadeSet = DBFault;
	DBInt stepNum = DBFault, offset = 0;
	DBObjData *tsData, *data;

	for (argPos = 1;argPos < argNum;)
		{
		if (CMargTest (argv [argPos],"-n","--number"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing step number!\n");  return (CMfailed); }
			if (sscanf (argv [argPos],"%d",&stepNum) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid stepnum!\n");      return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-o","--offset"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing offset!\n");       return (CMfailed); }
			if (sscanf (argv [argPos],"%d",&offset) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid offset!\n");       return (CMfailed); }
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
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
											DBDataFlagDispModeContGreyScale,
											DBDataFlagDispModeContBlueScale,
											DBDataFlagDispModeContBlueRed,
											DBDataFlagDispModeContElevation };
			const char *shadeSets [] = {	"standard","grey", "blue","blue-to-red", "elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!\n");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!\n");     return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			shadeSet = shadeCodes [shadeSet];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -n,--number   [# of steps]\n");
			CMmsgPrint (CMmsgInfo,"     -o,--offset    [offset]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]\n");
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

	if (stepNum == DBFault) { CMmsgPrint (CMmsgUsrError,"Cycle step number is not set!\n"); return (CMfailed); }
	tsData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? tsData->Read (argv [1]) : tsData->Read (stdin);
	if ((ret == DBFault) || (tsData->Type () != DBTypeGridContinuous))
		{ delete tsData; return (CMfailed); }

	data = DBGridToGrid (tsData);

	if (title	== (char *) NULL) title   = tsData->Name ();
	if (subject == (char *) NULL) subject = tsData->Document (DBDocSubject);
	if (domain  == (char *) NULL) domain  = tsData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = tsData->Document (DBDocVersion);
	if (shadeSet == DBFault)     shadeSet = DBDataFlagDispModeContGreyScale;

	data->Name (title);
	data->Document (DBDocSubject,subject);
	data->Document (DBDocGeoDomain,domain);
	data->Document (DBDocVersion,version);	
	data->Flags (DBDataFlagDispModeContShadeSets,DBClear);
	data->Flags (shadeSet,DBSet);

	if ((ret = RGlibCycleMean (tsData, data, stepNum, offset)) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete tsData;
	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
