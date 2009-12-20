/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

DBNet2Grid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

DBObjData *DBNetworkToGrid (DBObjData *netData,DBInt type)

	{
	DBPosition pos;
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
	DBNetworkIO *netIO;
	DBGridIO *gridIO;

	grdData->Projection (netData->Projection ());
	grdData->Precision  (netData->Precision ());
	grdData->MaxScale   (netData->MaxScale ());
	grdData->MinScale   (netData->MinScale ());
	grdData->Extent  	  (netData->Extent ());
	grdData->Document   (DBDocGeoDomain,netData->Document (DBDocGeoDomain));

	netIO = new DBNetworkIO (netData);

	layerTable->Add ("FirstLayer");
	if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL)
		{ delete netIO; return ((DBObjData *) NULL); }
	rowNumFLD->Int (layerRec,netIO->RowNum ());
	colNumFLD->Int (layerRec,netIO->ColNum ());
	cellWidthFLD->Float  (layerRec,netIO->CellWidth ());
	cellHeightFLD->Float (layerRec,netIO->CellHeight ());
	switch (type)
		{
		case DBTypeGridContinuous:
			valueTypeFLD->Int (layerRec,DBTableFieldFloat);
			valueSizeFLD->Int (layerRec,sizeof (DBFloat4));
			break;
		case DBTypeGridDiscrete:
			valueTypeFLD->Int (layerRec,DBTableFieldInt);
			valueSizeFLD->Int (layerRec,sizeof (DBShort));
			break;
		default:
			fprintf (stderr,"Invalid Data Type in: DBNetworkToGrid ()\n");
			delete grdData;
			delete netIO;
			return ((DBObjData *) NULL);
		}
	if ((dataRec = new DBObjRecord (layerRec->Name (),netIO->ColNum () * netIO->RowNum () * valueSizeFLD->Int (layerRec),valueSizeFLD->Int (layerRec))) == (DBObjRecord *) NULL)
		{ delete netIO; return ((DBObjData *) NULL); }
	(grdData->Arrays ())->Add (dataRec);
	layerFLD->Record (layerRec,dataRec);

	gridIO = new DBGridIO (grdData);
	if (type == DBTypeGridContinuous)
		{
		itemTable->Add (layerRec->Name ());
		DBObjTableField *missingValueFLD	= itemTable->Field (DBrNMissingValue);
		missingValueFLD->Float (itemTable->Item (layerRec->Name ()),DBDefaultMissingFloatVal);
		for (pos.Row = 0;pos.Row < netIO->RowNum ();pos.Row++)
			for (pos.Col = 0;pos.Col < netIO->ColNum ();pos.Col++)
				gridIO->Value (layerRec,pos,DBDefaultMissingFloatVal);
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
		for (pos.Row = 0;pos.Row < netIO->RowNum ();pos.Row++)
			for (pos.Col = 0;pos.Col < netIO->ColNum ();pos.Col++)
				gridIO->Value (layerRec,pos,0);
		}
	delete netIO;
	delete gridIO;
	return (grdData);
	}
