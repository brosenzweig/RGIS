/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

CMDds2rgis.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	FILE *inFile;
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *tmpName = (char *) NULL;
	DBInt shadeSet = DBFault;
	DBObjData *outData, *tmpData = (DBObjData *) NULL;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-m","--template"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing template!\n");	  return (CMfailed); }
			tmpName = argv [argPos];
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
			const char *shadeSets [] = {	"standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!\n");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!\n");     return (CMfailed); }
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
			fprintf (stdout,"%s [options] <data stream file> <rgis file>\n",CMprgName(argv[0]));
			fprintf (stdout,"     -m,--template  [template coverage]\n");
			fprintf (stdout,"     -t,--title     [dataset title]\n");
			fprintf (stdout,"     -u,--subject   [subject]\n");
			fprintf (stdout,"     -d,--domain    [domain]\n");
			fprintf (stdout,"     -v,--version   [version]\n");
			fprintf (stdout,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]\n");
			fprintf (stdout,"     -V,--verbose\n");
			fprintf (stdout,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ fprintf (stderr,"Unknown option: %s!\n",argv [argPos]); return (DBFault); }
		argPos++;
		}

	if (tmpName == (char *) NULL) { fprintf (stderr,"Missing template covarage!\n"); return (DBFault); }

	if (argNum > 3) { fprintf (stderr,"Extra arguments!\n"); return (DBFault); }
	if (verbose) RGlibPauseOpen (argv[0]);

	inFile = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? fopen (argv [1],"r") : stdin;
	if (inFile == (FILE *) NULL)
		{ perror ("Input data stream opening error in: ds2rgis"); return (DBFault); }

	tmpData = new DBObjData ();
	if (tmpData->Read (tmpName) == DBFault)
		{ if (inFile != stdin) fclose (inFile); delete tmpData; return (DBFault); }


	if (title	== (char *) NULL) { fprintf (stderr,"Title is not set!\n"); return (DBFault); }
	if (subject == (char *) NULL) subject = tmpData->Document (DBDocSubject);
	if (domain  == (char *) NULL) domain  = tmpData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";

	switch (tmpData->Type ())
		{
		default:
		case DBTypeVectorPoint:    outData = new DBObjData (title,DBTypeTable);              break;
		case DBTypeGridContinuous:
		case DBTypeGridDiscrete:   outData = DBGridToGrid (tmpData,DBTypeGridContinuous);    break;
		case DBTypeNetwork:        outData = DBNetworkToGrid (tmpData,DBTypeGridContinuous); break;
		}

	if (outData == (DBObjData *) NULL) { if (inFile != stdin) fclose (inFile); delete tmpData; return (DBFault); }

	if (RGlibDataStream2RGIS (outData,tmpData,inFile) == DBSuccess)
		{
		outData->Name (title);
		outData->Document (DBDocSubject,subject);
		outData->Document (DBDocGeoDomain,domain);
		outData->Document (DBDocVersion,version);
		if (shadeSet != DBFault)
			{
			outData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
			outData->Flags (shadeSet,DBSet);
			}
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? outData->Write (argv [2]) : outData->Write (stdout);
		}
	else	ret = DBFault;

	delete tmpData;
	delete outData;
	if (inFile != stdin) fclose (inFile);
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
