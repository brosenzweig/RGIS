/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDnetErosion.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int _CMDnetErosion (DBObjData *netData, DBObjData *inData, DBObjData *weightData,
			  DBObjData *outData, DBFloat coeff,DBInt areaMult)
	{
	DBInt ret = DBSuccess, layerID, cellID, progress, maxProgress;
	DBFloat inValue, weight, outValue, *sumWeights;
	DBPosition pos;
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBGridIF *inIF  = new DBGridIF (inData);
	DBGridIF *outIF = new DBGridIF (outData);
	DBGridIF *weightIF = weightData != (DBObjData *) NULL ? new DBGridIF (weightData) : (DBGridIF *) NULL;
	DBObjRecord *inLayerRec, *outLayerRec, *weightLayerRec, *cellRec, *toCell;

	if ((sumWeights = (DBFloat *) calloc (netIF->CellNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{ perror ("Memory allocation error in:_CMDnetErosion ()"); ret = DBFault; goto Stop; }

	layerID = 0;
	inLayerRec = inIF->Layer (layerID);

	outLayerRec = outIF->Layer (layerID);
	outIF->RenameLayer (outLayerRec,inLayerRec->Name ());
	outValue = outIF->MissingValue (outLayerRec);
	for (pos.Row = 0;pos.Row < outIF->RowNum ();pos.Row++)
		for (pos.Col = 0;pos.Col < outIF->ColNum ();pos.Col++) outIF->Value (outLayerRec,pos,outValue);

	for (layerID = 1;layerID < inIF->LayerNum ();++layerID)
		{
		inLayerRec = inIF->Layer (layerID);
		if ((outLayerRec = outIF->AddLayer (inLayerRec->Name ())) == (DBObjRecord *) NULL)
			{ ret = DBFault; goto Stop; }
		for (pos.Row = 0;pos.Row < outIF->RowNum ();pos.Row++)
			for (pos.Col = 0;pos.Col < outIF->ColNum ();pos.Col++) outIF->Value (outLayerRec,pos,outValue);
		}
	maxProgress = inIF->LayerNum () * netIF->CellNum ();
	for (layerID = 0;layerID < inIF->LayerNum ();++layerID)
		{
		inLayerRec  = inIF->Layer  (layerID);
		outLayerRec = outIF->Layer (layerID);
		if (weightIF != (DBGridIF *) NULL)
			weightLayerRec = weightIF->Layer (layerID % weightIF->LayerNum ());
		for (cellID = 0;cellID < netIF->CellNum ();cellID++)
			{
			sumWeights [cellID] = 0.0;
			cellRec = netIF->Cell (cellID);
			if (inIF->Value  (inLayerRec, netIF->Center (cellRec),&inValue) == false)
				outIF->Value (outLayerRec,netIF->CellPosition (cellRec),0.0);
			else
				{
				if (weightIF != (DBGridIF *) NULL)
					weight = weightIF->Value (weightLayerRec,netIF->Center (cellRec),&weight) == false ?
								0.0 : weight * coeff;
				else weight = coeff;
				if (areaMult)	weight = weight * netIF->CellArea (cellRec);
				sumWeights [cellID] = weight;
				outIF->Value (outLayerRec,netIF->CellPosition (cellRec),inValue * weight);
				}
			}

		for (cellID = netIF->CellNum () - 1;cellID >= 0;--cellID)
			{
			progress = layerID * netIF->CellNum () + (netIF->CellNum () - cellID);
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			cellRec = netIF->Cell (cellID);
			if ((toCell = netIF->ToCell (cellRec)) == (DBObjRecord *) NULL) continue;
			if (outIF->Value (outLayerRec,netIF->CellPosition (cellRec),&inValue)  == false) continue;
			if (outIF->Value (outLayerRec,netIF->CellPosition (toCell),&outValue) == false) continue;

			sumWeights [toCell->RowID ()] = sumWeights [toCell->RowID ()] + weight;
			outIF->Value (outLayerRec,netIF->CellPosition (toCell),outValue + inValue);
			}
		outIF->RecalcStats (outLayerRec);
		}

	free (sumWeights);
Stop:
	delete netIF; delete inIF; delete outIF;
	if (weightIF != (DBGridIF *) NULL) delete weightIF;
	return (ret);
	}

int main (int argc,char *argv [])
	{
	int argPos, argNum = argc, ret, verbose = false;
	float coeff;
	char *title  = (char *) NULL,		*subject = (char *) NULL;
	char *domain = (char *) NULL,		*version = (char *) NULL;
	char *netName = (char *) NULL,	*weightName = (char *) NULL;
	DBInt shadeSet = DBFault, areaMult = true, coeffSet = false;
	DBObjData *data, *netData, *weightData, *grdData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-n","--network"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing network!\n");      return (CMfailed); }
			netName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-w","--weight"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing weight grid!\n");	return (CMfailed); }
			weightName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			int modeCodes [] = {	true, false };
			const char *modes [] = {	"rate", "volume", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing accumulation mode!\n"); return (CMfailed); }
			if ((areaMult = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid accumulation mode!\n"); return (CMfailed); }
			areaMult = modeCodes [areaMult];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--coefficient"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing coefficient!\n");  return (CMfailed); }
			if (sscanf (argv [argPos],"%f",&coeff) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid coefficient\n");   return (CMfailed); }
			coeffSet = true;
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
			const char *shadeSets [] = { "standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!\n");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == DBFault)
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
			CMmsgPrint (CMmsgInfo,"     -n,--network     [network coverage]\n");
			CMmsgPrint (CMmsgInfo,"     -w,--weight      [weight grid]\n");
			CMmsgPrint (CMmsgInfo,"     -f,--coefficient [coefficient]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--mode        [rate|volume]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset    [standard|grey|blue|blue-to-red|elevation\n");
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

	if (netName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Network is not specified\n"); return (CMfailed); }

	netData = new DBObjData ();
	if ((netData->Read (netName) == DBFault) || (netData->Type () != DBTypeNetwork))
		{ delete netData; return (CMfailed); }
	if (weightName != (char *) NULL)
		{
		weightData = new DBObjData ();
		if (weightData->Read (weightName) == DBFault)
			{ delete netData; return (CMfailed); }
		}
	else weightData = (DBObjData *) NULL;

	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || (grdData->Type () != DBTypeGridContinuous))
		{ delete netData; delete grdData; return (CMfailed); }

	if (title   == (char *) NULL)   title = (char *) "Accumulated Grid";
	if (subject == (char *) NULL) subject = grdData->Document (DBDocSubject);
	if (domain  == (char *) NULL)  domain = netData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";

	data = DBNetworkToGrid (netData,DBTypeGridContinuous);
	data->Name (title);
	data->Document (DBDocSubject,subject);
	data->Document (DBDocGeoDomain,domain);
	data->Document (DBDocVersion,version);
	if (shadeSet != DBFault)
		{
		data->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		data->Flags (shadeSet,DBSet);
		}

	if (coeffSet == false)	coeff = areaMult ? 0.000001 : 1.0;
	ret = _CMDnetErosion (netData,grdData,weightData,data,coeff,areaMult);
	if (ret == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data; delete netData; delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
