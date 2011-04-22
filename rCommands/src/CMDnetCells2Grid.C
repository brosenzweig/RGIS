/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDnetCells2Grid.C

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
	char *fieldName  = (char *) NULL;
	DBInt shadeSet   = DBFault;
	DBInt gridType   = DBTypeGridContinuous;
	DBObjData  *netData, *grdData;
	DBObjTable *cellTable;
	DBObjTableField *field;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-f","--fieldname"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing discharge field!"); return (CMfailed); }
			fieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-g","--gridtype"))
			{
			int codes [] = { DBTypeGridContinuous, DBTypeGridDiscrete };
			const char *options [] = { "continuous", "discrete", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing correcition mode!"); return (CMfailed); }
			if ((gridType = CMoptLookup (options, argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError, "Invalid correction mode!");  return (CMfailed); }
			gridType = codes [gridType];
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
		if ((gridType == DBTypeGridContinuous) && CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int codes [] = { DBDataFlagDispModeContStandard,
			                 DBDataFlagDispModeContGreyScale,
			                 DBDataFlagDispModeContBlueScale,
			                 DBDataFlagDispModeContBlueRed,
			                 DBDataFlagDispModeContElevation };
			const char *options [] = { "standard", "grey", "blue", "blue-to-red", "elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (options,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!");     return (CMfailed); }
			shadeSet = codes [shadeSet];
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
			CMmsgPrint (CMmsgInfo,"     -f,--fieldname   [field name]");
			CMmsgPrint (CMmsgInfo,"     -g,--gridtype    [continuous|discrete]");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset    [standard|grey|blue|blue-to-red|elevation]");
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

	if (fieldName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Missing field name!"); return (CMfailed); }
	if ((gridType == DBTypeGridDiscrete) && (shadeSet != DBFault))
		{
		shadeSet = DBFault;
		CMmsgPrint (CMmsgUsrError,"Ignoring shadeset for discrete grid!");
		}

	netData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? netData->Read (argv [1]) : netData->Read (stdin);
	if ((ret == DBFault) || (netData->Type () != DBTypeNetwork))
		{
		CMmsgPrint (CMmsgUsrError,"Non-network coverage!");
		delete netData;
		return (CMfailed);
		}
	cellTable = netData->Table (DBrNCells);
	if ((field = cellTable->Field (fieldName)) == (DBObjTableField *) NULL)
		{
		CMmsgPrint (CMmsgUsrError,"Non-existing field [%s]!",fieldName);
		delete netData;
		return (CMfailed);
		}
	if ((gridType == DBTypeGridDiscrete) && !DBTableFieldIsCategory (field))
		{
		CMmsgPrint (CMmsgUsrError,"Non-category field");
		delete netData;
		return (CMfailed);
		}
	else if (!DBTableFieldIsNumeric  (field))
		{
		CMmsgPrint (CMmsgUsrError,"Non-numeric field");
		delete netData;
		return (CMfailed);
		}

	if (title	== (char *) NULL)   title = (char *) "Converted Grid";
	if (subject == (char *) NULL) subject = (char *) "fieldName";
	if (domain	== (char *) NULL)  domain = netData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";

	grdData = DBNetworkToGrid (netData,gridType);
	grdData->Name (title);
	grdData->Document (DBDocSubject,subject);
	grdData->Document (DBDocGeoDomain,domain);
	grdData->Document (DBDocVersion,version);
	if (shadeSet != DBFault)
		{
		grdData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		grdData->Flags (shadeSet,DBSet);
		}

	if ((ret = RGlibNetworkToGrid (netData,field,grdData)) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? grdData->Write (argv [2]) : grdData->Write (stdout);

	delete netData; delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
