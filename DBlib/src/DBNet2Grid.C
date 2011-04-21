/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBNet2Grid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

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
	DBNetworkIF *netIF;
	DBGridIF *gridIF;

	grdData->Projection (netData->Projection ());
	grdData->Precision  (netData->Precision ());
	grdData->MaxScale   (netData->MaxScale ());
	grdData->MinScale   (netData->MinScale ());
	grdData->Extent  	  (netData->Extent ());
	grdData->Document   (DBDocGeoDomain,netData->Document (DBDocGeoDomain));

	netIF = new DBNetworkIF (netData);

	layerTable->Add ("FirstLayer");
	if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL)
		{ delete netIF; return ((DBObjData *) NULL); }
	rowNumFLD->Int (layerRec,netIF->RowNum ());
	colNumFLD->Int (layerRec,netIF->ColNum ());
	cellWidthFLD->Float  (layerRec,netIF->CellWidth ());
	cellHeightFLD->Float (layerRec,netIF->CellHeight ());
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
			CMmsgPrint (CMmsgAppError, "Invalid Data Type in: %s %d",__FILE__,__LINE__);
			delete grdData;
			delete netIF;
			return ((DBObjData *) NULL);
		}
	if ((dataRec = new DBObjRecord (layerRec->Name (),netIF->ColNum () * netIF->RowNum () * valueSizeFLD->Int (layerRec),valueSizeFLD->Int (layerRec))) == (DBObjRecord *) NULL)
		{ delete netIF; return ((DBObjData *) NULL); }
	(grdData->Arrays ())->Add (dataRec);
	layerFLD->Record (layerRec,dataRec);

	gridIF = new DBGridIF (grdData);
	if (type == DBTypeGridContinuous)
		{
		itemTable->Add (layerRec->Name ());
		DBObjTableField *missingValueFLD	= itemTable->Field (DBrNMissingValue);
		missingValueFLD->Float (itemTable->Item (layerRec->Name ()),DBDefaultMissingFloatVal);
		for (pos.Row = 0;pos.Row < netIF->RowNum ();pos.Row++)
			for (pos.Col = 0;pos.Col < netIF->ColNum ();pos.Col++)
				gridIF->Value (layerRec,pos,DBDefaultMissingFloatVal);
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
		for (pos.Row = 0;pos.Row < netIF->RowNum ();pos.Row++)
			for (pos.Col = 0;pos.Col < netIF->ColNum ();pos.Col++)
				gridIF->Value (layerRec,pos,0);
		}
	delete netIF;
	delete gridIF;
	return (grdData);
	}
