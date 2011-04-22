/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdRunningMean.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main(int argc, char* argv[])

	{
	int argPos, argNum = argc, ret, verbose = false;
	int layerID, kernel = 3, offset = 0, i, layerNum, beginLayerID, endLayerID, num;
	DBPosition pos;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	int shadeSet = DBDataFlagDispModeContGreyScale;
	DBFloat value, sum;
	DBObjData *inData,   *outData;
	DBGridIF  *inGridIF, *outGridIF;
	DBObjRecord *inLayerRec, *outLayerRec;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-k","--kernelsize"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing kernel size!"); return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&kernel) != 1)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid kernel size!");
				return (CMfailed);
				}
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--kerneloffset"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing kernel offset!"); return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&offset) != 1)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid kernel offset!");
				return (CMfailed);
				}
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -k,--kernelsize");
			CMmsgPrint (CMmsgInfo,"     -f,--kerneloffset");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{
			CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]);
			return (CMfailed);
			}
		argPos++;
		}
	if (argNum > 3)
		{
		CMmsgPrint (CMmsgUsrError,"Extra arguments!");
		return (CMfailed);
		}
	if (verbose) RGlibPauseOpen (argv[0]);

	inData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? inData->Read (argv [1]) : inData->Read (stdin);
	if ((ret == DBFault) || ((inData->Type () != DBTypeGridContinuous) && (inData->Type () != DBTypeGridDiscrete)))
		{
		delete inData;
		return (CMfailed);
		}
	inGridIF = new DBGridIF (inData);
	if ((outData = DBGridToGrid (inData,DBTypeGridContinuous)) == (DBObjData *) NULL) return (CMfailed);

	if (title   != (char *) NULL) outData->Name     (title);
	if (subject != (char *) NULL) outData->Document (DBDocSubject,   subject);
	if (domain  != (char *) NULL) outData->Document (DBDocGeoDomain, domain);
	if (version != (char *) NULL) outData->Document (DBDocVersion,   version);
	outGridIF = new DBGridIF (outData);

	layerNum = inGridIF->LayerNum ();
	for (layerID = 0;layerID < layerNum;++layerID)
		{
		inLayerRec = inGridIF->Layer (layerID);;
		if (layerID == 0)
			{
			outLayerRec = outGridIF->Layer (layerID);
			outGridIF->RenameLayer (outLayerRec, inLayerRec->Name ());
			}
		else outLayerRec = outGridIF->AddLayer (inLayerRec->Name ());

		beginLayerID = layerID >= offset ? layerID - offset : 0;
		endLayerID   = (layerID - offset + kernel) < layerNum ? (layerID - offset + kernel) : layerNum;
//		CMmsgPrint (CMmsgDebug, "LayerID: %d Kernel: %d Offset: %d     Begin: %d  End: %d", layerID, kernel, offset, beginLayerID, endLayerID);
		for (pos.Row = 0; pos.Row < inGridIF->RowNum (); pos.Row++)
			{
			for (pos.Col = 0; pos.Col < inGridIF->ColNum (); pos.Col++)
				{
				num = 0;
				sum = 0.0;
				for (i = beginLayerID; i < endLayerID; ++i)
					{
					if (inGridIF->Value (i, pos, &value) == false) continue;
					sum = sum + value;
					num++;
					}
				outGridIF->Value (outLayerRec,pos,num > 0 ? sum / (DBFloat) num : outGridIF->MissingValue ());
				}
			}
		outGridIF->RecalcStats (outLayerRec);
		}
	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? outData->Write (argv [2]) : outData->Write (stdout);

	delete inGridIF;
	delete outGridIF;
	delete inData;
	delete outData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
