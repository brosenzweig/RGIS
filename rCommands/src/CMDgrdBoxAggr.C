/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

CMDgrdBlkAggr.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <math.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

typedef enum { CMDboxAverage, CMDboxMinimum, CMDboxMaximum, CMDboxSum } CMDboxMethod;
int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	CMDboxMethod method = CMDboxAverage;
	DBInt kernelSize = 2, layerID, *count = (DBInt *) NULL;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	int shadeSet = DBDataFlagDispModeContGreyScale;
	DBFloat var, *array = (DBFloat *) NULL;
	DBRegion   extent;
	DBPosition inPos, outPos;
	DBCoordinate coord, cellSize;
	DBObjData *inData, *outData;
	DBObjRecord *inLayerRec, *outLayerRec;
	DBGridIO *inGridIO, *outGridIO;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-z","--size"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing kernel size!\n");  return (CMfailed); }
			if (sscanf (argv [argPos],"%d", &kernelSize) != 1)
				{ CMmsgPrint (CMmsgUsrError, "Invalid kernel size!\n"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-m","--method")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing aggregate method!\n"); return (CMfailed); }
			else {
				const char *options [] = { "average", "minimum", "maximum", "sum", (char *) NULL };
				CMDboxMethod methods [] = { CMDboxAverage, CMDboxMinimum, CMDboxMaximum , CMDboxSum };
				DBInt code;

				if ((code = CMoptLookup (options,argv [argPos],false)) == CMfailed) {
					CMmsgPrint (CMmsgWarning,"Ignoring illformed aggregate method [%s]!\n",argv [argPos]);
				}
				else method = methods [code];
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
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
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!\n");            return (CMfailed); }
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
			const char *shadeSets [] = { "standard","grey","blue","blue-to-red","elevation", (char *) NULL };

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
			CMmsgPrint (CMmsgInfo,"%s [options] <input file> <output file>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -z,--size      [box size]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--method    [average|minimum|maximum|sum]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }

	if (verbose) RGlibPauseOpen (argv[0]);

	inData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? inData->Read (argv [1]) : inData->Read (stdin);
	if ((ret == DBFault) || (inData->Type () != DBTypeGridContinuous)) { delete inData; return (CMfailed); }

	if (title	== (char *) NULL)	title = (char *) "Box Aggregated";
	if (subject == (char *) NULL) subject = inData->Document (DBDocSubject);
	if (domain  == (char *) NULL) domain  = inData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";
	if (shadeSet == DBFault)     shadeSet = DBDataFlagDispModeContGreyScale;

	inGridIO  = new DBGridIO (inData);

	cellSize.X = (DBFloat) kernelSize * inGridIO->CellWidth  ();
	cellSize.Y = (DBFloat) kernelSize * inGridIO->CellHeight ();
	extent = inData->Extent ();
	if ((outData = DBGridCreate (title,extent,cellSize)) == (DBObjData *) NULL) return (CMfailed);
	outData->Document (DBDocSubject,subject);
	outData->Document (DBDocGeoDomain,domain);
	outData->Document (DBDocVersion,version);
	outData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
	outData->Flags (shadeSet,DBSet);
	outData->Projection (inData->Projection());
	outGridIO = new DBGridIO (outData);
	outGridIO->MissingValue (inGridIO->MissingValue ());
	if ((count = (DBInt *)   calloc (outGridIO->ColNum () * outGridIO->RowNum (), sizeof (DBInt)))   == (DBInt *)   NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__); return (CMfailed); }
	if ((array = (DBFloat *) calloc (outGridIO->ColNum () * outGridIO->RowNum (), sizeof (DBFloat))) == (DBFloat *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__); return (CMfailed); }

	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		inLayerRec = inGridIO->Layer (layerID);
		if (layerID == 0)
			{
			outLayerRec = outGridIO->Layer (layerID);
			outGridIO->RenameLayer (outLayerRec,inLayerRec->Name ());
			}
		else
			outLayerRec = outGridIO->AddLayer (inLayerRec->Name ());
		for (outPos.Row = 0;outPos.Row < outGridIO->RowNum (); ++outPos.Row)
			for (outPos.Col = 0;outPos.Col < outGridIO->ColNum ();++outPos.Col)
				{
				count [outPos.Row * outGridIO->ColNum () + outPos.Col] = 0;
				switch (method)
					{
					case CMDboxSum:
					case CMDboxAverage:
						array [outPos.Row * outGridIO->ColNum () + outPos.Col] = 0.0;
						break;
					case CMDboxMinimum:
						array [outPos.Row * outGridIO->ColNum () + outPos.Col] =  HUGE_VAL;
						break;
					case CMDboxMaximum:
						array [outPos.Row * outGridIO->ColNum () + outPos.Col] = -HUGE_VAL;
						break;
					}
				}
		for (inPos.Row = 0;inPos.Row < inGridIO->RowNum ();++inPos.Row)
			for (inPos.Col = 0;inPos.Col < inGridIO->ColNum ();++inPos.Col)
				{

				if (inGridIO->Value (inLayerRec,inPos,&var))
					{
				 	inGridIO->Pos2Coord  (inPos,coord);
				 	outGridIO->Coord2Pos (coord,outPos);
					count [outPos.Row * outGridIO->ColNum () + outPos.Col] += 1;
					switch (method)
						{
						case CMDboxSum:
						case CMDboxAverage:
							array [outPos.Row * outGridIO->ColNum () + outPos.Col] += var;
							break;
						case CMDboxMinimum:
							array [outPos.Row * outGridIO->ColNum () + outPos.Col] = var < array [outPos.Row * outGridIO->ColNum () + outPos.Col] ?
						                                                             var : array [outPos.Row * outGridIO->ColNum () + outPos.Col];
							break;
						case CMDboxMaximum:
							array [outPos.Row * outGridIO->ColNum () + outPos.Col] = var > array [outPos.Row * outGridIO->ColNum () + outPos.Col] ?
						                                                             var : array [outPos.Row * outGridIO->ColNum () + outPos.Col];
							break;
						}
					}
				}
		for (outPos.Row = 0;outPos.Row < outGridIO->RowNum (); ++outPos.Row)
			for (outPos.Col = 0;outPos.Col < outGridIO->ColNum ();++outPos.Col)
				{
				if (count [outPos.Row * outGridIO->ColNum () + outPos.Col] > 0)
					{
					var = array [outPos.Row * outGridIO->ColNum () + outPos.Col];
					if (method == CMDboxAverage)
						var = var / (DBFloat) count [outPos.Row * outGridIO->ColNum () + outPos.Col];
					outGridIO->Value (outLayerRec,outPos,var);
					}
				else
					outGridIO->Value (outLayerRec,outPos,outGridIO->MissingValue ());
				}
		outGridIO->RecalcStats (outLayerRec);
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? outData->Write (argv [2]) : outData->Write (stdout);

	if (verbose) RGlibPauseClose ();
	if (count != (DBInt *)   NULL) free (count);
	if (array != (DBFloat *) NULL) free (array);
	return (ret);
	}
