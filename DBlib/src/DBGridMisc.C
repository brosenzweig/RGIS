/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBGridMisc.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

void DBGridIF::Initialize (DBObjData *data, bool flat)
	{
	DBObjTableField *rowNumFLD;
	DBObjTableField *colNumFLD;
	DBObjTableField *cellWidthFLD;
	DBObjTableField *cellHeightFLD;
	DBObjTableField *valueTypeFLD;
	DBObjTableField *valueSizeFLD;
	DBObjRecord *layerRec;

	DataPTR = data;
	ItemTable = data->Table (DBrNItems);
	switch (data->Type ())
		{
		case DBTypeGridDiscrete:
			SymbolTable = data->Table (DBrNSymbols);
			AverageFLD = (DBObjTableField *) NULL;
			StdDevFLD  = (DBObjTableField *) NULL;
			MinimumFLD = (DBObjTableField *) NULL;
			MaximumFLD = (DBObjTableField *) NULL;
			MissingValueFLD = (DBObjTableField *) NULL;
			GridValueFLD 	= ItemTable->Field (DBrNGridValue);
			SymbolFLD 		= ItemTable->Field (DBrNSymbol);
			SymbolIDFLD		= SymbolTable->Field (DBrNSymbolID);
			ForegroundFLD	= SymbolTable->Field (DBrNForeground);
			BackgroundFLD	= SymbolTable->Field (DBrNBackground);
			StyleFLD 		= SymbolTable->Field (DBrNStyle);
			break;
		case DBTypeGridContinuous:
			SymbolTable = (DBObjTable *) NULL;
			AverageFLD = ItemTable->Field (DBrNAverage);
			StdDevFLD  = ItemTable->Field (DBrNStdDev);
			MinimumFLD = ItemTable->Field (DBrNMinimum);
			MaximumFLD = ItemTable->Field (DBrNMaximum);
			MissingValueFLD = ItemTable->Field (DBrNMissingValue);
			SymbolFLD 		= (DBObjTableField *) NULL;
			SymbolIDFLD 	= (DBObjTableField *) NULL;
			ForegroundFLD	= (DBObjTableField *) NULL;
			BackgroundFLD	= (DBObjTableField *) NULL;
			StyleFLD 		= (DBObjTableField *) NULL;
			break;
		default:	CMmsgPrint (CMmsgAppError, "Invalid Data Type in: %s %d",__FILE__,__LINE__); break;
		}
	LayerTable = data->Table (DBrNLayers);
	LayerFLD = LayerTable->Field (DBrNLayer);
	rowNumFLD = LayerTable->Field (DBrNRowNum);
	colNumFLD = LayerTable->Field (DBrNColNum);
	cellWidthFLD = LayerTable->Field (DBrNCellWidth);
	cellHeightFLD = LayerTable->Field (DBrNCellHeight);
	valueTypeFLD = LayerTable->Field (DBrNValueType);
	valueSizeFLD = LayerTable->Field (DBrNValueSize);

	layerRec = LayerTable->Item ();
	DimensionVAR.Row = rowNumFLD->Int (layerRec);
	DimensionVAR.Col = colNumFLD->Int (layerRec);
	CellWidthVAR  = cellWidthFLD->Float (layerRec);
	CellHeightVAR = cellHeightFLD->Float (layerRec);
	ValueTypeVAR  = valueTypeFLD->Int (layerRec);
	ValueSizeVAR  = valueSizeFLD->Int (layerRec);
	Flat = flat;
	}

void DBGridIF::RenameLayer (DBObjRecord *layerRec,char *name)

	{
	DBObjRecord *dataRec;

	layerRec = LayerTable->Item (layerRec->RowID ());
	layerRec->Name (name);
	dataRec = LayerFLD->Record (layerRec);
	dataRec->Name (name);
	if (DataPTR->Type () == DBTypeGridContinuous)
		{
		DBObjRecord *itemRec = ItemTable->Item (layerRec->RowID ());
		itemRec->Name (name);
		}
	}

DBObjRecord *DBGridIF::AddLayer (char *layerName)

	{
	DBInt index;
	DBObjRecord *firstLayer, *layerRec, *dataRec;
	DBObjTableField *rowNumFLD		= LayerTable->Field (DBrNRowNum);
	DBObjTableField *colNumFLD 	= LayerTable->Field (DBrNColNum);
	DBObjTableField *cellWidthFLD = LayerTable->Field (DBrNCellWidth);
	DBObjTableField *cellHeightFLD= LayerTable->Field (DBrNCellHeight);
	DBObjTableField *valueTypeFLD = LayerTable->Field (DBrNValueType);
	DBObjTableField *valueSizeFLD = LayerTable->Field (DBrNValueSize);

	firstLayer = LayerTable->First (&index);
	LayerTable->Add (layerName);
	if ((layerRec = LayerTable->Item ()) == (DBObjRecord *) NULL)
		return ((DBObjRecord *) NULL);
	rowNumFLD->Int (layerRec,rowNumFLD->Int (firstLayer));
	colNumFLD->Int (layerRec,colNumFLD->Int (firstLayer));
	cellWidthFLD->Float  (layerRec,cellWidthFLD->Float  (firstLayer));
	cellHeightFLD->Float (layerRec,cellHeightFLD->Float (firstLayer));
	valueTypeFLD->Int (layerRec,valueTypeFLD->Int (firstLayer));
	valueSizeFLD->Int (layerRec,valueSizeFLD->Int (firstLayer));
	if ((dataRec = new DBObjRecord (layerName,rowNumFLD->Int (firstLayer) * colNumFLD->Int (firstLayer) * valueSizeFLD->Int (firstLayer),valueSizeFLD->Int (firstLayer))) == (DBObjRecord *) NULL)
		{ return ((DBObjRecord *) NULL); }
	LayerFLD->Record (layerRec,dataRec);
	((Data ())->Arrays ())->Add (dataRec);
	if (DataPTR->Type () == DBTypeGridContinuous)
		{
		ItemTable->Add (layerName);
		MissingValueFLD->Float (ItemTable->Item (),MissingValueFLD->Float (ItemTable->First (&index)));
		}
	return (layerRec);
	}

DBInt DBGridIF::DeleteLayer (char *layerName)

	{
	DBObjRecord *layerRec, *dataRec;

	if (DataPTR->Type () == DBTypeGridContinuous)
		{
		if ((layerRec = ItemTable->Item (layerName)) == (DBObjRecord *) NULL)
			{ CMmsgPrint (CMmsgAppError, "Invalid layer name in: %s %d",__FILE__,__LINE__); return (DBFault); }
		ItemTable->Delete (layerRec);
		}
	if ((layerRec = LayerTable->Item (layerName)) == (DBObjRecord *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Total Gebasz in: %s %d",__FILE__,__LINE__); return (DBFault); }
	dataRec = LayerFLD->Record (layerRec);
	((Data ())->Arrays ())->Delete (dataRec);
	LayerTable->Delete (layerRec);
	return (DBSuccess);
	}

DBInt DBGridIF::DeleteLayers (char *firstLayer, char *lastLayer)

	{
	DBInt layerID;
	DBObjRecord *layerRec;

	if ((layerRec = LayerTable->Item (firstLayer)) == (DBObjRecord *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Invalid layer name in: %s %d",__FILE__,__LINE__); return (DBFault); }
	layerID = layerRec->RowID ();

	while (strcmp (layerRec->Name (),lastLayer) != 0)
		{
		DeleteLayer (layerRec->Name());
		if ((layerRec = LayerTable->Item (layerID)) == (DBObjRecord *) NULL) break;
		}

	return (DBSuccess);
	}

DBFloat DBGridIF::Maximum (DBInt layer) const

	{
	DBInt index;
	DBFloat retVal;
	DBObjRecord *dataRec;

	if ((layer == DBFault) || ((dataRec = ItemTable->Item (layer)) == (DBObjRecord *) NULL))
		{
		retVal = -DBHugeVal;
		for (dataRec = ItemTable->First (&index);dataRec != (DBObjRecord *) NULL;dataRec = ItemTable->Next (&index))
			retVal = retVal > MaximumFLD->Float (dataRec) ? retVal : MaximumFLD->Float (dataRec);
		}
	else	retVal = MaximumFLD->Float (dataRec);
	return (retVal);
	}

DBFloat DBGridIF::Minimum (DBInt layer) const

	{
	DBInt index;
	DBFloat retVal;
	DBObjRecord *dataRec;

	if ((layer == DBFault) || ((dataRec = ItemTable->Item (layer)) == (DBObjRecord *) NULL))
		{
		retVal = DBHugeVal;
		for (dataRec = ItemTable->First (&index);dataRec != (DBObjRecord *) NULL;dataRec = ItemTable->Next (&index))
			retVal = retVal < MinimumFLD->Float (dataRec) ? retVal : MinimumFLD->Float (dataRec);
		}
	else	retVal = MinimumFLD->Float (dataRec);
	return (retVal);
	}

DBInt DBGridIF::Coord2Pos (DBCoordinate coord,DBPosition &pos) const

	{
	DBInt row, col;
 	col = (DBInt) floor ((coord.X - DataPTR->Extent ().LowerLeft.X ) / CellWidth ());
	row = (DBInt) floor ((coord.Y - DataPTR->Extent ().LowerLeft.Y ) / CellHeight ());
	pos.Col = (DBUShort) col;
	pos.Row = (DBUShort) row;
	if ((col < 0) || (col >= ColNum ())) return (DBFault);
	if ((row < 0) || (row >= RowNum ())) return (DBFault);
	return (DBSuccess);
	}

DBInt DBGridIF::Pos2Coord (DBPosition pos,DBCoordinate &coord) const

	{
	DBInt ret = DBSuccess;
	if (pos.Col < 0) ret = DBFault;
	if (pos.Row < 0) ret = DBFault;
	if (pos.Col >= ColNum ()) ret = DBFault;
	if (pos.Row >= RowNum ()) ret = DBFault;

	coord.X = DataPTR->Extent ().LowerLeft.X + pos.Col * CellWidth  () + CellWidth  () / 2.0;
	coord.Y = DataPTR->Extent ().LowerLeft.Y + pos.Row * CellHeight () + CellHeight () / 2.0;
	return (ret);
	}

DBInt DBGridIF::Value (DBObjRecord *layerRec,DBPosition pos,DBInt value)

	{
	DBInt j;
	DBObjRecord *dataRec = LayerFLD->Record (layerRec);

	if (pos.Col < 0) return (false);
	if (pos.Row < 0) return (false);
	if (pos.Col >= DimensionVAR.Col) return (false);
	if (pos.Row >= DimensionVAR.Row) return (false);

	j = DimensionVAR.Col * (DimensionVAR.Row - pos.Row - 1) + pos.Col;
	switch (ValueTypeVAR)
		{
		case DBTableFieldFloat:
			switch (ValueSizeVAR)
				{
				case sizeof (DBFloat4): ((DBFloat4 *) (dataRec->Data ())) [j] = (DBFloat4) value; break;
				case sizeof (DBFloat):  ((DBFloat *)  (dataRec->Data ())) [j] = (DBFloat) 	value; break;
				}
			break;
		case DBTableFieldInt:
			switch (ValueSizeVAR)
				{
				case sizeof (DBByte):  ((DBByte *)	(dataRec->Data ())) [j] = (DBByte)	value;	break;
				case sizeof (DBShort): ((DBShort *) (dataRec->Data ())) [j] = (DBShort) value;	break;
				case sizeof (DBInt):   ((DBInt *)	(dataRec->Data ())) [j] = (DBInt) 	value;	break;
				}
			break;
		}
	return (true);
	}

DBInt DBGridIF::Value (DBObjRecord *layerRec,DBPosition pos,DBInt *value) const

	{
	DBInt j;
	DBObjRecord *dataRec = LayerFLD->Record (layerRec);

	if (pos.Col < 0) return (false);
	if (pos.Row < 0) return (false);
	if (pos.Col >= DimensionVAR.Col) return (false);
	if (pos.Row >= DimensionVAR.Row) return (false);

	j = DimensionVAR.Col * (DimensionVAR.Row - pos.Row - 1) + pos.Col;
	switch (ValueTypeVAR)
		{
		case DBTableFieldFloat:
			switch (ValueSizeVAR)
				{
				case sizeof (DBFloat4): *value = (DBInt) ((DBFloat4 *) (dataRec->Data ())) [j]; break;
				case sizeof (DBFloat):  *value = (DBInt) ((DBFloat *)  (dataRec->Data ())) [j]; break;
				}
			break;
		case DBTableFieldInt:
			switch (ValueSizeVAR)
				{
				case sizeof (DBByte):  *value = (DBInt) ((DBByte *)  (dataRec->Data ())) [j];	break;
				case sizeof (DBShort): *value = (DBInt) ((DBShort *) (dataRec->Data ())) [j];	break;
				case sizeof (DBInt):   *value = (DBInt) ((DBInt *)   (dataRec->Data ())) [j];	break;
				}
			break;
		}
	if (MissingValueFLD != (DBObjTableField *) NULL)
		return (*value == MissingValueFLD->Int (ItemTable->Item (layerRec->RowID ())) ? false : true);
	return (*value == DBFault ? false: true);
	}

DBInt DBGridIF::Value (DBObjRecord *layerRec,DBPosition pos,DBFloat *value) const

	{
	DBInt j;
	DBObjRecord *dataRec = LayerFLD->Record (layerRec);
	DBFloat missingValue = MissingValueFLD->Float (ItemTable->Item (layerRec->RowID ()));

//	if (pos.Col < 0) return (false); No longer possible
//	if (pos.Row < 0) return (false);
	if (pos.Col >= DimensionVAR.Col) return (false);
	if (pos.Row >= DimensionVAR.Row) return (false);

	j = DimensionVAR.Col * (DimensionVAR.Row - pos.Row - 1) + pos.Col;
	switch (ValueTypeVAR)
		{
		case DBTableFieldFloat:
			switch (ValueSizeVAR)
				{
				case sizeof (DBFloat4): *value = (DBFloat) ((DBFloat4 *) (dataRec->Data ())) [j]; break;
				case sizeof (DBFloat):  *value = (DBFloat) ((DBFloat *)  (dataRec->Data ())) [j]; break;
				}
			break;
		case DBTableFieldInt:
			switch (ValueSizeVAR)
				{
				case sizeof (DBByte):  *value = (DBFloat) ((DBByte *)	 (dataRec->Data ())) [j];	break;
				case sizeof (DBShort): *value = (DBFloat) ((DBShort *) (dataRec->Data ())) [j];	break;
				case sizeof (DBInt):   *value = (DBFloat) ((DBInt *)	 (dataRec->Data ())) [j];	break;
				}
			break;
		}
	return (CMmathEqualValues (*value,missingValue) ? false : true);
	}

DBInt DBGridIF::Value (DBObjRecord *layerRec,DBPosition pos,DBFloat value)

	{
	DBInt j = DimensionVAR.Col * (DimensionVAR.Row - pos.Row - 1) + pos.Col;
	DBObjRecord *dataRec = LayerFLD->Record (layerRec);

//	if (pos.Col < 0) return (DBFault); No longer possible
//	if (pos.Row < 0) return (DBFault);
	if (pos.Col >= DimensionVAR.Col) return (DBFault);
	if (pos.Row >= DimensionVAR.Row) return (DBFault);

	switch (ValueTypeVAR)
		{
		case DBTableFieldFloat:
			switch (ValueSizeVAR)
				{
				case sizeof (DBFloat4): ((DBFloat4 *) (dataRec->Data ())) [j] = (DBFloat4) value; break;
				case sizeof (DBFloat):  ((DBFloat *)  (dataRec->Data ())) [j] = value; break;
				}
			break;
		case DBTableFieldInt:
			switch (ValueSizeVAR)
				{
				case sizeof (DBByte):  ((DBByte *)  (dataRec->Data ())) [j] = (DBByte)  value;	break;
				case sizeof (DBShort): ((DBShort *) (dataRec->Data ())) [j] = (DBShort) value;	break;
				case sizeof (DBInt):   ((DBInt *)   (dataRec->Data ())) [j] = (DBInt)   value;	break;
				}
			break;
		}
	return (DBSuccess);
	}

DBInt DBGridIF::Value (DBObjRecord *layerRec,DBCoordinate coord,DBFloat *value) const

	{
	DBInt i, j, pointNum;
	DBInt row [9], col [9];
	DBPosition pos;
	DBCoordinate cellCoord;
	DBPosition cellPos;
	DBFloat precision, dist, wAvg, sumWeight, retVal;
	DBObjRecord *dataRec = LayerFLD->Record (layerRec);
	DBFloat missingValue = MissingValueFLD->Float (ItemTable->Item ());
	DBMathDistanceFunction distFunc =  DBMathGetDistanceFunction (DataPTR);

	if (DataPTR->Extent ().InRegion (coord) == false) return (false);
	precision = pow ((double) 10.0,(double) DataPTR->Precision ());
	Coord2Pos (coord,cellPos);
	Pos2Coord (cellPos,cellCoord);
	if (Flat ||
	    ((fabs (coord.X - cellCoord.X) < precision) &&
		 (fabs (coord.Y - cellCoord.Y) < precision)))
		{
		j = DimensionVAR.Col * (DimensionVAR.Row - cellPos.Row - 1) + cellPos.Col;
		switch (ValueTypeVAR)
			{
			case DBTableFieldFloat:
				switch (ValueSizeVAR)
					{
					case sizeof (DBFloat4): retVal = (DBFloat) ((DBFloat4 *) (dataRec->Data ())) [j]; break;
					case sizeof (DBFloat):  retVal = (DBFloat) ((DBFloat *)  (dataRec->Data ())) [j]; break;
					}
				break;
			case DBTableFieldInt:
				switch (ValueSizeVAR)
					{
					case sizeof (DBByte):  retVal = (DBFloat) ((DBByte *)	 (dataRec->Data ())) [j];	break;
					case sizeof (DBShort): retVal = (DBFloat) ((DBShort *) (dataRec->Data ())) [j];	break;
					case sizeof (DBInt):   retVal = (DBFloat) ((DBInt *)	 (dataRec->Data ())) [j];	break;
					}
				break;
			}
		if (!CMmathEqualValues (retVal,missingValue)) { *value = retVal;	return (true); }
		}
	col [0] = cellPos.Col;
	row [0] = cellPos.Row;
	if (coord.X < cellCoord.X) col [0] -= 1;
	if (coord.Y < cellCoord.Y) row [0] -= 1;
	col [1] = col [0] + 1;
	row [1] = row [0];
	col [2] = col [0] + 1;
	row [2] = row [0] + 1;
	col [3] = col [0];
	row [3] = row [0] + 1;

	pos.Col = col [0];
	pos.Row = row [0];
	Pos2Coord (pos,cellCoord);
	if ((coord.X - cellCoord.X) > (3.0 * CellWidth () / 4.0))	i = 1;
	else if ((coord.X - cellCoord.X) > (CellWidth () / 4.0))	i = 0;
	else i = -1;
	if ((coord.Y - cellCoord.Y) > (3.0 * CellHeight () / 4.0))	j = 1;
	else if ((coord.Y - cellCoord.Y) > (CellHeight () / 4.0))	j = 0;
	else j = -1;

	if ((i != 0) || (j != 0))
		{
		if (i == 0)
			{
			row [4] = row [5] = j > 0 ? row [2] + 1 : row [0] - 1;
			col [4] = col [0];
			col [5] = col [2];
			pointNum = 6;
			}
		else if (j == 0)
			{
			row [4] = row [0];
			row [5] = row [2];
			col [4] = col [5] = i > 0 ? col [2] + 1 : col [0] - 1;
			pointNum = 6;
			}
		else
			{
			row [7] = row [0];
			row [8] = row [2];
			if (j > 0)
				row [4] = row [5] = row [6] = row [2] + 1;
			else
				row [4] = row [5] = row [6] = row [0] - 1;

			if (i > 0)
				{
				col [4] = col [0];
				col [5] = col [2];
				col [6] = col [7] = col [8] = col [2] + 1;
				}
			else
				{
				col [5] = col [0];
				col [6] = col [2];
				col [4] = col [7] = col [8] = col [0] - 1;
				}
			pointNum = 9;
			}
		}
	else	pointNum = 4;

	wAvg = sumWeight = 0.0;
	for (i = 0;i < pointNum; ++i)
		{
		if (col [i] < 0) continue;
		if (row [i] < 0) continue;
		if (col [i] >= DimensionVAR.Col) continue;
		if (row [i] >= DimensionVAR.Row) continue;

		j = DimensionVAR.Col * (DimensionVAR.Row - row [i] - 1) + col [i];
		switch (ValueTypeVAR)
			{
			case DBTableFieldFloat:
				switch (ValueSizeVAR)
					{
					case sizeof (DBFloat4): retVal = (DBFloat) ((DBFloat4 *) (dataRec->Data ())) [j]; break;
					case sizeof (DBFloat):  retVal = (DBFloat) ((DBFloat *)  (dataRec->Data ())) [j]; break;
					}
				break;
			case DBTableFieldInt:
				switch (ValueSizeVAR)
					{
					case sizeof (DBByte):  retVal = (DBFloat) ((DBByte *)	 (dataRec->Data ())) [j];	break;
					case sizeof (DBShort): retVal = (DBFloat) ((DBShort *) (dataRec->Data ())) [j];	break;
					case sizeof (DBInt):   retVal = (DBFloat) ((DBInt *)	 (dataRec->Data ())) [j];	break;
					}
				break;
			}
		if (CMmathEqualValues (retVal,missingValue))
			{
			if ((col [i] == cellPos.Col) && (row [i] == cellPos.Row)) return (false);
			else continue;
			}
		pos.Row = (DBUShort) row [i];
		pos.Col = (DBUShort) col [i];
		Pos2Coord (pos,cellCoord);
		if (pointNum > 1)
			{
			dist = DBMathCoordinateDistance (distFunc,coord,cellCoord);
			dist *= dist;
			}
		else	dist = 1.0;
		wAvg = wAvg + retVal / dist;
		sumWeight = sumWeight + 1.0 / dist;
		}
	if (sumWeight > 0)	{ *value = wAvg / sumWeight;	return (true); }
	else  					{ *value = missingValue;		return (false); }
	}

DBObjRecord *DBGridIF::GridItem (DBObjRecord *layerRec,DBPosition pos) const

	{
	DBInt value;

	if (SymbolTable == (DBObjTable *) NULL)   return ((DBObjRecord *) NULL);
	if (Value (layerRec,pos,&value) == false)	return ((DBObjRecord *) NULL);
	if (value == DBFault) return ((DBObjRecord *) NULL);
	return (ItemTable->Item (value));
	}

char *DBGridIF::ValueFormat () const
	{
	switch (DataPTR->Type ())
		{
		case DBTypeGridDiscrete:	return ((char *) "%s");
		case DBTypeGridContinuous:	return (DBMathFloatAutoFormat (fabs (Maximum ()) > fabs (Minimum ()) ? Maximum () : Minimum ()));
		default: return ((char *) NULL);
		}
	}

char *DBGridIF::ValueString (DBObjRecord *layerRec,DBPosition pos)

	{
	static char retString [DBStringLength + 1];

	switch (DataPTR->Type ())
		{
		case DBTypeGridDiscrete:	return (GridItem (layerRec,pos))->Name ();
		case DBTypeGridContinuous:
			{
			DBFloat cellVal;
				if (Value (layerRec,pos,&cellVal) == false) return ((char *) "");
				sprintf (retString,ValueFormat (),cellVal);
			} break;
		default:	CMmsgPrint (CMmsgAppError, "Invalid Data Type in: %s %d",__FILE__,__LINE__); return ((char *) NULL);
		}
	return (retString);
	}

void DBGridIF::RecalcStats (DBObjRecord *layerRec)

	{
	DBInt obsNum = 0;
	DBPosition pos;
	DBFloat value, cellArea;
	DBFloat sumWeight = 0.0, minimum = DBHugeVal, maximum = -DBHugeVal, average = 0.0, stdDev = 0.0;

	for (pos.Row = 0;pos.Row < RowNum ();++pos.Row)
		for (pos.Col = 0;pos.Col < ColNum ();++pos.Col)
			if (Value (layerRec,pos,&value))
				{
				cellArea = CellArea (pos);
				sumWeight += cellArea;
				average = average + value * cellArea;
				minimum = minimum < value ? minimum : value;
				maximum = maximum > value ? maximum : value;
				stdDev = stdDev + value * value * cellArea;
				obsNum++;
				}
	if (obsNum > 0)
		{
		average = average / sumWeight;
		stdDev = stdDev / sumWeight;
		stdDev = stdDev - average * average;
		stdDev = sqrt (stdDev);
		}
	else
		average = stdDev = minimum = maximum = MissingValue ();
	layerRec = ItemTable->Item (layerRec->RowID ());
	AverageFLD->Float (layerRec,average);
	MinimumFLD->Float (layerRec,minimum);
	MaximumFLD->Float (layerRec,maximum);
	StdDevFLD->Float  (layerRec,stdDev);
	}

void DBGridIF::DiscreteStats ()

	{
	DBInt recordID, layerID;
	DBFloat area, sumArea;
	DBPosition pos;
	DBObjTableField *areaFLD = ItemTable->Field (DBrNGridArea);
	DBObjTableField *percentFLD = ItemTable->Field (DBrNGridPercent);
	DBObjRecord *layerRec, *record;

	if (areaFLD == (DBObjTableField *) NULL)
		ItemTable->AddField (areaFLD = new DBObjTableField (DBrNGridArea,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4)));
	if (percentFLD == (DBObjTableField *) NULL)
		ItemTable->AddField (percentFLD = new DBObjTableField (DBrNGridPercent,DBTableFieldFloat,"%5.1f",sizeof (DBFloat4)));
	for (recordID = 0;recordID < ItemTable->ItemNum ();++recordID)
		{
		record = ItemTable->Item (recordID);
		areaFLD->Float (record,0.0);
		percentFLD->Float (record,0.0);
		}
	for (layerID = 0;layerID < LayerNum ();++layerID)
		{
		layerRec = Layer (layerID);
		for (pos.Row = 0;pos.Row < RowNum ();++pos.Row)
			for (pos.Col = 0;pos.Col < ColNum ();++pos.Col)
				if ((record = GridItem (layerRec,pos)) != (DBObjRecord *) NULL)
					{
					area = CellArea (pos);
					sumArea += area;
					areaFLD->Float (record,areaFLD->Float (record) + area);
					}
		}
	for (recordID = 0;recordID < ItemTable->ItemNum ();++recordID)
		{
		record = ItemTable->Item (recordID);
		percentFLD->Float (record,areaFLD->Float (record) * 100.0 / sumArea);
		}
	}

DBFloat DBGridIF::CellArea (DBPosition pos) const

	{
	DBCoordinate coord0, coord1;

	coord0.X = DataPTR->Extent ().LowerLeft.X + pos.Col * CellWidth  ();
	coord0.Y = DataPTR->Extent ().LowerLeft.Y + pos.Row * CellHeight ();
	coord1.X = DataPTR->Extent ().LowerLeft.X + pos.Col * CellWidth  () + CellWidth  ();
	coord1.Y = DataPTR->Extent ().LowerLeft.Y + pos.Row * CellHeight () + CellHeight ();

	return (DBMathRectangleArea (DataPTR->Projection (),coord0,coord1));
	}

void DBGridOperation (DBObjData *leftGrd,DBObjData *rightGrd,DBInt oper,DBInt mergeMissingVal)

	{
	DBInt leftID, rightID;
	DBFloat leftVal, rightVal;
	DBPosition pos;
	DBCoordinate coord;
	DBObjRecord *leftRec, *rightRec;
	DBGridIF *leftIF = new DBGridIF (leftGrd);
	DBGridIF *rightIF = new DBGridIF (rightGrd);

	for (leftID = 0;leftID < leftIF->LayerNum ();++leftID)
		{
		leftRec = leftIF->Layer (leftID);
		if ((leftRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) break;
		}
	if (leftID == leftIF->LayerNum ()) { CMmsgPrint (CMmsgAppError, "No Layer to Process in %s %d",__FILE__,__LINE__); return; }
	for (rightID = 0;rightID < rightIF->LayerNum ();++rightID)
		{
		rightRec = rightIF->Layer (rightID);
		if ((rightRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) break;
		}
	if (rightID == rightIF->LayerNum ()) { CMmsgPrint (CMmsgAppError, "No Layer to Process in %s %d",__FILE__,__LINE__); return; }

	rightID = (DBInt) 0;
	for (leftID = 0;leftID < leftIF->LayerNum ();++leftID)
		{
		leftRec = leftIF->Layer (leftID);
		while ((leftRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			{
			++leftID;
			if (leftID == leftIF->LayerNum ()) goto Stop;
			leftRec = leftIF->Layer (leftID);
			}
		if ((rightRec = rightIF->Layer (rightID)) == (DBObjRecord *) NULL)
			{ rightID = 0; rightRec = rightIF->Layer (rightID); }
		while ((rightRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			{
			++rightID;
			if (rightID == rightIF->LayerNum ()) rightID = 0;
			rightRec = rightIF->Layer (rightID);
			}

		for (pos.Row = 0;pos.Row < leftIF->RowNum ();pos.Row++)
			{
			if (DBPause ((leftID * leftIF->RowNum () + pos.Row) * 100 / (leftIF->LayerNum () * leftIF->RowNum ())))
				goto Stop;
			for (pos.Col = 0;pos.Col < leftIF->ColNum ();pos.Col++)
				{
				leftIF->Pos2Coord (pos,coord);
				if (leftIF->Value (leftRec,pos,&leftVal))
					{
					if (rightIF->Value (rightRec,coord,&rightVal))
						switch (oper)
							{
							case DBMathOperatorAdd:	leftIF->Value (leftRec,pos,leftVal + rightVal); break;
							case DBMathOperatorSub:	leftIF->Value (leftRec,pos,leftVal - rightVal); break;
							case DBMathOperatorMul:	leftIF->Value (leftRec,pos,leftVal * rightVal); break;
							case DBMathOperatorDiv:
								if (fabs (rightVal) > 0.000001)	leftIF->Value (leftRec,pos,leftVal / rightVal);
								else	leftIF->Value (leftRec,pos,leftIF->MissingValue ());
								break;
							}
					else	if (mergeMissingVal)	leftIF->Value (leftRec,pos,leftIF->MissingValue ());
					}
				}
			}
		++rightID;
		leftIF->RecalcStats (leftRec);
		}
Stop:
	return;
	}

void DBGridOperation (DBObjData *grdData,DBFloat constant,DBInt oper)

	{
	DBInt layerID;
	DBFloat value;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIF *gridIF = new DBGridIF (grdData);

	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) break;
		}
	if (layerID == gridIF->LayerNum ()) { CMmsgPrint (CMmsgAppError, "No Layer to Process in %s %d",__FILE__,__LINE__); return; }

	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;

		for (pos.Row = 0;pos.Row < gridIF->RowNum ();pos.Row++)
			{
			if (DBPause ((layerID * gridIF->RowNum () + pos.Row) * 100 / (gridIF->LayerNum () * gridIF->RowNum ())))
				goto Stop;
			for (pos.Col = 0;pos.Col < gridIF->ColNum ();pos.Col++)
				{
				if (gridIF->Value (layerRec,pos,&value))
					switch (oper)
						{
						case DBMathOperatorAdd:	gridIF->Value (layerRec,pos,value + constant);	break;
						case DBMathOperatorSub:	gridIF->Value (layerRec,pos,value - constant);	break;
						case DBMathOperatorMul:	gridIF->Value (layerRec,pos,value * constant);	break;
						case DBMathOperatorDiv:	gridIF->Value (layerRec,pos,value / constant);	break;
						}
				}
			}
		gridIF->RecalcStats (layerRec);
		}
Stop:
	return;
	}

void DBGridOperationAbs (DBObjData *grdData)

	{
	DBInt layerID;
	DBFloat value;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIF *gridIF = new DBGridIF (grdData);

	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) break;
		}
	if (layerID == gridIF->LayerNum ()) { CMmsgPrint (CMmsgAppError, "No Layer to Process in %s %d",__FILE__,__LINE__); return; }

	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;

		for (pos.Row = 0;pos.Row < gridIF->RowNum ();pos.Row++)
			{
			if (DBPause ((layerID * gridIF->RowNum () + pos.Row) * 100 / (gridIF->LayerNum () * gridIF->RowNum ())))
				goto Stop;
			for (pos.Col = 0;pos.Col < gridIF->ColNum ();pos.Col++)
				if (gridIF->Value (layerRec,pos,&value)) gridIF->Value (layerRec,pos,fabs (value));
			}
		gridIF->RecalcStats (layerRec);
		}
Stop:
	return;
	}
