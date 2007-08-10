/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

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
		
		if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ("Select Nodata Category");
		if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) itemTable)) == (char *) NULL)
				noDataRec = (DBObjRecord *) NULL;
		else	noDataRec = itemTable->Item (selection);
		}
	outGData = DBNetworkToGrid (netData,inGData->Type ());
	outGData->Document (DBDocSubject,inGData->Document (DBDocSubject));
	outGData->Name (inGData->Name ());
	if (UIDataHeaderForm (outGData))
		{
		clip = UIYesOrNo ("Clip to Network");
		UIPauseDialogOpen ("Resampling Grid");
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

	if (selectWidget == (Widget) NULL)	selectWidget = UISelectionCreate ("Select Time Series");
	if (valueWidget == (Widget) NULL)	valueWidget  = UISelectionCreate ("Select Value Field");
	if (tStepWidget == (Widget) NULL)	tStepWidget	 = UISelectionCreate ("Time Step Field");

	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) relateTBL)) == (char *) NULL) return;
	if ((relateRec = relateTBL->Item (selection)) == (DBObjRecord *) NULL)
		{ fprintf (stderr,"Relate Record Error in: RGISAnGDiscUniRunoffCBK ()\n"); return; }

	relDataFLD	 = relateTBL->Field (DBrNRelateData);
	grdRelateFLD = relateTBL->Field (DBrNRelateField);
	tsJoinFLD	 = relateTBL->Field (DBrNRelateJoinField);

	if ((tsData = dataset->Data (relDataFLD->String (relateRec))) == (DBObjData *) NULL)
		{
		if ((metaEntry = dataset->Meta (relDataFLD->String (relateRec))) == (DBObjMetaEntry *) NULL)
			{ fprintf (stderr,"Meta Enrty Finding Error in: RGISAnGDiscUniRunoffCBK ()\n"); return; }
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
			{ fprintf (stderr,"Invalid time step field in: RGISAnGDiscUniformRunoffCBK ()\\nn"); return; }
		}
	
	if ((selection = UISelectObject (valueWidget,(DBObjectLIST<DBObject> *) tsTBL->Fields (),DBTableFieldIsNumeric)) == (char *) NULL) return;
	if ((tsValueFLD = tsTBL->Field (selection)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Corrupt Value Field in: RGISAnGDiscUniRunoffCBK ()\n"); return; }
	
	if ((runoffData = DBGridToGrid (gridData,DBTypeGridContinuous)) == (DBObjData *) NULL) return;
	runoffData->Document (DBDocSubject,GHAASSubjRunoff);
	if (UIDataHeaderForm (runoffData) == true)
		{
		UIPauseDialogOpen ("Creating Runoff Grid");
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

	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ("Select Time Series");
	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) (srcItemTable->Fields ()),DBTableFieldIsCategory)) == (char *) NULL)
		return;
	if ((dstData = DBGridToGrid (srcData,DBTypeGridDiscrete)) == (DBObjData *) NULL)
		{ fprintf (stderr,"Grid Creation Error in: RGISAnGDiscReclassDiscreteCBK ()\n"); return; }
	if (UIDataHeaderForm (dstData))
		{
		UIPauseDialogOpen ("Reclassing Grid");
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

	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ("Select Time Series");
	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) (srcItemTable->Fields ()),DBTableFieldIsNumeric)) == (char *) NULL)
		return;
	if ((dstData = DBGridToGrid (srcData,DBTypeGridContinuous)) == (DBObjData *) NULL)
		{ fprintf (stderr,"Grid Creation Error in: RGISAnGDiscReclassDiscreteCBK ()\n"); return; }

	if (UIDataHeaderForm (dstData))
		{
		UIPauseDialogOpen ("Reclassing Grid");
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
		UIPauseDialogOpen ("Calculating Histogram");	
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
		UIPauseDialogOpen ("Calculating Statistics");	
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
	DBNetworkIO *netIO;
	DBGridIO *gridIO;

	widget = widget; callData = callData;
	
	if (gridData == (DBObjData *) NULL)
		{ fprintf (stderr,"Null Data in: _RGISAnGContPitsCBK ()\n"); return; }

	if ((netData = gridData->LinkedData ()) == (DBObjData *) NULL)
		{ fprintf (stderr,"Null Linked Data in: _RGISAnGContPitsCBK ()\n"); return; }

	if ((cellTable = netData->Table (DBrNCells)) == (DBObjTable *) NULL)
		{ fprintf (stderr,"Corrupt Linked Data in: _RGISAnGContPitsCBK ()\n"); return; }
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

	gridIO = new DBGridIO (gridData);
	netIO = new DBNetworkIO (netData);
	
	for (cellID = 0;cellID < netIO->CellNum ();++cellID)
		{
		cellRec = netIO->Cell (cellID);
		if (gridIO->Value (netIO->Center (cellRec),&cellElev) == false) cellElev = 0.0;
		pourIDFLD->Int (cellRec,cellRec->RowID ());
		pitAreaFLD->Float (cellRec,0.0);
		pitVolumeFLD->Float (cellRec,0.0);
		pitDepthFLD->Float (cellRec,0.0);
		pourElevFLD->Float (cellRec,cellElev);
		}

	for (cellID = 0;cellID < netIO->CellNum ();++cellID)
		{
		cellRec = netIO->Cell (cellID);
		if ((toCellRec = netIO->ToCell (cellRec)) == (DBObjRecord *) NULL) continue;

		pourCellRec =  netIO->Cell (pourIDFLD->Int (toCellRec));
		
		if (gridIO->Value (netIO->Center (pourCellRec),&pourElev) == false)	pourElev = 0.0;
		if (gridIO->Value (netIO->Center (cellRec),    &cellElev) == false)  cellElev = pourElev;
		if (pourElev > cellElev)
			{
			volume = netIO->CellArea (cellRec) * (pourElev - cellElev);
			pourIDFLD->Int (cellRec,pourCellRec->RowID ());
			pourElevFLD->Float (cellRec,pourElev);
			pitAreaFLD->Float (pourCellRec,pitAreaFLD->Float (pourCellRec) + netIO->CellArea (cellRec));
			pitVolumeFLD->Float (pourCellRec,pitVolumeFLD->Float (pourCellRec) + volume);
			pitDepthFLD->Float (pourCellRec,	pitDepthFLD->Float (pourCellRec) > pourElev - cellElev ?
														pitDepthFLD->Float (pourCellRec) : pourElev - cellElev);
			pits = true;
			}
		}
	
	if (pits == false) { UIMessage ("No Pits were found"); return; }
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

		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{
			cellRec = netIO->Cell (cellID);
			if (pourIDFLD->Int (cellRec) != cellRec->RowID ())
				{
				pourCellRec =  netIO->Cell (pourIDFLD->Int (cellRec));
				pourIDFLD->Int (cellRec,pourIDFLD->Int (pourCellRec));
				}
			else
				{
				if (pitVolumeFLD->Float (cellRec) > 0.0)
					{
					sprintf (name,"STN Pit:%04d",items->ItemNum ());
					pntRec = items->Add (name);
					coord = netIO->Center (cellRec);
					coordField->Coordinate (pntRec,coord);
					symbolFLD->Record (pntRec,symRec);
					areaFLD->Float (pntRec,pitAreaFLD->Float (cellRec));
					volumeFLD->Float (pntRec,pitVolumeFLD->Float (cellRec) / 1000.0);
					depthFLD->Float (pntRec,pitDepthFLD->Float (cellRec));
					elevFLD->Float (pntRec,pourElevFLD->Float (cellRec));
					subbasinAreaFLD->Float (pntRec,netIO->CellBasinArea (cellRec));
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
	delete netIO;
	delete gridIO;
	}

static DBInt 	_RGISAnGContPourCellID;
static DBFloat _RGISAnGContPourElev;
static DBGridIO *_RGISAnnGContPourGridIO;


static DBInt _RGISAnnGContFindBasinMinCrest (void *ptr,DBObjRecord *cellRec)

	{
	DBFloat elev;
	DBNetworkIO *netIO = (DBNetworkIO *) ptr;
	
	if (netIO->CellBasinCells (cellRec) > 1) return (false);
	printf ("Itt Jartam: %d %d\n",cellRec->RowID (),netIO->CellBasinCells (cellRec));
	if (_RGISAnnGContPourGridIO->Value (netIO->Center (cellRec),&elev))
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
	DBNetworkIO *netIO;

	widget = widget; workspace = workspace; callData = callData;
	
	if (gridData == (DBObjData *) NULL) { fprintf (stderr,"Null Data in: _RGISAnGContPourCBK ()\n"); return; }

	if ((netData = gridData->LinkedData ()) == (DBObjData *) NULL)
		{ fprintf (stderr,"Null Linked Data in: _RGISAnGContPourPointsCBK ()\n"); return; }

	_RGISAnnGContPourGridIO = new DBGridIO (gridData);
	netIO = new DBNetworkIO (netData);
	
	for (basinID = 0;basinID < netIO->BasinNum ();++basinID)
		{
		basinRec = netIO->Basin (basinID);
		_RGISAnGContPourCellID = DBFault;
		_RGISAnGContPourElev = DBHugeVal;
		netIO->UpStreamSearch (netIO->MouthCell (basinRec),(DBNetworkACTION) _RGISAnnGContFindBasinMinCrest);
		if (_RGISAnGContPourCellID != DBFault)
			{
			coord = netIO->Center (netIO->Cell(_RGISAnGContPourCellID));
			delta = netIO->Delta (netIO->Cell(_RGISAnGContPourCellID));
			printf ("%d,%f,%f,%f\n", _RGISAnGContPourCellID,coord.X + delta.X,coord.Y + delta.Y,_RGISAnGContPourElev);
			}
		}

	delete netIO;
	delete _RGISAnnGContPourGridIO;
	}

void RGISAnGContCreateNetworkCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *gridData = dataset->Data ();
	DBObjData *netData = new DBObjData ("",DBTypeNetwork);
	DBInt DBGridCont2Network (DBObjData *,DBObjData *);

	widget = widget; callData = callData;

	netData->Document (DBDocGeoDomain,gridData->Document (DBDocGeoDomain));
	netData->Document (DBDocSubject,"STNetwork");
	if (UIDataHeaderForm (netData))
		{
		UIPauseDialogOpen ("Creating Networks");
		if (DBGridCont2Network (gridData,netData) == DBFault)	delete netData;
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
		DBGridIO *gridIO = new DBGridIO (mGridData);
		DBGridIO *nGridIO;
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
		
		UIPauseDialogOpen ("Merging Continuous Grids");
		pos.Col = (int) (ceil ((extent.UpperRight.X - extent.LowerLeft.X) / gridIO->CellWidth  ()));
		pos.Row = (int) (ceil ((extent.UpperRight.Y - extent.LowerLeft.Y) / gridIO->CellHeight ()));
		for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
			{
			layerRec = gridIO->Layer (layerID);
			if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if ((nLayerRec = layerTable->Add (layerRec->Name ())) == (DBObjRecord *) NULL)
				{ delete gridIO; delete nGridData; return; }
			
			cellWidthFLD->Float  (nLayerRec,gridIO->CellWidth ());
			cellHeightFLD->Float (nLayerRec,gridIO->CellHeight ());
			rowNumFLD->Int (nLayerRec,pos.Row);
			colNumFLD->Int (nLayerRec,pos.Col);
			valueTypeFLD->Int (nLayerRec,gridIO->ValueType ());
			valueSizeFLD->Int (nLayerRec,gridIO->ValueSize ());
			if ((dataRec = new DBObjRecord (nLayerRec->Name (), pos.Col * pos.Row * valueSizeFLD->Int (nLayerRec),valueSizeFLD->Int (nLayerRec))) == (DBObjRecord *) NULL)
				{ delete gridIO; delete nGridData; return; }
			(nGridData->Arrays ())->Add (dataRec);
			layerFLD->Record (nLayerRec,dataRec);
			itemTable->Add (nLayerRec->Name ());
			missingValueFLD->Float (itemTable->Item (nLayerRec->Name ()),gridIO->MissingValue ());
			}
		delete gridIO;
		nGridIO = new DBGridIO (nGridData);

		for (layerID = 0;layerID < nGridIO->LayerNum ();++layerID)
			{
			nLayerRec = nGridIO->Layer (layerID);
			for (pos.Row = 0;pos.Row < nGridIO->RowNum ();pos.Row++)
				{
				DBPause (20 * nLayerRec->RowID () * pos.Row / (nGridIO->LayerNum () * nGridIO->RowNum ()));
				for (pos.Col = 0;pos.Col < nGridIO->ColNum ();pos.Col++)
					nGridIO->Value (nLayerRec,pos,nGridIO->MissingValue (nLayerRec));
				}
			}
		dataNum = 0;
		for (lGridData = mGridData;(lGridData != (DBObjData *) NULL);lGridData = lGridData->LinkedData ())
			if ((lGridData->Flags () & DBObjectFlagProcessed) == DBObjectFlagProcessed)	break;
			else	if (lGridData->Type () == DBTypeGridContinuous)
				{
				gridIO = new DBGridIO (lGridData);
				for (layerID = 0;layerID < nGridIO->LayerNum ();++layerID)
					{
					nLayerRec = nGridIO->Layer (layerID);
					if ((layerRec = gridIO->Layer (nLayerRec->Name ())) != (DBObjRecord *) NULL)
						for (pos.Row = 0;pos.Row < gridIO->RowNum ();pos.Row++)
							{
							DBPause (20 + dataNum * incr + incr * layerID * pos.Row / (nGridIO->LayerNum () * nGridIO->RowNum ()));
							for (pos.Col = 0;pos.Col < gridIO->ColNum ();pos.Col++)
								if (gridIO->Value (layerRec,pos,&value))
									{
									gridIO->Pos2Coord (pos,coord);
									nGridIO->Value (nLayerRec,coord,value);
									}
							}
					}
				lGridData->Flags (DBObjectFlagProcessed,DBSet);
				delete gridIO;
				++dataNum;
				}
		nGridIO->RecalcStats ();
		UIPauseDialogClose ();
		workspace->CurrentData (nGridData);
		delete nGridIO;

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
		UIPauseDialogOpen ("Calculating Absolute Values");
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
		setZero = UIYesOrNo ("Set to Zero");
		UIPauseDialogOpen ("Eliminating Negative Values");
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
		mergeMissingVal = UIYesOrNo ("Merge Missing Values?");
		UIPauseDialogOpen ("Adding Grids");
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
		mergeMissingVal = UIYesOrNo ("Merge Missing Values?");
		UIPauseDialogOpen ("Subtracting Grids");
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
		mergeMissingVal = UIYesOrNo ("Merge Missing Values?");
		UIPauseDialogOpen ("Multiplying Grids");
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
		mergeMissingVal = UIYesOrNo ("Merge Missing Values?");
		UIPauseDialogOpen ("Dividing Grids");
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

	if (fileSelect == NULL) fileSelect = UIFileSelectionCreate ("Load Binfile",NULL,"*.txt",XmFILE_REGULAR);
	if ((fileName = UIFileSelection (fileSelect,true)) == NULL) return;

	if ((inFile = fopen (fileName,"r")) == (FILE *)  NULL)
		{ perror ("File Opening Error in: RGISAnGContMakeDiscreteLoadButtonCBK ()"); return; }
	
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

	if (fileSelect == NULL) fileSelect = UIFileSelectionCreate ("Load Binfile",NULL,"*.txt",XmFILE_REGULAR);
	if ((fileName = UIFileSelection (fileSelect,false)) == NULL) return;

	if ((outFile = fopen (fileName,"w")) == (FILE *)  NULL)
		{ perror ("File Opening Error in: RGISAnGContMakeDiscreteSaveButtonCBK ()"); return; }
	
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
		
		dShell = UIDialogForm ("Bin Dialog",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ("Add",UICharSetBold);
		addButton = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteAddButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginWidth,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNnoResize,				True,
								XmNsensitive,				False,
								NULL);
		XmStringFree (string);

		textField = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteField",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				addButton,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			addButton,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			addButton,
								XmNmaxLength,				10,
								XmNcolumns,					10,
								NULL);

		string = XmStringCreate ("Bin value:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteFieldLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				textField,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				20,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			textField,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ("Remove",UICharSetBold);
		removeButton = XtVaCreateManagedWidget ("RGISAnGContMakeDiscreteRemoveButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				addButton,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginWidth,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNsensitive,				False,
								NULL);
		XmStringFree (string);
		XtVaGetValues (removeButton,XmNwidth, &bWidth, NULL);
		XtVaSetValues (addButton,XmNwidth, bWidth, NULL);

		argNum = 0;
		XtSetArg (wargs [argNum],	XmNtopAttachment,				XmATTACH_WIDGET);	++argNum;
		XtSetArg (wargs [argNum],	XmNtopWidget,					textField);			++argNum;
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
		XtManageChild (list = XmCreateScrolledList (mainForm,"RGISAnGContMakeDiscreteList",wargs,argNum));
		XtAddCallback (list,XmNsingleSelectionCallback,	(XtCallbackProc) RGISAnGContMakeDiscreteListCBK,(void *) textField);

		string = XmStringCreate ("Save",UICharSetBold);
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

		string = XmStringCreate ("Load",UICharSetBold);
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
				{ perror ("Memory allocation error in: RGISAnGContMakeDiscreteCBK ()"); return; }
			for (item = 0;item < itemCount;++item)
				{
				XmStringGetLtoR (items [item],UICharSetNormal,&fText);
				sscanf (fText,"%f",binValues + item);
				}
			UIPauseDialogOpen ("Making Discrete Grid");
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
		UIPauseDialogOpen ("Calculating Cell Statistics");
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
