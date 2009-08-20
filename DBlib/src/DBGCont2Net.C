/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

DBGCont2Net.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

DBInt DBGridCont2Network (DBObjData *gridData,DBObjData *netData)

	{
	DBInt basinID, layerID, zLayerID, zLayerNum, dir, maxDir, projection = gridData->Projection (), *zones;
	DBFloat elev0, elev1, delta, maxDelta, distance;
	DBCoordinate coord0, coord1;
	DBInt row, col;
	DBPosition pos, auxPos;
	char nameSTR [DBStringLength];
	DBObjTable *basinTable = netData->Table (DBrNItems);
	DBObjTable *cellTable  = netData->Table (DBrNCells);
	DBObjTable *layerTable = netData->Table (DBrNLayers);
	DBObjRecord *layerRec, *dataRec, *cellRec, *basinRec;

	DBObjTableField *mouthPosFLD = basinTable->Field (DBrNMouthPos);
	DBObjTableField *colorFLD		= basinTable->Field (DBrNColor);

	DBObjTableField *positionFLD	= cellTable->Field (DBrNPosition);
	DBObjTableField *toCellFLD		= cellTable->Field (DBrNToCell);
	DBObjTableField *fromCellFLD	= cellTable->Field (DBrNFromCell);
	DBObjTableField *orderFLD		= cellTable->Field (DBrNOrder);
	DBObjTableField *basinFLD		= cellTable->Field (DBrNBasin);
	DBObjTableField *basinCellsFLD= cellTable->Field (DBrNBasinCells);
	DBObjTableField *travelFLD		= cellTable->Field (DBrNTravel);
	DBObjTableField *upCellPosFLD	= cellTable->Field (DBrNUpCellPos);
	DBObjTableField *cellAreaFLD	= cellTable->Field (DBrNCellArea);
	DBObjTableField *subbasinLengthFLD = cellTable->Field (DBrNSubbasinLength);
	DBObjTableField *subbasinAreaFLD = cellTable->Field (DBrNSubbasinArea);

	DBObjTableField *rowNumFLD = layerTable->Field (DBrNRowNum);
	DBObjTableField *colNumFLD = layerTable->Field (DBrNColNum);
	DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
	DBObjTableField *cellHeightFLD = layerTable->Field (DBrNCellHeight);
	DBObjTableField *valueTypeFLD = layerTable->Field (DBrNValueType);
	DBObjTableField *valueSizeFLD = layerTable->Field (DBrNValueSize);
	DBObjTableField *layerFLD = layerTable->Field (DBrNLayer);
	DBObjData *zGridData = gridData->LinkedData ();
	DBGridIO *gridIO = new DBGridIO (gridData), *zGridIO;
	DBNetworkIO *networkIO;

	if ((zGridData != (DBObjData *) NULL) && ((zGridData->Type () == DBTypeGridDiscrete) || (zGridData->Type () == DBTypeGridContinuous)))
		{
		zGridIO = new DBGridIO (zGridData);
		zLayerNum = zGridIO->LayerNum () + 1;
		}
	else { zGridIO = (DBGridIO *) NULL; zLayerNum = 1; }

	if ((zones = (DBInt *) calloc (9 * zLayerNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		perror ("Memory Allocation Error in: DBGridCont2Network ()");
		if (zGridIO != (DBGridIO *) NULL) delete zGridIO;
		delete gridIO;
		return (DBFault);
		}
	layerTable->Add (DBrNLookupGrid);
	if ((layerRec = layerTable->Item (DBrNLookupGrid)) == (DBObjRecord *) NULL)
		{
		free (zones);
		if (zGridIO != (DBGridIO *) NULL) delete zGridIO;
		delete gridIO;
		return (DBFault);
		}

	netData->Projection (projection);
	netData->Extent (gridData->Extent ());
	cellWidthFLD->Float  (layerRec,gridIO->CellWidth ());
	cellHeightFLD->Float (layerRec,gridIO->CellHeight ());
	valueTypeFLD->Int (layerRec,DBTableFieldInt);
	valueSizeFLD->Int (layerRec,sizeof (DBInt));
	rowNumFLD->Int (layerRec,gridIO->RowNum ());
	colNumFLD->Int (layerRec,gridIO->ColNum ());

	dataRec = new DBObjRecord ("NetLookupGridRecord",gridIO->RowNum () * gridIO->ColNum () * sizeof (DBInt),sizeof (DBInt));
	if (dataRec == (DBObjRecord *) NULL)
		{
		if (zGridIO != (DBGridIO *) NULL) delete zGridIO;
		return (DBFault);
		}
	layerFLD->Record (layerRec,dataRec);
	(netData->Arrays ())->Add (dataRec);
	for (pos.Row = 0;pos.Row < gridIO->RowNum ();pos.Row++)
		for (pos.Col = 0;pos.Col < gridIO->ColNum ();pos.Col++)
			((DBInt *) dataRec->Data ()) [pos.Row * gridIO->ColNum () + pos.Col] = DBFault;

	for (pos.Row = 0;pos.Row < gridIO->RowNum ();pos.Row++)
		{
		if (DBPause (10 * pos.Row / gridIO->RowNum ())) goto PauseStop;
		for (pos.Col = 0;pos.Col < gridIO->ColNum ();pos.Col++)
			{
			gridIO->Pos2Coord (pos,coord0);
			zLayerNum = 0;
			if (zGridIO != (DBGridIO *) NULL)
				for (zLayerID = 0; zLayerID < zGridIO->LayerNum (); zLayerID++)
					{
					layerRec = zGridIO->Layer (zLayerID);
					if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
					for (dir = 0;dir < 8;++dir)
						{
						row = pos.Row;
						col = pos.Col;
						if (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirN) || ((0x01 << dir) == DBNetDirNE)) row++;
						if (((0x01 << dir) == DBNetDirSE) || ((0x01 << dir) == DBNetDirS) || ((0x01 << dir) == DBNetDirSW)) row--;
						if (((0x01 << dir) == DBNetDirNE) || ((0x01 << dir) == DBNetDirE) || ((0x01 << dir) == DBNetDirSE)) col++;
						if (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirW) || ((0x01 << dir) == DBNetDirSW)) col--;
						if (row < 0) continue;
						if (col < 0) continue;
						if (row >= gridIO->RowNum ()) continue;
						if (col >= gridIO->ColNum ()) continue;
						gridIO->Pos2Coord (auxPos,coord1);
						switch (zGridData->Type ())
							{
							case DBTypeGridDiscrete:	basinID = zGridIO->GridValue (layerRec,coord1);	break;
							case DBTypeGridContinuous:	zGridIO->Value (layerRec,coord1,&basinID);		break;
							}
						zones [zLayerNum * 9 + dir] = basinID;
						}
					switch (zGridData->Type ())
						{
						case DBTypeGridDiscrete:	basinID = zGridIO->GridValue (layerRec,coord0);	break;
						case DBTypeGridContinuous: zGridIO->Value (layerRec,coord0,&basinID);		break;
						}
					zones [zLayerNum * 9 + 8] = basinID;
					zLayerNum++;
					}
			else { for (dir = 0;dir < 9;++dir) zones [zLayerNum * 9 + dir] = DBFault; zLayerNum++; }

			maxDir = DBFault;
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				if (gridIO->Value (layerRec,pos,&elev0))
					{
					delta = maxDelta = (DBFloat) 0.0;
					maxDir = 0;

					for (zLayerID = 0;zLayerID < zLayerNum;++zLayerID)
						{
						for (dir = 0;dir < 8;++dir)
							{
							row = pos.Row;
							col = pos.Col;
							if (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirN) || ((0x01 << dir) == DBNetDirNE)) row++;
							if (((0x01 << dir) == DBNetDirSE) || ((0x01 << dir) == DBNetDirS) || ((0x01 << dir) == DBNetDirSW)) row--;
							if (((0x01 << dir) == DBNetDirNE) || ((0x01 << dir) == DBNetDirE) || ((0x01 << dir) == DBNetDirSE)) col++;
							if (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirW) || ((0x01 << dir) == DBNetDirSW)) col--;
							if (col < 0) continue;
							if (row < 0) continue;
							if (col >= gridIO->ColNum ()) continue;
							if (row >= gridIO->RowNum ()) continue;
							auxPos.Row = row;
							auxPos.Col = col;
							gridIO->Pos2Coord (auxPos,coord1);
							if ((zones [zLayerID * 9 + dir] == zones [zLayerID * 9 + 8]) && (gridIO->Value (layerRec,auxPos,&elev1)))
								{
								distance = DBMathCoordinateDistance (projection,coord0,coord1);
								delta = (elev1 - elev0) / distance;
								if (maxDelta > delta) { maxDelta = delta; maxDir = (0x01 << dir); }
								}
							}
						if (maxDir != 0) goto SlopeStop;
						}
					}
				}
SlopeStop:
			if (maxDir != DBFault)
				{
				sprintf (nameSTR,"GHAASCell:%d",cellTable->ItemNum ());
				cellRec = cellTable->Add (nameSTR);
				positionFLD->Position(cellRec,pos);
				toCellFLD->Int			(cellRec,(DBInt) maxDir);
				fromCellFLD->Int		(cellRec,(DBInt) 0);
				orderFLD->Int			(cellRec,(DBInt) 0);
				basinFLD->Int			(cellRec,(DBInt) 0);
				basinCellsFLD->Int	(cellRec,(DBInt) 0);
				travelFLD->Int			(cellRec,(DBInt) 0);
				upCellPosFLD->Position	(cellRec,pos);
				cellAreaFLD->Float	(cellRec,(DBFloat) 0.0);
				subbasinLengthFLD->Float(cellRec,(DBFloat) 0.0);
				subbasinAreaFLD->Float	(cellRec,(DBFloat) 0.0);
				((DBInt *) dataRec->Data ()) [pos.Row * gridIO->ColNum () + pos.Col] = cellRec->RowID ();
				}
			}
		}
PauseStop:
	if (pos.Row < gridIO->RowNum ())	return (DBFault);
	sprintf (nameSTR,"GHAASBasin%d",(DBInt) 0);
	basinRec = basinTable->Add (nameSTR);
	mouthPosFLD->Position	(basinRec,positionFLD->Position (cellTable->Item (0)));
	colorFLD->Int				(basinRec,0);

	free (zones);
	delete gridIO;
	if (zGridIO != (DBGridIO *) NULL) delete zGridIO;

	netData->Precision (DBMathMin (gridIO->CellWidth (),gridIO->CellHeight ()) / 25.0);
	networkIO = new DBNetworkIO (netData);
	networkIO->Build ();
	delete networkIO;
	return (DBSuccess);
	}
