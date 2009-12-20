/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGlibNetwork.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>
#include <RG.H>

DBInt RGlibNetworkToGrid (DBObjData *netData,DBObjTableField *field, DBObjData *grdData)

	{
	DBInt cellID, intVal;
	DBFloat floatVal;
	DBPosition pos;
	DBObjRecord *cellRec, *layerRec;
	DBGridIO *grdIO;
	DBNetworkIO *netIO;

	if (field == (DBObjTableField *) NULL) return (DBFault);

	netIO = new DBNetworkIO (netData);
	grdIO = new DBGridIO (grdData);
	layerRec = grdIO->Layer (0);
	grdIO->RenameLayer (layerRec,field->Name ());
	switch (grdData->Type ())
		{
		case DBTypeGridContinuous:
			for (pos.Row = 0;pos.Row < grdIO->RowNum ();++pos.Row)
				for (pos.Col = 0;pos.Col < grdIO->ColNum ();++pos.Col)
					grdIO->Value (layerRec,pos,grdIO->MissingValue ());

			for (cellID = 0;cellID < netIO->CellNum ();++cellID)
				{
				if (DBPause (cellID * 100 / netIO->CellNum ())) goto Stop;
				cellRec = netIO->Cell (cellID);
				if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;

				if (field->Type () == DBTableFieldInt)
					{
					intVal = field->Int (cellRec);
					if (intVal == field->IntNoData ())
							grdIO->Value (layerRec,netIO->CellPosition (cellRec),grdIO->MissingValue ());
					else	grdIO->Value (layerRec,netIO->CellPosition (cellRec),(DBFloat) intVal);
					}
				else
					{
					floatVal = field->Float (cellRec);
					if (CMmathEqualValues (floatVal,field->FloatNoData ()))
						grdIO->Value (layerRec,netIO->CellPosition (cellRec),grdIO->MissingValue ());
					else	grdIO->Value (layerRec,netIO->CellPosition (cellRec),floatVal);
					}
				}
			grdIO->RecalcStats ();
			break;
		case DBTypeGridDiscrete:
			{
			char nameStr [DBStringLength];
			DBObjRecord *itemRec;
			DBObjRecord *symRec = (grdData->Table (DBrNSymbols))->Item ();
			DBObjTable *itemTable	 = grdData->Table (DBrNItems);
			DBObjTableField *gridValueFLD  = itemTable->Field (DBrNGridValue);
			DBObjTableField *gridSymbolFLD = itemTable->Field (DBrNSymbol);
			DBObjTableField *symbolIDFLD	 = (grdData->Table (DBrNSymbols))->Field (DBrNSymbolID);
			DBObjTableField *foregroundFLD = (grdData->Table (DBrNSymbols))->Field (DBrNForeground);
			DBObjTableField *backgroundFLD = (grdData->Table (DBrNSymbols))->Field (DBrNBackground);
			DBObjTableField *styleFLD = (grdData->Table (DBrNSymbols))->Field (DBrNStyle);

			symbolIDFLD->Int (symRec,0);
			foregroundFLD->Int (symRec,1);
			backgroundFLD->Int (symRec,0);
			styleFLD->Int (symRec,0);

			for (pos.Row = 0;pos.Row < grdIO->RowNum ();++pos.Row)
				for (pos.Col = 0;pos.Col < grdIO->ColNum ();++pos.Col)
					grdIO->Value (layerRec,pos,DBFault);

			for (cellID = 0;cellID < netIO->CellNum ();++cellID)
				{
				if (DBPause (cellID * 100 / netIO->CellNum ())) goto Stop;
				cellRec = netIO->Cell (cellID);
				if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				if ((intVal = field->Int (cellRec)) == field->IntNoData ())
					grdIO->Value (netIO->CellPosition (cellRec),DBFault);
				else
					{
					sprintf (nameStr,"Category%04d",intVal);
					if ((itemRec = itemTable->Item (nameStr)) == (DBObjRecord *) NULL)
						{
						if ((itemRec = itemTable->Add (nameStr)) == (DBObjRecord *) NULL)
							{ fprintf (stderr,"Item Object Creation Error in: DMDataset::Read ()\n"); return (DBFault); }
						gridValueFLD->Int (itemRec,intVal);
						gridSymbolFLD->Record (itemRec,symRec);
						}
					grdIO->Value (netIO->CellPosition (cellRec),itemRec->RowID ());
					}
				}
			itemTable->ListSort (gridValueFLD);
			itemTable->ItemSort ();
			grdIO->DiscreteStats ();
			} break;
		default:
			perror ("Invalid data type in: RGlibNetworkToGrid ()!\n");
			break;
		}
Stop:
	delete netIO;
	if (cellID == netIO->CellNum ()) { delete grdIO; return (DBSuccess); }
	delete grdIO;
	return (DBFault);
	}

DBInt RGlibNetworkBasinGrid (DBObjData *netData, DBObjData *grdData)

	{
	char symbolName [DBStringLength];
	DBInt cellID, basinID, inFieldID, symbol, symMax = 0;
	DBPosition pos;
	DBObjRecord *cellRec, *basinRec, *itemRec, *symRec;
	DBObjTable *grdTable		= grdData->Table (DBrNItems);
	DBObjTable *symTable		= grdData->Table (DBrNSymbols);
	DBObjTable *basinTable	= netData->Table (DBrNItems);
	DBObjTable *cellTable	= netData->Table (DBrNCells);
	DBObjTableField *gValueFLD = grdTable->Field (DBrNGridValue);
	DBObjTableField *gSymFLD 	= grdTable->Field (DBrNSymbol);
	DBObjTableField *gFGSymFLD = symTable->Field (DBrNForeground);
	DBObjTableField *gBGSymFLD = symTable->Field (DBrNBackground);
	DBObjTableField *gStSymFLD = symTable->Field (DBrNStyle);
	DBObjTableField *colorFLD 	= basinTable->Field (DBrNColor);
	DBObjTableField *basinFLD 	= cellTable->Field (DBrNBasin);
	DBObjTableField *inFLD, *outFLD;
	DBGridIO *grdIO;
	DBNetworkIO *netIO;

	netIO = new DBNetworkIO (netData);
	grdIO = new DBGridIO (grdData);

	grdIO->RenameLayer (grdIO->Layer (0),(char *) "Basin Grid");

	for (inFieldID = 0;inFieldID < basinTable->FieldNum ();++inFieldID)
		{
		inFLD = basinTable->Field (inFieldID);
		if (DBTableFieldIsVisible (inFLD))
			{
			grdTable->AddField (outFLD = new DBObjTableField (*inFLD));
			if (inFLD->Required ()) outFLD->Required (false);
			}
		}
	for (basinID = 0;basinID < netIO->BasinNum ();++basinID)
		{
		basinRec = basinTable->Item (basinID);
		itemRec	= grdTable->Add (basinRec->Name ());
		gValueFLD->Int (itemRec,basinID + 1);
		symbol =  colorFLD->Int (basinRec) - 7;
		symMax = symMax > symbol ? symMax : symbol;
		for (inFieldID = 0;inFieldID < basinTable->FieldNum ();++inFieldID)
			{
			inFLD  = basinTable->Field (inFieldID);
			if (DBTableFieldIsVisible (inFLD))
				{
				outFLD = grdTable->Field (inFLD->Name ());
				switch (inFLD->Type ())
					{
					case DBTableFieldString:	outFLD->String (itemRec,inFLD->String	(basinRec));	break;
					case DBTableFieldInt:		outFLD->Int 	(itemRec,inFLD->Int		(basinRec));	break;
					case DBTableFieldFloat:		outFLD->Float 	(itemRec,inFLD->Float	(basinRec));	break;
					case DBTableFieldDate:		outFLD->Date 	(itemRec,inFLD->Date		(basinRec));	break;
					default: fprintf (stderr,"Invalid field type in: RGlibNetworkToBasinGrid ()\n");	break;
					}
				}
			}
		}
	symTable->DeleteAll ();
	for (symbol = 0;symbol < symMax;++symbol)
		{
		sprintf (symbolName,"Symbol %2d",symbol + 1);
		symRec = symTable->Add (symbolName);
		gFGSymFLD->Int (symRec,7 + symbol);
		gBGSymFLD->Int (symRec,symbol);
		gStSymFLD->Int (symRec,0);
		}
	for (basinID = 0;basinID < netIO->BasinNum ();++basinID)
		{
		basinRec = basinTable->Item (basinID);
		itemRec	= grdTable->Item (basinID);
		symbol =  colorFLD->Int (basinRec) - 7;
		gSymFLD->Record (itemRec,symTable->Item (symbol));
		}

	for (pos.Row = 0;pos.Row < netIO->RowNum (); pos.Row++)
		for (pos.Col = 0;pos.Col < netIO->ColNum (); pos.Col++) grdIO->Value (pos,DBFault);
	for (cellID = 0;cellID < netIO->CellNum ();++cellID)
		{
		if (DBPause (cellID * 100 / netIO->CellNum ())) goto Stop;
		cellRec = netIO->Cell (cellID);
		if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		grdIO->Value (netIO->CellPosition (cellRec),basinFLD->Int (cellRec) - 1);
		}
Stop:
	delete netIO;
	if (cellID == netIO->CellNum ())
		{ grdIO->DiscreteStats (); delete grdIO; return (DBSuccess); }
	delete grdIO;
	return (DBFault);
	}

DBInt RGlibNetworkStations (DBObjData *netData,DBFloat area, DBFloat tolerance,DBObjData *pntData)

	{
	char name [DBStringLength];
	DBInt cellID;
	DBFloat *areaARR;
	DBObjRecord *cellRec, *toCell;
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBCoordinate coord;
	DBObjTable *items 	= pntData->Table (DBrNItems);
	DBObjTable *symbols	= pntData->Table (DBrNSymbols);
	DBObjTableField *coordField = items->Field (DBrNCoord);
	DBObjTableField *symbolFLD	 = items->Field (DBrNSymbol);
	DBObjTableField *foregroundFLD = symbols->Field (DBrNForeground);
	DBObjTableField *backgroundFLD = symbols->Field (DBrNBackground);
	DBObjTableField *styleFLD = symbols->Field (DBrNStyle);
	DBObjRecord *pntRec, *symRec;
	DBRegion dataExtent;

	symRec = symbols->Add ("Default Symbol");
	foregroundFLD->Int (symRec,1);
	backgroundFLD->Int (symRec,2);
	styleFLD->Int (symRec,0);

	if ((areaARR = (DBFloat *) calloc (netIO->CellNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibNetworkStations ()");
		delete netIO;
		return (DBFault);
		}
	for (cellID = 0;cellID < netIO->CellNum ();++cellID)
		areaARR [cellID] = netIO->CellBasinArea (netIO->Cell (cellID));

	for (cellID = netIO->CellNum () - 1;cellID >= 0;--cellID)
		{
		if (DBPause ((netIO->CellNum () - cellID) * 100 / netIO->CellNum ())) goto Stop;

		if (areaARR [cellID] > area)	toCell = netIO->ToCell (cellRec = netIO->Cell (cellID));
		else	if (areaARR [cellID] > area * (1.0 - tolerance / 100.0))
			{
			if ((toCell = netIO->ToCell (netIO->Cell (cellID))) != (DBObjRecord *) NULL)
				{
				if (areaARR [toCell->RowID ()] < area * (1.0 + tolerance / 100.0))
					cellRec = areaARR [toCell->RowID ()] / area > area / areaARR [cellID] ?
								 netIO->Cell (cellID) : (DBObjRecord *) NULL;
				}
			else	cellRec = netIO->Cell (cellID);
			}
		else	cellRec = (DBObjRecord *) NULL;

		if (cellRec != (DBObjRecord *) NULL)
			{
			if (toCell != (DBObjRecord *) NULL)
				do	areaARR [toCell->RowID ()] -= areaARR [cellID];
				while ((toCell = netIO->ToCell (toCell)) != (DBObjRecord *) NULL);

			sprintf (name,"Point: %d",items->ItemNum ());
			items->Add (name); pntRec = items->Item ();
			coord = netIO->Center (netIO->Cell (cellID));
			coordField->Coordinate (pntRec,coord);
			symbolFLD->Record (pntRec,symRec);
			dataExtent.Expand (coord);
			}
		}
Stop:
	delete netIO;
	free (areaARR);
	pntData->Extent (dataExtent);
	return (cellID >= 0 ? DBFault : DBSuccess);
	}

DBInt RGlibNetworkAccumulate (DBObjData *netData, DBObjData *inGridData, DBObjData *outGridData)

	{
	return (RGlibNetworkAccumulate (netData,inGridData,
			  (DBObjData *) NULL,(DBObjData *) NULL,(char **) NULL,0.000001,true,false,true,outGridData));
	}

class RGlibNetAccum
	{
	public:
		DBFloat Correction;
		DBObjTableField *StnIDFLD;
		DBObjTableField *AreaFLD;
		DBObjTableField *DischFLD;
		DBGridIO *GridIO;
		DBObjRecord *LayerRec;
	};

static DBInt _RGlibUpStreamACTION (DBNetworkIO *netIO,DBObjRecord *cellRec,RGlibNetAccum *netAccum)

	{
	DBFloat value, obsVal;
	if ((cellRec->Flags () & DBObjectFlagProcessed) == DBObjectFlagProcessed) return (true);
	if (netAccum->StnIDFLD->Int (cellRec) != DBFault) return (false);
	if (netAccum->GridIO->Value (netAccum->LayerRec,netIO->CellPosition (cellRec),&value))
		{
		obsVal = netAccum->DischFLD->Float (cellRec);
		value = (value - obsVal) * netAccum->Correction + obsVal;
		netAccum->GridIO->Value (netAccum->LayerRec,netIO->CellPosition (cellRec),value);
		}
	return (true);
	}

static DBInt _RGlibUniformACTION (DBNetworkIO *netIO,DBObjRecord *cellRec,RGlibNetAccum *netAccum)

	{
	DBFloat value;
	if ((cellRec->Flags () & DBObjectFlagProcessed) == DBObjectFlagProcessed) return (true);
	if (netAccum->StnIDFLD->Int (cellRec) != DBFault) return (false);
	if (netAccum->GridIO->Value (netAccum->LayerRec,netIO->CellPosition (cellRec),&value))
		{
		value = value + netAccum->AreaFLD->Float (cellRec) * netAccum->Correction;
		netAccum->GridIO->Value (netAccum->LayerRec,netIO->CellPosition (cellRec),value);
		}
	return (true);
	}

static DBInt _RGlibMainstemACTION (DBNetworkIO *netIO,DBObjRecord *cellRec,RGlibNetAccum *netAccum)

	{
	DBFloat value;
	if ((cellRec->Flags () & DBObjectFlagProcessed) == DBObjectFlagProcessed) return (true);
	if (netAccum->StnIDFLD->Int (cellRec) != DBFault) return (false);
	if ((cellRec->Flags () & DBObjectFlagLocked) != DBObjectFlagLocked) return (false);

	value = netAccum->DischFLD->Float (cellRec);
	value = value + netAccum->AreaFLD->Float (cellRec) * netAccum->Correction;
	netAccum->GridIO->Value (netAccum->LayerRec,netIO->CellPosition (cellRec),value);
	return (true);
	}

DBInt RGlibNetworkAccumulate (DBObjData *netData,
										DBObjData *inGridData,
										DBObjData *stnData,
										DBObjData *disData,
										char *fields [],
										DBFloat coeff,
										bool areaMult,
										bool correction,
										bool allowNegative,
										DBObjData *outGridData)

	{
	DBInt layerID, layerNum = 0, progress = 0, maxProgress;
	DBInt cellID, pointID, fieldID, disID;
	DBFloat value, obsVal, upObsVal, accumVal;
	DBPosition pos;
	DBCoordinate coord;
	DBDate date;
	DBObjRecord *outLayerRec, *cellRec, *toCellRec, *nextCellRec, *layerRec, *pointRec, *dischRec;
	DBGridIO *inGridIO;
	DBVPointIO *stnIO = (DBVPointIO *) NULL;
	DBNetworkIO *netIO;
	DBObjTable *stnTable, *disTable, *cellTable = netData->Table (DBrNCells);
	DBObjTableField *relateFLD    = (DBObjTableField *) NULL;
	DBObjTableField *nextStnFLD   = (DBObjTableField *) NULL;
	DBObjTableField *joinFLD      = (DBObjTableField *) NULL;
	DBObjTableField *dateFLD      = (DBObjTableField *) NULL;
	DBObjTableField *dischargeFLD = (DBObjTableField *) NULL;
	DBObjTableField *tmpDischFLD  = (DBObjTableField *) NULL;
	RGlibNetAccum netAccum;

	if (stnData != (DBObjData *) NULL)
		{
		if (disData == (DBObjData *) NULL) return (DBFault);
		if (stnData->Type () != DBTypeVectorPoint)
			{ fprintf (stderr,"Invalid station data\n"); return (DBFault); }
/*		if (disData->Type () != DBTypeTable)
			{ fprintf (stderr,"Invalid time series data\n"); return (DBFault); }
*/		stnTable = stnData->Table (DBrNItems);
		disTable = disData->Table (DBrNItems);
		if ((fields [0] != (char *) NULL) &&
			((relateFLD = stnTable->Field (fields [0])) == (DBObjTableField *) NULL))
			{ fprintf (stderr,"Invalid relate field [%s]!\n",fields [0]); return (DBFault); }
		nextStnFLD = stnTable->Field (fields [1] == (char *) NULL ?  RGlibNextStation : fields [1]);
		if (nextStnFLD == (DBObjTableField *) NULL)
			{ fprintf (stderr,"Invalid next station field!\n"); return (DBFault); }
		if (nextStnFLD->Type () != DBVariableInt)
			{ fprintf (stderr,"Invalid next station field\n"); return (DBFault); }
		if ((fields [2] != (char *) NULL) &&
			((joinFLD = disTable->Field (fields [2])) == (DBObjTableField *) NULL))
			{ fprintf (stderr,"Invalid join field [%s]!\n",fields [2]); return (DBFault); }
		if ((relateFLD != (DBObjTableField *) NULL) && (DBTableFieldIsCategory (relateFLD) != true))
			{ fprintf (stderr,"Invalid relate field type!\n"); return (DBFault); }
		if ((joinFLD != (DBObjTableField *) NULL) && (DBTableFieldIsCategory (joinFLD) != true))
			{ fprintf (stderr,"Invalid join field type!\n"); return (DBFault); }
		if ((relateFLD != (DBObjTableField *) NULL) &&
			 (joinFLD   != (DBObjTableField *) NULL) &&
			 (relateFLD->Type () != joinFLD->Type ()))
			{ fprintf (stderr,"Relate and join fields have different types!\n"); return (DBFault); }
		if ((relateFLD != (DBObjTableField *) NULL) &&
			 (joinFLD == (DBObjTableField *) NULL) &&
			 (relateFLD->Type () != DBVariableString))
			{ fprintf (stderr,"Invalid relate field type!\n"); return (DBFault); }
		if ((relateFLD == (DBObjTableField *) NULL) &&
			 (joinFLD != (DBObjTableField *) NULL) &&
			 (joinFLD->Type () != DBVariableString))
			{ fprintf (stderr,"Invalid join field type!\n"); return (DBFault); }

		if (fields [3] != (char *) NULL)
			{
			if ((dateFLD = disTable->Field (fields [3])) == (DBObjTableField *) NULL)
				{ fprintf (stderr,"Invalid date field [%s]!\n",fields [3]); return (DBFault); }
			}
		else
			{
			for (fieldID = 0;disTable->FieldNum ();++fieldID)
				{
				dateFLD = disTable->Field (fieldID);
				if (dateFLD->Type () == DBVariableDate) break;
				}
			if (fieldID == disTable->FieldNum ())
				{ fprintf (stderr,"No date field in time series!\n"); return (DBFault); }
			}
		if (fields [4] != (char *) NULL)
			{
			if ((dischargeFLD = disTable->Field (fields [4])) == (DBObjTableField *) NULL)
				{ fprintf (stderr,"Invalid discharge field [%s]!\n",fields [4]); return (DBFault); }
			}
		else
			{
			for (fieldID = 0;disTable->FieldNum ();++fieldID)
				{
				dischargeFLD = disTable->Field (fieldID);
				if (dischargeFLD->Type () == DBVariableFloat) break;
				}
			if (fieldID == disTable->FieldNum ())
				{ fprintf (stderr,"No discharge field in time series!\n"); return (DBFault); }
			}
		stnIO = new DBVPointIO (stnData);

		tmpDischFLD  = new DBObjTableField ("_tempDisch_", DBVariableFloat,"%8.2f",sizeof(DBFloat4),false);
		stnTable->AddField (tmpDischFLD);
		disTable->ListSort (dateFLD);
		netAccum.StnIDFLD = new DBObjTableField("_tempID_",DBVariableInt,"%8d",sizeof(DBInt),false);
		cellTable->AddField (netAccum.StnIDFLD);
		netAccum.AreaFLD = new DBObjTableField("_tempArea_",DBVariableFloat,"%8.2f",sizeof(DBFloat),false);
		cellTable->AddField (netAccum.AreaFLD);
		netAccum.DischFLD = new DBObjTableField("_tempDisch_",DBVariableFloat,"%8.2f",sizeof(DBFloat),false);
		cellTable->AddField (netAccum.DischFLD);
		dischRec = disTable->First ();
		}

	inGridIO = new DBGridIO (inGridData);
	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		layerRec = inGridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{
		fprintf (stderr,"No Layer to Process in RGlibNetworkAccumulate ()\n");
		delete inGridIO; return (DBFault);
		}

	netIO 	= new DBNetworkIO (netData);
	netAccum.GridIO = new DBGridIO (outGridData);

	outLayerRec = netAccum.GridIO->Layer ((DBInt) 0);
	maxProgress = layerNum * netIO->RowNum ();

	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		layerRec = inGridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		netAccum.GridIO->RenameLayer (outLayerRec,layerRec->Name ());
		for (pos.Row = 0;pos.Row < netIO->RowNum ();pos.Row++)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < netIO->ColNum ();pos.Col++)
				if ((cellRec = netIO->Cell (pos)) == (DBObjRecord *) NULL)
					netAccum.GridIO->Value (outLayerRec,pos,DBDefaultMissingFloatVal);
				else
					{
					if (inGridIO->Value (layerRec,netIO->Center (cellRec),&value))
						{
						value = (areaMult ? value * netIO->CellArea (cellRec) : value) * coeff;
						netAccum.GridIO->Value (outLayerRec,pos,value);
						}
					else netAccum.GridIO->Value (outLayerRec,pos,0.0);
					}
			}
		if (stnIO != (DBVPointIO *) NULL)
			{
			date.Set (layerRec->Name ());
			for (cellID = 0;cellID < netIO->CellNum ();++cellID)
				{
				cellRec = netIO->Cell (cellID);
				cellRec->Flags (DBObjectFlagLocked,   DBClear);
				cellRec->Flags (DBObjectFlagProcessed,DBClear);
				netAccum.StnIDFLD->Int (cellRec,DBFault);
				netAccum.AreaFLD->Float  (cellRec,0.0);
				netAccum.DischFLD->Float (cellRec,0.0);
				}

			if (dischRec == (DBObjRecord *) NULL) dischRec = disTable->First ();
			else
				{
				disID = dischRec->RowID ();
				for ( ;dischRec != (DBObjRecord *) NULL;dischRec = disTable->Next ())
					{ if (date == dateFLD->Date (dischRec)) break; }
				if (dischRec == (DBObjRecord *) NULL)
					{
					for (dischRec = disTable->First () ;
						  (dischRec != (DBObjRecord *) NULL) && (dischRec->RowID () != disID);
						  dischRec = disTable->Next ())
						{ if (date == dateFLD->Date (dischRec)) break; }
					}
				}
			if (dischRec != (DBObjRecord *) NULL)
				{
				for (pointID = 0;pointID < stnIO->ItemNum ();++pointID)
					{
					pointRec = stnIO->Item (pointID);
					tmpDischFLD->Float  (pointRec,tmpDischFLD->FloatNoData ());
					}
				for ( ;dischRec != (DBObjRecord *) NULL;dischRec = disTable->Next ())
					{
					if (date != dateFLD->Date (dischRec)) break;

					for (pointID = 0;pointID < stnIO->ItemNum ();++pointID)
						{
						pointRec = stnIO->Item (pointID);
						if (DBTableFieldMatch (relateFLD,pointRec,joinFLD,dischRec)) break;
						}
					if (pointID == stnIO->ItemNum ()) continue;

					coord = stnIO->Coordinate (pointRec);
					if ((cellRec = netIO->Cell (coord)) == (DBObjRecord *) NULL) continue;
					value = dischargeFLD->Float (dischRec);
					if (CMmathEqualValues (value,dischargeFLD->FloatNoData ()) == false)
						{
						tmpDischFLD->Float  (pointRec,value);
						netAccum.StnIDFLD->Int (cellRec,pointRec->RowID ());
						}
					}
				}
			for (cellID = netIO->CellNum () - 1;cellID >= 0;--cellID)
				{
				cellRec = netIO->Cell (cellID);
				obsVal  = netIO->CellArea (cellRec);
				for (nextCellRec = cellRec;
					  nextCellRec != (DBObjRecord *) NULL;
					  nextCellRec = netIO->ToCell (nextCellRec))
					{
					netAccum.AreaFLD->Float (nextCellRec,netAccum.AreaFLD->Float (nextCellRec) + obsVal);
					if (netAccum.StnIDFLD->Int (nextCellRec) != DBFault) break;
					}
				}
			}

		for (cellID = netIO->CellNum () - 1;cellID >= 0;--cellID)
			{
			cellRec = netIO->Cell (cellID);
			if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if ((netAccum.GridIO->Value (outLayerRec,netIO->CellPosition (cellRec),&value) == false) ||
				 ((allowNegative == false) && (value < 0.0)))
				{
				value = 0.0;
				netAccum.GridIO->Value (outLayerRec,netIO->CellPosition (cellRec),value);
				}
			if ((toCellRec = netIO->ToCell (cellRec)) == (DBObjRecord *) NULL) continue;
			if (netAccum.GridIO->Value (outLayerRec,netIO->CellPosition (toCellRec),&accumVal) == false)
				accumVal = 0.0;
			if ((stnIO != (DBVPointIO *) NULL) && ((pointID = netAccum.StnIDFLD->Int (cellRec)) != DBFault))
				{
				pointRec = stnIO->Item (pointID);
				obsVal = tmpDischFLD->Float (pointRec);
				if (correction)
					{
					for (nextCellRec = netIO->ToCell (cellRec);
						  nextCellRec != (DBObjRecord *) NULL;
						  nextCellRec = netIO->ToCell (nextCellRec))
						{
						nextCellRec->Flags (DBObjectFlagLocked,DBSet);
						netAccum.DischFLD->Float (nextCellRec,netAccum.DischFLD->Float (nextCellRec) + obsVal);
						if (netAccum.StnIDFLD->Int (nextCellRec) != DBFault) break;
						}
					upObsVal = netAccum.DischFLD->Float (cellRec);
					netAccum.LayerRec = outLayerRec;
					cellRec->Flags (DBObjectFlagProcessed,DBSet);
					if (obsVal > upObsVal)
						{
						if (value > upObsVal)
							{
							netAccum.Correction = (obsVal - upObsVal) / (value - upObsVal);
							netIO->UpStreamSearch (cellRec,(DBNetworkACTION) _RGlibUpStreamACTION,
														 	(void *) &netAccum);
							}
						else
							{
							netAccum.Correction = (obsVal - value) / netAccum.AreaFLD->Float (cellRec);
							netIO->UpStreamSearch (cellRec,(DBNetworkACTION) _RGlibUniformACTION,
															(void *) &netAccum);
							}
						}
					else
						{
						netAccum.Correction = (obsVal - upObsVal) / netAccum.AreaFLD->Float (cellRec);
						netIO->UpStreamSearch (cellRec,(DBNetworkACTION)_RGlibMainstemACTION,(void*) &netAccum);
						}
					cellRec->Flags (DBObjectFlagProcessed,DBClear);
					}
				value = obsVal;
				netAccum.GridIO->Value (outLayerRec,netIO->CellPosition (cellRec),value);
				}
			accumVal = accumVal + value;
			netAccum.GridIO->Value (outLayerRec,netIO->CellPosition (toCellRec),accumVal);
			}
		netAccum.GridIO->RecalcStats (outLayerRec);
		if (netAccum.GridIO->LayerNum () < layerNum) outLayerRec = netAccum.GridIO->AddLayer ((char *) "Next Layer");
		}
Stop:
	outGridData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
	outGridData->Flags (DBDataFlagDispModeContBlueScale,DBSet);
	if (stnIO != (DBVPointIO *) NULL)
		{
		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{ cellRec = netIO->Cell (cellID); cellRec->Flags (DBObjectFlagLocked,DBClear); }
		cellTable->DeleteField (netAccum.StnIDFLD);
		cellTable->DeleteField (netAccum.AreaFLD);
		cellTable->DeleteField (netAccum.DischFLD);
		stnTable->DeleteField (tmpDischFLD);
		delete stnIO;
		}
	delete netIO;
	delete inGridIO;
	delete netAccum.GridIO;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibNetworkUnaccumulate (DBObjData *netData, DBObjData *inGridData, DBFloat coeff, bool areaDiv, DBObjData *outGridData)

	{
	DBInt layerID, progress = 0, maxProgress;
	DBInt cellID;
	DBFloat value, unAccum;
	DBPosition pos;
	DBObjRecord *cellRec, *toCell, *inLayerRec, *outLayerRec;
	DBGridIO *inGridIO  = new DBGridIO (inGridData);
	DBGridIO *outGridIO = new DBGridIO (outGridData);
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	maxProgress = inGridIO->LayerNum () * netIO->CellNum ();
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

		value = outGridIO->MissingValue ();
		for (pos.Col = 0;pos.Col < netIO->ColNum ();++pos.Col)
			for (pos.Row = 0;pos.Row < netIO->RowNum ();++pos.Row) outGridIO->Value (outLayerRec,pos,value);

		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{
			progress = layerID * netIO->CellNum () + cellID;
			if (DBPause (progress * 100 / maxProgress)) goto Stop;

			cellRec = netIO->Cell (cellID);
			if (inGridIO->Value (inLayerRec,netIO->Center (cellRec),&value) == false) continue;
			outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),value);

			if (((toCell = netIO->ToCell (cellRec)) == (DBObjRecord *) NULL) ||
				 (outGridIO->Value (outLayerRec,netIO->CellPosition (toCell),&unAccum) == false))  continue;
			unAccum = unAccum - value;
			outGridIO->Value (outLayerRec,netIO->CellPosition (toCell),unAccum);
			}
		if (areaDiv)
			for (cellID = 0;cellID < netIO->CellNum ();++cellID)
				{
				cellRec = netIO->Cell (cellID);
				outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),&value);
				value = coeff * value / netIO->CellArea (cellRec);
				outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),value);
				}
		else
			for (cellID = 0;cellID < netIO->CellNum ();++cellID)
				{
				cellRec = netIO->Cell (cellID);
				outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),&value);
				value = coeff * value;
				outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),value);
				}

		}
	outGridIO->RecalcStats ();
Stop:
	return (progress + 1 == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibNetworkInvAccumulate (DBObjData *netData, DBObjData *inGridData, DBObjData *outGridData, DBFloat coeff)

	{
	DBInt layerID, progress = 0, maxProgress;
	DBInt cellID;
	DBFloat value, accumVal;
	DBPosition pos;
	DBObjRecord *cellRec, *toCell, *inLayerRec, *outLayerRec;
	DBGridIO *inGridIO  = new DBGridIO (inGridData);
	DBGridIO *outGridIO = new DBGridIO (outGridData);
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	maxProgress = inGridIO->LayerNum () * netIO->CellNum ();
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

		accumVal = outGridIO->MissingValue ();
		for (pos.Col = 0;pos.Col < netIO->ColNum ();++pos.Col)
			for (pos.Row = 0;pos.Row < netIO->RowNum ();++pos.Row) outGridIO->Value (outLayerRec,pos,accumVal);

		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{
			progress = layerID * netIO->CellNum () + cellID;
			if (DBPause (progress * 100 / maxProgress)) goto Stop;

			cellRec = netIO->Cell (cellID);
			accumVal = 0.0;
			for (toCell = cellRec;toCell != (DBObjRecord *) NULL;toCell = netIO->ToCell (toCell))
				{
				if (inGridIO->Value (inLayerRec,netIO->Center (toCell),&value) == false) continue;
				else	accumVal = accumVal + value * coeff;
				}
			outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),accumVal);
			}
		}
	outGridIO->RecalcStats ();
Stop:
	return (progress + 1 == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibNetworkUpStreamAvg (DBObjData *netData,DBObjData *inGridData,DBObjData *outGridData)

	{
	DBInt layerID, layerNum = 0, progress = 0, maxProgress, cellID;
	DBFloat value, accumVal, *upstreamArea;
	DBPosition pos;
	DBObjRecord *outLayerRec, *cellRec, *toCellRec, *layerRec;
	DBGridIO *inGridIO, *outGridIO;
	DBNetworkIO *netIO;

	inGridIO = new DBGridIO (inGridData);
	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		layerRec = inGridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibNetworkUpStreamAvg ()\n"); delete inGridIO; return (DBFault); }
	netIO 	= new DBNetworkIO (netData);
	outGridIO = new DBGridIO (outGridData);
	outGridIO->MissingValue (inGridIO->MissingValue ());

	outLayerRec = outGridIO->Layer ((DBInt) 0);
	maxProgress = layerNum * netIO->RowNum ();
	if ((upstreamArea = (DBFloat *) calloc (netIO->CellNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibNetworkUpStreamAvg ()");
		delete netIO;
		delete outGridIO;
		delete inGridIO;
		return (DBFault);
		}

	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		layerRec = inGridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;

		outGridIO->RenameLayer (outLayerRec,layerRec->Name ());
		for (cellID = 0;cellID < netIO->CellNum ();cellID++) upstreamArea [cellID] = 0.0;
		for (pos.Row = 0;pos.Row < netIO->RowNum ();pos.Row++)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < netIO->ColNum ();pos.Col++)
				if ((cellRec = netIO->Cell (pos)) == (DBObjRecord *) NULL)
					outGridIO->Value (outLayerRec,pos,outGridIO->MissingValue ());
				else
					{
					if (inGridIO->Value (layerRec,netIO->Center (cellRec),&value))
						{
						outGridIO->Value (outLayerRec,pos,value * netIO->CellArea (cellRec));
						upstreamArea [cellRec->RowID ()] = netIO->CellArea (cellRec);
						}
					else
						{
						outGridIO->Value (outLayerRec,pos,0.0);
						upstreamArea [cellRec->RowID ()] = 0.0;
						}
					}
			}
		for (cellID = netIO->CellNum () - 1;cellID >= 0;--cellID)
			{
			cellRec = netIO->Cell (cellID);
			if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if (outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),&value) == false)
				{ fprintf (stderr,"Total metal Gebasz in: RGlibNetworkUpStreamAvg ()\n"); value = 0.0; }
			if ((toCellRec = netIO->ToCell (cellRec)) != (DBObjRecord *) NULL)
				{
				if (outGridIO->Value (outLayerRec,netIO->CellPosition (toCellRec),&accumVal) == false) accumVal = 0.0;
				outGridIO->Value (outLayerRec,netIO->CellPosition (toCellRec),accumVal + value);
				upstreamArea [toCellRec->RowID ()] += upstreamArea [cellID];
				}
			if (upstreamArea [cellID] > 0.0)
					outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),value / upstreamArea [cellID]);
			else	outGridIO->Value (outLayerRec,netIO->CellPosition (cellRec),DBDefaultMissingFloatVal);
			}
		outGridIO->RecalcStats (outLayerRec);
		if (outGridIO->LayerNum () < layerNum) outLayerRec = outGridIO->AddLayer ((char *) "Next Layer");
		}
Stop:
	outGridData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
	outGridData->Flags (DBDataFlagDispModeContBlueScale,DBSet);
	free (upstreamArea);
	delete netIO;
	delete outGridIO;
	delete inGridIO;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibNetworkCellSlopes (DBObjData *netData,DBObjData *inGridData,DBObjData *outGridData)

	{
	DBInt layerID, layerNum = 0, progress = 0, maxProgress;
	DBFloat value, slope;
	DBPosition pos;
	DBCoordinate coord0, coord1;
	DBObjRecord *outLayerRec, *cellRec, *layerRec;
	DBGridIO *inGridIO, *outGridIO;
	DBNetworkIO *netIO;
	DBMathDistanceFunction distFunc = DBMathGetDistanceFunction (netData);

	inGridIO = new DBGridIO (inGridData);
	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		layerRec = inGridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibNetworkCellSlopes ()\n"); delete inGridIO; return (DBFault); }

	netIO 	= new DBNetworkIO (netData);
	outGridIO = new DBGridIO (outGridData);

	outLayerRec = outGridIO->Layer ((DBInt) 0);

	maxProgress = layerNum * netIO->RowNum ();

	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		layerRec = inGridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;

		outGridIO->RenameLayer (outLayerRec,layerRec->Name ());
		for (pos.Row = 0;pos.Row < netIO->RowNum ();pos.Row++)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < netIO->ColNum ();pos.Col++)
				if ((cellRec = netIO->Cell (pos)) == (DBObjRecord *) NULL)
					outGridIO->Value (outLayerRec,pos,DBDefaultMissingFloatVal);
				else
					{
					coord0 = netIO->Center (cellRec);
					if (inGridIO->Value (layerRec,coord0,&slope))
						{
						coord1 = coord0 + netIO->Delta (cellRec);
						if ((coord0.X == coord1.X) && (coord0.Y == coord1.Y))
							outGridIO->Value (outLayerRec,pos,(DBFloat) RGlibMinSLOPE);
						else
							{
							if (inGridIO->Value (layerRec,coord1,&value))
								{
								slope = (slope - value) / DBMathCoordinateDistance (distFunc,coord0,coord1);
								outGridIO->Value (outLayerRec,pos,slope);
								}
							else	outGridIO->Value (outLayerRec,pos,(DBFloat) RGlibMinSLOPE);
							}
						}
					else	outGridIO->Value (outLayerRec,pos,DBDefaultMissingFloatVal);
					}
			}
		outGridIO->RecalcStats (outLayerRec);
		if (outGridIO->LayerNum () < layerNum) outLayerRec = outGridIO->AddLayer ((char *) "Next Layer");
		}
Stop:
	outGridData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
	outGridData->Flags (DBDataFlagDispModeContGreyScale,DBSet);
	delete netIO;
	delete outGridIO;
	delete inGridIO;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibNetworkBasinProf (DBObjData *netData,DBObjData *gridData,DBObjData *tblData)

	{
	DBInt layerID, layerNum = 0, basinID, ret;
	DBFloat dist, value;
	DBCoordinate coord;
	DBNetworkIO *netIO;
	DBGridIO *gridIO;
	DBObjTable *table;
	DBObjTableField *basinFLD;
	DBObjTableField *xCoordFLD;
	DBObjTableField *yCoordFLD;
	DBObjTableField *distFLD;
	DBObjTableField *valueFLD;
	DBObjectLIST<DBObjTableField> *fields;
	DBObjRecord *basinRec, *cellRec, *layerRec, *tblRec;

	gridIO = new DBGridIO (gridData);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibNetworkBasinProf ()\n"); delete gridIO; return (DBFault); }

	table = tblData->Table (DBrNItems);
	fields = table->Fields ();
	netIO = new DBNetworkIO (netData);
	table->AddField (basinFLD = new DBObjTableField (DBrNBasin,DBTableFieldInt,"%8d",sizeof (DBInt)));
	table->AddField (xCoordFLD = new DBObjTableField ("XCoord",DBTableFieldFloat,"%8.3f",sizeof (DBFloat)));
	table->AddField (yCoordFLD = new DBObjTableField ("YCoord",DBTableFieldFloat,"%8.3f",sizeof (DBFloat)));
	table->AddField (distFLD = new DBObjTableField ("Distance",DBTableFieldFloat,"%8.0f",sizeof (DBFloat)));

	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		table->AddField (valueFLD = new DBObjTableField (layerRec->Name (),DBTableFieldFloat,"%8.3f",sizeof (DBFloat)));
		valueFLD->FloatNoData (gridIO->MissingValue (layerRec));
		}

	for (basinID = 0;basinID < netIO->BasinNum ();++basinID)
		{
		basinRec = netIO->Basin (basinID);
		dist = 0.0;
		if (DBPause (basinID * 100 / netIO->BasinNum ())) goto Stop;
		for (cellRec = netIO->MouthCell (basinRec);cellRec != (DBObjRecord *) NULL;cellRec = netIO->FromCell (cellRec))
			{
			coord = netIO->Center (cellRec);
			tblRec = table->Add (basinRec->Name ());
			basinFLD->Int (tblRec,basinRec->RowID () + 1);
			xCoordFLD->Float (tblRec,coord.X);
			yCoordFLD->Float (tblRec,coord.Y);
			dist += netIO->CellLength (cellRec);
			distFLD->Float (tblRec,dist);
			valueFLD = fields->First ();
			valueFLD = fields->Next ();
			valueFLD = fields->Next ();
			valueFLD = fields->Next ();
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				valueFLD = fields->Next ();
				if (gridIO->Value (layerRec,coord,&value)) valueFLD->Float (tblRec,value);
				else valueFLD->Float (tblRec,valueFLD->FloatNoData ());
				}
			}
		}
Stop:
	ret = basinID == netIO->BasinNum () ? DBSuccess : DBFault;
	delete netIO;
	delete gridIO;
	return (ret);
	}

#define RGISStatMin		"StatMinimum"
#define RGISStatMax		"StatMaximum"
#define RGISStatMean		"StatMean"
#define RGISStatStdDev	"StatStdDev"
#define RGISStatArea		"StatArea"

DBInt RGlibNetworkBasinStats (DBObjData *netData, DBObjData *grdData, DBObjData *tblData)

	{
	DBInt layerID, layerNum = 0, cellID, basinID, progress = 0, maxProgress;
	DBFloat minimum, maximum, average, stdDev, area, value;
	DBObjTable *table;
	DBObjTableField *basinIDFLD;
	DBObjTableField *layerIDFLD;
	DBObjTableField *layerNameFLD;
	DBObjTableField *minimumFLD;
	DBObjTableField *maximumFLD;
	DBObjTableField *averageFLD;
	DBObjTableField *stdDevFLD;
	DBObjTableField *areaFLD;
	DBNetworkIO *netIO;
	DBGridIO 	*gridIO;
	DBObjRecord *layerRec, *cellRec, *basinRec, *tblRec;
	DBObjectLIST<DBObjTableField> *fields;

	gridIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibNetworkBasinStats ()\n"); delete gridIO; return (DBFault); }

	table = tblData->Table (DBrNItems);
	netIO = new DBNetworkIO (netData);
	table->AddField (basinIDFLD	= new DBObjTableField (DBrNBasin,		DBTableFieldInt,		"%8d",						sizeof (DBInt)));
	table->AddField (layerIDFLD	= new DBObjTableField ("LayerID",		DBTableFieldInt,		"%4d",						sizeof (DBShort)));
	table->AddField (layerNameFLD	= new DBObjTableField ("LayerName",		DBTableFieldString,	"%s",							DBStringLength));
	table->AddField (averageFLD	= new DBObjTableField (RGISStatMean,	DBTableFieldFloat,	gridIO->ValueFormat (),	sizeof (DBFloat4)));
	table->AddField (minimumFLD	= new DBObjTableField (RGISStatMin,		DBTableFieldFloat,	gridIO->ValueFormat (),	sizeof (DBFloat4)));
	table->AddField (maximumFLD	= new DBObjTableField (RGISStatMax,		DBTableFieldFloat,	gridIO->ValueFormat (),	sizeof (DBFloat4)));
	table->AddField (stdDevFLD		= new DBObjTableField (RGISStatStdDev,	DBTableFieldFloat,	gridIO->ValueFormat (),	sizeof (DBFloat4)));
	table->AddField (areaFLD		= new DBObjTableField (RGISStatArea,	DBTableFieldFloat,	gridIO->ValueFormat (),	sizeof (DBFloat4)));

	maxProgress = netIO->CellNum () * layerNum;
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		basinID = DBFault;
		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{
			cellRec = netIO->Cell (cellID);
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if (basinID != netIO->CellBasinID (cellRec))
				{
				if (basinID != DBFault)
					{
					average = average / area;
					stdDev = stdDev / area;
					stdDev = stdDev - average * average;
					stdDev = sqrt (stdDev);
					minimumFLD->Float (tblRec,minimum);
					maximumFLD->Float (tblRec,maximum);
					averageFLD->Float (tblRec,average);
					stdDevFLD->Float  (tblRec,stdDev);
					areaFLD->Float		(tblRec,area);
					}
				basinID = netIO->CellBasinID (cellRec);
				basinRec = netIO->Basin (cellRec);
				tblRec = table->Add (basinRec->Name ());
				basinIDFLD->Int (tblRec,basinID);
				layerIDFLD->Int (tblRec,layerRec->RowID ());
				layerNameFLD->String (tblRec,layerRec->Name ());
				area = 0.0;
				minimum =  DBHugeVal;
				maximum = -DBHugeVal;
				average = 0.0;
				stdDev = 0.0;
				}
			if (gridIO->Value (layerRec,netIO->Center (cellRec),&value) == true)
				{
				average = average + value * netIO->CellArea (cellRec);
				minimum = minimum < value ? minimum : value;
				maximum = maximum > value ? maximum : value;
				stdDev = stdDev + value * value * netIO->CellArea (cellRec);
				area = area + netIO->CellArea (cellRec);
				}
			}
		average = average / area;
		stdDev = stdDev / area;
		stdDev = stdDev - average * average;
		stdDev = sqrt (stdDev);
		minimumFLD->Float (tblRec,minimum);
		maximumFLD->Float (tblRec,maximum);
		averageFLD->Float (tblRec,average);
		stdDevFLD->Float	(tblRec,stdDev);
		areaFLD->Float		(tblRec,area);
		}
Stop:
	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*basinIDFLD));
	fields->Add (new DBObjTableField (*layerIDFLD));
	table->ListSort (fields);
	delete fields;
	delete netIO;
	delete gridIO;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

#define RGISHeadMin			"HeadMinimum"
#define RGISHeadMax			"HeadMaximum"
#define RGISHeadMean			"HeadMean"
#define RGISHeadStdDev		"HeadStdDev"

#define RGISDivideMin		"DivideMinimum"
#define RGISDivideMax		"DivideMaximum"
#define RGISDivideMean		"DivideMean"
#define RGISDivideStdDev	"DivideStdDev"

DBInt RGlibNetworkHeadStats (DBObjData *netData, DBObjData *grdData, DBObjData *tblData,DBInt divideOnly)

	{
	DBInt layerID, layerNum = 0, cellID, basinID, progress = 0, maxProgress, dir, divide;
	DBFloat minimum, maximum, average, stdDev, value, area;
	DBObjTable *table;
	DBObjTableField *basinIDFLD;
	DBObjTableField *layerIDFLD;
	DBObjTableField *layerNameFLD;
	DBObjTableField *minimumFLD;
	DBObjTableField *maximumFLD;
	DBObjTableField *averageFLD;
	DBObjTableField *stdDevFLD;
	DBNetworkIO *netIO;
	DBGridIO *gridIO;
	DBObjRecord *layerRec, *cellRec, *fromCell, *basinRec, *tblRec;
	DBObjectLIST<DBObjTableField> *fields;

	gridIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibNetworkHeadStats ()\n"); delete gridIO; return (DBFault); }

	table = tblData->Table (DBrNItems);
	netIO = new DBNetworkIO (netData);
	table->AddField (basinIDFLD = new DBObjTableField (DBrNBasin,DBTableFieldInt,"%8d",sizeof (DBInt)));
	table->AddField (layerIDFLD = new DBObjTableField ("LayerID",DBTableFieldInt,"%4d",sizeof (DBShort)));
	table->AddField (layerNameFLD = new DBObjTableField ("LayerName",DBTableFieldString,"%s",DBStringLength));
	table->AddField (averageFLD = new DBObjTableField (divideOnly ? RGISDivideMean	: RGISHeadMean,	DBTableFieldFloat,gridIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (minimumFLD = new DBObjTableField (divideOnly ? RGISDivideMin		: RGISHeadMin,		DBTableFieldFloat,gridIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (maximumFLD = new DBObjTableField (divideOnly ? RGISDivideMax		: RGISHeadMax,		DBTableFieldFloat,gridIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (stdDevFLD  = new DBObjTableField (divideOnly ? RGISDivideStdDev : RGISHeadStdDev,	DBTableFieldFloat,gridIO->ValueFormat (),sizeof (DBFloat4)));

	maxProgress = netIO->CellNum () * layerNum;
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		basinID = DBFault;
		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{
			cellRec = netIO->Cell (cellID);
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if (basinID != netIO->CellBasinID (cellRec))
				{
				if (basinID != DBFault)
					{
					average = average / area;
					stdDev = stdDev / area;
					stdDev = stdDev - average * average;
					stdDev = sqrt (stdDev);
					minimumFLD->Float (tblRec,minimum);
					maximumFLD->Float (tblRec,maximum);
					averageFLD->Float (tblRec,average);
					stdDevFLD->Float  (tblRec,stdDev);
					}
				basinID = netIO->CellBasinID (cellRec);
				basinRec = netIO->Basin (cellRec);
				tblRec = table->Add (basinRec->Name ());
				basinIDFLD->Int (tblRec,basinID);
				layerIDFLD->Int (tblRec,layerRec->RowID ());
				layerNameFLD->String (tblRec,layerRec->Name ());
				area = 0.0;
				minimum = DBHugeVal;
				maximum = -DBHugeVal;
				average = 0.0;
				stdDev = 0.0;
				}
			if ((netIO->CellBasinCells (cellRec) == 1)  && (gridIO->Value (layerRec,netIO->Center (cellRec),&value) == true))
				{
				if (divideOnly)
					{
					divide = false;
					for (dir = 0;dir < 8;++dir)
						if ((fromCell = netIO->FromCell (cellRec,0x01 << dir,false)) == (DBObjRecord *) NULL)
							{ divide = true; break; }
						else
							if (netIO->CellBasinID (fromCell) != basinID) { divide = true; break; }
					}
				else	divide = true;
				if (divide)
					{
					average = average + value * netIO->CellArea (cellRec);
					minimum = minimum < value ? minimum : value;
					maximum = maximum > value ? maximum : value;
					stdDev = stdDev + value * value * netIO->CellArea (cellRec);
					area = area + netIO->CellArea (cellRec);
					}
				}
			}
		average = average / area;
		stdDev = stdDev / area;
		stdDev = stdDev - average * average;
		stdDev = sqrt (stdDev);
		minimumFLD->Float (tblRec,minimum);
		maximumFLD->Float (tblRec,maximum);
		averageFLD->Float (tblRec,average);
		stdDevFLD->Float  (tblRec,stdDev);
		}
Stop:
	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*basinIDFLD));
	fields->Add (new DBObjTableField (*layerIDFLD));
	table->ListSort (fields);
	delete fields;
	delete netIO;
	delete gridIO;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibNetworkHistogram (DBObjData *netData,DBObjData *grdData, DBObjData *tblData)

	{
	char string [DBStringLength];
	DBInt cellID, basinID = DBFault, i = 0, ret = DBSuccess;
	DBFloat area = 0.0, elev;
	DBObjTable *itemTable = tblData->Table (DBrNItems);
	DBObjTableField *basinIDFLD;
	DBObjTableField *areaFLD;
	DBObjTableField *elevationFLD;
	DBObjTableField *percentFLD;
	DBNetworkIO *netIO;
	DBGridIO 	*gridIO;
	DBObjRecord *cellRec, *tblRec;
	DBObjectLIST<DBObjTableField> *fields;

	itemTable->AddField (basinIDFLD = new DBObjTableField (DBrNBasin,DBTableFieldInt,"%8d",sizeof (DBInt)));
	itemTable->AddField (areaFLD = new DBObjTableField ("Area",DBTableFieldFloat,"%10.1f",sizeof (DBFloat4)));
	itemTable->AddField (elevationFLD = new DBObjTableField ("Elevation",DBTableFieldFloat,"%8.1f",sizeof (DBFloat4)));
	itemTable->AddField (percentFLD = new DBObjTableField ("Percent",DBTableFieldFloat,"%6.2f",sizeof (DBFloat4)));
	netIO = new DBNetworkIO (netData);
	gridIO = new DBGridIO (grdData);

	for (cellID = 0;cellID < netIO->CellNum ();++cellID)
		{
		cellRec = netIO->Cell (cellID);
		if (DBPause (cellID * 33 / netIO->CellNum ())) { ret = DBFault; goto Stop; }
		if (gridIO->Value (netIO->Center (cellRec),&elev))
			{
			tblRec = itemTable->Add ();
			basinIDFLD->Int (tblRec,netIO->CellBasinID (cellRec));
			areaFLD->Float (tblRec,netIO->CellArea (cellRec));
			elevationFLD->Float (tblRec,elev);
			}
		}
	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*basinIDFLD));
	fields->Add (new DBObjTableField (*elevationFLD));
	itemTable->ListSort (fields);
	itemTable->ItemSort ();
	delete fields;
	for (tblRec = itemTable->First ();tblRec != (DBObjRecord *) NULL;tblRec = itemTable->Next ())
		{
		if (DBPause (tblRec->RowID () * 33 / itemTable->ItemNum () + 33)) { ret = DBFault; goto Stop; }
		if (basinID != basinIDFLD->Int (tblRec))
			{ area = 0.0; basinID = basinIDFLD->Int (tblRec); }
		area += areaFLD->Float (tblRec);
		areaFLD->Float (tblRec,area);
		}
	for (tblRec = itemTable->Last ();tblRec != (DBObjRecord *) NULL;tblRec = itemTable->Next (DBBackward))
		{
		if (DBPause ((itemTable->ItemNum () - tblRec->RowID ()) * 33 / itemTable->ItemNum () + 66))
			{ ret = DBFault; goto Stop; }
		tblRec->Name (string);
		if (basinID != basinIDFLD->Int (tblRec))
			{ area = areaFLD->Float (tblRec); basinID = basinIDFLD->Int (tblRec); }
		sprintf (string,"Basin:%04d Histogram:%06d",basinID,i++);
		percentFLD->Float (tblRec,100.0 * areaFLD->Float (tblRec) / area);
		}
Stop:
	delete netIO;
	delete gridIO;
	return (ret);
	}

DBInt RGlibNetworkBasinDistrib (DBObjData *netData,DBObjData *grdData, DBObjData *tblData)

	{
	DBInt layerID, layerNum = 0, cellID, basinID, progress = 0, maxProgress;
	DBInt value;
	DBObjTable *table, *gridTable;
	DBObjTableField *gridValueFLD;
	DBObjTableField *basinIDFLD;
	DBObjTableField *layerIDFLD;
	DBObjTableField *layerNameFLD;
	DBObjTableField *categoryIDFLD;
	DBObjTableField *categoryFLD;
	DBObjTableField *percentFLD;
	DBObjTableField *areaFLD;
	DBObjTableField *cellNumFLD;
	DBNetworkIO *netIO;
	DBGridIO 	*gridIO;
	DBObjRecord *layerRec, *gridRec, *cellRec, *basinRec, *tblRec;
	DBObjectLIST<DBObjTableField> *fields;
	class Histogram {
		public:
			DBInt cellNum;
			DBFloat area;
			void Initialize () { cellNum = 0; area = 0.0; }
		} *histogram = (Histogram *) NULL;

	gridIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibNetworkBasinDistrib ()\n"); delete gridIO; return (DBFault); }

	table = tblData->Table (DBrNItems);
	netIO = new DBNetworkIO (netData);
	table->AddField (basinIDFLD	= new DBObjTableField (DBrNBasin,	DBTableFieldInt,	"%8d",	sizeof (DBInt)));
	table->AddField (layerIDFLD	= new DBObjTableField ("LayerID",	DBTableFieldInt,	"%4d",	sizeof (DBShort)));
	table->AddField (layerNameFLD	= new DBObjTableField ("LayerName",	DBTableFieldString,"%s",	DBStringLength));
	table->AddField (categoryIDFLD= new DBObjTableField (DBrNCategoryID,DBTableFieldInt, "%2d",	sizeof (DBShort)));
	table->AddField (categoryFLD	= new DBObjTableField (DBrNCategory,DBTableFieldString,"%s",	DBStringLength));
	table->AddField (percentFLD	= new DBObjTableField (DBrNPercent,	DBTableFieldFloat,"%6.2f",	sizeof (DBFloat4)));
	table->AddField (areaFLD 		= new DBObjTableField (DBrNArea,		DBTableFieldFloat,"%10.1f",sizeof (DBFloat4)));
	table->AddField (cellNumFLD	= new DBObjTableField ("CellNum",	DBTableFieldInt,	"%8d",	sizeof (DBInt)));

	gridTable = grdData->Table (DBrNItems);
	gridValueFLD = gridTable->Field (DBrNGridValue);

	if (histogram == (Histogram *) NULL) histogram = (Histogram *) malloc (gridTable->ItemNum () * sizeof (Histogram));
	else histogram = (Histogram *) realloc (histogram,gridTable->ItemNum () * sizeof (Histogram));
	if (histogram == (Histogram *) NULL)
		{ perror ("Memory Allocation Error in: _RGISAnNetworkBasinDistribCBK ()"); return (DBFault); }
	maxProgress = netIO->CellNum () * layerNum;
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;

		basinID = DBFault;
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{
			cellRec = netIO->Cell (cellID);
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			if ((cellRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if (basinID != netIO->CellBasinID (cellRec))
				{
				if (basinID != DBFault)
					for (gridRec = gridTable->First ();gridRec != (DBObjRecord *) NULL;gridRec = gridTable->Next ())
					if (histogram [gridRec->RowID ()].cellNum > 0)
						{
						tblRec = table->Add (basinRec->Name ());
						basinIDFLD->Int (tblRec,basinID);
						layerIDFLD->Int (tblRec,layerRec->RowID ());
						layerNameFLD->String (tblRec,layerRec->Name ());
						categoryIDFLD->Int (tblRec,gridValueFLD->Int (gridRec));
						categoryFLD->String (tblRec,gridRec->Name ());
						areaFLD->Float (tblRec,histogram [gridRec->RowID ()].area);
						percentFLD->Float (tblRec,histogram [gridRec->RowID ()].area / netIO->BasinArea (basinRec) * 100.0);
						cellNumFLD->Int (tblRec,histogram [gridRec->RowID ()].cellNum);
						}
				for (gridRec = gridTable->First ();gridRec != (DBObjRecord *) NULL;gridRec = gridTable->Next ())
					histogram [gridRec->RowID ()].Initialize ();
				basinID = netIO->CellBasinID (cellRec);
				basinRec = netIO->Basin (cellRec);
				}
			if (gridIO->Value (layerRec,netIO->Center (cellRec),&value) == true)
				{
				histogram [value].cellNum++;
				histogram [value].area += netIO->CellArea (cellRec);
				}
			}
		for (gridRec = gridTable->First ();gridRec != (DBObjRecord *) NULL;gridRec = gridTable->Next ())
			if (histogram [gridRec->RowID ()].cellNum > 0)
				{
				tblRec = table->Add (basinRec->Name ());
				basinIDFLD->Int (tblRec,basinID);
				layerIDFLD->Int (tblRec,layerRec->RowID ());
				layerNameFLD->String (tblRec,layerRec->Name ());
				categoryIDFLD->Int (tblRec,gridValueFLD->Int (gridRec));
				categoryFLD->String (tblRec,gridRec->Name ());
				areaFLD->Float (tblRec,histogram [gridRec->RowID ()].area);
				percentFLD->Float (tblRec,histogram [gridRec->RowID ()].area / netIO->BasinArea (basinRec) * 100.0);
				cellNumFLD->Int (tblRec,histogram [gridRec->RowID ()].cellNum);
				}
		}
Stop:
	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*basinIDFLD));
	fields->Add (new DBObjTableField (*layerIDFLD));
	fields->Add (areaFLD = new DBObjTableField (*areaFLD));
	areaFLD->Flags (DBObjectFlagSortReversed,DBSet);
	table->ListSort (fields);
	delete fields;
	delete netIO;
	delete gridIO;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibNetworkPsmRouting (DBObjData *netData,
											DBObjData *inData,
											DBObjData *velData,
											DBInt timeStep,
											DBFloat coeff,
											DBObjData *outData)

	{
	DBInt ret = DBSuccess, layerID, cellID, tStep, progress, maxProgress;
	DBFloat inValue, outValue, velocity, time, tau, tFactor, cellLength;
	DBPosition pos;
	DBCoordinate coord;
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBGridIO *inIO  = new DBGridIO (inData);
	DBGridIO *velIO = new DBGridIO (velData);
	DBGridIO *outIO = new DBGridIO (outData);
	DBObjRecord *inLayerRec, *outLayerRec, *cellRec, *toCell;

	tFactor = 3600.0 * 24.0;
	if (timeStep == DBTimeStepMonth) tFactor = tFactor * 30.0;

	layerID = 0;
	inLayerRec = inIO->Layer (layerID);
	outLayerRec = outIO->Layer (layerID);
	outIO->RenameLayer (outLayerRec,inLayerRec->Name ());
	inValue = outIO->MissingValue (outLayerRec);
	for (pos.Row = 0;pos.Row < outIO->RowNum ();pos.Row++)
		for (pos.Col = 0;pos.Col < outIO->ColNum ();pos.Col++)
			outIO->Value (outLayerRec,pos,inValue);
	for (layerID = 1;layerID < inIO->LayerNum ();++layerID)
		{
		inLayerRec = inIO->Layer (layerID);
		if ((outLayerRec = outIO->AddLayer (inLayerRec->Name ())) == (DBObjRecord *) NULL)
			{ ret = DBFault; goto Stop; }
		for (pos.Row = 0;pos.Row < outIO->RowNum ();pos.Row++)
			for (pos.Col = 0;pos.Col < outIO->ColNum ();pos.Col++) outIO->Value (outLayerRec,pos,inValue);
		}
	maxProgress = inIO->LayerNum () * netIO->CellNum ();
	for (layerID = 0;layerID < inIO->LayerNum ();++layerID)
		{
		inLayerRec  = inIO->Layer (layerID);

		for (cellID = 0;cellID < netIO->CellNum ();cellID++)
			{
			progress = layerID * netIO->CellNum () + cellID;
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			cellRec = netIO->Cell (cellID);
			if (inIO->Value (inLayerRec,netIO->Center (cellRec),&inValue) == false) continue;
			inValue = inValue * netIO->CellArea (cellRec) * coeff / tFactor;
			time = 0.0;
			for (toCell = cellRec;toCell != (DBObjRecord *) NULL;toCell = netIO->ToCell (toCell))
				if ((cellLength = netIO->CellLength (toCell)) > 0.0)
					{
					coord = netIO->Center (toCell);
					if ((velIO->Value (coord,&velocity) == false) || (velocity <= 0.0)) continue;
					netIO->Coord2Pos (coord,pos);
					tau = cellLength * 1000.0 / velocity;
					time = time + tau;
					tStep = (DBInt) floor (time / tFactor);

					outLayerRec = outIO->Layer ((layerID + tStep) % outIO->LayerNum ());
					if (outIO->Value (outLayerRec,pos,&outValue) == false) outValue = 0.0;
					outIO->Value (outLayerRec,pos,outValue + inValue);
					}
			}
		}
	outIO->RecalcStats ();

Stop:
	delete netIO; delete inIO; delete velIO; delete outIO;
	return (ret);
	}
