/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISAnGrid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <rgis.H>

void RGISAnGridResampleCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt clip;
	DBDataset *dataset = UIDataset ();
	DBObjData *inGData = dataset->Data ();
	DBObjData *netData = inGData->LinkedData ();
	DBObjData *outGData;
	DBObjRecord *noDataRec = (DBObjRecord *) NULL;

	if (inGData->Type () == DBTypeGridDiscrete)
		{
		char *selection;
		DBObjTable *itemTable = inGData->Table (DBrNItems);
		static Widget selectWidget = (Widget) NULL;

		if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ((char *) "Select Nodata Category");
		if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) itemTable)) == (char *) NULL)
				noDataRec = (DBObjRecord *) NULL;
		else	noDataRec = itemTable->Item (selection);
		}
	outGData = DBNetworkToGrid (netData,inGData->Type ());
	outGData->Document (DBDocSubject,inGData->Document (DBDocSubject));
	outGData->Name (inGData->Name ());
	if (UIDataHeaderForm (outGData))
		{
		clip = UIYesOrNo ((char *) "Clip to Network");
		UIPauseDialogOpen ((char *) "Resampling Grid");
		if (RGlibGridResampling (inGData,noDataRec,clip ? netData : (DBObjData *) NULL, outGData) == DBSuccess)
			workspace->CurrentData (outGData);
		else	delete outGData;
		UIPauseDialogClose ();
		}
	else delete outGData;
	}

void RGISAnGDiscUniformRunoffCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjMetaEntry *metaEntry;
	DBObjData *gridData = dataset->Data ();
	DBObjData *tsData, *runoffData;
	DBObjectLIST<DBObjTableField> *fields;
	DBObjTable *relateTBL = gridData->Table (DBrNRelations), *tsTBL;
	DBObjTableField *relDataFLD;
	DBObjTableField *grdRelateFLD;
	DBObjTableField *tsJoinFLD;
	DBObjTableField *tsTimeFLD;
	DBObjTableField *tsValueFLD;
	DBObjRecord *relateRec;
	static Widget selectWidget = (Widget) NULL, valueWidget = (Widget) NULL, tStepWidget = (Widget) NULL;

	if (selectWidget == (Widget) NULL)	selectWidget = UISelectionCreate ((char *) "Select Time Series");
	if (valueWidget == (Widget) NULL)	valueWidget  = UISelectionCreate ((char *) "Select Value Field");
	if (tStepWidget == (Widget) NULL)	tStepWidget	 = UISelectionCreate ((char *) "Time Step Field");

	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) relateTBL)) == (char *) NULL) return;
	if ((relateRec = relateTBL->Item (selection)) == (DBObjRecord *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Relate Record Error in: %s %d",__FILE__,__LINE__); return; }

	relDataFLD	 = relateTBL->Field (DBrNRelateData);
	grdRelateFLD = relateTBL->Field (DBrNRelateField);
	tsJoinFLD	 = relateTBL->Field (DBrNRelateJoinField);

	if ((tsData = dataset->Data (relDataFLD->String (relateRec))) == (DBObjData *) NULL)
		{
		if ((metaEntry = dataset->Meta (relDataFLD->String (relateRec))) == (DBObjMetaEntry *) NULL)
			{ CMmsgPrint (CMmsgAppError, "Meta Enrty Finding Error in: %s %d",__FILE__,__LINE__); return; }
		tsData = new DBObjData ();
		if (tsData->Read (metaEntry->FileName ()) != DBSuccess) return;
		}
	workspace->CurrentData (tsData);
	tsTBL = tsData->Table (DBrNItems);

	fields = tsTBL->Fields ();
	for (tsTimeFLD = fields->First ();tsTimeFLD != (DBObjTableField *) NULL;tsTimeFLD = fields->Next ())
			if (tsTimeFLD->Type () == DBTableFieldDate) break;
	if (tsTimeFLD == (DBObjTableField *) NULL)
		{
		selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) tsTBL->Fields (), DBTableFieldIsString);
		if (selection == (char *) NULL) return;
		if ((tsTimeFLD = tsTBL->Field (selection)) == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgAppError, "Invalid time step field in: %s %d",__FILE__,__LINE__); return; }
		}

	if ((selection = UISelectObject (valueWidget,(DBObjectLIST<DBObject> *) tsTBL->Fields (),DBTableFieldIsNumeric)) == (char *) NULL) return;
	if ((tsValueFLD = tsTBL->Field (selection)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Corrupt Value Field in: %s %d",__FILE__,__LINE__); return; }

	if ((runoffData = DBGridToGrid (gridData,DBTypeGridContinuous)) == (DBObjData *) NULL) return;
	runoffData->Document (DBDocSubject,GHAASSubjRunoff);
	if (UIDataHeaderForm (runoffData) == true)
		{
		UIPauseDialogOpen ((char *) "Creating Runoff Grid");
		if (RGlibGridUniformRunoff	  (gridData, tsData,
												grdRelateFLD->String (relateRec),
												tsJoinFLD->String (relateRec),
												tsTimeFLD->Name (),
												tsValueFLD->Name (),
												runoffData) == DBSuccess)
			workspace->CurrentData (runoffData);
		else	delete runoffData;
		UIPauseDialogClose ();
		}
	else	delete runoffData;
	}

void RGISAnGDiscReclassDiscreteCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *srcData = dataset->Data (), *dstData;
	DBObjTable *srcItemTable = srcData->Table (DBrNItems);
	static Widget selectWidget = (Widget) NULL;

	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ((char *) "Select Time Series");
	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) (srcItemTable->Fields ()),DBTableFieldIsCategory)) == (char *) NULL)
		return;
	if ((dstData = DBGridToGrid (srcData,DBTypeGridDiscrete)) == (DBObjData *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Grid Creation Error in: %s %d",__FILE__,__LINE__); return; }
	if (UIDataHeaderForm (dstData))
		{
		UIPauseDialogOpen ((char *) "Reclassing Grid");
		if (RGlibGridReclassDiscrete (srcData,selection,dstData) == DBSuccess)
			workspace->CurrentData (dstData);
		else delete dstData;
		UIPauseDialogClose ();
		}
	else
		delete dstData;
	}

void RGISAnGDiscReclassContinuousCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *srcData = dataset->Data (), *dstData;
	DBObjTable *srcItemTable = srcData->Table (DBrNItems);
	static Widget selectWidget = (Widget) NULL;

	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ((char *) "Select Time Series");
	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) (srcItemTable->Fields ()),DBTableFieldIsNumeric)) == (char *) NULL)
		return;
	if ((dstData = DBGridToGrid (srcData,DBTypeGridContinuous)) == (DBObjData *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Grid Creation Error in: %s %d",__FILE__,__LINE__); return; }

	if (UIDataHeaderForm (dstData))
		{
		UIPauseDialogOpen ((char *) "Reclassing Grid");
		if (RGlibGridReclassContinuous (srcData,selection,dstData) == DBSuccess)
			workspace->CurrentData (dstData);
		else	delete dstData;
		UIPauseDialogClose ();
		}
	else	delete dstData;
	}

void RGISAnGDiscZoneHistCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset  = UIDataset ();
	DBObjData *zGrdData = dataset->Data ();
	DBObjData *cGrdData = zGrdData->LinkedData ();
	DBObjData *tblData;

	if ((zGrdData == (DBObjData *) NULL) || (cGrdData == (DBObjData *) NULL)) return;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,zGrdData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Zone Histogram");
	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Histogram");
		if (RGlibGridZoneHistogram (zGrdData,cGrdData,tblData) == DBSuccess)
			workspace->CurrentData (tblData);
		else
			delete tblData;
		UIPauseDialogClose ();
		}
	else	delete tblData;
	}

void RGISAnGDiscZoneStatsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset  = UIDataset ();
	DBObjData *zGrdData = dataset->Data ();
	DBObjData *wGrdData = zGrdData->LinkedData ();
	DBObjData *tblData;

	if ((zGrdData == (DBObjData *) NULL) || (wGrdData == (DBObjData *) NULL)) return;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,zGrdData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Zone Statistics");
	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Statistics");
		if (RGlibGridZoneStatistics (zGrdData, wGrdData, tblData) ==  DBSuccess)
			workspace->CurrentData (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else	delete tblData;
	}

#define RGISPourID		"PourID"
#define RGISPitArea		"PitArea"
#define RGISPitVolume	"PitVolume"
#define RGISPitDepth		"PitDepth"
#define RGISPourElev		"PourElevation"

void RGISAnGContPitsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt cellID, pits = false;
	DBFloat cellElev, pourElev, volume;
	DBCoordinate coord;
	DBObjRecord *cellRec, *toCellRec, *pourCellRec;
	DBObjTable *cellTable;
	DBObjTableField *pourIDFLD;
	DBObjTableField *pitAreaFLD;
	DBObjTableField *pitVolumeFLD;
	DBObjTableField *pitDepthFLD;
	DBObjTableField *pourElevFLD;
	DBDataset *dataset = UIDataset ();
	DBObjData *gridData = dataset->Data (), *netData, *pntData;
	DBNetworkIF *netIF;
	DBGridIF *gridIF;

	widget = widget; callData = callData;

	if (gridData == (DBObjData *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Null Data in: %s %d",__FILE__,__LINE__); return; }

	if ((netData = gridData->LinkedData ()) == (DBObjData *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Null Linked Data in: %s %d",__FILE__,__LINE__); return; }

	if ((cellTable = netData->Table (DBrNCells)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Corrupt Linked Data in: %s %d",__FILE__,__LINE__); return; }
	if ((pourIDFLD =  cellTable->Field (RGISPourID)) == (DBObjTableField  *) NULL)
		{
		pourIDFLD = new DBObjTableField (RGISPourID,DBTableFieldInt,"%6d",sizeof (int));
		cellTable->AddField (pourIDFLD);
		}
	if ((pitAreaFLD =  cellTable->Field (RGISPitArea)) == (DBObjTableField  *) NULL)
		{
		pitAreaFLD = new DBObjTableField (RGISPitArea,DBTableFieldFloat,"%10.1f",sizeof (float));
		cellTable->AddField (pitAreaFLD);
		}
	if ((pitVolumeFLD = cellTable->Field (RGISPitVolume)) == (DBObjTableField  *) NULL)
		{
		pitVolumeFLD = new DBObjTableField (RGISPitVolume,DBTableFieldFloat,"%10.1f",sizeof (float));
		cellTable->AddField (pitVolumeFLD);
		}
	if ((pitDepthFLD = cellTable->Field (RGISPitDepth)) == (DBObjTableField  *) NULL)
		{
		pitDepthFLD = new DBObjTableField (RGISPitDepth,DBTableFieldFloat,"%10.1f",sizeof (float));
		cellTable->AddField (pitDepthFLD);
		}
	if ((pourElevFLD = cellTable->Field (RGISPourElev)) == (DBObjTableField  *) NULL)
		{
		pourElevFLD = new DBObjTableField (RGISPourElev,DBTableFieldFloat,"%10.1f",sizeof (float));
		cellTable->AddField (pourElevFLD);
		}

	gridIF = new DBGridIF (gridData);
	netIF = new DBNetworkIF (netData);

	for (cellID = 0;cellID < netIF->CellNum ();++cellID)
		{
		cellRec = netIF->Cell (cellID);
		if (gridIF->Value (netIF->Center (cellRec),&cellElev) == false) cellElev = 0.0;
		pourIDFLD->Int (cellRec,cellRec->RowID ());
		pitAreaFLD->Float (cellRec,0.0);
		pitVolumeFLD->Float (cellRec,0.0);
		pitDepthFLD->Float (cellRec,0.0);
		pourElevFLD->Float (cellRec,cellElev);
		}

	for (cellID = 0;cellID < netIF->CellNum ();++cellID)
		{
		cellRec = netIF->Cell (cellID);
		if ((toCellRec = netIF->ToCell (cellRec)) == (DBObjRecord *) NULL) continue;

		pourCellRec =  netIF->Cell (pourIDFLD->Int (toCellRec));

		if (gridIF->Value (netIF->Center (pourCellRec),&pourElev) == false)	pourElev = 0.0;
		if (gridIF->Value (netIF->Center (cellRec),    &cellElev) == false)  cellElev = pourElev;
		if (pourElev > cellElev)
			{
			volume = netIF->CellArea (cellRec) * (pourElev - cellElev);
			pourIDFLD->Int (cellRec,pourCellRec->RowID ());
			pourElevFLD->Float (cellRec,pourElev);
			pitAreaFLD->Float (pourCellRec,pitAreaFLD->Float (pourCellRec) + netIF->CellArea (cellRec));
			pitVolumeFLD->Float (pourCellRec,pitVolumeFLD->Float (pourCellRec) + volume);
			pitDepthFLD->Float (pourCellRec,	pitDepthFLD->Float (pourCellRec) > pourElev - cellElev ?
														pitDepthFLD->Float (pourCellRec) : pourElev - cellElev);
			pits = true;
			}
		}

	if (pits == false) { UIMessage ((char *) "No Pits were found"); return; }
	pntData = new DBObjData ("",DBTypeVectorPoint);
	pntData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	pntData->Document (DBDocSubject,GHAASSubjPits);
	if (UIDataHeaderForm (pntData))
		{
		char name [DBStringLength];
		DBObjTable *items 	= pntData->Table (DBrNItems);
		DBObjTable *symbols	= pntData->Table (DBrNSymbols);
		DBObjTableField *coordField = items->Field (DBrNCoord);
		DBObjTableField *symbolFLD	 = items->Field (DBrNSymbol);
		DBObjTableField *areaFLD	= new DBObjTableField (RGISPitArea,DBTableFieldFloat,"%10.1f",sizeof (float));
		DBObjTableField *volumeFLD = new DBObjTableField (RGISPitVolume,DBTableFieldFloat,"%10.1f",sizeof (float));
		DBObjTableField *depthFLD  = new DBObjTableField (RGISPitDepth,DBTableFieldFloat,"%10.1f",sizeof (float));
		DBObjTableField *elevFLD 	= new DBObjTableField (RGISPourElev,DBTableFieldFloat,"%10.1f",sizeof (float));
		DBObjTableField *subbasinAreaFLD = new DBObjTableField (DBrNSubbasinArea,DBTableFieldFloat,"%10.1f",sizeof (float));
		DBObjTableField *foregroundFLD = symbols->Field (DBrNForeground);
		DBObjTableField *backgroundFLD = symbols->Field (DBrNBackground);
		DBObjTableField *styleFLD = symbols->Field (DBrNStyle);
		DBObjRecord *pntRec, *symRec;
		DBRegion dataExtent;

		items->AddField (areaFLD);
		items->AddField (volumeFLD);
		items->AddField (depthFLD);
		items->AddField (elevFLD);
		items->AddField (subbasinAreaFLD);
		sprintf (name,"Pit Pour Point");
		symRec = symbols->Add (name);
		foregroundFLD->Int (symRec,1);
		backgroundFLD->Int (symRec,0);
		styleFLD->Int (symRec,0);

		for (cellID = 0;cellID < netIF->CellNum ();++cellID)
			{
			cellRec = netIF->Cell (cellID);
			if (pourIDFLD->Int (cellRec) != cellRec->RowID ())
				{
				pourCellRec =  netIF->Cell (pourIDFLD->Int (cellRec));
				pourIDFLD->Int (cellRec,pourIDFLD->Int (pourCellRec));
				}
			else
				{
				if (pitVolumeFLD->Float (cellRec) > 0.0)
					{
					sprintf (name,"STN Pit:%04d",items->ItemNum ());
					pntRec = items->Add (name);
					coord = netIF->Center (cellRec);
					coordField->Coordinate (pntRec,coord);
					symbolFLD->Record (pntRec,symRec);
					areaFLD->Float (pntRec,pitAreaFLD->Float (cellRec));
					volumeFLD->Float (pntRec,pitVolumeFLD->Float (cellRec) / 1000.0);
					depthFLD->Float (pntRec,pitDepthFLD->Float (cellRec));
					elevFLD->Float (pntRec,pourElevFLD->Float (cellRec));
					subbasinAreaFLD->Float (pntRec,netIF->CellBasinArea (cellRec));
					dataExtent.Expand (coord);
					pourIDFLD->Int (cellRec,pntRec->RowID () + 1);
					}
				else pourIDFLD->Int (cellRec,pourIDFLD->IntNoData ());
				}
			}
		pntData->Extent (dataExtent);
		workspace->CurrentData  (pntData);
		}
	else
		{
		cellTable->DeleteField (pourIDFLD);
		delete pntData;
		}
	cellTable->DeleteField (pitAreaFLD);
	cellTable->DeleteField (pitVolumeFLD);
	cellTable->DeleteField (pitDepthFLD);
	delete netIF;
	delete gridIF;
	}

static DBInt 	_RGISAnGContPourCellID;
static DBFloat _RGISAnGContPourElev;
static DBGridIF *_RGISAnnGContPourGridIF;


static DBInt _RGISAnnGContFindBasinMinCrest (void *ptr,DBObjRecord *cellRec)

	{
	DBFloat elev;
	DBNetworkIF *netIF = (DBNetworkIF *) ptr;

	if (netIF->CellBasinCells (cellRec) > 1) return (false);
	printf ("Itt Jartam: %d %d\n",cellRec->RowID (),netIF->CellBasinCells (cellRec));
	if (_RGISAnnGContPourGridIF->Value (netIF->Center (cellRec),&elev))
		if (_RGISAnGContPourElev > elev)
			{
			_RGISAnGContPourElev = elev;
			_RGISAnGContPourCellID = cellRec->RowID ();
			}
	return (true);
	}

void RGISAnGContPourCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt basinID;
	DBCoordinate coord, delta;
	DBObjRecord *basinRec;
	DBDataset *dataset = UIDataset ();
	DBObjData *gridData = dataset->Data (), *netData;
	DBNetworkIF *netIF;

	widget = widget; workspace = workspace; callData = callData;

	if (gridData == (DBObjData *) NULL) { CMmsgPrint (CMmsgAppError, "Null Data in: %s %d",__FILE__,__LINE__); return; }

	if ((netData = gridData->LinkedData ()) == (DBObjData *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Null Linked Data in: %s %d",__FILE__,__LINE__); return; }

	_RGISAnnGContPourGridIF = new DBGridIF (gridData);
	netIF = new DBNetworkIF (netData);

	for (basinID = 0;basinID < netIF->BasinNum ();++basinID)
		{
		basinRec = netIF->Basin (basinID);
		_RGISAnGContPourCellID = DBFault;
		_RGISAnGContPourElev = DBHugeVal;
		netIF->UpStreamSearch (netIF->MouthCell (basinRec),(DBNetworkACTION) _RGISAnnGContFindBasinMinCrest);
		if (_RGISAnGContPourCellID != DBFault)
			{
			coord = netIF->Center (netIF->Cell(_RGISAnGContPourCellID));
			delta = netIF->Delta (netIF->Cell(_RGISAnGContPourCellID));
			printf ("%d,%f,%f,%f\n", _RGISAnGContPourCellID,coord.X + delta.X,coord.Y + delta.Y,_RGISAnGContPourElev);
			}
		}

	delete netIF;
	delete _RGISAnnGContPourGridIF;
	}

void RGISAnGContCreateNetworkCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *gridData = dataset->Data ();
	DBObjData *netData = new DBObjData ("",DBTypeNetwork);
	DBInt DBGridCont2Network (DBObjData *,DBObjData *, bool);

	widget = widget; callData = callData;

	netData->Document (DBDocGeoDomain,gridData->Document (DBDocGeoDomain));
	netData->Document (DBDocSubject,"STNetwork");
	if (UIDataHeaderForm (netData))
		{
		UIPauseDialogOpen ((char *) "Creating Networks");
		if (DBGridCont2Network (gridData,netData, true) == DBFault)	delete netData;
		else workspace->CurrentData (netData);
		UIPauseDialogClose ();
		}
	else delete netData;
	}

void RGISAnGContMergeCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt dataNum = 0, incr;
	DBDataset *dataset = UIDataset ();
	DBObjData *mGridData = dataset->Data (), *lGridData, *nGridData;;
	DBRegion extent;

	for (lGridData = mGridData;(lGridData != (DBObjData *) NULL);lGridData = lGridData->LinkedData ())
		if ((lGridData->Flags () & DBObjectFlagProcessed) == DBObjectFlagProcessed)	break;
		else	if (lGridData->Type () == DBTypeGridContinuous)
			{
			extent.Expand (lGridData->Extent ());
			lGridData->Flags (DBObjectFlagProcessed,DBSet);
			dataNum++;
			}
	incr = 80 / dataNum;
	for (lGridData = dataset->FirstData ();lGridData != (DBObjData *) NULL;lGridData = dataset->NextData ())
		lGridData->Flags (DBObjectFlagProcessed,DBClear);
	nGridData = new DBObjData ("",DBTypeGridContinuous);
	nGridData->Extent (extent);
	if (UIDataHeaderForm (nGridData))
		{
		DBPosition pos;
		DBCoordinate coord;
		DBInt layerID;
		DBFloat value;
		DBGridIF *gridIF = new DBGridIF (mGridData);
		DBGridIF *nGridIF;
		DBObjRecord *layerRec, *nLayerRec, *dataRec;
		DBObjTable *layerTable = nGridData->Table (DBrNLayers);
		DBObjTable *itemTable  = nGridData->Table (DBrNItems);
		DBObjTableField *rowNumFLD		= layerTable->Field (DBrNRowNum);
		DBObjTableField *colNumFLD 	= layerTable->Field (DBrNColNum);
		DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
		DBObjTableField *cellHeightFLD= layerTable->Field (DBrNCellHeight);
		DBObjTableField *valueTypeFLD = layerTable->Field (DBrNValueType);
		DBObjTableField *valueSizeFLD = layerTable->Field (DBrNValueSize);
		DBObjTableField *layerFLD 		= layerTable->Field (DBrNLayer);
		DBObjTableField *missingValueFLD	= itemTable->Field (DBrNMissingValue);

		UIPauseDialogOpen ((char *) "Merging Continuous Grids");
		pos.Col = (int) (ceil ((extent.UpperRight.X - extent.LowerLeft.X) / gridIF->CellWidth  ()));
		pos.Row = (int) (ceil ((extent.UpperRight.Y - extent.LowerLeft.Y) / gridIF->CellHeight ()));
		for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
			{
			layerRec = gridIF->Layer (layerID);
			if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if ((nLayerRec = layerTable->Add (layerRec->Name ())) == (DBObjRecord *) NULL)
				{ delete gridIF; delete nGridData; return; }

			cellWidthFLD->Float  (nLayerRec,gridIF->CellWidth ());
			cellHeightFLD->Float (nLayerRec,gridIF->CellHeight ());
			rowNumFLD->Int (nLayerRec,pos.Row);
			colNumFLD->Int (nLayerRec,pos.Col);
			valueTypeFLD->Int (nLayerRec,gridIF->ValueType ());
			valueSizeFLD->Int (nLayerRec,gridIF->ValueSize ());
			if ((dataRec = new DBObjRecord (nLayerRec->Name (), pos.Col * pos.Row * valueSizeFLD->Int (nLayerRec),valueSizeFLD->Int (nLayerRec))) == (DBObjRecord *) NULL)
				{ delete gridIF; delete nGridData; return; }
			(nGridData->Arrays ())->Add (dataRec);
			layerFLD->Record (nLayerRec,dataRec);
			itemTable->Add (nLayerRec->Name ());
			missingValueFLD->Float (itemTable->Item (nLayerRec->Name ()),gridIF->MissingValue ());
			}
		delete gridIF;
		nGridIF = new DBGridIF (nGridData);

		for (layerID = 0;layerID < nGridIF->LayerNum ();++layerID)
			{
			nLayerRec = nGridIF->Layer (layerID);
			for (pos.Row = 0;pos.Row < nGridIF->RowNum ();pos.Row++)
				{
				DBPause (20 * nLayerRec->RowID () * pos.Row / (nGridIF->LayerNum () * nGridIF->RowNum ()));
				for (pos.Col = 0;pos.Col < nGridIF->ColNum ();pos.Col++)
					nGridIF->Value (nLayerRec,pos,nGridIF->MissingValue (nLayerRec));
				}
			}
		dataNum = 0;
		for (lGridData = mGridData;(lGridData != (DBObjData *) NULL);lGridData = lGridData->LinkedData ())
			if ((lGridData->Flags () & DBObjectFlagProcessed) == DBObjectFlagProcessed)	break;
			else	if (lGridData->Type () == DBTypeGridContinuous)
				{
				gridIF = new DBGridIF (lGridData);
				for (layerID = 0;layerID < nGridIF->LayerNum ();++layerID)
					{
					nLayerRec = nGridIF->Layer (layerID);
					if ((layerRec = gridIF->Layer (nLayerRec->Name ())) != (DBObjRecord *) NULL)
						for (pos.Row = 0;pos.Row < gridIF->RowNum ();pos.Row++)
							{
							DBPause (20 + dataNum * incr + incr * layerID * pos.Row / (nGridIF->LayerNum () * nGridIF->RowNum ()));
							for (pos.Col = 0;pos.Col < gridIF->ColNum ();pos.Col++)
								if (gridIF->Value (layerRec,pos,&value))
									{
									gridIF->Pos2Coord (pos,coord);
									nGridIF->Value (nLayerRec,coord,value);
									}
							}
					}
				lGridData->Flags (DBObjectFlagProcessed,DBSet);
				delete gridIF;
				++dataNum;
				}
		nGridIF->RecalcStats ();
		UIPauseDialogClose ();
		workspace->CurrentData (nGridData);
		delete nGridIF;

		for (lGridData = dataset->FirstData ();lGridData != (DBObjData *) NULL;lGridData = dataset->NextData ())
			lGridData->Flags (DBObjectFlagProcessed,DBClear);
		}
	else delete nGridData;
	}

void RGISAnGContAbsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();

	grdData = new DBObjData (*grdData);
	if (UIDataHeaderForm (grdData))
		{
		UIPauseDialogOpen ((char *) "Calculating Absolute Values");
		DBGridOperationAbs (grdData);
		UIPauseDialogClose ();
		workspace->CurrentData (grdData);
		}
	else	delete grdData;
	}

void RGISAnGContNoNegCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt setZero;
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();

	grdData = new DBObjData (*grdData);
	if (UIDataHeaderForm (grdData))
		{
		setZero = UIYesOrNo ((char *) "Set to Zero");
		UIPauseDialogOpen ((char *) "Eliminating Negative Values");
		if (RGlibGridNoNegatives (grdData,setZero) == DBSuccess)
			workspace->CurrentData (grdData);
		else	delete grdData;
		UIPauseDialogClose ();
		}
	else	delete grdData;
	}

void RGISAnGContAddCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt mergeMissingVal;
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();
	DBObjData *lnkData = grdData->LinkedData ();

	widget = widget; callData = callData;
	grdData = new DBObjData (*grdData);
	if (UIDataHeaderForm (grdData))
		{
		mergeMissingVal = UIYesOrNo ((char *) "Merge Missing Values?");
		UIPauseDialogOpen ((char *) "Adding Grids");
		DBGridOperation (grdData,lnkData,DBMathOperatorAdd,mergeMissingVal);
		UIPauseDialogClose ();
		workspace->CurrentData (grdData);
		}
	else delete grdData;
	}

void RGISAnGContSubtractCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt mergeMissingVal;
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();
	DBObjData *lnkData = grdData->LinkedData ();

	widget = widget; callData = callData;
	grdData = new DBObjData (*grdData);
	if (UIDataHeaderForm (grdData))
		{
		mergeMissingVal = UIYesOrNo ((char *) "Merge Missing Values?");
		UIPauseDialogOpen ((char *) "Subtracting Grids");
		DBGridOperation (grdData,lnkData,DBMathOperatorSub,mergeMissingVal);
		UIPauseDialogClose ();
		workspace->CurrentData (grdData);
		}
	else delete grdData;
	}

void RGISAnGContMultiplyCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt mergeMissingVal;
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();
	DBObjData *lnkData = grdData->LinkedData ();

	widget = widget; callData = callData;
	grdData = new DBObjData (*grdData);
	if (UIDataHeaderForm (grdData))
		{
		mergeMissingVal = UIYesOrNo ((char *) "Merge Missing Values?");
		UIPauseDialogOpen ((char *) "Multiplying Grids");
		DBGridOperation (grdData,lnkData,DBMathOperatorMul,mergeMissingVal);
		UIPauseDialogClose ();
		workspace->CurrentData (grdData);
		}
	else delete grdData;
	}

void RGISAnGContDivideCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt mergeMissingVal;
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();
	DBObjData *lnkData = grdData->LinkedData ();

	widget = widget; callData = callData;
	grdData = new DBObjData (*grdData);
	if (UIDataHeaderForm (grdData))
		{
		mergeMissingVal = UIYesOrNo ((char *) "Merge Missing Values?");
		UIPauseDialogOpen ((char *) "Dividing Grids");
		DBGridOperation (grdData,lnkData,DBMathOperatorDiv,mergeMissingVal);
		UIPauseDialogClose ();
		workspace->CurrentData (grdData);
		}
	else delete grdData;
	}

void RGISAnGContMakeDiscreteAddButtonCBK (Widget button, Widget textField, XmAnyCallbackStruct *callData)

	{
	char *f0Text, *f1Text;
	int i, itemCount;
	double newBin, bin;
	Widget list;
	XmString string, *items;

	XtVaGetValues (button, XmNuserData, &list, NULL);
	XtVaGetValues (list,XmNitems, &items, XmNitemCount, &itemCount, NULL);

	f0Text = XmTextFieldGetString (textField);

	for (i = 0;i < (int) strlen (f0Text);i++)
		if (((f0Text [i] < '0') || (f0Text [i] > '9')) && (f0Text [i] != '-') && (f0Text [i] != '+') && (f0Text [i] != '.'))	f0Text [i] = '\0';
	newBin = atof (f0Text);
	for (i = 0;i < itemCount;i++)
		{
		XmStringGetLtoR (items [i],UICharSetNormal,&f1Text);
		bin = atof (f1Text);
		XtFree (f1Text);
		if (bin > newBin) break;
		}
	string = XmStringCreate (f0Text,UICharSetNormal);
	XmListAddItem (list, string, i + 1);
	XmListSelectPos (list,i + 1,True);
	XmStringFree (string);
	XtFree (f0Text);
	}

void RGISAnGContMakeDiscreteLoadButtonCBK (Widget button, Widget textField, XmAnyCallbackStruct *callData)

	{
	FILE *inFile;
	char fText [256], *fileName;
	int i;
	Widget list;
	static Widget fileSelect = (Widget) NULL;
	XmString string;

	XtVaGetValues (button, XmNuserData, &list, NULL);

	if (fileSelect == NULL) fileSelect = UIFileSelectionCreate ((char *) "Load Binfile",NULL,(char *) "*.txt",XmFILE_REGULAR);
	if ((fileName = UIFileSelection (fileSelect,true)) == NULL) return;

	if ((inFile = fopen (fileName,"r")) == (FILE *)  NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return; }

	XmListDeleteAllItems (list);
	for (i = 0;fgets (fText,sizeof (fText) - 1,inFile) != (char *) NULL; i++)
		{
		if (fText [strlen (fText) - 1] == '\n') fText [strlen (fText) - 1] = '\0';
		string = XmStringCreate (fText,UICharSetNormal);
		XmListAddItem (list,string,i + 1);
		}
	XmListSelectPos (list,i,True);
	fclose (inFile);
	}

void RGISAnGContMakeDiscreteSaveButtonCBK (Widget button, Widget textField, XmAnyCallbackStruct *callData)

	{
	FILE *outFile;
	char *fText, *fileName;
	int i, itemCount;
	Widget list;
	static Widget fileSelect = (Widget) NULL;
	XmString *items;

	XtVaGetValues (button, XmNuserData, &list, NULL);
	XtVaGetValues (list,XmNitems, &items, XmNitemCount, &itemCount, NULL);

	if (fileSelect == NULL) fileSelect = UIFileSelectionCreate ((char *) "Load Binfile",NULL,(char *) "*.txt",XmFILE_REGULAR);
	if ((fileName = UIFileSelection (fileSelect,false)) == NULL) return;

	if ((outFile = fopen (fileName,"w")) == (FILE *)  NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return; }

	for (i = 0;i < itemCount;i++)
		{
		XmStringGetLtoR (items [i],UICharSetNormal,&fText);
		fprintf (outFile,"%s\n",fText);
		XtFree (fText);
		}
	fclose (outFile);
	}

void RGISAnGContMakeDiscreteRemoveButtonCBK (Widget button, Widget textField, XmAnyCallbackStruct *callData)

	{
	char *f0Text, *f1Text;
	int i, itemCount, test;
	Widget list;
	XmString *items;

	XtVaGetValues (button, XmNuserData, &list, NULL);
	XtVaGetValues (list,XmNitems, &items, XmNitemCount, &itemCount, NULL);

	f0Text = XmTextFieldGetString (textField);
	for (i = 0;i < itemCount;i++)
		{
		XmStringGetLtoR (items [i],UICharSetNormal,&f1Text);
		test = strcmp (f0Text,f1Text);
		XtFree (f1Text);
		if (test == 0) break;
		}
	XmListDeletePos (list, i + 1);
	if (itemCount > 1) XmListSelectPos (list,i + (itemCount - 1 > i? 1 : 0), True);
	else	XtSetSensitive (button, False);
	XtFree (f0Text);
	}

void RGISAnGContMakeDiscreteListCBK (Widget list, Widget textField, XmListCallbackStruct *callData)

	{
	char *fText;
	Widget removeButton;

	XtVaGetValues (list,XmNuserData, &removeButton, NULL);

	XmStringGetLtoR (callData->item,UICharSetNormal,&fText);
	XmTextFieldSetString (textField,fText);
	XtSetSensitive (removeButton, True);
	XtFree (fText);
	}

void RGISAnGContMakeDiscreteCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fText;
	float bin;
	static int proc;
	int itemCount;
	DBDataset *dataset = UIDataset ();
	DBObjData *srcData = dataset->Data ();
	static Widget dShell = (Widget) NULL, mainForm;
	static Widget textField, list, addButton, removeButton, saveButton;

	if (dShell == (Widget) NULL)
		{
		int argNum;
		Dimension bWidth;
		Arg wargs [20];
		Widget label, loadButton;
		XmString string;

		dShell = UIDialogForm ((char *) "Bin Dialog",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Add",UICharSetBold);
		addButton = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteAddButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_FORM,
								XmNtopOffset,           10,
								XmNrightAttachment,     XmATTACH_FORM,
								XmNrightOffset,         10,
								XmNmarginWidth,         10,
								XmNmarginHeight,        5,
								XmNtraversalOn,         False,
								XmNlabelString,         string,
								XmNnoResize,            True,
								XmNsensitive,           False,
								NULL);
		XmStringFree (string);

		textField = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteField",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           addButton,
								XmNrightAttachment,     XmATTACH_WIDGET,
								XmNrightWidget,         addButton,
								XmNrightOffset,         10,
								XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,        addButton,
								XmNmaxLength,           10,
								XmNcolumns,             10,
								NULL);

		string = XmStringCreate ((char *) "Bin value:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteFieldLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           textField,
								XmNleftAttachment,      XmATTACH_FORM,
								XmNleftOffset,          20,
								XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,        textField,
								XmNlabelString,         string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Remove",UICharSetBold);
		removeButton = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteRemoveButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_WIDGET,
								XmNtopWidget,           addButton,
								XmNtopOffset,           10,
								XmNrightAttachment,     XmATTACH_FORM,
								XmNrightOffset,         10,
								XmNmarginWidth,         10,
								XmNmarginHeight,        5,
								XmNtraversalOn,         False,
								XmNlabelString,         string,
								XmNsensitive,           False,
								NULL);
		XmStringFree (string);
		XtVaGetValues (removeButton,XmNwidth, &bWidth, NULL);
		XtVaSetValues (addButton,XmNwidth, bWidth, NULL);

		argNum = 0;
		XtSetArg (wargs [argNum],	XmNtopAttachment,   XmATTACH_WIDGET);  ++argNum;
		XtSetArg (wargs [argNum],	XmNtopWidget,       textField);        ++argNum;
		XtSetArg (wargs [argNum],	XmNtopOffset,					10);					++argNum;
		XtSetArg (wargs [argNum],	XmNleftAttachment,			XmATTACH_FORM);	++argNum;
		XtSetArg (wargs [argNum],	XmNleftOffset,					20); 					++argNum;
		XtSetArg (wargs [argNum],	XmNrightAttachment,			XmATTACH_OPPOSITE_WIDGET);	++argNum;
		XtSetArg (wargs [argNum],	XmNrightWidget,				textField); 		++argNum;
		XtSetArg (wargs [argNum],	XmNbottomAttachment,			XmATTACH_FORM);	++argNum;
		XtSetArg (wargs [argNum],	XmNbottomOffset,				10);					++argNum;
		XtSetArg (wargs [argNum],	XmNscrollingPolicy,			XmAUTOMATIC);		++argNum;
		XtSetArg (wargs [argNum],	XmNscrollBarDisplayPolicy,	XmAS_NEEDED);		++argNum;
		XtSetArg (wargs [argNum],	XmNvisualPolicy,				XmVARIABLE);		++argNum;
		XtSetArg (wargs [argNum],	XmNvisibleItemCount,			7);					++argNum;
		XtSetArg (wargs [argNum],	XmNshadowThickness,			2);					++argNum;
		XtSetArg (wargs [argNum],	XmNselectionPolicy,			XmSINGLE_SELECT);	++argNum;
		XtSetArg (wargs [argNum],	XmNtextColumns,				DBStringLength);	++argNum;
		XtSetArg (wargs [argNum],	XmNuserData,					removeButton);		++argNum;
		XtManageChild (list = XmCreateScrolledList (mainForm,(char *) "RGISAnGContMakeDiscreteList",wargs,argNum));
		XtAddCallback (list,XmNsingleSelectionCallback,	(XtCallbackProc) RGISAnGContMakeDiscreteListCBK,(void *) textField);

		string = XmStringCreate ((char *) "Save",UICharSetBold);
		saveButton = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteSaveButton",xmPushButtonWidgetClass,mainForm,
								XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNleftWidget,				removeButton,
								XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNrightWidget,			removeButton,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNmarginWidth,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNsensitive,				False,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Load",UICharSetBold);
		loadButton = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteLoadButton",xmPushButtonWidgetClass,mainForm,
								XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNleftWidget,				removeButton,
								XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNrightWidget,			removeButton,
								XmNbottomAttachment,		XmATTACH_WIDGET,
								XmNbottomWidget,			saveButton,
								XmNbottomOffset,			5,
								XmNmarginWidth,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&proc);
		XtVaSetValues (addButton, XmNuserData, list, NULL);
		XtVaSetValues (removeButton, XmNuserData, list, NULL);
		XtVaSetValues (loadButton, XmNuserData, list, NULL);
		XtVaSetValues (saveButton, XmNuserData, list, NULL);
		XtAddCallback (addButton,XmNactivateCallback,(XtCallbackProc) RGISAnGContMakeDiscreteAddButtonCBK,textField);
		XtAddCallback (removeButton,XmNactivateCallback,(XtCallbackProc) RGISAnGContMakeDiscreteRemoveButtonCBK,textField);
		XtAddCallback (loadButton,XmNactivateCallback,(XtCallbackProc) RGISAnGContMakeDiscreteLoadButtonCBK,textField);
		XtAddCallback (saveButton,XmNactivateCallback,(XtCallbackProc) RGISAnGContMakeDiscreteSaveButtonCBK,textField);
		}

	proc = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		fText = XmTextFieldGetString (textField);
		XtSetSensitive (addButton, (strlen (fText) > 0) && (sscanf (fText,"%f",&bin) == 1));
		XtFree (fText);
		XtVaGetValues (list,XmNitemCount, &itemCount, NULL);
		if (itemCount  > 0 && (XtIsSensitive (saveButton) == False))
			{
			XtSetSensitive (saveButton, True);
			XtSetSensitive (UIDialogFormGetOkButton (dShell), True);
			}
		if (itemCount == 0 && (XtIsSensitive (saveButton) != False))
			{
			XtSetSensitive (saveButton, False);
			XtSetSensitive (UIDialogFormGetOkButton (dShell), False);
			}
		}
	UIDialogFormPopdown (dShell);
	if (proc)
		{
		float *binValues;
		int item;
		DBObjData *dstData;
		XmString *items;

		XtVaGetValues (list,XmNitems, &items, XmNitemCount, &itemCount, NULL);

		if ((dstData = DBGridToGrid (srcData,DBTypeGridDiscrete)) == (DBObjData *) NULL) return;
		if (UIDataHeaderForm (dstData))
			{
			if ((binValues = (float *) calloc (itemCount, sizeof (float))) == (float *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return; }
			for (item = 0;item < itemCount;++item)
				{
				XmStringGetLtoR (items [item],UICharSetNormal,&fText);
				sscanf (fText,"%f",binValues + item);
				}
			UIPauseDialogOpen ((char *) "Making Discrete Grid");
			if (RGlibGridMakeDiscrete (srcData,dstData,binValues,itemCount) == DBSuccess)
				workspace->CurrentData (dstData);
			else	delete dstData;
			UIPauseDialogClose ();
			}
		else	delete dstData;
		}
	}

void RGISAnGContGirdCellStatsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt ret;
	DBDataset *dataset = UIDataset ();
	DBObjData *srcData = dataset->Data (), *xSrcData;
	DBObjData *statData;

	statData = DBGridToGrid (srcData);

	if (UIDataHeaderForm (statData))
		{
		UIPauseDialogOpen ((char *) "Calculating Cell Statistics");
		if (((xSrcData = srcData->LinkedData ()) == (DBObjData *) NULL) || (xSrcData->Type () != DBTypeGridContinuous))
			ret = RGlibGridCellStats (srcData,statData);
		else
			ret = RGlibGridBivarCellStats (xSrcData, srcData,statData);
		if (ret == DBSuccess) workspace->CurrentData (statData);
		else	delete statData;
		UIPauseDialogClose ();
		}
	else delete statData;
	}
