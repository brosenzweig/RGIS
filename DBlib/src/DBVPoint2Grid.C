/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBVPoint2Grid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

DBInt DBPointToGrid (DBObjData *pntData,DBObjData *netData,DBObjData *grdData)

	{
	DBInt i;
	DBPosition pos;
	DBObjTable *pntTable = pntData->Table (DBrNItems);
	DBObjTable *grdTable = grdData->Table (DBrNItems);
	DBObjTable *symTable = grdData->Table (DBrNSymbols);
	DBObjectLIST<DBObjTableField> *pntFields = pntTable->Fields ();
	DBObjTableField *pntFLD;
	DBObjTableField *grdAttribFLD;
	DBObjTableField *grdFLD = grdTable->Field (DBrNGridValue);
	DBObjTableField *symFLD = grdTable->Field (DBrNSymbol);
	DBObjRecord *cellRec, *toCell, *pntRec, *itemRec;
	DBObjRecord *symRec = symTable->First ();
	DBVPointIF	*pntIF;
	DBNetworkIF *netIF;
	DBGridIF 	*grdIF;

	pntIF = new DBVPointIF  (pntData);
	netIF = new DBNetworkIF (netData);
	grdIF = new DBGridIF    (grdData);

	grdIF->RenameLayer (grdIF->Layer ((DBInt) 0),(char *) "Subbasins");

	for (pos.Row = 0;pos.Row < grdIF->RowNum ();++pos.Row)
		for (pos.Col = 0;pos.Col < grdIF->ColNum ();++pos.Col)	grdIF->Value (pos,DBFault);

	for (pntFLD = pntFields->First ();pntFLD != (DBObjTableField *) NULL;pntFLD = pntFields->Next ())
		if (DBTableFieldIsVisible (pntFLD)) grdTable->AddField (new DBObjTableField (*pntFLD));

	for (i = 0;i < pntIF->ItemNum ();++i)
		{
		DBPause (i * 100 / pntIF->ItemNum ());
		pntRec = pntIF->Item (i);
		if ((pntRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)          continue;
		if ((cellRec = netIF->Cell (pntIF->Coordinate (pntRec))) == (DBObjRecord *) NULL) continue;

		itemRec = grdTable->Add (pntRec->Name ());
		grdFLD->Int    (itemRec,pntRec->RowID () + 1);
		symFLD->Record (itemRec,symRec);

		for (pntFLD = pntFields->First ();pntFLD != (DBObjTableField *) NULL;pntFLD = pntFields->Next ())
			if ((grdAttribFLD = grdTable->Field (pntFLD->Name ())) != (DBObjTableField *) NULL)
				switch (pntFLD->Type ())
					{
					case DBTableFieldString:
						grdAttribFLD->String (itemRec, pntFLD->String (pntRec));
						break;
					case DBTableFieldInt:
						grdAttribFLD->Int 	 (itemRec, pntFLD->Int    (pntRec));
						break;
					case DBTableFieldFloat:
						grdAttribFLD->Float  (itemRec, pntFLD->Float  (pntRec));
						break;
					case DBTableFieldDate:
						grdAttribFLD->Date   (itemRec, pntFLD->Date   (pntRec));
						break;
					}
		grdIF->Value (netIF->CellPosition (cellRec),itemRec->RowID ());
		}

	for (i = 0;i < netIF->CellNum(); ++i)
		{
		if ((cellRec = netIF->Cell (i)) == (DBObjRecord *) NULL) continue;
		if ((itemRec = grdIF->GridItem (netIF->CellPosition (cellRec))) != (DBObjRecord *) NULL) continue;
		if ((toCell = netIF->ToCell (cellRec)) == (DBObjRecord *) NULL) continue;
		if ((itemRec = grdIF->GridItem (netIF->CellPosition (toCell)))  != (DBObjRecord *) NULL) grdIF->Value (netIF->CellPosition (cellRec),itemRec->RowID ());
		}
	grdIF->DiscreteStats ();
	delete pntIF;
	delete netIF;
	delete grdIF;
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
