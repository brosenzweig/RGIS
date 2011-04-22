/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdDifference.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<DB.H>
#include<DBif.H>
#include<RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *gridName = (char *) NULL;
	int shadeSet = DBFault;
	int mode = 0;
	DBObjData *leftData, *rightData, *retData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-i","--difference"))
			{
			const char *modes [] = { "absolute", "relative", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing differnce method!");  return (CMfailed); }
			if ((mode = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid difference method!"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-g","--grid"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing grid!");         return (CMfailed); }
			gridName = argv [argPos];
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
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
											DBDataFlagDispModeContGreyScale,
											DBDataFlagDispModeContBlueScale,
											DBDataFlagDispModeContBlueRed,
											DBDataFlagDispModeContElevation };
			const char *shadeSets [] = {	"standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!");     return (CMfailed); }
			shadeSet = shadeCodes [shadeSet];
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
			CMmsgPrint (CMmsgInfo,"     -g,--grid       [continuous grid coverage]");
			CMmsgPrint (CMmsgInfo,"     -i,--difference	[absolute|relative]");
			CMmsgPrint (CMmsgInfo,"     -t,--title      [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject    [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain     [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version    [version]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset   [standard|grey|blue|blue-to-red|elevation]");
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

	rightData = new DBObjData ();
	if (gridName == (char *) NULL) { CMmsgPrint (CMmsgUsrError,"Missing operand grid"); return (CMfailed); }

	if ((ret = rightData->Read (gridName)) || (rightData->Type () != DBTypeGridContinuous))
		{ delete rightData; return (CMfailed); }
	
	retData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? retData->Read (argv [1]) : retData->Read (stdin);
	if ((ret == DBFault) || (retData->Type () != DBTypeGridContinuous))
		{ delete rightData; delete retData; return (CMfailed); }

	if (mode == 1)
		{
		leftData = new DBObjData (*retData);
		DBGridOperation (leftData,rightData,DBMathOperatorAdd,   true);
		DBGridOperation (retData, rightData,DBMathOperatorSub,	true);
		DBGridOperation (retData, leftData, DBMathOperatorDiv,	true);
		delete leftData;
		}
	else DBGridOperation (retData,rightData,DBMathOperatorSub,	true);

	if (title   != (char *) NULL) retData->Name (title);
	if (subject != (char *) NULL) retData->Document (DBDocSubject,subject);
	if (domain  != (char *) NULL) retData->Document (DBDocGeoDomain,domain);
	if (version != (char *) NULL) retData->Document (DBDocVersion,version);
	if (shadeSet != DBFault)
		{
		retData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		retData->Flags (shadeSet,DBSet);
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? retData->Write (argv [2]) : retData->Write (stdout);

	delete retData;
	delete rightData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
