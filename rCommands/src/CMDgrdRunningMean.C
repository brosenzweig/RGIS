/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

CMDgrdRunningMean.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main(int argc, char* argv[])

	{
	int argPos, argNum = argc, ret, verbose = false;
	int layerID, kernel = 3, offset = CMfailed, i, layerNum, beginLayerID, endLayerID, num;
	DBPosition pos;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	int shadeSet = DBDataFlagDispModeContGreyScale;
	DBFloat value, sum;
	DBObjData *inData,   *outData;
	DBGridIO  *inGridIO, *outGridIO;
	DBObjRecord *inLayerRec, *outLayerRec;
  
	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-k","--kernelsize"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing kernel size!\n"); return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&kernel) != 1)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid kernel size!\n");
				return (CMfailed);
				}
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--kerneloffset"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing kernel offset!\n"); return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&offset) != 1)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid kernel offset!\n");
				return (CMfailed);
				}
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -k,--kernelsize\n");
			CMmsgPrint (CMmsgInfo,"     -f,--kerneloffset\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{
			CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]);
			return (CMfailed);
			}
		argPos++;
		}
	if (argNum > 3)
		{
		CMmsgPrint (CMmsgUsrError,"Extra arguments!\n");
		return (CMfailed);
		}
	if (verbose) RGlibPauseOpen (argv[0]);

	offset = offset == CMfailed ? offset : kernel >> 0x02;

	inData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? inData->Read (argv [1]) : inData->Read (stdin);
	if ((ret == DBFault) || ((inData->Type () != DBTypeGridContinuous) && (inData->Type () != DBTypeGridDiscrete)))
		{
		delete inData;
		return (CMfailed);
		}
	inGridIO = new DBGridIO (inData);
	if ((outData = DBGridToGrid (inData,DBTypeGridContinuous)) == (DBObjData *) NULL) return (CMfailed);

	if (title   != (char *) NULL) outData->Name     (title);
	if (subject != (char *) NULL) outData->Document (DBDocSubject,   subject);
	if (domain  != (char *) NULL) outData->Document (DBDocGeoDomain, domain);
	if (version != (char *) NULL) outData->Document (DBDocVersion,   version);	
	outGridIO = new DBGridIO (outData);

	layerNum = inGridIO->LayerNum ();
	for (layerID = 0;layerID < layerNum;++layerID)
		{
		inLayerRec = inGridIO->Layer (layerID);;
		if (layerID == 0)
			{
			outLayerRec = outGridIO->Layer (layerID);
			outGridIO->RenameLayer (outLayerRec, inLayerRec->Name ());
			}
		else outLayerRec = outGridIO->AddLayer (inLayerRec->Name ());

		beginLayerID = layerID > offset ? layerID - offset : 0;
		endLayerID   = layerID - offset + kernel < layerNum ? layerID - offset + kernel : layerNum;
		for (pos.Row = 0; pos.Row < inGridIO->RowNum (); pos.Row++)
			{
			for (pos.Col = 0; pos.Col < inGridIO->ColNum (); pos.Col++)
				{
				num = 0;
				sum = 0.0;
				for (i = beginLayerID; i < endLayerID; ++i)
					{
					if (inGridIO->Value (i, pos, &value) == false) continue;
					sum = sum + value;
					num++;
					}
				outGridIO->Value (outLayerRec,pos,num > 0 ? sum / (DBFloat) num : outGridIO->MissingValue ());
				}
			}
		outGridIO->RecalcStats (outLayerRec);
		}
	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? outData->Write (argv [2]) : outData->Write (stdout);

	delete inGridIO;
	delete outGridIO;
	delete inData;
	delete outData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
