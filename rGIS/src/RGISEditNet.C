/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGISEditNet.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

void RGISEditNetBuildCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	widget = widget; workspace = workspace; callData = callData;

	UIPauseDialogOpen ((char *) "Building Topological Networks");
	netIO->Build ();
	UIPauseDialogClose ();

	delete netIO;
	}

void RGISEditNetTrimCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	widget = widget; callData = callData;

	UIPauseDialogOpen ((char *) "Building Topological Networks");
	netIO->Trim ();
	workspace->CurrentData (netData);
	UIPauseDialogClose ();

	delete netIO;
	}

#define RGISNetCellXCoord ((char *) "CellXCoord")
#define RGISNetCellYCoord ((char *) "CellYCoord")

void RGISEditNetAddCellXYCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt cellID;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData =dataset->Data ();
	DBNetworkIO *netIO = new DBNetworkIO (dbData);
	DBObjTable *cellTable = dbData->Table (DBrNCells);
	DBObjTableField *xCoordFLD	= cellTable->Field (RGISNetCellXCoord);
	DBObjTableField *yCoordFLD = cellTable->Field (RGISNetCellYCoord);
	DBCoordinate coord;
	DBObjRecord *cellRec;
	UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,cellTable));

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Adding XY Coordinates");
	if (xCoordFLD == NULL)
		{
		xCoordFLD = new DBObjTableField (RGISNetCellXCoord,DBTableFieldFloat,(char *) "%10.3f",sizeof (DBFloat4));
		cellTable->AddField (xCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (xCoordFLD);
		UIPause (40);
		}
	if (yCoordFLD == NULL)
		{
		yCoordFLD = new DBObjTableField (RGISNetCellYCoord,DBTableFieldFloat,(char *) "%10.3f",sizeof (DBFloat4));
		cellTable->AddField (yCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (yCoordFLD);
		UIPause (80);
		}
	for (cellID = 0;cellID < netIO->CellNum ();++cellID)
		{
		cellRec = netIO->Cell (cellID);
		if (UIPause (80 + cellID * 20 / netIO->CellNum ())) goto Stop;
		coord = netIO->Center  (cellRec);
		xCoordFLD->Float (cellRec,coord.X);
		yCoordFLD->Float (cellRec,coord.Y);
		}
Stop:
	UIPauseDialogClose ();
	if (tableCLS != (UITable *) NULL) tableCLS->Draw ();
	}

#define RGISNetMouthXCoord "MouthXCoord"
#define RGISNetMouthYCoord "MouthYCoord"

void RGISEditNetAddBasinXYCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt basinID;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData =dataset->Data ();
	DBNetworkIO *netIO = new DBNetworkIO (dbData);
	DBObjTable *itemTable = dbData->Table (DBrNItems);
	DBObjTableField *xCoordFLD	= itemTable->Field (RGISNetMouthXCoord);
	DBObjTableField *yCoordFLD = itemTable->Field (RGISNetMouthYCoord);
	DBCoordinate coord;
	DBObjRecord *basinRec;
	UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,itemTable));

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Adding XY Coordinates");
	if (xCoordFLD == NULL)
		{
		xCoordFLD = new DBObjTableField (RGISNetMouthXCoord,DBTableFieldFloat,(char *) "%10.3f",sizeof (DBFloat4));
		itemTable->AddField (xCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (xCoordFLD);
		UIPause (40);
		}
	if (yCoordFLD == NULL)
		{
		yCoordFLD = new DBObjTableField (RGISNetMouthYCoord,DBTableFieldFloat,(char *) "%10.3f",sizeof (DBFloat4));
		itemTable->AddField (yCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (yCoordFLD);
		UIPause (80);
		}
	for (basinID = 0;basinID < netIO->BasinNum ();++basinID)
		{
		basinRec = netIO->Basin (basinID);
		if (UIPause (80 + basinID * 20 / netIO->BasinNum ())) goto Stop;
		coord = netIO->Center  (netIO->MouthCell (basinRec));
		xCoordFLD->Float (basinRec,coord.X);
		yCoordFLD->Float (basinRec,coord.Y);
		}
Stop:
	UIPauseDialogClose ();
	if (tableCLS != (UITable *) NULL) tableCLS->Draw ();
	}

static  DBObjRecord **_RGISReallocCellList (DBObjRecord **cellList,DBInt &maxCellNum, DBInt cellNum)

	{
	if (cellNum <= maxCellNum) return (cellList);
	maxCellNum = cellNum;
	if ((cellList = (DBObjRecord **) realloc (cellList, maxCellNum * sizeof (DBObjRecord *))) == (DBObjRecord **) NULL)
		{ perror ("Memory Allocation Error in: _RGISEditAdjustNetworkCBK "); return ((DBObjRecord **) NULL); }
	return (cellList);
	}

void RGISEditNetMagnitudeCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Calculate Shreve Magnitude");
	netIO->SetMagnitude ();
	UIPauseDialogClose ();
	delete netIO;
	}

void RGISEditNetDistToMouthCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Calculate Distance from Ocean");
	netIO->SetDistToMouth ();
	UIPauseDialogClose ();
	delete netIO;
	}

void RGISEditNetDistToOceanCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Calculate Distance to Ocean");
	netIO->SetDistToOcean ();
	UIPauseDialogClose ();
	delete netIO;
	}

void RGISEditAdjustNetworkCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt vertex, vertexNum, dir, cell, cellNum, maxCellNum = 0;
	DBPosition pos0, pos1;
	DBCoordinate *vertexes;
	DBObjRecord *lineRec, **cellList = (DBObjRecord **) NULL;
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *lineData = netData->LinkedData ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBVLineIO *lineIO = new DBVLineIO (lineData);

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Adjusting Networks");
	cellNum = 0;
	for (lineRec = lineIO->FirstItem ();lineRec != (DBObjRecord *) NULL; lineRec = lineIO->NextItem ())
		{
		UIPause (lineRec->RowID () * 100 / lineIO->ItemNum ());
		if (lineIO->ToNode (lineRec) == lineIO->FromNode (lineRec)) continue;
		if ((cellList = _RGISReallocCellList (cellList,maxCellNum,cellNum + 1)) == (DBObjRecord **) NULL) return;
		if ((cellList [cellNum] = netIO->Cell (lineIO->FromCoord (lineRec))) == (DBObjRecord *) NULL)
			if((cellList [cellNum] = netIO->CellAdd (lineIO->FromCoord (lineRec))) != (DBObjRecord *) NULL) cellNum++;
		if ((vertexNum = lineIO->VertexNum (lineRec)) > 0)
			{
			vertexes = lineIO->Vertexes (lineRec);
			for (vertex = 0; vertex < vertexNum; ++vertex)
				{
				if ((cellList = _RGISReallocCellList (cellList,maxCellNum,cellNum + 1)) == (DBObjRecord **) NULL) return;
				if ((cellList [cellNum] = netIO->Cell (vertexes [vertex])) == (DBObjRecord *) NULL)
					if((cellList [cellNum] = netIO->CellAdd (vertexes [vertex])) == (DBObjRecord *) NULL) continue;
				for (cell = 0;cell < cellNum;++cell) if (cellList [cell] == cellList [cellNum]) break;
				if (cell != cellNum) continue;
				cellNum++;
/*				if (cellNum > 1)
					{
					pos0 = netIO->CellPosition (cellList [cellNum - 2]);
					pos1 = netIO->CellPosition (cellList [cellNum - 1]);
					printf ("%5d   %3d %3d  %3d %3d\n",lineRec->RowID (),pos0.Col, pos0.Row, pos1.Col, pos1.Row);
					if ((abs (pos0.Row - pos1.Row) <= 1) && (abs (pos0.Col - pos1.Col) <= 1)) continue;
					if (abs (pos0.Row - pos1.Row) > abs (pos0.Col - pos1.Col))
						for (pos.Row = pos0.Row; pos.Row  != pos1.Row;
							  pos.Row = (pos0.Row < pos1.Row) ? pos.Row + 1 : pos.Row - 1)
							{
							pos.Col = pos0.Col + (DBUShort) ((((DBInt) pos.Row - (DBInt) pos0.Row) * ((DBInt) pos1.Col - (DBInt) pos0.Col)) / ((DBInt) pos1.Row - (DBInt) pos0.Row));
							if ((cellList = _RGISReallocCellList (cellList,maxCellNum,cellNum + 1)) == (DBObjRecord **) NULL) return;
							if ((cellRec = netIO->Cell (pos)) != (DBObjRecord *) NULL)
								{
								cellList [cellNum] = cellList [cellNum - 1];
								cellList [cellNum - 1] = cellRec;
								cellNum++;
								}
							}
					else
						for (pos.Col = pos0.Col; pos.Col != pos1.Col;
							  pos.Col = (pos0.Col < pos1.Col) ? pos.Col + 1 : pos.Col - 1)
							{
							pos.Row = pos0.Row + (DBUShort) ((((DBInt) pos.Col - (DBInt) pos0.Col) * ((DBInt) pos1.Row - (DBInt) pos0.Row)) / ((DBInt) pos1.Col - (DBInt) pos0.Col));
							if ((cellList = _RGISReallocCellList (cellList,maxCellNum,cellNum + 1)) == (DBObjRecord **) NULL) return;
							if ((cellRec = netIO->Cell (pos)) != (DBObjRecord *) NULL)
								{
								cellList [cellNum] = cellList [cellNum - 1];
								cellList [cellNum - 1] = cellRec;
								cellNum++;
								}
							}
					}
*/				}
			}
 		for (cell = 0;cell < cellNum - 1;++cell)
 			{
			pos0 = netIO->CellPosition (cellList [cell]);
			pos1 = netIO->CellPosition (cellList [cell + 1]);
			if ((pos0.Col <  pos1.Col) && (pos0.Row == pos1.Row))	dir = DBNetDirE;
			else if ((pos0.Col <  pos1.Col) && (pos0.Row >  pos1.Row))	dir = DBNetDirSE;
			else if ((pos0.Col == pos1.Col) && (pos0.Row >  pos1.Row))	dir = DBNetDirS;
			else if ((pos0.Col >  pos1.Col) && (pos0.Row >  pos1.Row))	dir = DBNetDirSW;
			else if ((pos0.Col >  pos1.Col) && (pos0.Row == pos1.Row))	dir = DBNetDirW;
			else if ((pos0.Col >  pos1.Col) && (pos0.Row <  pos1.Row))	dir = DBNetDirNW;
			else if ((pos0.Col == pos1.Col) && (pos0.Row <  pos1.Row))	dir = DBNetDirN;
			else if ((pos0.Col <  pos1.Col) && (pos0.Row <  pos1.Row))	dir = DBNetDirNE;
//			printf ("%5d   %3d %3d  %3d %3d  %2x\n",lineRec->RowID (),pos0.Col, pos0.Row, pos1.Col, pos1.Row,dir);
			netIO->CellDirection (cellList [cell],dir);
			}
		}
	if (maxCellNum > 0) free (cellList);
	UIPauseDialogClose ();
	UIPauseDialogOpen ((char *) "Building Networks");
//	netIO->Build ();
	UIPauseDialogClose ();
	}
