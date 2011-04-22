/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdBlkAggr.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <math.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

typedef enum { CMDboxAverage, CMDboxMinimum, CMDboxMaximum, CMDboxSum } CMDboxMethod;
int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	CMDboxMethod method = CMDboxAverage;
	DBInt kernelSize = 2, layerID;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	int shadeSet = DBDataFlagDispModeContGreyScale;
	DBFloat var, *array = (DBFloat *) NULL, *sumArea = (DBFloat *) NULL, *misArea = (DBFloat *) NULL, cellArea;
	DBRegion   extent;
	DBPosition inPos, outPos;
	DBCoordinate coord, cellSize;
	DBObjData *inData, *outData;
	DBObjRecord *inLayerRec, *outLayerRec;
	DBGridIF *inGridIF, *outGridIF;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-z","--size"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing kernel size!");  return (CMfailed); }
			if (sscanf (argv [argPos],"%d", &kernelSize) != 1)
				{ CMmsgPrint (CMmsgUsrError, "Invalid kernel size!"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-m","--method")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing aggregate method!"); return (CMfailed); }
			else {
				const char *options [] = { "average", "minimum", "maximum", "sum", (char *) NULL };
				CMDboxMethod methods [] = { CMDboxAverage, CMDboxMinimum, CMDboxMaximum , CMDboxSum };
				DBInt code;

				if ((code = CMoptLookup (options,argv [argPos],false)) == CMfailed) {
					CMmsgPrint (CMmsgWarning,"Ignoring illformed aggregate method [%s]!",argv [argPos]);
				}
				else method = methods [code];
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
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
			                        DBDataFlagDispModeContGreyScale,
			                        DBDataFlagDispModeContBlueScale,
			                        DBDataFlagDispModeContBlueRed,
			                        DBDataFlagDispModeContElevation };
			const char *shadeSets [] = { "standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == CMfailed)
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input file> <output file>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -z,--size      [box size]");
			CMmsgPrint (CMmsgInfo,"     -m,--method    [average|minimum|maximum|sum]");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]");
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
	if ((ret == DBFault) || (inData->Type () != DBTypeGridContinuous)) { delete inData; return (CMfailed); }

	if (title	== (char *) NULL)	title = (char *) "Box Aggregated";
	if (subject == (char *) NULL) subject = inData->Document (DBDocSubject);
	if (domain  == (char *) NULL) domain  = inData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";
	if (shadeSet == DBFault)     shadeSet = DBDataFlagDispModeContGreyScale;

	inGridIF  = new DBGridIF (inData);

	cellSize.X = (DBFloat) kernelSize * inGridIF->CellWidth  ();
	cellSize.Y = (DBFloat) kernelSize * inGridIF->CellHeight ();
	extent = inData->Extent ();
	if ((outData = DBGridCreate (title,extent,cellSize)) == (DBObjData *) NULL) return (CMfailed);
	outData->Document (DBDocSubject,subject);
	outData->Document (DBDocGeoDomain,domain);
	outData->Document (DBDocVersion,version);
	outData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
	outData->Flags (shadeSet,DBSet);
	outData->Projection (inData->Projection());
	outGridIF = new DBGridIF (outData);
	outGridIF->MissingValue (inGridIF->MissingValue ());
	if ((sumArea = (DBFloat *) calloc (outGridIF->ColNum () * outGridIF->RowNum (), sizeof (DBFloat))) == (DBFloat *)   NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__); return (CMfailed); }
	if ((misArea = (DBFloat *) calloc (outGridIF->ColNum () * outGridIF->RowNum (), sizeof (DBFloat))) == (DBFloat *)   NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__); return (CMfailed); }
	if ((array   = (DBFloat *) calloc (outGridIF->ColNum () * outGridIF->RowNum (), sizeof (DBFloat))) == (DBFloat *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__); return (CMfailed); }

	for (layerID = 0;layerID < inGridIF->LayerNum ();++layerID)
		{
		inLayerRec = inGridIF->Layer (layerID);
		if (layerID == 0)
			{
			outLayerRec = outGridIF->Layer (layerID);
			outGridIF->RenameLayer (outLayerRec,inLayerRec->Name ());
			}
		else
			outLayerRec = outGridIF->AddLayer (inLayerRec->Name ());
		for (outPos.Row = 0;outPos.Row < outGridIF->RowNum (); ++outPos.Row)
			for (outPos.Col = 0;outPos.Col < outGridIF->ColNum ();++outPos.Col)
				{
				sumArea [outPos.Row * outGridIF->ColNum () + outPos.Col] = 0.0;
				misArea [outPos.Row * outGridIF->ColNum () + outPos.Col] = 0.0;
				switch (method)
					{
					case CMDboxSum:
					case CMDboxAverage:
						array [outPos.Row * outGridIF->ColNum () + outPos.Col] = 0.0;
						break;
					case CMDboxMinimum:
						array [outPos.Row * outGridIF->ColNum () + outPos.Col] =  HUGE_VAL;
						break;
					case CMDboxMaximum:
						array [outPos.Row * outGridIF->ColNum () + outPos.Col] = -HUGE_VAL;
						break;
					}
				}
		for (inPos.Row = 0;inPos.Row < inGridIF->RowNum ();++inPos.Row)
			for (inPos.Col = 0;inPos.Col < inGridIF->ColNum ();++inPos.Col)
				{
			 	inGridIF->Pos2Coord  (inPos,coord);
			 	outGridIF->Coord2Pos (coord,outPos);
			 	cellArea = inGridIF->CellArea(inPos);
				if (inGridIF->Value (inLayerRec,inPos,&var))
					{
					sumArea [outPos.Row * outGridIF->ColNum () + outPos.Col] += cellArea;
					switch (method)
						{
						case CMDboxSum:
						case CMDboxAverage:
							array [outPos.Row * outGridIF->ColNum () + outPos.Col] += var * cellArea;
							break;
						case CMDboxMinimum:
							array [outPos.Row * outGridIF->ColNum () + outPos.Col] = var < array [outPos.Row * outGridIF->ColNum () + outPos.Col] ?
						                                                             var : array [outPos.Row * outGridIF->ColNum () + outPos.Col];
							break;
						case CMDboxMaximum:
							array [outPos.Row * outGridIF->ColNum () + outPos.Col] = var > array [outPos.Row * outGridIF->ColNum () + outPos.Col] ?
						                                                             var : array [outPos.Row * outGridIF->ColNum () + outPos.Col];
							break;
						}
					}
				else
					misArea [outPos.Row * outGridIF->ColNum () + outPos.Col] += cellArea;
				}
		for (outPos.Row = 0;outPos.Row < outGridIF->RowNum (); ++outPos.Row)
			for (outPos.Col = 0;outPos.Col < outGridIF->ColNum ();++outPos.Col)
				{
				if (sumArea [outPos.Row * outGridIF->ColNum () + outPos.Col] > 0.0)
					{
					var = array [outPos.Row * outGridIF->ColNum () + outPos.Col];
					if (method == CMDboxAverage)
						var = var / (DBFloat) sumArea [outPos.Row * outGridIF->ColNum () + outPos.Col];
					else if (method == CMDboxSum)
						var = var * (misArea [outPos.Row * outGridIF->ColNum () + outPos.Col] + sumArea [outPos.Row * outGridIF->ColNum () + outPos.Col])
						    / (DBFloat) sumArea [outPos.Row * outGridIF->ColNum () + outPos.Col];
					outGridIF->Value (outLayerRec,outPos,var);
					}
				else
					outGridIF->Value (outLayerRec,outPos,outGridIF->MissingValue ());
				}
		outGridIF->RecalcStats (outLayerRec);
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? outData->Write (argv [2]) : outData->Write (stdout);

	if (verbose) RGlibPauseClose ();
	if (sumArea != (DBFloat *) NULL) free (sumArea);
	if (misArea != (DBFloat *) NULL) free (misArea);
	if (array   != (DBFloat *) NULL) free (array);
	return (ret);
	}
