/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdTimeSeries.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<DB.H>
#include<RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	DBInt shadeSet = DBFault;
	char *gridName   = (char *) NULL;
	char *relateName = (char *) NULL;
	char *joinName   = (char *) NULL;
	char *varName    = (char *) NULL;
	char *dateName   = (char *) NULL;
	DBObjData *grdData, *tabData, *outData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-g","--grid"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing grid!");         return (CMfailed); }
			gridName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--relate"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing relate field!"); return (CMfailed); }
			relateName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-j","--join"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing join field!");   return (CMfailed); }
			joinName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-i","--variable"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing variable field!"); return (CMfailed); }
			varName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-a","--date"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing date field!");   return (CMfailed); }
			dateName = argv [argPos];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input table> <output grid>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -g,--grid      [discrete grid coverage]");
			CMmsgPrint (CMmsgInfo,"     -r,--relate    [relate field]");
			CMmsgPrint (CMmsgInfo,"     -j,--join      [join field]");
			CMmsgPrint (CMmsgInfo,"     -i,--variable  [variable field]");
			CMmsgPrint (CMmsgInfo,"     -a,--date      [date field]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]");
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

	if (gridName   == (char *) NULL) { CMmsgPrint (CMmsgUsrError,"Discrete grid is mandatory");		return (CMfailed); }
	if (relateName == (char *) NULL) { CMmsgPrint (CMmsgUsrError,"Relate field is mandatory"); 		return (CMfailed); }
	if (joinName   == (char *) NULL) { CMmsgPrint (CMmsgUsrError,"Join field is mandatory");			return (CMfailed); }
	if (varName    == (char *) NULL) { CMmsgPrint (CMmsgUsrError,"Variable field is mandatory");	return (CMfailed); }

	grdData = new DBObjData ();
	if ((grdData->Read (gridName) != DBSuccess) || (grdData->Type () != DBTypeGridDiscrete))
		{ delete grdData; CMmsgPrint (CMmsgUsrError,"Grid File Reading Error!"); return (CMfailed); }

	tabData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? tabData->Read (argv [1]) : tabData->Read (stdin);
	if ((ret == DBFault) || (tabData->Type () != DBTypeTable))
		{ delete grdData; delete tabData; return (CMfailed); }

	if (title	== (char *) NULL)  title   = (char *) "Time Series";
	if (subject == (char *) NULL)  subject = tabData->Document (DBDocSubject);
	if (domain	== (char *) NULL)  domain  = grdData->Document (DBDocGeoDomain);
	if (version == (char *) NULL)  version = (char *) "0.01pre";

	if ((outData = DBGridToGrid (grdData,DBTypeGridContinuous)) == (DBObjData *) NULL) return (CMfailed);

	outData->Name (title);
	outData->Document (DBDocSubject,subject);
	outData->Document (DBDocGeoDomain,domain);
	outData->Document (DBDocVersion,version);
	if (shadeSet != DBFault)
		{
		outData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		outData->Flags (shadeSet,DBSet);
		}

	if ((ret = RGlibGridUniformGrid (grdData,tabData,relateName,joinName,varName,dateName,outData)) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? outData->Write (argv [2]) : outData->Write (stdout);

	delete grdData; delete tabData; delete outData;
	if (verbose) RGlibPauseClose ();
	return (DBSuccess);
	}
