/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISAnNetStreams.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

#define RGISNetStreamArea	 "StreamArea"
#define RGISNetBasinArea	 "SubbasinArea"
#define RGISNetStreamLength "StreamLength"
#define RGISNetStreamNext	 "NextStream"

static DBObjRecord   *_RGISAnNetworkCellRec;
static DBObjTableField *_RGISAnNetOrderField;
static DBObjTableField *_RGISAnNetStreamIDFLD;
static DBInt			_RGISAnNetStreamID;
static DBInt			_RGISAnNetVertex;
static DBInt			_RGISAnNetVertexNum = 0;
static DBCoordinate *_RGISAnNetCoord = (DBCoordinate *) NULL;
static DBFloat  		_RGISAnNetArea;

static int _RGISAnNetworkUpStreamAction (DBNetworkIF *netIF,DBObjRecord *cellRec)

	{
	DBInt nextOrder [2];

	if (_RGISAnNetVertex < 1)
		{
		nextOrder[0] = _RGISAnNetOrderField->Int (cellRec);
		nextOrder[1] = netIF->CellOrder (cellRec);
		}
	else
		{
		nextOrder[0] = _RGISAnNetOrderField->Int (netIF->ToCell (cellRec));
		nextOrder[1] = netIF->CellOrder (netIF->ToCell(cellRec));
		}

	if ((_RGISAnNetOrderField->Int (cellRec) == nextOrder[0]) && (netIF->CellOrder (cellRec) == nextOrder[1]))
		{
		_RGISAnNetStreamIDFLD->Int (cellRec,_RGISAnNetStreamID);
		_RGISAnNetworkCellRec = cellRec;
		_RGISAnNetVertex += 1;
		return (true);
		}
	return (false);
	}

static int _RGISAnNetworkDownStreamAction (DBNetworkIF *netIF,DBObjRecord *cellRec)

	{
	if (_RGISAnNetStreamIDFLD->Int (cellRec) != _RGISAnNetStreamID) return (false);
	_RGISAnNetArea += netIF->CellArea (cellRec);
	_RGISAnNetCoord [_RGISAnNetVertex++] = netIF->Center (cellRec);
	return (true);
	}

void RGISAnNetworkStreamLinesCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjTable *cellTable = netData->Table (DBrNCells);
	DBObjData *arcData = new DBObjData ("",DBTypeVectorLine);
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	static Widget fieldSelect = (Widget) NULL;

	widget = widget; callData = callData;

	if (fieldSelect == (Widget) NULL)	fieldSelect = UISelectionCreate ((char *) "Select Field");
	if ((selection = UISelectObject (fieldSelect,(DBObjectLIST<DBObject> *) cellTable->Fields (),DBTableFieldIsInteger)) == (char *) NULL)
		return;
	if ((_RGISAnNetOrderField = cellTable->Field (selection)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Field Selection Error in: _RGISAnNetworkStreamLinesCBK ()\n"); return; }
	arcData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	arcData->Document (DBDocSubject,"Stream Lines");
	if (UIDataHeaderForm (arcData))
		{
		DBInt cellID;
		char objName [DBStringLength];
		DBVLineIF *lineIF = new DBVLineIF (arcData);
		DBObjTable *cellTable 	= netData->Table (DBrNCells);
		DBObjTable *lineTable 	= arcData->Table (DBrNItems);
		DBObjTableField *basinFLD  = new DBObjTableField (DBrNBasin,DBTableFieldInt,"%8d",sizeof (DBInt));
		DBObjTableField *fieldFLD  = new DBObjTableField (_RGISAnNetOrderField->Name (),
		                                                  _RGISAnNetOrderField->Type (),
		                                                  _RGISAnNetOrderField->Format (),
		                                                  _RGISAnNetOrderField->Length ());
		DBObjTableField *lengthFLD = new DBObjTableField (RGISNetStreamLength,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4));
		DBObjTableField *areaFLD   = new DBObjTableField (RGISNetStreamArea,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4));
		DBObjTableField *basinAreaFLD = new DBObjTableField (RGISNetBasinArea,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4));
		DBObjTableField *nextFLD  = new DBObjTableField (RGISNetStreamNext,DBTableFieldInt,"%6d",sizeof (DBInt));
		DBObjRecord *cellRec, *toCellRec, *lineRec;

		arcData->Projection (netData->Projection ());
		arcData->Precision  (netData->Precision ());
		arcData->MaxScale   (netData->MaxScale ());
		arcData->MinScale   (netData->MinScale ());

		lineTable->AddField (basinFLD);
		lineTable->AddField (fieldFLD);
		lineTable->AddField (lengthFLD);
		lineTable->AddField (areaFLD);
		lineTable->AddField (basinAreaFLD);
		lineTable->AddField (nextFLD);

		cellTable->AddField (_RGISAnNetStreamIDFLD = new DBObjTableField ("StreamID",DBTableFieldInt,"%8d",sizeof (DBInt)));

		_RGISAnNetStreamID = 0;
		UIPauseDialogOpen ((char *) "Creating Stream Lines");
		cellID = netIF->CellNum () - 1;
		cellRec = netIF->Cell (cellID);
		if (lineIF->NewSymbol ("Default Symbol") == (DBObjRecord *) NULL)
			{ fprintf (stderr,"Symbol Creation Error in: _RGISAnNetworkStreamLines ()\n"); return; }

		for (;cellID >= 0;--cellID)
			{
			cellRec = netIF->Cell (cellID);
			if (((toCellRec = netIF->ToCell (cellRec)) == (DBObjRecord *) NULL) ||
			    (_RGISAnNetOrderField->Int (cellRec) != _RGISAnNetOrderField->Int (toCellRec)) ||
	 		    (netIF->CellOrder (cellRec)          != netIF->CellOrder (toCellRec)))
				{
				if (UIPause ((netIF->CellNum () - cellRec->RowID ()) * 100 / netIF->CellNum ())) goto Stop;
				sprintf (objName,"Line: %5d",_RGISAnNetStreamID + 1);
				if ((lineRec = lineIF->NewItem (objName)) == (DBObjRecord *) NULL)
					{ fprintf (stderr,"Line Insertion Error in: _RGISAnNetworkStreamLines ()\n"); return; }
				nextFLD->Int (lineRec,toCellRec == (DBObjRecord *) NULL ? 0 : _RGISAnNetStreamIDFLD->Int (toCellRec) + 1);
				basinFLD->Int (lineRec,netIF->CellBasinID (cellRec));
				fieldFLD->Int (lineRec,_RGISAnNetOrderField->Int (cellRec));

				_RGISAnNetVertex = 0;
				netIF->UpStreamSearch (_RGISAnNetworkCellRec = cellRec,(DBNetworkACTION) _RGISAnNetworkUpStreamAction);
				lineIF->FromNode (lineRec,lineIF->Node (netIF->Center (_RGISAnNetworkCellRec),true));
				lineIF->ToNode (lineRec,lineIF->Node (netIF->Center (cellRec) + netIF->Delta (cellRec),true));

				_RGISAnNetArea = netIF->CellArea (_RGISAnNetworkCellRec);
				if (_RGISAnNetVertex > 1)
					{
					if (_RGISAnNetVertexNum < _RGISAnNetVertex - 1)
						{
						_RGISAnNetCoord = (DBCoordinate *) realloc (_RGISAnNetCoord,(_RGISAnNetVertex - 1) * sizeof (DBCoordinate));
						if (_RGISAnNetCoord == (DBCoordinate *) NULL)
							{ perror ("Memory Allocation Error in: _RGISAnNetworkStreamLines ()"); return; }
						}
					_RGISAnNetVertex = 0;
					netIF->DownStreamSearch (netIF->ToCell (_RGISAnNetworkCellRec),(DBNetworkACTION) _RGISAnNetworkDownStreamAction);
					}
				else	_RGISAnNetVertex = 0;
				lineIF->Vertexes (lineRec,_RGISAnNetCoord,_RGISAnNetVertex);
				lineIF->ItemSymbol (lineRec,lineIF->Symbol (0));
				lengthFLD->Float (lineRec,netIF->CellBasinLength (cellRec));
				areaFLD->Float (lineRec,_RGISAnNetArea);
				basinAreaFLD->Float (lineRec,netIF->CellBasinArea (cellRec));
				_RGISAnNetStreamID += 1;
				}
			}
Stop:	UIPauseDialogClose ();
		if (_RGISAnNetCoord != (DBCoordinate *) NULL) free (_RGISAnNetCoord);

		delete lineIF;
		if (cellID >= 0) delete arcData;
		else	workspace->CurrentData (arcData);
		}
	else delete arcData;
	delete netIF;
	}
