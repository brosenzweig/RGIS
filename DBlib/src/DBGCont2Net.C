/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBGCont2Net.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

DBInt DBGridCont2Network (DBObjData *gridData,DBObjData *netData, bool downhill)

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
	DBGridIF *gridIF = new DBGridIF (gridData), *zGridIF;
	DBNetworkIF *netIF;

	if ((zGridData != (DBObjData *) NULL) && ((zGridData->Type () == DBTypeGridDiscrete) || (zGridData->Type () == DBTypeGridContinuous)))
		{
		zGridIF = new DBGridIF (zGridData);
		zLayerNum = zGridIF->LayerNum () + 1;
		}
	else { zGridIF = (DBGridIF *) NULL; zLayerNum = 1; }

	if ((zones = (DBInt *) calloc (9 * zLayerNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		perror ("Memory Allocation Error in: DBGridCont2Network ()");
		if (zGridIF != (DBGridIF *) NULL) delete zGridIF;
		delete gridIF;
		return (DBFault);
		}
	layerTable->Add (DBrNLookupGrid);
	if ((layerRec = layerTable->Item (DBrNLookupGrid)) == (DBObjRecord *) NULL)
		{
		free (zones);
		if (zGridIF != (DBGridIF *) NULL) delete zGridIF;
		delete gridIF;
		return (DBFault);
		}

	netData->Projection (projection);
	netData->Extent (gridData->Extent ());
	cellWidthFLD->Float  (layerRec,gridIF->CellWidth ());
	cellHeightFLD->Float (layerRec,gridIF->CellHeight ());
	valueTypeFLD->Int (layerRec,DBTableFieldInt);
	valueSizeFLD->Int (layerRec,sizeof (DBInt));
	rowNumFLD->Int (layerRec,gridIF->RowNum ());
	colNumFLD->Int (layerRec,gridIF->ColNum ());

	dataRec = new DBObjRecord ("NetLookupGridRecord",gridIF->RowNum () * gridIF->ColNum () * sizeof (DBInt),sizeof (DBInt));
	if (dataRec == (DBObjRecord *) NULL)
		{
		if (zGridIF != (DBGridIF *) NULL) delete zGridIF;
		return (DBFault);
		}
	layerFLD->Record (layerRec,dataRec);
	(netData->Arrays ())->Add (dataRec);
	for (pos.Row = 0;pos.Row < gridIF->RowNum ();pos.Row++)
		for (pos.Col = 0;pos.Col < gridIF->ColNum ();pos.Col++)
			((DBInt *) dataRec->Data ()) [pos.Row * gridIF->ColNum () + pos.Col] = DBFault;

	for (pos.Row = 0;pos.Row < gridIF->RowNum ();pos.Row++)
		{
		if (DBPause (10 * pos.Row / gridIF->RowNum ())) goto PauseStop;
		for (pos.Col = 0;pos.Col < gridIF->ColNum ();pos.Col++)
			{
			gridIF->Pos2Coord (pos,coord0);
			zLayerID = 0;
			if (zGridIF != (DBGridIF *) NULL)
				for ( ; zLayerID < zGridIF->LayerNum (); zLayerID++)
					{
					layerRec = zGridIF->Layer (zLayerID);
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
						if (row >= gridIF->RowNum ()) continue;
						if (col >= gridIF->ColNum ()) continue;
						auxPos.Row = row;
						auxPos.Col = col;
						gridIF->Pos2Coord (auxPos,coord1);
						switch (zGridData->Type ())
							{
							case DBTypeGridDiscrete:	basinID = zGridIF->GridValue (layerRec,coord1);	break;
							case DBTypeGridContinuous:	zGridIF->Value (layerRec,coord1,&basinID);		break;
							}
						zones [zLayerID * 9 + dir] = basinID;
						}
					switch (zGridData->Type ())
						{
						case DBTypeGridDiscrete:	basinID = zGridIF->GridValue (layerRec,coord0);	break;
						case DBTypeGridContinuous: zGridIF->Value (layerRec,coord0,&basinID);		break;
						}
					zones [zLayerID * 9 + 8] = basinID;
					}
			for (dir = 0;dir < 9;++dir) zones [zLayerID * 9 + dir] = 0;

			maxDir = DBFault;
			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				if (gridIF->Value (layerRec,pos,&elev0))
					{
					maxDelta = (DBFloat) 0.0;
					maxDir   = 0;

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
							if (col >= gridIF->ColNum ()) continue;
							if (row >= gridIF->RowNum ()) continue;
							auxPos.Row = row;
							auxPos.Col = col;
							gridIF->Pos2Coord (auxPos,coord1);
							distance = DBMathCoordinateDistance (projection,coord0,coord1);
							if ((zones [zLayerID * 9 + dir] == zones [zLayerID * 9 + 8]) && (gridIF->Value (layerRec,auxPos,&elev1)))
								{
								delta = (downhill ? (elev1 - elev0) : (elev0 - elev1)) / distance;
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
				((DBInt *) dataRec->Data ()) [pos.Row * gridIF->ColNum () + pos.Col] = cellRec->RowID ();
				}
			}
		}
PauseStop:
	if (pos.Row < gridIF->RowNum ())	return (DBFault);
	sprintf (nameSTR,"GHAASBasin%d",(DBInt) 0);
	basinRec = basinTable->Add (nameSTR);
	mouthPosFLD->Position	(basinRec,positionFLD->Position (cellTable->Item (0)));
	colorFLD->Int				(basinRec,0);

	free (zones);
	delete gridIF;
	if (zGridIF != (DBGridIF *) NULL) delete zGridIF;

	netData->Precision (DBMathMin (gridIF->CellWidth (),gridIF->CellHeight ()) / 25.0);
	netIF = new DBNetworkIF (netData);
	netIF->Build ();
	delete netIF;
	return (DBSuccess);
	}
