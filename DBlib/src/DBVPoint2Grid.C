/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBVPoint2Grid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

DBVPointIF	*_DBPntIF;
DBNetworkIF *_DBNetIF;
DBGridIF 	*_DBGrdIF;

static DBInt _DBPointSort (const void *obj0,const void *obj1)

	{
	DBObjRecord *cellRec0 = _DBNetIF->Cell (_DBPntIF->Coordinate (((DBObjRecord **) obj0) [0]));
	DBObjRecord *cellRec1 = _DBNetIF->Cell (_DBPntIF->Coordinate (((DBObjRecord **) obj1) [0]));

	if (cellRec0 == cellRec1) return (0);
	if (_DBNetIF->CellBasinArea (cellRec0) > _DBNetIF->CellBasinArea (cellRec1)) return  (1);
	if (_DBNetIF->CellBasinArea (cellRec0) < _DBNetIF->CellBasinArea (cellRec1)) return (-1);
	return (0);
	}


static DBInt DBPointUpStreamAction (DBNetworkIF *netIF,DBObjRecord *cellRec,DBInt pntID)

	{
	_DBGrdIF->Value (netIF->CellPosition (cellRec),pntID);
	return (true);
	}

DBInt DBPointToGrid (DBObjData *pntData,DBObjData *netData,DBObjData *grdData)

	{
	DBInt i, recID;
	DBPosition pos;
	DBObjTable *pntTable = pntData->Table (DBrNItems);
	DBObjTable *grdTable = grdData->Table (DBrNItems);
	DBObjTable *symTable = grdData->Table (DBrNSymbols);
	DBObjectLIST<DBObjTableField> *pntFields = pntTable->Fields ();
	DBObjTableField *pntFLD;
	DBObjTableField *grdAttribFLD;
	DBObjTableField *grdFLD = grdTable->Field (DBrNGridValue);
	DBObjTableField *symFLD = grdTable->Field (DBrNSymbol);
	DBObjRecord *cellRec, **recARR;
	DBObjRecord *symRec = symTable->First ();

	_DBPntIF = new DBVPointIF(pntData);

	if ((recARR = (DBObjRecord **) calloc (_DBPntIF->ItemNum (),2 * sizeof (DBObjRecord *))) == (DBObjRecord **) NULL)
		{
		CMmsgPrint (CMmsgAppError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		delete _DBPntIF;
		return (DBFault);
		}
	_DBGrdIF = new DBGridIF (grdData);
	_DBGrdIF->RenameLayer (_DBGrdIF->Layer ((DBInt) 0),(char *) "Subbasins");
	_DBNetIF	= new DBNetworkIF (netData);

	for (pos.Row = 0;pos.Row < _DBGrdIF->RowNum ();++pos.Row)
		for (pos.Col = 0;pos.Col < _DBGrdIF->ColNum ();++pos.Col)	_DBGrdIF->Value (pos,DBFault);
	grdTable->AddField (new DBObjTableField (DBrNGridArea,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4)));
	grdTable->AddField (new DBObjTableField (DBrNGridPercent,DBTableFieldFloat,"%5.1f",sizeof (DBFloat4)));
	for (pntFLD = pntFields->First ();pntFLD != (DBObjTableField *) NULL;pntFLD = pntFields->Next ())
		if (DBTableFieldIsVisible (pntFLD)) grdTable->AddField (new DBObjTableField (*pntFLD));

	recID = 0;
	for (i = 0;i < _DBPntIF->ItemNum ();++i)
		{
		DBPause (i * 100 / _DBPntIF->ItemNum ());
		recARR [(recID << 0x01)] = _DBPntIF->Item (i);
		if ((recARR [(recID << 0x01)]->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		if (_DBNetIF->Cell (_DBPntIF->Coordinate (recARR [(recID << 0x01)])) == (DBObjRecord *) NULL) continue;
		recARR [(recID << 0x01) + 1] = grdTable->Add (recARR [(recID << 0x01)]->Name ());
		grdFLD->Int (recARR [(recID << 0x01) + 1],recARR [(recID << 0x01)]->RowID () + 1);
		symFLD->Record (recARR [(recID << 0x01) + 1],symRec);
		for (pntFLD = pntFields->First ();pntFLD != (DBObjTableField *) NULL;pntFLD = pntFields->Next ())
			if ((grdAttribFLD = grdTable->Field (pntFLD->Name ())) != (DBObjTableField *) NULL)
				switch (pntFLD->Type ())
					{
					case DBTableFieldString:
						grdAttribFLD->String (recARR [(recID << 0x01) + 1], pntFLD->String	(recARR [(recID << 0x01)]));
						break;
					case DBTableFieldInt:
						grdAttribFLD->Int 	(recARR [(recID << 0x01) + 1], pntFLD->Int 	   (recARR [(recID << 0x01)]));
						break;
					case DBTableFieldFloat:
						grdAttribFLD->Float 	(recARR [(recID << 0x01) + 1], pntFLD->Float	   (recARR [(recID << 0x01)]));
						break;
					case DBTableFieldDate:
						grdAttribFLD->Date 	(recARR [(recID << 0x01) + 1], pntFLD->Date	   (recARR [(recID << 0x01)]));
						break;
					}
		recID++;
		}

	qsort (recARR,recID,2 * sizeof (DBObjRecord *),_DBPointSort);

	for (recID = recID - 1;recID >= 0;--recID)
		{
		if ((cellRec = _DBNetIF->Cell (_DBPntIF->Coordinate (recARR [(recID << 0x01)]))) == (DBObjRecord *) NULL) continue;
		_DBNetIF->UpStreamSearch (cellRec,(DBNetworkACTION) DBPointUpStreamAction,(void *) recARR [(recID << 0x01) + 1]->RowID ());
		}
	_DBGrdIF->DiscreteStats ();
	delete _DBPntIF;
	delete _DBGrdIF;
	delete _DBNetIF;
	free (recARR);
	return (DBSuccess);
	}

DBInt DBPointToGrid (DBObjData *pntData,DBObjData *grdData,DBFloat factor)

	{
	DBInt startID, pnt0ID, pntID, id, itemNum;
	double dist, minDist, box, box0, bWidth, bHeight;
	DBPosition pos;
	DBCoordinate gCoord, *pCoord;
	DBObjRecord *grdRec, *pntRec, *symRec;
	DBObjTable  *itemTable, *symTable;
	DBObjTableField *valField, *symField;
	DBVPointIF	*pntIF;
	DBGridIF 	*gridIF;
	DBMathDistanceFunction distFunc = DBMathGetDistanceFunction (pntData);

	if (distFunc != DBMathGetDistanceFunction (grdData))
	{ CMmsgPrint (CMmsgAppError, "Incompatible projections in: %s %d",__FILE__,__LINE__); return (DBFault); }

	pntIF = new DBVPointIF(pntData);
	itemNum = pntIF->ItemNum ();
	if ((pCoord = (DBCoordinate *) calloc (itemNum,sizeof (DBCoordinate))) == (DBCoordinate *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation Error in: %s %d",__FILE__,__LINE__); return (DBFault); }

	gridIF = new DBGridIF (grdData);
	for (pntID = 0;pntID < itemNum;++pntID) pCoord [pntID] = pntIF->Coordinate (pntIF->Item (pntID));

	if (grdData->Type () == DBTypeGridContinuous)
		gridIF->RenameLayer (gridIF->Layer ((DBInt) 0),(char *) "Distance to Station");
	else
		{
		gridIF->RenameLayer (gridIF->Layer ((DBInt) 0),(char *) "Station grid");
		itemTable = grdData->Table (DBrNItems);
		symTable  = grdData->Table (DBrNSymbols);
	   valField  = itemTable->Field (DBrNGridValue);
		symField  = itemTable->Field (DBrNSymbol);
		if ((symRec = symTable->Item (0)) == (DBObjRecord *) NULL)
			CMmsgPrint (CMmsgAppError, "Total Metal Gebasz in: %s %d",__FILE__,__LINE__);
		for (pntID = 0;pntID < itemNum; ++pntID)
			{
			pntRec = pntIF->Item (pntID);
			grdRec = itemTable->Add (pntRec->Name ());
			valField->Int (grdRec,pntID + 1);
			symField->Record (grdRec,symRec);
			}
		}

	startID = 0;
	for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
		{
		DBPause (pos.Row * 100 / gridIF->RowNum ());
		for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
			{
			gridIF->Pos2Coord (pos,gCoord);
			minDist = box0  = DBHugeVal;
			pnt0ID  = pntID = startID;
			id = DBFault;
			do {
				bWidth  = fabs (gCoord.X - pCoord [pntID].X);
				bHeight = fabs (gCoord.Y - pCoord [pntID].Y);
				box = bWidth > bHeight ? bWidth : bHeight;
				if ((box < box0) && ((dist = DBMathCoordinateDistance (distFunc,gCoord, pCoord [pntID])) < minDist))
					{
					minDist = dist;
					id = startID = pntID;
					box *= factor;
					if (box0 > box) box0 = box;
					}
				pntID = pntID + 1 < itemNum ? pntID + 1 : 0;
				} while (pntID != pnt0ID);
			if (grdData->Type () == DBTypeGridContinuous) gridIF->Value (pos,minDist); else gridIF->Value (pos,id);
			}
		}
	if (grdData->Type () == DBTypeGridContinuous) gridIF->RecalcStats (); else gridIF->DiscreteStats ();
	delete gridIF;
	free (pCoord);
	return (DBSuccess);
	}
