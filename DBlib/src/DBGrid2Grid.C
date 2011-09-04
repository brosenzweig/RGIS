/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBGrid2Grid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

DBObjData *DBGridToGrid (DBObjData *srcGridData)

	{ return (DBGridToGrid (srcGridData,srcGridData->Type ())); }

DBObjData *DBGridToGrid (DBObjData *srcGridData,DBInt type)
	{
	DBInt valueType, valueSize;

	if (srcGridData->Type () == type)
		{
		DBGridIF *gridIF;

		gridIF = new DBGridIF (srcGridData);
		switch (type)
			{
			case DBTypeGridContinuous:
				valueType = DBTableFieldFloat;
				valueSize = gridIF->ValueType () == DBTableFieldFloat ? gridIF->ValueSize () : sizeof (DBFloat4);
				break;
			case DBTypeGridDiscrete:
				valueType = gridIF->ValueType ();
				valueSize = gridIF->ValueSize ();
				break;
			default:
				CMmsgPrint (CMmsgAppError, "Invalid Data Type in: %s (%s %d)",__FUNCTION__,__FILE__,__LINE__);
				return ((DBObjData *) NULL);
			}
		delete gridIF;
		}
	else
		switch (type)
			{
			case DBTypeGridContinuous:
				valueType = DBTableFieldFloat;
				valueSize = sizeof (DBFloat4);
				break;
			case DBTypeGridDiscrete:
				valueType = DBTableFieldInt;
				valueSize = sizeof (DBInt);
				break;
			default:
				CMmsgPrint (CMmsgAppError, "Invalid Data Type in: %s %d",__FILE__,__LINE__);
				return ((DBObjData *) NULL);
			}
	return (DBGridToGrid (srcGridData,type,valueType,valueSize));
	}

DBObjData *DBGridToGrid (DBObjData *srcGridData,DBInt type, DBInt valueType, DBInt valueSize)

	{
	DBObjData *grdData = new DBObjData ("",type);
	DBObjRecord *layerRec, *dataRec;
	DBObjTable *layerTable = grdData->Table (DBrNLayers);
	DBObjTable *itemTable  = grdData->Table (DBrNItems);
	DBObjTableField *rowNumFLD     = layerTable->Field (DBrNRowNum);
	DBObjTableField *colNumFLD     = layerTable->Field (DBrNColNum);
	DBObjTableField *cellWidthFLD  = layerTable->Field (DBrNCellWidth);
	DBObjTableField *cellHeightFLD = layerTable->Field (DBrNCellHeight);
	DBObjTableField *valueTypeFLD  = layerTable->Field (DBrNValueType);
	DBObjTableField *valueSizeFLD  = layerTable->Field (DBrNValueSize);
	DBObjTableField *layerFLD      = layerTable->Field (DBrNLayer);
	DBGridIF *gridIF;

	grdData->Projection (srcGridData->Projection ());
	grdData->Precision  (srcGridData->Precision ());
	grdData->MaxScale   (srcGridData->MaxScale ());
	grdData->MinScale   (srcGridData->MinScale ());
	grdData->Extent     (srcGridData->Extent ());
	grdData->Document   (DBDocGeoDomain,srcGridData->Document (DBDocGeoDomain));
	grdData->Document   (DBDocSubject,  srcGridData->Document (DBDocSubject));

	gridIF = new DBGridIF (srcGridData);

	layerTable->Add ("FirstLayer");
	if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL)
		{ delete gridIF; return ((DBObjData *) NULL); }
	rowNumFLD->Int (layerRec,gridIF->RowNum ());
	colNumFLD->Int (layerRec,gridIF->ColNum ());
	cellWidthFLD->Float  (layerRec,gridIF->CellWidth ());
	cellHeightFLD->Float (layerRec,gridIF->CellHeight ());

	valueTypeFLD->Int (layerRec,valueType);
	valueSizeFLD->Int (layerRec,valueSize);
	if ((srcGridData->Type () == type) && (type == DBTypeGridContinuous))
		{
		grdData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		grdData->Flags (srcGridData->Flags () & DBDataFlagDispModeContShadeSets,DBSet);
		}
	if ((dataRec = new DBObjRecord (layerRec->Name (),gridIF->ColNum () * gridIF->RowNum () * valueSize,valueSize)) == (DBObjRecord *) NULL)
		{ delete gridIF; return ((DBObjData *) NULL); }
	(grdData->Arrays ())->Add (dataRec);
	layerFLD->Record (layerRec,dataRec);

	if (type == DBTypeGridContinuous)
		{
		itemTable->Add (layerRec->Name ());
		DBObjTableField *missingValueFLD	= itemTable->Field (DBrNMissingValue);
		if (srcGridData->Type () == type)
				missingValueFLD->Float (itemTable->Item (layerRec->Name ()),gridIF->MissingValue ());
		else	missingValueFLD->Float (itemTable->Item (layerRec->Name ()),DBDefaultMissingFloatVal);
		}
	else
		{
		DBObjTable *symbolTable =  grdData->Table (DBrNSymbols);
		DBObjTableField *foregroundFLD = symbolTable->Field (DBrNForeground);
		DBObjTableField *backgroundFLD = symbolTable->Field (DBrNBackground);
		DBObjTableField *styleFLD      = symbolTable->Field (DBrNStyle);
		DBObjRecord *symbolRec = symbolTable->Add ("Default Symbol");

		foregroundFLD->Int (symbolRec,1);
		backgroundFLD->Int (symbolRec,0);
		styleFLD->Int (symbolRec,0);
		}
	delete gridIF;
	return (grdData);
	}

DBObjData *DBGridCreate (char *name,DBRegion extent,DBCoordinate cellSize)

	{ return (DBGridCreate (name,extent,cellSize,DBTypeGridContinuous)); }

DBObjData *DBGridCreate (char *title,DBRegion extent,DBCoordinate cellSize,DBInt type)

	{
	DBInt rowNum, colNum, varSize, varType;
	DBObjTable *layerTable;
	DBObjTable *itemTable;
	DBObjTableField *rowNumFLD;
	DBObjTableField *colNumFLD;
	DBObjTableField *cellWidthFLD;
	DBObjTableField *cellHeightFLD;
	DBObjTableField *valueTypeFLD;
	DBObjTableField *valueSizeFLD;
	DBObjTableField *layerFLD;
	DBObjRecord *layerRec, *dataRec;
	DBObjData *data;

	if ((type & DBTypeGrid) != DBTypeGrid)
		{ CMmsgPrint (CMmsgAppError, "Invalid data type in: %s %d",__FILE__,__LINE__); return ((DBObjData *) NULL); }

	colNum = (DBShort) ceil ((extent.UpperRight.X - extent.LowerLeft.X) / cellSize.X);
	rowNum = (DBShort) ceil ((extent.UpperRight.Y - extent.LowerLeft.Y) / cellSize.Y);
	extent.UpperRight.X = extent.LowerLeft.X + (DBFloat) colNum * cellSize.X;
	extent.UpperRight.Y = extent.LowerLeft.Y + (DBFloat) rowNum * cellSize.Y;

	data = new DBObjData (title,type);
	data->Extent (extent);
	data->Projection (DBMathGuessProjection (extent));
	data->Precision  (DBMathGuessPrecision  (extent));

	layerTable = data->Table (DBrNLayers);
	itemTable  = data->Table (DBrNItems);
	rowNumFLD     = layerTable->Field (DBrNRowNum);
	colNumFLD     = layerTable->Field (DBrNColNum);
	cellWidthFLD  = layerTable->Field (DBrNCellWidth);
	cellHeightFLD = layerTable->Field (DBrNCellHeight);
	valueTypeFLD  = layerTable->Field (DBrNValueType);
	valueSizeFLD  = layerTable->Field (DBrNValueSize);
	layerFLD      = layerTable->Field (DBrNLayer);

	layerTable->Add ("FirstLayer");
	if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL) return ((DBObjData *) NULL);
	rowNumFLD->Int (layerRec,rowNum);
	colNumFLD->Int (layerRec,colNum);
	cellWidthFLD->Float  (layerRec,cellSize.X);
	cellHeightFLD->Float (layerRec,cellSize.Y);
	switch (type)
		{
		case DBTypeGridContinuous:	varType = DBVariableFloat;	varSize = sizeof (DBFloat4); break;
		case DBTypeGridDiscrete:	varType = DBVariableInt;	varSize = sizeof (DBInt);	 break;
		default:
			CMmsgPrint (CMmsgAppError, "Invalid Data Type in: %s %d",__FILE__,__LINE__);
			delete data;
			return ((DBObjData *) NULL);
		}
	valueTypeFLD->Int (layerRec,varType);
	valueSizeFLD->Int (layerRec,varSize);
	dataRec = new DBObjRecord (layerRec->Name (),colNum * rowNum * varSize,varSize);
	if (dataRec == (DBObjRecord *) NULL) { delete data; return ((DBObjData *) NULL); }

	(data->Arrays ())->Add (dataRec);
	layerFLD->Record (layerRec,dataRec);

	if (type == DBTypeGridContinuous)
		{
		itemTable->Add (layerRec->Name ());
		DBObjTableField *missingValueFLD	= itemTable->Field (DBrNMissingValue);
		missingValueFLD->Float (itemTable->Item (layerRec->Name ()),DBDefaultMissingFloatVal);
		}
	else
		{
		DBObjTable *symbolTable = data->Table (DBrNSymbols);
		DBObjTableField *foregroundFLD = symbolTable->Field (DBrNForeground);
		DBObjTableField *backgroundFLD = symbolTable->Field (DBrNBackground);
		DBObjTableField *styleFLD = symbolTable->Field (DBrNStyle);
		DBObjRecord *symbolRec = symbolTable->Add ("Default Symbol");

		foregroundFLD->Int (symbolRec,1);
		backgroundFLD->Int (symbolRec,0);
		styleFLD->Int (symbolRec,0);
		}
	return (data);
	}

DBInt DBGridAppend (DBObjData *grdData, DBObjData *appData)

	{
	DBInt appLayerID;
	DBFloat gridValue;
	DBPosition pos;
	DBCoordinate coord;
	DBGridIF *gridIF, *appIF;
	DBObjRecord *grdLayerRec, *appLayerRec;

	if (((grdData->Type () != DBTypeGridDiscrete) && (grdData->Type () != DBTypeGridContinuous)) ||
		((appData->Type () != DBTypeGridDiscrete) && (appData->Type () != DBTypeGridContinuous)) ||
		(grdData->Type () != appData->Type ()))
		return (DBFault);

	gridIF = new DBGridIF (grdData);
	appIF = new DBGridIF (appData);

	for (appLayerID = 0;appLayerID < appIF->LayerNum ();++appLayerID)
		{
		appLayerRec = appIF->Layer (appLayerID);
		grdLayerRec = gridIF->AddLayer (appLayerRec->Name ());
		switch (grdData->Type ())
			{
			case DBTypeGridContinuous:
				for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
						{
					 	gridIF->Pos2Coord (pos,coord);
						if (appIF->Value (appLayerRec,coord,&gridValue))
								gridIF->Value (grdLayerRec,pos,gridValue);
						else	gridIF->Value (grdLayerRec,pos,gridIF->MissingValue());
						}
				gridIF->RecalcStats (grdLayerRec);
				break;
			case DBTypeGridDiscrete:
				{
				DBInt tblRecID;
				DBObjTable *grdTable = grdData->Table (DBrNItems);
				DBObjTable *appTable = appData->Table (DBrNItems);
				DBObjTableField *grdValueFLD  = grdTable->Field (DBrNGridValue);
				DBObjTableField *grdSymbolFLD = grdTable->Field (DBrNSymbol);
				DBObjTableField *appValueFLD = appTable->Field (DBrNGridValue);
				DBObjRecord *symRec = (grdData->Table (DBrNSymbols))->Item ();
				DBObjRecord *appRec, *grdRec;

				for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
						{
						gridIF->Pos2Coord (pos,coord);
						if ((appRec = appIF->GridItem (appLayerRec,coord)) != (DBObjRecord *) NULL)
							{
							for (tblRecID = 0;tblRecID  < grdTable->ItemNum ();++tblRecID)
								{
								grdRec = grdTable->Item (tblRecID);
								if (grdValueFLD->Int (grdRec) == appValueFLD->Int (appRec)) break;
								}
							if (tblRecID >= grdTable->ItemNum ())
								{
								grdRec = grdTable->Add (appRec->Name ());
								grdValueFLD->Int (grdRec,appValueFLD->Int (appRec));
								grdSymbolFLD->Record (grdRec,symRec);
								}
							gridIF->Value (grdLayerRec,pos,grdRec->RowID ());
							}
						else	gridIF->Value (grdLayerRec,pos, DBFault);
						}
				gridIF->DiscreteStats ();
				} break;
			}
		}
	delete gridIF;
	delete appIF;
	return (DBSuccess);
	}

DBObjData *DBGridMerge (DBObjData *grd0Data, DBObjData *grd1Data)

	{
	DBInt layerID, layerNum, fieldID, fieldNum;
	DBFloat gridValue;
	DBPosition pos, offset0, offset1;
	DBCoordinate coord, cellSize;
	DBRegion extent;
	DBObjTable  *grd0ItemTable, *grd1ItemTable;
	DBObjTable  *grd0CellTable, *grd1CellTable;
	DBGridIF    *grd0IF = (DBGridIF *)    NULL,    *grd1IF = (DBGridIF *)    NULL,    *rGrdIF = (DBGridIF *)    NULL;
	DBNetworkIF *net0IF = (DBNetworkIF *) NULL,    *net1IF = (DBNetworkIF *) NULL,    *rNetIF = (DBNetworkIF *) NULL;
	DBObjRecord *grd0LayerRec, *grd1LayerRec, *rGrdLayerRec;
	DBObjTableField *grd0ItemFLD, *grd1ItemFLD;
	DBObjTableField *grd0CellFLD, *grd1CellFLD;
	DBObjData *retData = (DBObjData *) NULL;

	if (((grd0Data->Type () != DBTypeGridDiscrete) && (grd0Data->Type () != DBTypeGridContinuous) && (grd0Data->Type () != DBTypeNetwork)) ||
		((grd1Data->Type () != DBTypeGridDiscrete) && (grd1Data->Type () != DBTypeGridContinuous) && (grd1Data->Type () != DBTypeNetwork)) ||
		 (grd0Data->Type () != grd1Data->Type ()))
		return ((DBObjData *) NULL);

	grd0ItemTable = grd0Data->Table (DBrNItems);
	grd1ItemTable = grd1Data->Table (DBrNItems);
	if ((fieldNum = grd0ItemTable->FieldNum ()) != grd1ItemTable->FieldNum ())
		{
		CMmsgPrint (CMmsgUsrError,"Incompatible item tables");
		goto Stop;
		}
	for (fieldID = 0; fieldID < fieldNum; ++fieldID)
		{
		grd0ItemFLD = grd0ItemTable->Field (fieldID);
		grd1ItemFLD = grd1ItemTable->Field (fieldID);
		if ((strcmp (grd0ItemFLD->Name (), grd1ItemFLD->Name ()) != 0) ||
			(grd0ItemFLD->Type   () != grd1ItemFLD->Type   ()) ||
			(grd0ItemFLD->Length () != grd1ItemFLD->Length ()) ||
			(grd0ItemFLD->Size   () != grd1ItemFLD->Size   ()))
			{
			CMmsgPrint (CMmsgUsrError,"Incompatible item table structure");
			goto Stop;
			}
		}

	extent.Expand (grd0Data->Extent ());
	extent.Expand (grd1Data->Extent ());
	if ((grd0Data->Type () & DBTypeGrid) == DBTypeGrid)
		{
		grd0ItemTable = grd0Data->Table (DBrNLayers);
		grd1ItemTable = grd1Data->Table (DBrNLayers);
		if ((layerNum = grd0ItemTable->ItemNum ()) != grd1ItemTable->ItemNum ())
			{
			CMmsgPrint (CMmsgUsrError,"Incompatible grids");
			goto Stop;
			}
		for (layerID = 0; layerID < layerNum; ++layerID)
			{
			grd0LayerRec = grd0ItemTable->Item (layerID);
			grd1LayerRec = grd1ItemTable->Item (layerID);
			if (strcmp (grd0LayerRec->Name (), grd1LayerRec->Name ()) != 0)
				{
				CMmsgPrint (CMmsgUsrError,"Incompatible grid layers");
				goto Stop;
				}
			}

		grd0IF = new DBGridIF (grd0Data);
		grd1IF = new DBGridIF (grd1Data);

		cellSize.X  = grd0IF->CellWidth  ();
		cellSize.Y = grd0IF->CellHeight ();
		if ((CMmathEqualValues (cellSize.X, grd1IF->CellWidth  ()) == false) ||
		    (CMmathEqualValues (cellSize.Y, grd1IF->CellHeight ()) == false))
			{
			CMmsgPrint (CMmsgUsrError,"Different cell size");
			goto Stop;
			}
		}
	else
		{
		grd0CellTable = grd0Data->Table (DBrNCells);
		grd1CellTable = grd1Data->Table (DBrNCells);
		if ((fieldID = grd0CellTable->FieldNum ()) != grd1CellTable->FieldNum ())
			{
			CMmsgPrint (CMmsgUsrError,"Different cell table");
			goto Stop;
			}
		for (fieldID = 0; fieldID < fieldNum; ++fieldID)
			{
			grd0CellFLD = grd0CellTable->Field (fieldID);
			grd1CellFLD = grd1CellTable->Field (fieldID);
			if ((strcmp (grd0CellFLD->Name (), grd1CellFLD->Name ()) != 0) ||
				(grd0CellFLD->Type   () != grd1CellFLD->Type   ()) ||
				(grd0CellFLD->Length () != grd1CellFLD->Length ()) ||
				(grd0CellFLD->Size   () != grd1CellFLD->Size   ()))
				{
				CMmsgPrint (CMmsgUsrError,"Incompatible cell tables");
				goto Stop;
				}
			}

		net0IF = new DBNetworkIF (grd0Data);
		net1IF = new DBNetworkIF (grd1Data);

		cellSize.X = net0IF->CellWidth  ();
		cellSize.Y = net0IF->CellHeight ();

		if ((CMmathEqualValues (cellSize.X, net1IF->CellWidth  ()) == false) ||
		    (CMmathEqualValues (cellSize.Y, net1IF->CellHeight ()) == false))
			{
			CMmsgPrint (CMmsgUsrError,"Different cell size");
			goto Stop;
			}
		}

	offset0.Col = floor ((grd0Data->Extent().LowerLeft.X - extent.LowerLeft.X) / cellSize.X);
	offset0.Row = floor ((grd0Data->Extent().LowerLeft.Y - extent.LowerLeft.Y) / cellSize.Y);
	offset1.Col = floor ((grd1Data->Extent().LowerLeft.X - extent.LowerLeft.X) / cellSize.X);
	offset1.Row = floor ((grd1Data->Extent().LowerLeft.Y - extent.LowerLeft.Y) / cellSize.Y);
	if ((CMmathEqualValues (offset0.Col * cellSize.X, grd0Data->Extent().LowerLeft.X - extent.LowerLeft.X) == false) ||
       (CMmathEqualValues (offset0.Row * cellSize.Y, grd0Data->Extent().LowerLeft.Y - extent.LowerLeft.Y) == false) ||
		 (CMmathEqualValues (offset1.Col * cellSize.X, grd1Data->Extent().LowerLeft.X - extent.LowerLeft.X) == false) ||
		 (CMmathEqualValues (offset1.Row * cellSize.Y, grd1Data->Extent().LowerLeft.Y - extent.LowerLeft.Y) == false))
		{
		CMmsgPrint (CMmsgUsrError,"Unalligned Grids");
		goto Stop;
		}

	switch (grd0Data->Type ())
		{
		case DBTypeGridContinuous:
			if ((retData = DBGridCreate (grd0Data->Name (),extent,cellSize,DBTypeGridContinuous)) == (DBObjData *) NULL)
				{
				CMmsgPrint (CMmsgAppError,"Grid Creation Error in: %s:%d",__FILE__,__LINE__);
				goto Stop;
				}
         retData->Precision  (grd0Data->Precision ());
         retData->Projection (grd0Data->Projection ());
         retData->MaxScale   (grd0Data->MaxScale  ());
         retData->MinScale   (grd0Data->MinScale  ());
         retData->Document   (DBDocSubject,   grd0Data->Document (DBDocSubject));
         retData->Document   (DBDocGeoDomain, grd0Data->Document (DBDocGeoDomain));
         retData->Document   (DBDocVersion,   grd0Data->Document (DBDocVersion));
         retData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
         retData->Flags (grd0Data->Flags () & DBDataFlagDispModeContShadeSets, DBSet);

			rGrdIF = new DBGridIF (retData);
			rGrdIF->MissingValue (grd0IF->MissingValue ());
			rGrdIF->RenameLayer (grd0IF->Layer (0)->Name ());
			for (layerID = 0;layerID < grd0IF->LayerNum (); layerID++)
				{
				grd0LayerRec = grd0IF->Layer (layerID);
				grd1LayerRec = grd1IF->Layer (layerID);
				rGrdLayerRec = layerID == 0 ? rGrdIF->Layer (layerID) : rGrdIF->AddLayer (grd0LayerRec->Name ());
				for (pos.Row = 0; pos.Row < rGrdIF->RowNum ();pos.Row++)
					for (pos.Col = 0; pos.Col < rGrdIF->ColNum (); pos.Col++)
						rGrdIF->Value (rGrdLayerRec, pos, rGrdIF->MissingValue ());
				for (pos.Row = 0; pos.Row < grd0IF->RowNum ();pos.Row++)
					for (pos.Col = 0; pos.Col < grd0IF->ColNum (); pos.Col++)
						if (grd0IF->Value (grd0LayerRec,pos,&gridValue) == true)
							rGrdIF->Value(rGrdLayerRec,offset0 + pos,gridValue);
				for (pos.Row = 0; pos.Row < grd1IF->RowNum ();pos.Row++)
					for (pos.Col = 0; pos.Col < grd1IF->ColNum (); pos.Col++)
						if (grd1IF->Value (grd1LayerRec,pos,&gridValue) == true)
							rGrdIF->Value(rGrdLayerRec,offset1 + pos,gridValue);
				rGrdIF->RecalcStats(rGrdLayerRec);
				}
			break;
		case DBTypeGridDiscrete:
         // TODO
			break;
		default:
         if ((retData = new DBObjData (grd0Data->Name (),DBTypeNetwork)) == (DBObjData *) NULL)
            {
    			CMmsgPrint (CMmsgAppError,"Network Creation Error in: %s:%d",__FILE__,__LINE__);
				goto Stop;
            }
         else
            {
            DBObjTable *basinTable = retData->Table (DBrNItems);
            DBObjTable *cellTable  = retData->Table (DBrNCells);
            DBObjTable *layerTable = retData->Table (DBrNLayers);

            DBObjTableField *mouthPosFLD   = basinTable->Field (DBrNMouthPos);
            DBObjTableField *colorFLD		 = basinTable->Field (DBrNColor);

            DBObjTableField *positionFLD	 = cellTable->Field (DBrNPosition);
            DBObjTableField *toCellFLD		 = cellTable->Field (DBrNToCell);
            DBObjTableField *fromCellFLD	 = cellTable->Field (DBrNFromCell);
            DBObjTableField *orderFLD		 = cellTable->Field (DBrNOrder);
            DBObjTableField *basinFLD		 = cellTable->Field (DBrNBasin);
            DBObjTableField *basinCellsFLD = cellTable->Field (DBrNBasinCells);
            DBObjTableField *travelFLD		 = cellTable->Field (DBrNTravel);
            DBObjTableField *upCellPosFLD	 = cellTable->Field (DBrNUpCellPos);
            DBObjTableField *cellAreaFLD	 = cellTable->Field (DBrNCellArea);
            DBObjTableField *subbasinLengthFLD = cellTable->Field (DBrNSubbasinLength);
            DBObjTableField *subbasinAreaFLD = cellTable->Field (DBrNSubbasinArea);

            DBObjTableField *rowNumFLD = layerTable->Field (DBrNRowNum);
            DBObjTableField *colNumFLD = layerTable->Field (DBrNColNum);
            DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
            DBObjTableField *cellHeightFLD = layerTable->Field (DBrNCellHeight);
            DBObjTableField *valueTypeFLD = layerTable->Field (DBrNValueType);
            DBObjTableField *valueSizeFLD = layerTable->Field (DBrNValueSize);
            DBObjTableField *layerFLD = layerTable->Field (DBrNLayer);

            DBObjRecord *layerRec, *dataRec, *cellRec, *basinRec, *srcCell;
            DBInt colNum = (DBInt) ((extent.UpperRight.X - extent.LowerLeft.X) / cellSize.X);
            DBInt rowNum = (DBInt) ((extent.UpperRight.Y - extent.LowerLeft.Y) / cellSize.Y);
            DBInt cellID;
            char nameSTR [DBStringLength];

            retData->Extent (extent);
            retData->Precision  (grd0Data->Precision ());
            retData->Projection (grd0Data->Projection ());
            retData->MaxScale   (grd0Data->MaxScale  ());
            retData->MinScale   (grd0Data->MinScale  ());
            retData->Document   (DBDocSubject,   grd0Data->Document (DBDocSubject));
            retData->Document   (DBDocGeoDomain, grd0Data->Document (DBDocGeoDomain));
            retData->Document   (DBDocVersion,   grd0Data->Document (DBDocVersion));
            retData->Flags (DBDataFlagDispModeNetColors,DBClear);
            retData->Flags (grd0Data->Flags () & DBDataFlagDispModeNetColors, DBSet);

         	layerTable->Add (DBrNLookupGrid);
            if ((layerRec = layerTable->Item (DBrNLookupGrid)) == (DBObjRecord *) NULL)
               {
               CMmsgPrint (CMmsgAppError, "Network Layer Creation Error in: %s %d",__FILE__,__LINE__);
               delete retData;
               retData = (DBObjData *) NULL;
         		goto Stop;
               }
            cellWidthFLD->Float  (layerRec,cellSize.X);
            cellHeightFLD->Float (layerRec,cellSize.Y);
            valueTypeFLD->Int (layerRec,DBTableFieldInt);
            valueSizeFLD->Int (layerRec,sizeof (DBInt));
            colNumFLD->Int (layerRec, colNum);
            rowNumFLD->Int (layerRec, rowNum);
            if ((dataRec = new DBObjRecord ("NetLookupGridRecord",rowNum * colNum * sizeof (DBInt),sizeof (DBInt))) == (DBObjRecord *) NULL)
               {
        			CMmsgPrint (CMmsgAppError,"Look Grid Record creation error");
               delete retData;
               retData = (DBObjData *) NULL;
         		goto Stop;
               }
            layerFLD->Record (layerRec,dataRec);
            (retData->Arrays ())->Add (dataRec);
            for (pos.Row = 0;pos.Row < rowNum; ++pos.Row)
               for (pos.Col = 0;pos.Col < colNum;++pos.Col)
                  ((DBInt *) dataRec->Data ()) [pos.Row * colNum + pos.Col] = DBFault;
      
            for (cellID = 0;cellID < net0IF->CellNum (); ++cellID)
               {
               srcCell = net0IF->Cell (cellID);
               pos = offset0 + net0IF->CellPosition (srcCell);
               sprintf (nameSTR,"GHAASCell:%d",cellTable->ItemNum ());
               cellRec = cellTable->Add (nameSTR);
               positionFLD->Position    (cellRec, pos);
               toCellFLD->Int			    (cellRec, net0IF->CellDirection(srcCell));
               fromCellFLD->Int		    (cellRec, (DBInt) 0);
               orderFLD->Int			    (cellRec, (DBInt) 0);
               basinFLD->Int			    (cellRec, (DBInt) 0);
               basinCellsFLD->Int	    (cellRec, (DBInt) 0);
               travelFLD->Int			    (cellRec, (DBInt) 0);
               upCellPosFLD->Position   (cellRec, pos);
               cellAreaFLD->Float	    (cellRec,(DBFloat) 0.0);
               subbasinLengthFLD->Float (cellRec,(DBFloat) 0.0);
               subbasinAreaFLD->Float	 (cellRec,(DBFloat) 0.0);

               ((DBInt *) dataRec->Data ()) [pos.Row * colNum + pos.Col] = cellRec->RowID ();
               }
           for (cellID = 0;cellID < net1IF->CellNum (); ++cellID)
               {
               srcCell = net1IF->Cell (cellID);
               pos = offset1 + net1IF->CellPosition (srcCell);
               sprintf (nameSTR,"GHAASCell:%d",cellTable->ItemNum ());
               cellRec = cellTable->Add (nameSTR);
               positionFLD->Position    (cellRec, pos);
               toCellFLD->Int			    (cellRec, net1IF->CellDirection(srcCell));
               fromCellFLD->Int		    (cellRec, (DBInt) 0);
               orderFLD->Int			    (cellRec, (DBInt) 0);
               basinFLD->Int			    (cellRec, (DBInt) 0);
               basinCellsFLD->Int	    (cellRec, (DBInt) 0);
               travelFLD->Int			    (cellRec, (DBInt) 0);
               upCellPosFLD->Position   (cellRec, pos);
               cellAreaFLD->Float	    (cellRec,(DBFloat) 0.0);
               subbasinLengthFLD->Float (cellRec,(DBFloat) 0.0);
               subbasinAreaFLD->Float	 (cellRec,(DBFloat) 0.0);

               ((DBInt *) dataRec->Data ()) [pos.Row * colNum + pos.Col] = cellRec->RowID ();
               }
            sprintf (nameSTR,"GHAASBasin%d",(DBInt) 0);
            basinRec = basinTable->Add (nameSTR);
            mouthPosFLD->Position	(basinRec,positionFLD->Position (cellTable->Item (0)));
            colorFLD->Int				(basinRec,0);
            rNetIF = new DBNetworkIF (retData);
            rNetIF->Build ();
            delete rNetIF;
            }
			break;
		}
   
Stop:
	if (grd0IF != (DBGridIF *)    NULL) delete grd0IF;
	if (grd1IF != (DBGridIF *)    NULL) delete grd1IF;
	if (net0IF != (DBNetworkIF *) NULL) delete net0IF;
	if (net1IF != (DBNetworkIF *) NULL) delete net1IF;
	return (retData);
	}
