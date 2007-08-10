/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

DBVPoint2Grid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

DBVPointIO	*_DBPntIO;
DBNetworkIO *_DBNetIO;
DBGridIO 	*_DBGrdIO;

static DBInt _DBPointSort (const void *obj0,const void *obj1)

	{
	DBObjRecord *cellRec0 = _DBNetIO->Cell (_DBPntIO->Coordinate (((DBObjRecord **) obj0) [0]));
	DBObjRecord *cellRec1 = _DBNetIO->Cell (_DBPntIO->Coordinate (((DBObjRecord **) obj1) [0]));

	if (cellRec0 == cellRec1) return (0);
	if (_DBNetIO->CellBasinArea (cellRec0) > _DBNetIO->CellBasinArea (cellRec1)) return  (1);
	if (_DBNetIO->CellBasinArea (cellRec0) < _DBNetIO->CellBasinArea (cellRec1)) return (-1);
	return (0);
	}


static DBInt DBPointUpStreamAction (DBNetworkIO *netIO,DBObjRecord *cellRec,DBInt pntID)

	{
	_DBGrdIO->Value (netIO->CellPosition (cellRec),pntID);
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

	_DBPntIO = new DBVPointIO(pntData);

	if ((recARR = (DBObjRecord **) calloc (_DBPntIO->ItemNum (),2 * sizeof (DBObjRecord *))) == (DBObjRecord **) NULL)
		{
		perror ("Memory Allocation Error in: DBPointToGrid ()");
		delete _DBPntIO;
		return (DBFault);
		}
	_DBGrdIO = new DBGridIO (grdData);
	_DBGrdIO->RenameLayer (_DBGrdIO->Layer ((DBInt) 0),"Subbasins");
	_DBNetIO	= new DBNetworkIO (netData);

	for (pos.Row = 0;pos.Row < _DBGrdIO->RowNum ();++pos.Row)
		for (pos.Col = 0;pos.Col < _DBGrdIO->ColNum ();++pos.Col)	_DBGrdIO->Value (pos,DBFault);
	grdTable->AddField (new DBObjTableField (DBrNGridArea,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4)));
	grdTable->AddField (new DBObjTableField (DBrNGridPercent,DBTableFieldFloat,"%5.1f",sizeof (DBFloat4)));
	for (pntFLD = pntFields->First ();pntFLD != (DBObjTableField *) NULL;pntFLD = pntFields->Next ())
		if (DBTableFieldIsVisible (pntFLD)) grdTable->AddField (new DBObjTableField (*pntFLD));
	
	recID = 0;
	for (i = 0;i < _DBPntIO->ItemNum ();++i)
		{
		DBPause (i * 100 / _DBPntIO->ItemNum ());
		recARR [(recID << 0x01)] = _DBPntIO->Item (i);
		if ((recARR [(recID << 0x01)]->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		if (_DBNetIO->Cell (_DBPntIO->Coordinate (recARR [(recID << 0x01)])) == (DBObjRecord *) NULL) continue;
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
		if ((cellRec = _DBNetIO->Cell (_DBPntIO->Coordinate (recARR [(recID << 0x01)]))) == (DBObjRecord *) NULL) continue;
		_DBNetIO->UpStreamSearch (cellRec,(DBNetworkACTION) DBPointUpStreamAction,(void *) recARR [(recID << 0x01) + 1]->RowID ());
		}
	_DBGrdIO->DiscreteStats ();
	delete _DBPntIO;
	delete _DBGrdIO;
	delete _DBNetIO;
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
	DBVPointIO	*pntIO;
	DBGridIO 	*grdIO;
	DBMathDistanceFunction distFunc = DBMathGetDistanceFunction (pntData);

	if (distFunc != DBMathGetDistanceFunction (grdData))
	{ fprintf (stderr,"Incompatible projections in: DBPointToGrid ()\n"); return (DBFault); }

	pntIO = new DBVPointIO(pntData);
	itemNum = pntIO->ItemNum ();
	if ((pCoord = (DBCoordinate *) calloc (itemNum,sizeof (DBCoordinate))) == (DBCoordinate *) NULL)
		{ perror ("Memory allocation Error in: DBPointToGrid ()"); return (DBFault); }

	grdIO = new DBGridIO (grdData);
	for (pntID = 0;pntID < itemNum;++pntID) pCoord [pntID] = pntIO->Coordinate (pntIO->Item (pntID));
	
	if (grdData->Type () == DBTypeGridContinuous)
		grdIO->RenameLayer (grdIO->Layer ((DBInt) 0),"Distance to Station");
	else
		{
		grdIO->RenameLayer (grdIO->Layer ((DBInt) 0),"Station grid");
		itemTable = grdData->Table (DBrNItems);
		symTable  = grdData->Table (DBrNSymbols);
	   valField  = itemTable->Field (DBrNGridValue);
		symField  = itemTable->Field (DBrNSymbol);
		if ((symRec = symTable->Item (0)) == (DBObjRecord *) NULL) fprintf (stderr, "Total Metal Gebasz in: DBPointToGrid ()\n");
		for (pntID = 0;pntID < itemNum; ++pntID)
			{
			pntRec = pntIO->Item (pntID);
			grdRec = itemTable->Add (pntRec->Name ());
			valField->Int (grdRec,pntID + 1);
			symField->Record (grdRec,symRec);
			}
		}

	startID = 0;
	for (pos.Row = 0;pos.Row < grdIO->RowNum ();++pos.Row)
		{
		DBPause (pos.Row * 100 / grdIO->RowNum ());
		for (pos.Col = 0;pos.Col < grdIO->ColNum ();++pos.Col)
			{
			grdIO->Pos2Coord (pos,gCoord);
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
			if (grdData->Type () == DBTypeGridContinuous) grdIO->Value (pos,minDist); else grdIO->Value (pos,id);
			}
		}
	if (grdData->Type () == DBTypeGridContinuous) grdIO->RecalcStats (); else grdIO->DiscreteStats ();
	delete grdIO;
	free (pCoord);
	return (DBSuccess);
	}
