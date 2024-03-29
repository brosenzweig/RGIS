/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdCreateNetwork.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <math.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	bool downhill = true;
	char *title  = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	DBObjData *outData, *inData, *basinData = (DBObjData *) NULL;
	DBInt DBGridCont2Network (DBObjData *,DBObjData *, bool);

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest(argv[argPos],"-b","--basin_pack")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing basin pack filename!"); return (CMfailed); }
			else {
				if (basinData != (DBObjData *) NULL)
					CMmsgPrint (CMmsgWarning,"Ignoring redefined basin pack");
				else {
					basinData = new DBObjData ();
					if (basinData->Read (argv [argPos]) == DBFault) {
						CMmsgPrint (CMmsgUsrError, "Basin data reading error");
						delete basinData;
						basinData = (DBObjData *) NULL;
						return (CMfailed);
					}
				}
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-g","--gradient")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing gradient method!"); return (CMfailed); }
			else {
				const char *options [] = { "down", "up", (char *) NULL };
				bool methods [] = { true, false };
				DBInt code;

				if ((code = CMoptLookup (options,argv [argPos],false)) == CMfailed) {
					CMmsgPrint (CMmsgWarning,"Ignoring illformed gradient method [%s]!",argv [argPos]);
				}
				else downhill = methods [code];
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
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
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!");            return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input file> <output file>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -b,--basin_pack [basin pack file]");
			CMmsgPrint (CMmsgInfo,"     -g,--gradient   [down|up]");
			CMmsgPrint (CMmsgInfo,"     -t,--title      [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain     [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version    [version]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }

	if (verbose) RGlibPauseOpen (argv[0]);

	inData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? inData->Read (argv [1]) : inData->Read (stdin);
	if ((ret == DBFault) || (inData->Type () != DBTypeGridContinuous)) { ret = DBFault;  goto Stop; }
	inData->LinkedData (basinData);

	if (title	== (char *) NULL)	title = (char *) "Regridded Network";
	if (domain  == (char *) NULL) domain  = inData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";

	outData = new DBObjData (title,DBTypeNetwork);
	outData->Document (DBDocSubject,"STNetwork");
	outData->Document (DBDocGeoDomain,domain);
	outData->Document (DBDocVersion,version);

	if (DBGridCont2Network (inData,outData, downhill) == DBFault) {
		CMmsgPrint (CMmsgUsrError,"Grid create network failed!");
		ret = DBFault;
		goto Stop;
	}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? outData->Write (argv [2]) : outData->Write (stdout);

Stop:
	if (basinData != (DBObjData *) NULL) delete basinData;
	if (inData    != (DBObjData *) NULL) delete inData;
	if (outData   != (DBObjData *) NULL) delete outData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
