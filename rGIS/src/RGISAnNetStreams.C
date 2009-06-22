/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

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

static int _RGISAnNetworkUpStreamAction (DBNetworkIO *netIO,DBObjRecord *cellRec)

	{
	DBInt nextOrder [2];

	if (_RGISAnNetVertex < 1)
		{
		nextOrder[0] = _RGISAnNetOrderField->Int (cellRec);
		nextOrder[1] = netIO->CellOrder (cellRec);
		}
	else
		{
		nextOrder[0] = _RGISAnNetOrderField->Int (netIO->ToCell (cellRec));
		nextOrder[1] = netIO->CellOrder (netIO->ToCell(cellRec));
		}

	if ((_RGISAnNetOrderField->Int (cellRec) == nextOrder[0]) && (netIO->CellOrder (cellRec) == nextOrder[1]))
		{
		_RGISAnNetStreamIDFLD->Int (cellRec,_RGISAnNetStreamID);
		_RGISAnNetworkCellRec = cellRec;
		_RGISAnNetVertex += 1;
		return (true);
		}
	return (false);
	}

static int _RGISAnNetworkDownStreamAction (DBNetworkIO *netIO,DBObjRecord *cellRec)

	{
	if (_RGISAnNetStreamIDFLD->Int (cellRec) != _RGISAnNetStreamID) return (false);
	_RGISAnNetArea += netIO->CellArea (cellRec);
	_RGISAnNetCoord [_RGISAnNetVertex++] = netIO->Center (cellRec);
	return (true);
	}

void RGISAnNetworkStreamLinesCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjTable *cellTable = netData->Table (DBrNCells);
	DBObjData *arcData = new DBObjData ("",DBTypeVectorLine);
	DBNetworkIO *netIO = new DBNetworkIO (netData);
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
		DBVLineIO *lineIO = new DBVLineIO (arcData);
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
		cellID = netIO->CellNum () - 1;
		cellRec = netIO->Cell (cellID);
		if (lineIO->NewSymbol ("Default Symbol") == (DBObjRecord *) NULL)
			{ fprintf (stderr,"Symbol Creation Error in: _RGISAnNetworkStreamLines ()\n"); return; }

		for (;cellID >= 0;--cellID)
			{
			cellRec = netIO->Cell (cellID);
			if (((toCellRec = netIO->ToCell (cellRec)) == (DBObjRecord *) NULL) ||
			    (_RGISAnNetOrderField->Int (cellRec) != _RGISAnNetOrderField->Int (toCellRec)) ||
	 		    (netIO->CellOrder (cellRec)          != netIO->CellOrder (toCellRec)))
				{
				if (UIPause ((netIO->CellNum () - cellRec->RowID ()) * 100 / netIO->CellNum ())) goto Stop;
				sprintf (objName,"Line: %5d",_RGISAnNetStreamID + 1);
				if ((lineRec = lineIO->NewItem (objName)) == (DBObjRecord *) NULL)
					{ fprintf (stderr,"Line Insertion Error in: _RGISAnNetworkStreamLines ()\n"); return; }
				nextFLD->Int (lineRec,toCellRec == (DBObjRecord *) NULL ? 0 : _RGISAnNetStreamIDFLD->Int (toCellRec) + 1);
				basinFLD->Int (lineRec,netIO->CellBasinID (cellRec));
				fieldFLD->Int (lineRec,_RGISAnNetOrderField->Int (cellRec));

				_RGISAnNetVertex = 0;
				netIO->UpStreamSearch (_RGISAnNetworkCellRec = cellRec,(DBNetworkACTION) _RGISAnNetworkUpStreamAction);
				lineIO->FromNode (lineRec,lineIO->Node (netIO->Center (_RGISAnNetworkCellRec),true));
				lineIO->ToNode (lineRec,lineIO->Node (netIO->Center (cellRec) + netIO->Delta (cellRec),true));

				_RGISAnNetArea = netIO->CellArea (_RGISAnNetworkCellRec);
				if (_RGISAnNetVertex > 1)
					{
					if (_RGISAnNetVertexNum < _RGISAnNetVertex - 1)
						{
						_RGISAnNetCoord = (DBCoordinate *) realloc (_RGISAnNetCoord,(_RGISAnNetVertex - 1) * sizeof (DBCoordinate));
						if (_RGISAnNetCoord == (DBCoordinate *) NULL)
							{ perror ("Memory Allocation Error in: _RGISAnNetworkStreamLines ()"); return; }
						}
					_RGISAnNetVertex = 0;
					netIO->DownStreamSearch (netIO->ToCell (_RGISAnNetworkCellRec),(DBNetworkACTION) _RGISAnNetworkDownStreamAction);
					}
				else	_RGISAnNetVertex = 0;
				lineIO->Vertexes (lineRec,_RGISAnNetCoord,_RGISAnNetVertex);
				lineIO->ItemSymbol (lineRec,lineIO->Symbol (0));
				lengthFLD->Float (lineRec,netIO->CellBasinLength (cellRec));
				areaFLD->Float (lineRec,_RGISAnNetArea);
				basinAreaFLD->Float (lineRec,netIO->CellBasinArea (cellRec));
				_RGISAnNetStreamID += 1;
				}
			}
Stop:	UIPauseDialogClose ();
		if (_RGISAnNetCoord != (DBCoordinate *) NULL) free (_RGISAnNetCoord);

		delete lineIO;
		if (cellID >= 0) delete arcData;
		else	workspace->CurrentData (arcData);
		}
	else delete arcData;
	delete netIO;
	}
