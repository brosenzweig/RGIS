/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

DBio.H

balazs.fekete@unh.edu

*******************************************************************************/

class DBVectorIO

	{
	protected:
		DBObjData *DataPTR;
		DBObjTable *ItemTable;
		DBObjTable *SymbolTable;

		DBObjTableField *SymbolFLD;

		DBObjTableField *SymbolIDFLD;
		DBObjTableField *ForegroundFLD;
		DBObjTableField *BackgroundFLD;
		DBObjTableField *StyleFLD;
	public:
		DBVectorIO () { DataPTR = (DBObjData *) NULL; ItemTable = SymbolTable = (DBObjTable *) NULL; }
		DBVectorIO (DBObjData *);
		DBObjData *Data () const { return (DataPTR); }
		DBObjRecord *Item (DBCoordinate) const;
		DBObjRecord *Item (DBInt id)	const { return (ItemTable->Item (id)); }
		DBObjRecord *FirstItem ()	const { return (ItemTable->First ()); }
		DBObjRecord *FirstItem (DBInt *index)	const { return (ItemTable->First (index)); }
		DBObjRecord *LastItem  ()  const { return (ItemTable->Last ()); }
		DBObjRecord *LastItem  (DBInt *index)  const { return (ItemTable->Last (index)); }
		DBObjRecord *NextItem  ()	const { return (ItemTable->Next ()); }
		DBObjRecord *NextItem  (DBInt *index)	const { return (ItemTable->Next (index)); }
		DBObjRecord *NextItem  (DBInt dir)	const { return (ItemTable->Next (dir)); }
		DBObjRecord *NextItem  (DBInt *index,DBInt dir)	const { return (ItemTable->Next (index,dir)); }
		DBObjRecord *NewItem (const char *symName) { return (ItemTable->Add (symName)); }

		DBObjRecord *Symbol (DBInt id) const { return (SymbolTable->Item (id)); }
		DBObjRecord *FirstSymbol () const { return (SymbolTable->First ()); }
		DBObjRecord *LastSymbol  () const { return (SymbolTable->Last ()); }
		DBObjRecord *NextSymbol  () const { return (SymbolTable->Next ()); }
		DBObjRecord *NextSymbol  (DBInt dir)	const { return (SymbolTable->Next (dir)); }
		DBObjRecord *NewSymbol (const char *symName)
			{
			DBObjRecord *symRec = SymbolTable->Add (symName);
			StyleFLD->Int (symRec,0);
			ForegroundFLD->Int (symRec,1);
			BackgroundFLD->Int (symRec,0);
			return (symRec);
			}
		DBInt ItemNum () const { return (ItemTable->ItemNum ()); }
		void ListSort (int (*compFunc) (const DBObjRecord **,const DBObjRecord **)) { ItemTable->ListSort (compFunc); }
		void ListSort (DBObjectLIST<DBObjTableField> *fields) { ItemTable->ListSort (fields); }
		void ListSort (DBObjTableField *field) { ItemTable->ListSort (field); }
		void ListSort (DBInt dir) { ItemTable->ListSort (dir); }
		void ListSort () { ItemTable->ListSort (); }
		void ListReset () { ItemTable->ListReset (); }
		void ItemSymbol (DBObjRecord *itemRec,DBObjRecord *symRec) { SymbolFLD->Record (itemRec,symRec); }
		DBInt ItemStyle (const DBObjRecord *itemRec)
			{ DBObjRecord *symRec = SymbolFLD->Record (itemRec); return (StyleFLD->Int (symRec)); }
		DBInt ItemForeground (const DBObjRecord *itemRec)
			{ DBObjRecord *symRec = SymbolFLD->Record (itemRec); return (ForegroundFLD->Int (symRec)); }
		DBInt ItemBackground (const DBObjRecord *itemRec)
			{ DBObjRecord *symRec = SymbolFLD->Record (itemRec); return (BackgroundFLD->Int (symRec)); }
	};

class DBVPointIO : public DBVectorIO

	{
	private:
		DBObjTableField *CoordFLD;
	public:
		DBVPointIO (DBObjData *data) : DBVectorIO (data)
			{ DBObjTable *points = data->Table (DBrNItems); CoordFLD  = points->Field (DBrNCoord); }
		DBCoordinate Coordinate (const DBObjRecord *pntRec) const { return (CoordFLD->Coordinate (pntRec)); }
		void  Coordinate (DBObjRecord *pntRec,DBCoordinate coord) { CoordFLD->Coordinate (pntRec,coord); }
	};

class DBVLineIO : public DBVectorIO
	{
	private:
		DBObjTableField *FromNodeFLD;
		DBObjTableField *ToNodeFLD;
		DBObjTableField *VertexesFLD;
		DBObjTableField *VertexNumFLD;
		DBObjTableField *ExtentFLD;
		DBObjTableField *PerimeterFLD;

		DBObjTable *NodeTable;

		DBObjTableField *NodeCoordFLD;
		void Initialize (DBObjData *,DBObjTable *);
	public:
		DBVLineIO (DBObjData *data,DBObjTable *items) : DBVectorIO (data) { Initialize (data,items); }
		DBVLineIO (DBObjData *data) : DBVectorIO (data) { Initialize (data,data->Table (DBrNItems)); }
		DBCoordinate NodeCoord (const DBObjRecord *nodeRec) const
			{ return (NodeCoordFLD->Coordinate (nodeRec)); }
		DBCoordinate FromCoord (const DBObjRecord *lineRec) const
			{ return (NodeCoord (FromNodeFLD->Record (lineRec))); }
		DBCoordinate ToCoord (const DBObjRecord *lineRec) const
			{ return (NodeCoord (ToNodeFLD->Record (lineRec))); }
		DBInt VertexNum (const DBObjRecord *lineRec) const { return (VertexNumFLD->Int (lineRec)); }
		void Vertexes (DBObjRecord *,DBCoordinate *,DBInt);
		DBCoordinate *Vertexes (const DBObjRecord *lineRec) const
			{ return ((DBCoordinate *) (VertexesFLD->Record (lineRec))->Data ()); }
		DBRegion Extent (const DBObjRecord *lineRec) const { return (ExtentFLD->Region (lineRec)); }
		DBFloat Length (const DBObjRecord *lineRec) const { return (PerimeterFLD->Float (lineRec)); }
		DBObjRecord *Node (DBCoordinate,DBInt);
		DBObjRecord *Node (DBCoordinate coord) { return (Node (coord,false)); }
		DBObjRecord *FromNode (DBObjRecord *lineRec) const { return (FromNodeFLD->Record (lineRec)); }
		void FromNode (DBObjRecord *lineRec,DBObjRecord *nodeRec) { FromNodeFLD->Record (lineRec,nodeRec); }
		DBObjRecord *ToNode (DBObjRecord *lineRec) const { return (ToNodeFLD->Record (lineRec)); }
		void ToNode (DBObjRecord *lineRec,DBObjRecord *nodeRec) { ToNodeFLD->Record (lineRec,nodeRec); }
		void Flip (DBObjRecord *);
	};

class DBVPolyIO : public DBVLineIO

	{
	private:
		DBObjTableField *FirstLineFLD;
		DBObjTableField *LineNumFLD;
		DBObjTableField *ExtentFLD;
		DBObjTableField *VertexNumFLD;
		DBObjTableField *LeftPolyFLD;
		DBObjTableField *RightPolyFLD;
		DBObjTableField *NextLineFLD;
		DBObjTableField *PrevLineFLD;
		DBObjTableField *AreaFLD;
		DBObjTable *LineTable;
		DBObjRecord *LineRightPoly(DBObjRecord *lineRec) const { return (RightPolyFLD->Record (lineRec)); }
		DBObjRecord *LineLeftPoly (DBObjRecord *lineRec) const { return (LeftPolyFLD->Record (lineRec)); }
		DBObjRecord *LineNextLine (DBObjRecord *lineRec) const { return (NextLineFLD->Record (lineRec)); }
		DBObjRecord *LinePrevLine (DBObjRecord *lineRec) const { return (PrevLineFLD->Record (lineRec)); }
		DBInt MaxVertexNumVAR;
		DBCoordinate *CoordsPTR;
	public:
		DBVPolyIO (DBObjData *);
		~DBVPolyIO () { if (CoordsPTR != (DBCoordinate *) NULL) free (CoordsPTR); }
		DBRegion Extent (const DBObjRecord *polyRec) const { return (ExtentFLD->Region (polyRec)); }
		DBInt LineNum (const DBObjRecord *polyRec) const {return (LineNumFLD->Int (polyRec)); }
		DBCoordinate *Vertexes (const DBObjRecord *) const;
		DBInt VertexNum (const DBObjRecord *polyRec) const { return (VertexNumFLD->Int (polyRec)); }
		void  VertexNum ( DBObjRecord *polyRec,DBInt vertexNum) { VertexNumFLD->Int (polyRec,vertexNum); }
		DBObjRecord *FirstLine (const DBObjRecord *polyRec) const { return (FirstLineFLD->Record (polyRec)); }
		DBFloat Area (const DBObjRecord *polyRec) const { return (AreaFLD->Float (polyRec)); }
		void FourColoring ();
	};

class DBGridIO
	{
	private:
		DBObjData *DataPTR;
		DBObjTable *ItemTable;
		DBObjTable *SymbolTable;
		DBObjTableField *AverageFLD;
		DBObjTableField *StdDevFLD;
		DBObjTableField *MinimumFLD;
		DBObjTableField *MaximumFLD;
		DBObjTableField *MissingValueFLD;
		DBObjTableField *GridValueFLD;
		DBObjTableField *SymbolFLD;

		DBObjTableField *SymbolIDFLD;
		DBObjTableField *ForegroundFLD;
		DBObjTableField *BackgroundFLD;
		DBObjTableField *StyleFLD;

		DBObjTable *LayerTable;
		DBObjTableField *LayerFLD;
		DBPosition DimensionVAR;
		DBFloat CellWidthVAR;
		DBFloat CellHeightVAR;
		DBInt ValueTypeVAR;
		DBInt ValueSizeVAR;
		bool Flat;
	public:
		DBGridIO (DBObjData *data,bool flat) { Initialize (data, flat);  }
		DBGridIO (DBObjData *data)           { Initialize (data, false); }
		void Initialize (DBObjData *, bool);
		void RenameLayer (DBObjRecord *,char *);
		void RenameLayer (char *name) { RenameLayer (LayerTable->Item (),name); }
		DBObjRecord *AddLayer (char *);
		DBInt DeleteLayer  (char *);
		DBInt DeleteLayers (char *,char *);
		DBObjData *Data () const { return (DataPTR); }
		DBInt Coord2Pos (DBCoordinate,DBPosition &) const;
		DBInt Pos2Coord (DBPosition,DBCoordinate &) const;
		DBPosition Dimension () { return (DimensionVAR); }
		DBInt ColNum () const { return (DimensionVAR.Col); }
		DBInt RowNum () const { return (DimensionVAR.Row); }

		DBInt ValueType () const { return (ValueTypeVAR); }
		DBInt ValueSize () const { return (ValueSizeVAR); }
		DBInt Value (DBObjRecord *,DBPosition,DBInt *) const;
		DBInt Value (DBObjRecord *,DBPosition,DBInt);
		DBInt Value (DBInt layerID,DBPosition pos, DBInt *value) const
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) { *value = (DBInt) MissingValue (); return (false); }
			else	return (Value (layerRec,pos,value));
			}
		DBInt Value (DBInt layerID,DBPosition pos, DBInt value)
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) return (DBFault);
			else	return (Value (layerRec,pos,value));
			}
		DBInt Value (DBPosition pos,DBInt *value)	const { return (Value (LayerTable->Item (),pos,value)); }
		DBInt Value (DBPosition pos,DBInt value)			{ return (Value (LayerTable->Item (),pos,value)); }
		DBInt Value (DBObjRecord *layerRec,DBCoordinate coord,DBInt *value) const
			{
			DBPosition pos;
			if (Coord2Pos (coord,pos) == DBFault) { *value = (DBInt) MissingValue (); return (false); }
			return (Value (layerRec,pos,value));
			}
		DBInt Value (DBObjRecord *layerRec,DBCoordinate coord,DBInt value)
			{
			DBPosition pos;
			if (Coord2Pos (coord,pos) == DBFault) return (DBFault);
			return (Value (layerRec,pos,value));
			}
		DBInt Value (DBInt layerID,DBCoordinate coord, DBInt *value) const
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) { *value = (DBInt) MissingValue (); return (false); }
			else	return (Value (layerRec,coord,value));
			}
		DBInt Value (DBInt layerID,DBCoordinate coord, DBInt value)
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) return (DBFault);
			else	return (Value (layerRec,coord,value));
			}
		DBInt Value (DBCoordinate coord,DBInt *value) const	{ return (Value (LayerTable->Item (),coord,value)); }
		DBInt Value (DBCoordinate coord,DBInt value)				{ return (Value (LayerTable->Item (),coord,value)); }

		DBInt Value (DBObjRecord *,DBPosition,DBFloat *) const;
		DBInt Value (DBObjRecord *,DBPosition,DBFloat);
		DBInt Value (DBInt layerID,DBPosition pos, DBFloat *value) const
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) { *value = MissingValue (); return (false); }
			else	return (Value (layerRec,pos,value));
			}
		DBInt Value (DBInt layerID,DBPosition pos, DBFloat value)
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) return (DBFault);
			else	return (Value (layerRec,pos,value));
			}
		DBInt Value (DBPosition pos,DBFloat *value) const	{ return (Value (LayerTable->Item (),pos,value)); }
		DBInt Value (DBPosition pos,DBFloat value)			{ return (Value (LayerTable->Item (),pos,value)); }
		DBInt Value (DBObjRecord *,DBCoordinate,DBFloat *) const;
		DBInt Value (DBObjRecord *layerRec,DBCoordinate coord,DBFloat value)
			{
			DBPosition pos;
			if (Coord2Pos (coord,pos) == DBFault) return (DBFault);
			return (Value (layerRec,pos,value));
			}
		DBInt Value (DBInt layerID,DBCoordinate coord, DBFloat *value) const
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) { *value = MissingValue (); return (false); }
			else	return (Value (layerRec,coord,value));
			}
		DBInt Value (DBInt layerID,DBCoordinate coord, DBFloat value)
			{
			DBObjRecord *layerRec = LayerTable->Item (layerID);
			if (layerRec == (DBObjRecord *) NULL) return (DBFault);
			else	return (Value (layerRec,coord,value));
			}
		DBInt Value (DBCoordinate coord,DBFloat *value) const { return (Value (LayerTable->Item (),coord,value)); }
		DBInt Value (DBCoordinate coord,DBFloat value)			{ return (Value (LayerTable->Item (),coord,value)); }

		DBFloat MissingValue (DBObjRecord *layerRec)	const	{ return (MissingValueFLD->Float (ItemTable->Item (layerRec->RowID ()))); }
		void MissingValue (DBObjRecord *layerRec,DBFloat value)
			{ MissingValueFLD->Float (ItemTable->Item (layerRec->RowID ()),value); }
		DBFloat MissingValue () const	{ return (MissingValue (LayerTable->Item ())); }
		void MissingValue (DBFloat value)
			{
			DBInt layerID;
			for (layerID = 0;layerID < LayerNum ();++layerID) MissingValue (LayerTable->Item (layerID),value);
			}

		DBObjRecord *GridItem (DBObjRecord *,DBPosition) const;
		DBObjRecord *GridItem (DBPosition pos)	const { return (GridItem (LayerTable->Item (),pos)); }
		DBObjRecord *GridItem (DBObjRecord *layerRec,DBCoordinate coord) const
			{
			DBPosition pos;
			if (Coord2Pos (coord,pos) == DBFault) return ((DBObjRecord *) NULL);
			return (GridItem (layerRec,pos));
			}
		DBObjRecord *GridItem (DBCoordinate coord)	const { return (GridItem (LayerTable->Item (),coord)); }
		DBInt GridValue (DBObjRecord *layerRec,DBPosition pos) const
			{
			DBObjRecord *gridRec = GridItem (layerRec,pos);
			if (gridRec == NULL) return (DBFault);
			else	return (GridValueFLD->Int (gridRec));
			}
		DBInt GridValue (DBPosition pos) const { return (GridValue (LayerTable->Item (),pos)); }
		DBInt GridValue (DBObjRecord *layerRec,DBCoordinate coord) const
			{
			DBObjRecord *gridRec = GridItem (layerRec,coord);
			if (gridRec == NULL) return (DBFault);
			else	return (GridValueFLD->Int (gridRec));
			}
		DBInt GridValue (DBCoordinate coord) const { return (GridValue (LayerTable->Item (),coord)); }
		DBInt SymbolFGColor (DBObjRecord *itemRec)
			{
			DBObjRecord *symRec;
			if (itemRec == (DBObjRecord *) NULL) return (DBFault);
			symRec = SymbolFLD->Record (itemRec);
			if (symRec == (DBObjRecord *) NULL)	return (DBFault);
			return (ForegroundFLD->Int (symRec));
			}
		DBInt SymbolFGColor (DBPosition pos) { return (SymbolFGColor (GridItem (pos))); }
		DBInt SymbolFGColor (DBCoordinate coord) { return (SymbolFGColor (GridItem (coord))); }
		DBInt SymbolBGColor (DBObjRecord *itemRec)
			{
			DBObjRecord *symRec;
			if (itemRec == (DBObjRecord *) NULL) return (DBFault);
			symRec = SymbolFLD->Record (itemRec);
			if (symRec == (DBObjRecord *) NULL)	return (DBFault);
			return (BackgroundFLD->Int (symRec));
			}
		DBInt SymbolBGColor (DBPosition pos) { return (SymbolBGColor (GridItem (pos))); }
		DBInt SymbolBGColor (DBCoordinate coord) { return (SymbolBGColor (GridItem (coord))); }
		DBInt SymbolStyle (DBObjRecord *itemRec)
			{
			DBObjRecord *symRec;
			if (itemRec == (DBObjRecord *) NULL) return (DBFault);
			symRec = SymbolFLD->Record (itemRec);
			if (symRec == (DBObjRecord *) NULL)	return (DBFault);
			return (BackgroundFLD->Int (symRec));
			}
		DBInt SymbolStyle (DBPosition pos) { return (SymbolStyle (GridItem (pos))); }
		DBInt SymbolStyle (DBCoordinate coord) { return (SymbolStyle (GridItem (coord))); }

		int ValueDecimals () const { return (AverageFLD->FormatDecimals ()); }
		char *ValueFormat () const;
		char *ValueString (DBObjRecord *, DBPosition);
		char *ValueString (DBObjRecord *record,DBCoordinate coord)
			{
			DBPosition pos;
			if (Coord2Pos (coord,pos) == DBFault) return ((char *) NULL);
			return (ValueString (record,pos));
			}
		char *ValueString (DBCoordinate coord) { return (ValueString (LayerTable->Item (),coord)); }

		DBFloat Maximum (DBInt) const;
		DBFloat Maximum (char *layerName) const
			{ DBObjRecord *layerRec = ItemTable->Item (layerName); return (Maximum (layerRec->RowID ())); }
		DBFloat Maximum () const { return (Maximum (DBFault)); }
		DBFloat Minimum (DBInt) const;
		DBFloat Minimum (char *layerName) const
			{ DBObjRecord *layerRec = ItemTable->Item (layerName); return (Minimum (layerRec->RowID ())); }
		DBFloat Minimum () const { return (Minimum (DBFault)); }
		void RecalcStats (DBObjRecord *);
		void RecalcStats ()
			{
			DBInt layerID;
			DBObjRecord *layerRec;
			for (layerID = 0;layerID < LayerNum ();++layerID)
				{ layerRec = Layer (layerID);  RecalcStats (layerRec); }
			}
		void DiscreteStats ();
		DBFloat CellWidth () const { return (CellWidthVAR); }
		DBFloat CellHeight () const { return (CellHeightVAR); }
		DBFloat CellArea (DBPosition pos) const;
		DBFloat CellArea (DBCoordinate coord) const
			{ DBPosition pos; Coord2Pos (coord,pos); return (CellArea (pos)); }
		DBInt LayerNum () const { return (LayerTable->ItemNum ()); }
		DBObjRecord *Layer () { return (LayerTable->Item ()); }
		DBObjRecord *Layer (DBInt layerID) { return (LayerTable->Item (layerID)); }
		DBObjRecord *Layer (const char *name) { return (LayerTable->Item (name)); }
	};

class DBNetworkIO;

typedef DBInt (*DBNetworkACTION) (DBNetworkIO *,DBObjRecord *,void *);
DBInt DBNetworkSelect   (DBNetworkIO *,DBObjRecord *);
DBInt DBNetworkUnselect (DBNetworkIO *,DBObjRecord *);

class DBNetworkIO
	{
	private:
		DBObjData *DataPTR;
		DBObjTable *BasinTable;
		DBObjTable *CellTable;
		DBObjTable *LayerTable;
		DBObjTable *SymbolTable;

		DBObjTableField *RowNumFLD;
		DBObjTableField *ColNumFLD;
		DBObjTableField *LayerFLD;
		DBObjTableField *CellWidthFLD;
		DBObjTableField *CellHeightFLD;

		DBObjTableField *MouthPosFLD;
		DBObjTableField *ColorFLD;
		DBObjTableField *BasinOrderFLD;
		DBObjTableField *SymbolFLD;
		DBObjTableField *BasinLengthFLD;
		DBObjTableField *BasinAreaFLD;

		DBObjTableField *SymbolIDFLD;
		DBObjTableField *ForegroundFLD;
		DBObjTableField *BackgroundFLD;

		DBObjTableField *PositionFLD;
		DBObjTableField *ToCellFLD;
		DBObjTableField *FromCellFLD;
		DBObjTableField *OrderFLD;
		DBObjTableField *MagnitudeFLD;
		DBObjTableField *BasinFLD;
		DBObjTableField *BasinCellsFLD;
		DBObjTableField *TravelFLD;
		DBObjTableField *UpCellPosFLD;
		DBObjTableField *CellAreaFLD;
		DBObjTableField *CellLengthFLD;
		DBObjTableField *DistToMouthFLD;
		DBObjTableField *DistToOceanFLD;
		DBObjTableField *SubbasinLengthFLD;
		DBObjTableField *SubbasinAreaFLD;

		DBObjRecord *DataRec, *LayerRecord;
		void Climb (DBObjRecord *,DBInt);
		void SetBasin (DBObjRecord *,DBInt);
	public:
		DBNetworkIO (DBObjData *);
		DBObjData *Data () const { return (DataPTR); }
		DBFloat CellWidth ()  const { return (CellWidthFLD->Float  (LayerRecord)); }
		DBFloat CellHeight () const { return (CellHeightFLD->Float (LayerRecord)); }
		DBCoordinate CellSize () const
			{ DBCoordinate coord (CellHeightFLD->Float (LayerRecord),CellWidthFLD->Float (LayerRecord)); return (coord); }
		DBShort RowNum () const { return (RowNumFLD->Int (LayerRecord)); }
		DBShort ColNum () const { return (ColNumFLD->Int (LayerRecord)); }

		DBInt Coord2Pos (DBCoordinate,DBPosition &) const;
		DBInt Pos2Coord (DBPosition,DBCoordinate &) const;

		DBInt BasinNum () const { return (BasinTable->ItemNum ()); }
		DBObjRecord *Basin (DBInt basinID) const
			{ return (BasinTable->Item (basinID)); }
		DBObjRecord *Basin (DBObjRecord *cellRec)	const
			{
			DBInt basinID;
			if (cellRec == (DBObjRecord *) NULL) return ((DBObjRecord *) NULL);
			basinID = BasinFLD->Int (cellRec);
			return (basinID != DBFault ? BasinTable->Item (basinID - 1) : (DBObjRecord *) NULL);
			}

		DBObjRecord *MouthCell(const DBObjRecord *basinRec) const { return (Cell(MouthPosFLD->Position (basinRec))); }
		DBInt Color 	(const DBObjRecord *basinRec) const { return (ColorFLD->Int (basinRec)); }
		DBInt BasinOrder 	(const DBObjRecord *basinRec) const { return (BasinOrderFLD->Int (basinRec)); }
		DBFloat BasinLength	(const DBObjRecord *basinRec) const { return (BasinLengthFLD->Float	(basinRec)); }
		DBFloat BasinArea		(const DBObjRecord *basinRec) const { return (BasinAreaFLD->Float		(basinRec)); }
		DBObjRecord *BasinHeadCell (const DBObjRecord *cellRec) const { return (HeadCell (MouthCell (cellRec))); }

		DBInt			CellNum () const { return (CellTable->ItemNum ()); }
		DBPosition	CellPosition(const DBObjRecord *cellRec)	const { return (PositionFLD->Position (cellRec)); }
		DBInt			CellOrder	(const DBObjRecord *cellRec)
			const { if (cellRec == (DBObjRecord *) NULL) return (DBFault); else  return (OrderFLD->Int (cellRec)); }
		DBInt			CellBasinID	(const DBObjRecord *cellRec)	const
			{ if (cellRec == (DBObjRecord *) NULL) return (DBFault); else  return (BasinFLD->Int (cellRec)); }
		DBInt			CellBasinID	(DBObjRecord *cellRec, DBInt id)
			{ if (cellRec == (DBObjRecord *) NULL) return (DBFault); BasinFLD->Int (cellRec,id); return (DBSuccess); }
		DBInt			CellTravel	(const DBObjRecord *cellRec)	const
			{ if (cellRec == (DBObjRecord *) NULL) return (DBFault); else  return (TravelFLD->Int (cellRec)); }
		DBInt			CellBasinCells	(const DBObjRecord *cellRec)	const { return (BasinCellsFLD->Int (cellRec)); }
		DBFloat		CellArea			(const DBObjRecord *cellRec)	const { return (CellAreaFLD->Float (cellRec)); }
		DBFloat		CellLength		(const DBObjRecord *cellRec)	const	{ return (CellLengthFLD->Float (cellRec)); }
		DBFloat		CellDistToMouth (const DBObjRecord *cellRec)	const { return (DistToMouthFLD->Float (cellRec)); }
		DBFloat		CellDistToOcean (const DBObjRecord *cellRec)	const { return (DistToOceanFLD->Float (cellRec)); }
		DBFloat		CellBasinLength(const DBObjRecord *cellRec)	const { return (SubbasinLengthFLD->Float (cellRec)); }
		DBFloat		CellBasinArea	(const DBObjRecord *cellRec)	const { return (SubbasinAreaFLD->Float (cellRec)); }
		DBCoordinate Center (const DBObjRecord *) const;
		DBCoordinate Delta  (const DBObjRecord *) const;

		DBObjRecord *Cell (DBInt id)		const { return (CellTable->Item (id)); }
		DBObjRecord *Cell (DBPosition)	const;
		DBObjRecord *Cell (DBPosition,DBFloat)	const;
		DBObjRecord *Cell (DBCoordinate coord)	const
			{ DBPosition pos; if (Coord2Pos (coord,pos) == DBSuccess) return (Cell (pos)); else return ((DBObjRecord *) NULL); }
		DBObjRecord *Cell (DBCoordinate coord,DBFloat area)	const
			{ DBPosition pos; if (Coord2Pos (coord,pos) == DBSuccess) return (Cell (pos,area)); else return ((DBObjRecord *) NULL); }

		DBObjRecord *ToCell (const DBObjRecord *) const;
		DBInt ToCellDir (const DBObjRecord *cellRec) const
			{ if (cellRec == (DBObjRecord *) NULL) return (DBFault); else return (ToCellFLD->Int (cellRec)); }
		DBObjRecord *FromCell (const DBObjRecord *,DBInt,DBInt) const;
		DBObjRecord *FromCell (const DBObjRecord *cellRec,DBInt dir) const
			{ return (FromCell (cellRec,dir,true)); }
		DBObjRecord *FromCell (const DBObjRecord *cellRec) const;
		DBInt FromCellDirs (const DBObjRecord *cellRec) const
			{ if (cellRec == (DBObjRecord *) NULL) return (DBFault); else return (FromCellFLD->Int (cellRec)); }
		DBObjRecord *HeadCell (const DBObjRecord *cellRec) const;
		void UpStreamSearch (DBObjRecord *,DBNetworkACTION,DBNetworkACTION,void *);
		void UpStreamSearch (DBObjRecord *record,DBNetworkACTION forAction,DBNetworkACTION backAction)
			{ UpStreamSearch (record,forAction,backAction,(void *) NULL); }
		void UpStreamSearch (DBObjRecord *record,DBNetworkACTION action,void *data)
			{ UpStreamSearch (record,action,(DBNetworkACTION) NULL,data); }
		void UpStreamSearch (DBObjRecord *record,DBNetworkACTION action)
			{ UpStreamSearch (record,action,(DBNetworkACTION) NULL,(void *) NULL); }
		void DownStreamSearch (DBObjRecord *,DBNetworkACTION,DBNetworkACTION,void *data);
		void DownStreamSearch (DBObjRecord *record,DBNetworkACTION forAction,DBNetworkACTION backAction)
			{ DownStreamSearch (record,forAction,backAction,(void *) NULL); }
		void DownStreamSearch (DBObjRecord *record,DBNetworkACTION action,void *data)
			{ DownStreamSearch (record,action,(DBNetworkACTION) NULL,data); }
		void DownStreamSearch (DBObjRecord *record,DBNetworkACTION action)
			{ DownStreamSearch (record,action,(DBNetworkACTION) NULL,(void *) NULL); }

		DBObjRecord *CellAdd (DBPosition);
		DBObjRecord *CellAdd (DBCoordinate coord)
			{ DBPosition pos; if (Coord2Pos (coord,pos) == DBSuccess) return (CellAdd (pos)); else return ((DBObjRecord *) NULL); }
		DBInt			CellDelete (DBObjRecord *cellRec);
		DBInt			CellDelete (DBPosition position) { return (CellDelete (Cell (position))); }
		DBInt			CellDelete (DBCoordinate coord)  { return (CellDelete (Cell (coord))); }
		DBInt			CellDirection (DBObjRecord *cellRec,DBInt dir)
			{ if (cellRec == (DBObjRecord *) NULL) return (DBFault); ToCellFLD->Int (cellRec,dir); return (DBSuccess);}
		DBInt			CellDirection (DBPosition position,DBInt dir) { return (CellDirection (Cell (position),dir)); }
		DBInt			CellDirection (DBCoordinate coord,DBInt dir) { return (CellDirection (Cell (coord),dir)); }
		DBInt			CellDirection (DBObjRecord *cellRec)
			{ if (cellRec == (DBObjRecord *) NULL) return (DBFault); return (ToCellFLD->Int (cellRec)); }
		DBInt			CellDirection (DBPosition position) { return (CellDirection (Cell (position))); }
		DBInt			CellDirection (DBCoordinate coord) { return (CellDirection (Cell (coord))); }

		DBInt			CellRotate (DBObjRecord *,DBInt dir);
		DBInt			CellRotate (DBPosition position,DBInt dir) { return (CellRotate (Cell (position),dir)); }
		DBInt			CellRotate (DBCoordinate coord,DBInt dir) { return (CellRotate (Cell (coord),dir)); }

		DBInt			Build ();
		DBInt			Trim ();
		void 			SetDistToMouth ();
		void 			SetDistToOcean ();
		DBInt 		DistToMouth () { return (DistToMouthFLD != (DBObjTableField *) NULL ? true : false); }
		DBInt 		DistToOcean () { return (DistToOceanFLD != (DBObjTableField *) NULL ? true : false); }
		void			SetMagnitude ();
		DBInt 		Magnitude () { return (MagnitudeFLD != (DBObjTableField *) NULL ? true : false); }
		DBObjRecord *Symbol (DBInt id) const { return (SymbolTable->Item (id)); }
		DBObjRecord *FirstSymbol () const { return (SymbolTable->First ()); }
		DBObjRecord *LastSymbol  () const { return (SymbolTable->Last ()); }
		DBObjRecord *NextSymbol  () const { return (SymbolTable->Next ()); }
		DBObjRecord *NextSymbol  (DBInt dir)	const { return (SymbolTable->Next (dir)); }
		DBObjRecord *NewSymbol (const char *symName)
			{
			DBObjRecord *symRec = SymbolTable->Add (symName);
			ForegroundFLD->Int (symRec,1);
			BackgroundFLD->Int (symRec,0);
			return (symRec);
			}
		void ItemSymbol (DBObjRecord *itemRec,DBObjRecord *symRec) { SymbolFLD->Record (itemRec,symRec); }
		DBInt ItemForeground (const DBObjRecord *itemRec)
			{ DBObjRecord *symRec = SymbolFLD->Record (itemRec); return (ForegroundFLD->Int (symRec)); }
		DBInt ItemBackground (const DBObjRecord *itemRec)
			{ DBObjRecord *symRec = SymbolFLD->Record (itemRec); return (BackgroundFLD->Int (symRec)); }
		};

class DBTableIO

	{
	private:
		DBObjData  *DataPTR;
		DBObjTable *ItemTablePTR;
	public:
		DBTableIO (DBObjData *);
		DBInt AppendASCII (char *);
	};
