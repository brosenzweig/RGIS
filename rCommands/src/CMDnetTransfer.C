/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDnetTransfer.C
Based on CMDnetErosion.C

wil.wollheim@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

/*char *_CMDWhat = GHAASVersion;*/

int _CMDnetTransfer (DBObjData *netData,
						  DBObjData *inData,
						  DBObjData *weightData,
						  DBObjData *outData,
						  DBFloat coeff,
						  DBObjData *coeffData,
						  DBObjData *QData,
						  DBObjData *HLData,
						  DBFloat umax,
						  DBFloat ksat,
						  DBInt areaMult)
	{
	DBInt ret = DBSuccess, layerID, cellID, progress, maxProgress;
	DBFloat inValue, weight, outValue, *sumWeights, kCoeff, q, hl, Conc, Uptake, Vf;
	DBPosition pos;
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBGridIF *inIF  = new DBGridIF (inData);
	DBGridIF *outIF = new DBGridIF (outData);
	DBGridIF *weightIF = weightData != (DBObjData *) NULL ? new DBGridIF (weightData) : (DBGridIF *) NULL;
	DBGridIF *coeffIF = coeffData != (DBObjData *) NULL ? new DBGridIF (coeffData) : (DBGridIF *) NULL;
	DBGridIF *Q_IF = QData != (DBObjData *) NULL ? new DBGridIF (QData) : (DBGridIF *) NULL;
	DBGridIF *HL_IF = HLData != (DBObjData *) NULL ? new DBGridIF (HLData) : (DBGridIF *) NULL;
	DBObjRecord *inLayerRec, *outLayerRec, *weightLayerRec, *coeffLayerRec, *Q_LayerRec, *HL_LayerRec, *cellRec, *toCell;
	DBCoordinate coord;


	if ((sumWeights = (DBFloat *) calloc (netIF->CellNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{ perror ("Memory allocation error in:_CMDnetTransfer ()"); ret = DBFault; goto Stop; }

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
		if (coeffIF != (DBGridIF *) NULL)
			coeffLayerRec = coeffIF->Layer (layerID % coeffIF->LayerNum ());
		if (Q_IF != (DBGridIF *) NULL)
			Q_LayerRec = Q_IF->Layer (layerID % Q_IF->LayerNum ());
		if (HL_IF != (DBGridIF *) NULL)
			HL_LayerRec = HL_IF->Layer (layerID % HL_IF->LayerNum ());

		for (cellID = 0;cellID < netIF->CellNum ();cellID++)
			{
			sumWeights [cellID] = 0.0;
			cellRec = netIF->Cell (cellID);
			coord = netIF->Center (cellRec);
			if (inIF->Value  (inLayerRec,coord, &inValue) == false)
				outIF->Value (outLayerRec,netIF->CellPosition (cellRec),0.0);
			else
				{
				if (weightIF != (DBGridIF *) NULL)
					weight = weightIF->Value (weightLayerRec,coord,&weight) == false ?
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
			coord = netIF->Center (cellRec);
			if ((toCell = netIF->ToCell (cellRec)) == (DBObjRecord *) NULL) continue;
			if (outIF->Value (outLayerRec,netIF->CellPosition (cellRec),&inValue)  == false) continue;
			if (outIF->Value (outLayerRec,netIF->CellPosition (toCell),&outValue) == false) continue;

			sumWeights [toCell->RowID ()] = sumWeights [toCell->RowID ()] + weight;


			if (coeffIF != (DBGridIF *) NULL)
				{
				if (coeffIF->Value (coeffLayerRec,netIF->Center (cellRec),&kCoeff) == false) kCoeff = 1.0;
				}
			else
				{
				if ( ((Q_IF == (DBGridIF *) NULL) || (Q_IF->Value (Q_LayerRec,netIF->Center (cellRec),&q) == false)) ||
				     ((HL_IF == (DBGridIF *) NULL) || (HL_IF->Value (HL_LayerRec,netIF->Center (cellRec),&hl) == false)) ||
					  (umax == 0) || (ksat == 0) )	kCoeff = 1.0;
				else {
							if ((q > 0) && (hl > 0))
							{
								Conc = ((inValue / (q * 86400 * 365)) * 1000 * 1000); /* mg/m3 - assume input = kg/yr, Q is m3/s convert to m3/yr kg to mg */
					  			Uptake = (umax * 24 * 365 * Conc) / (ksat * 1000 + Conc ); /* mg/m2/yr - umax and ksat/Conc are in mg/m2/hr, mg/m3 */
								if (Uptake > 0)
									{
									Vf = Uptake / Conc; /* Vf in m/yr */
									}
								else Vf = 0;
								kCoeff = pow(2.71828, (-1.0 * Vf / hl)); /* HL in m/yr */
							/*
							 if ((cellID == 5390) || (cellID == 5015) || (cellID  == 4905) || (cellID == 4857))
							     printf("%i, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f \n", cellID, outValue, inValue, q, hl, umax, ksat, Conc, Uptake, Vf, kCoeff);
							*/
							}
							else kCoeff = 0;
						 }
				}
			/*
			if ((cellID == 5390) || (cellID == 5015) || (cellID  == 4905) || (cellID == 4857))
			     printf("%i, %f \n", cellID, inValue);
			*/
			inValue = kCoeff * inValue;

			outIF->Value (outLayerRec,netIF->CellPosition (toCell),outValue + inValue);
			}
		outIF->RecalcStats (outLayerRec);
		}

	free (sumWeights);
Stop:
	delete netIF; delete inIF; delete outIF;
	if (weightIF != (DBGridIF *) NULL) delete weightIF;
	return (ret);

	return (DBSuccess);
	}

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	float coeff, umax, ksat;
	char *title  = (char *) NULL,		*subject = (char *) NULL;
	char *domain = (char *) NULL,		*version = (char *) NULL;
	char *netName = (char *) NULL,	*weightName = (char *) NULL, *coeffName = (char *) NULL;
	char *QName = (char *) NULL , *HLName = (char *) NULL;
	DBInt shadeSet = DBFault, areaMult = true, coeffSet = false;
/*	float umax = false, ksat = false;*/
	DBObjData *data, *netData, *weightData, *coeffData, *grdData, *QData, *HLData;

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
		if (CMargTest (argv [argPos],"-k","--transfer"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing transfer coefficient!\n"); return (CMfailed); }
			coeffName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-q","--discharge"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Discharge grid!\n"); return (CMfailed); }
			QName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-l","--hydraulic load"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing hydraulic load grid!\n"); return (CMfailed); }
			HLName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-p","--umax"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing umax!\n");         return (CMfailed); }
			if (sscanf (argv [argPos],"%f",&umax) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid umax\n");          return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--ksat"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing ksat!\n");         return (CMfailed); }
			if (sscanf (argv [argPos],"%f",&ksat) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid ksat\n");          return (CMfailed); }
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
			CMmsgPrint (CMmsgUsrError,"netTransfer [options] <input grid> <output grid>\n");
			CMmsgPrint (CMmsgUsrError,"     -n, --network     [network coverage]\n");
			CMmsgPrint (CMmsgUsrError,"     -w, --weight      [weight grid]\n");
			CMmsgPrint (CMmsgUsrError,"     -f, --coefficient [coefficient]\n");
			CMmsgPrint (CMmsgUsrError,"     -k, --transfer    [coefficient grid]\n");
			CMmsgPrint (CMmsgUsrError,"     -q, --discharge   [discharge grid - m/s]\n");
			CMmsgPrint (CMmsgUsrError,"     -l, --hydrload    [hydraulic load grid - m/yr]\n");
			CMmsgPrint (CMmsgUsrError,"     -p, --umax   		 [umax value - mg/m2/hr]\n");
			CMmsgPrint (CMmsgUsrError,"     -r, --halfsat     [halfsat value - mg/l]\n");
			CMmsgPrint (CMmsgUsrError,"     -m, --mode        [rate|volume]\n");
			CMmsgPrint (CMmsgUsrError,"     -t, --title       [dataset title]\n");
			CMmsgPrint (CMmsgUsrError,"     -u, --subject     [subject]\n");
			CMmsgPrint (CMmsgUsrError,"     -d, --domain      [domain]\n");
			CMmsgPrint (CMmsgUsrError,"     -v, --version     [version]\n");
			CMmsgPrint (CMmsgUsrError,"     -s, --shadeset    [standard|grey|blue|blue-to-red|elevation\n");
			CMmsgPrint (CMmsgUsrError,"     -V, --verbose\n");
			CMmsgPrint (CMmsgUsrError,"     -h, --help\n");
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

	if (coeffName != (char *) NULL)
		{
		coeffData = new DBObjData ();
		if (coeffData->Read (coeffName) == DBFault)
			{ delete netData; return (CMfailed); }
		}
	else coeffData = (DBObjData *) NULL;

	if (QName != (char *) NULL)
		{
		QData = new DBObjData ();
		if (QData->Read (QName) == DBFault)
			{ delete netData; return (CMfailed); }/* not changes */
		}
	else QData = (DBObjData *) NULL;

	if (HLName != (char *) NULL)
		{
		HLData = new DBObjData ();
		if (HLData->Read (HLName) == DBFault)
			{ delete netData; return (CMfailed); }/* not changes */
		}
	else HLData = (DBObjData *) NULL;


	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || (grdData->Type () != DBTypeGridContinuous))
		{ delete netData; delete grdData; return (CMfailed); }

	if (title	== (char *) NULL)   title = (char *) "Accumulated Grid";
	if (subject == (char *) NULL) subject = grdData->Document (DBDocSubject);
	if (domain	== (char *) NULL)  domain = netData->Document (DBDocGeoDomain);
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
	ret = _CMDnetTransfer (netData,grdData,weightData,data,coeff,coeffData,QData,HLData,umax,ksat,areaMult);
	if (ret == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data; delete netData; delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
