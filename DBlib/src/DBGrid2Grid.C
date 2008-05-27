/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

DBGrid2Grid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

DBObjData *DBGridToGrid (DBObjData *srcGridData)

	{ return (DBGridToGrid (srcGridData,srcGridData->Type ())); }

DBObjData *DBGridToGrid (DBObjData *srcGridData,DBInt type)
	{
	DBInt valueType, valueSize;

	if (srcGridData->Type () == type)
		{
		DBGridIO *gridIO;

		gridIO = new DBGridIO (srcGridData);
		valueType = gridIO->ValueType ();
		valueSize = gridIO->ValueSize ();
		delete gridIO;
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
				valueSize = sizeof (DBShort);
				break;
			default:
				fprintf (stderr,"Invalid Data Type in: DBGridToGrid ()\n");
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
	DBObjTableField *rowNumFLD		= layerTable->Field (DBrNRowNum);
	DBObjTableField *colNumFLD 	= layerTable->Field (DBrNColNum);
	DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
	DBObjTableField *cellHeightFLD= layerTable->Field (DBrNCellHeight);
	DBObjTableField *valueTypeFLD = layerTable->Field (DBrNValueType);
	DBObjTableField *valueSizeFLD = layerTable->Field (DBrNValueSize);
	DBObjTableField *layerFLD 		= layerTable->Field (DBrNLayer);
	DBGridIO *gridIO;
	
	grdData->Projection (srcGridData->Projection ());
	grdData->Precision  (srcGridData->Precision ());
	grdData->MaxScale   (srcGridData->MaxScale ());
	grdData->MinScale   (srcGridData->MinScale ());
	grdData->Extent  	  (srcGridData->Extent ());
	grdData->Document   (DBDocGeoDomain,srcGridData->Document (DBDocGeoDomain));
	grdData->Document   (DBDocSubject,  srcGridData->Document (DBDocSubject));

	gridIO = new DBGridIO (srcGridData);
	
	layerTable->Add ("FirstLayer");
	if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL)
		{ delete gridIO; return ((DBObjData *) NULL); }
	rowNumFLD->Int (layerRec,gridIO->RowNum ());
	colNumFLD->Int (layerRec,gridIO->ColNum ());
	cellWidthFLD->Float  (layerRec,gridIO->CellWidth ());
	cellHeightFLD->Float (layerRec,gridIO->CellHeight ());
	
	valueTypeFLD->Int (layerRec,valueType);
	valueSizeFLD->Int (layerRec,valueSize);
	if ((srcGridData->Type () == type) && (type == DBTypeGridContinuous))
		{
		grdData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		grdData->Flags (srcGridData->Flags () & DBDataFlagDispModeContShadeSets,DBSet);
		}
	if ((dataRec = new DBObjRecord (layerRec->Name (),gridIO->ColNum () * gridIO->RowNum () * valueSize,valueSize)) == (DBObjRecord *) NULL)
		{ delete gridIO; return ((DBObjData *) NULL); }
	(grdData->Arrays ())->Add (dataRec);
	layerFLD->Record (layerRec,dataRec);

	if (type == DBTypeGridContinuous)
		{
		itemTable->Add (layerRec->Name ());
		DBObjTableField *missingValueFLD	= itemTable->Field (DBrNMissingValue);
		if (srcGridData->Type () == type)
				missingValueFLD->Float (itemTable->Item (layerRec->Name ()),gridIO->MissingValue ());
		else	missingValueFLD->Float (itemTable->Item (layerRec->Name ()),DBDefaultMissingFloatVal);
		}
	else
		{
		DBObjTable *symbolTable =  grdData->Table (DBrNSymbols);
		DBObjTableField *foregroundFLD = symbolTable->Field (DBrNForeground);
		DBObjTableField *backgroundFLD = symbolTable->Field (DBrNBackground);
		DBObjTableField *styleFLD = symbolTable->Field (DBrNStyle);
		DBObjRecord *symbolRec = symbolTable->Add ("Default Symbol");

		foregroundFLD->Int (symbolRec,1);
		backgroundFLD->Int (symbolRec,0);
		styleFLD->Int (symbolRec,0);
		}
	delete gridIO;
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
		{ fprintf (stderr,"Invalid data type in: DBGridCreate ()\n"); return ((DBObjData *) NULL); }

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
	rowNumFLD  = layerTable->Field (DBrNRowNum);
	colNumFLD  = layerTable->Field (DBrNColNum);
	cellWidthFLD  = layerTable->Field (DBrNCellWidth);
	cellHeightFLD = layerTable->Field (DBrNCellHeight);
	valueTypeFLD  = layerTable->Field (DBrNValueType);
	valueSizeFLD  = layerTable->Field (DBrNValueSize);
	layerFLD = layerTable->Field (DBrNLayer);

	layerTable->Add ("FirstLayer");
	if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL) return ((DBObjData *) NULL);
	rowNumFLD->Int (layerRec,rowNum);
	colNumFLD->Int (layerRec,colNum);
	cellWidthFLD->Float  (layerRec,cellSize.X);
	cellHeightFLD->Float (layerRec,cellSize.Y);
	switch (type)
		{
		case DBTypeGridContinuous:	varType = DBVariableFloat;	varSize = sizeof (DBFloat4);	break;
		case DBTypeGridDiscrete:	varType = DBVariableInt;	varSize = sizeof (DBShort);	break;
		default:
			fprintf (stderr,"Invalid Data Type in: DBGridCreate ()\n");
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
	DBGridIO *grdIO, *appIO;
	DBObjRecord *grdLayerRec, *appLayerRec;

	if (((grdData->Type () != DBTypeGridDiscrete) && (grdData->Type () != DBTypeGridContinuous)) ||
		((appData->Type () != DBTypeGridDiscrete) && (appData->Type () != DBTypeGridContinuous)) ||
		(grdData->Type () != appData->Type ()))
		return (DBFault);

	grdIO = new DBGridIO (grdData);
	appIO = new DBGridIO (appData);

	for (appLayerID = 0;appLayerID < appIO->LayerNum ();++appLayerID)
		{
		appLayerRec = appIO->Layer (appLayerID);
		grdLayerRec = grdIO->AddLayer (appLayerRec->Name ());
		switch (grdData->Type ())
			{
			case DBTypeGridContinuous:
				for (pos.Row = 0;pos.Row < grdIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < grdIO->ColNum ();++pos.Col)
						{
					 	grdIO->Pos2Coord (pos,coord);
						if (appIO->Value (appLayerRec,coord,&gridValue))
								grdIO->Value (grdLayerRec,pos,gridValue);
						else	grdIO->Value (grdLayerRec,pos,grdIO->MissingValue());
						}
				grdIO->RecalcStats (grdLayerRec);
				break;
			case DBTypeGridDiscrete:
				{
				DBInt tblRecID;
				DBObjTable *grdTable = grdData->Table (DBrNItems);
				DBObjTable *appTable = appData->Table (DBrNItems);
				DBObjTableField *grdValueFLD = grdTable->Field (DBrNGridValue);
				DBObjTableField *appValueFLD = appTable->Field (DBrNGridValue);
				DBObjRecord *appRec, *grdRec;
				for (pos.Row = 0;pos.Row < grdIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < grdIO->ColNum ();++pos.Col)
						{
						grdIO->Pos2Coord (pos,coord);
						if ((appRec = appIO->GridItem (appLayerRec,coord)) != (DBObjRecord *) NULL)
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
								}
							grdIO->Value (grdLayerRec,pos,grdRec->RowID ());
							}
						else	grdIO->Value (grdLayerRec,pos, DBFault);
						} 
				grdIO->DiscreteStats ();
				} break;
			}
		}
	delete grdIO;
	delete appIO;
	return (DBSuccess);
	}
