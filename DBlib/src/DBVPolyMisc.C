/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBVPolyMisc.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

DBVPolyIO::DBVPolyIO (DBObjData *data) : DBVLineIO (data,data->Table (DBrNContours))

	{
	DBObjTable *items = data->Table (DBrNItems);
	LineTable = data->Table (DBrNContours);
	DBObjRecord *polyRec;
	
	FirstLineFLD	= items->Field (DBrNFirstLine);
	LineNumFLD	= items->Field (DBrNLineNum);
	ExtentFLD 	= items->Field (DBrNRegion);
	LeftPolyFLD	= LineTable->Field (DBrNLeftPoly);
	RightPolyFLD= LineTable->Field (DBrNRightPoly);
	NextLineFLD	= LineTable->Field (DBrNNextLine);
	PrevLineFLD	= LineTable->Field (DBrNPrevLine);
	VertexNumFLD = items->Field (DBrNVertexNum);
	AreaFLD		= items->Field (DBrNArea);
	MaxVertexNumVAR = 0;
	
	for (polyRec = FirstItem ();polyRec != (DBObjRecord *) NULL;polyRec = NextItem ())
		MaxVertexNumVAR = MaxVertexNumVAR > VertexNum (polyRec) ? MaxVertexNumVAR : VertexNum (polyRec);
	if ((CoordsPTR = (DBCoordinate *) malloc (MaxVertexNumVAR * sizeof (DBCoordinate))) == (DBCoordinate *) NULL)
		{ perror ("Memory Allocation Error in: DBVPolyIO::DBVPolyIO ()"); }
	}

DBCoordinate *DBVPolyIO::Vertexes (const DBObjRecord *polyRec) const

	{
	DBInt vertex, i, line;
	DBObjRecord *lineRec, *nodeRec,*firstNodeRec;
	DBCoordinate *lcoords;
	
	vertex = 0;
	lineRec = FirstLine (polyRec);
	firstNodeRec = nodeRec = LineRightPoly (lineRec) == polyRec ? FromNode (lineRec) : ToNode (lineRec);
	for (line = 0;line < LineNum (polyRec);++line)
		{
		if (LineRightPoly (lineRec) == polyRec)
			{
			if (nodeRec != FromNode (lineRec))
				{
				CoordsPTR [vertex++] = NodeCoord (nodeRec);
				if (nodeRec != firstNodeRec) CoordsPTR [vertex++] = NodeCoord (firstNodeRec);
				nodeRec = FromNode (lineRec);
				}
			CoordsPTR [vertex++] = NodeCoord (nodeRec);
			if (DBVLineIO::VertexNum(lineRec) > 0)
				{
				lcoords = DBVLineIO::Vertexes (lineRec);
				for (i = 0;i < DBVLineIO::VertexNum (lineRec);++i) CoordsPTR [vertex++] = lcoords [i];
				}
			nodeRec = ToNode (lineRec);
			lineRec = LineNextLine (lineRec);
			}
		else
			{
			if (nodeRec != ToNode (lineRec))
				{
				CoordsPTR [vertex++] = NodeCoord (nodeRec);
				if (nodeRec != firstNodeRec) CoordsPTR [vertex++] = NodeCoord (firstNodeRec);
				nodeRec = ToNode (lineRec);
				}
			CoordsPTR [vertex++] = NodeCoord (nodeRec);
			if (DBVLineIO::VertexNum (lineRec) > 0)
				{
				lcoords = (DBCoordinate *) (DBVLineIO::Vertexes (lineRec));
				for (i = DBVLineIO::VertexNum (lineRec);i > 0;--i) CoordsPTR [vertex++] = lcoords [i - 1];
				}
			nodeRec = FromNode (lineRec);
			lineRec = LinePrevLine (lineRec);
			}
		}
	CoordsPTR [vertex++] = NodeCoord (nodeRec);
	if (nodeRec != firstNodeRec) CoordsPTR [vertex++] = NodeCoord (firstNodeRec); 
	if (VertexNum (polyRec) != vertex)
		fprintf (stderr,"Warning in Polygon %d VertexNum [%d %d] in: DBVPolyIO::Vertexes ()\n",polyRec->RowID (), VertexNum (polyRec),vertex);
	return (CoordsPTR);
	}

void DBVPolyIO::FourColoring ()

	{
	char nameStr [DBStringLength];
	DBInt symbol, maxSymbol = 0, line, index;
	DBObjRecord *polyRec,*searchPoly, *neighborPolyRec, *lineRec, *symRec;

 	for (polyRec = FirstItem ();polyRec != (DBObjRecord *) NULL;polyRec = NextItem ())
 		SymbolFLD->Record (polyRec,(DBObjRecord *) NULL);
	ListSort (LineNumFLD);
 	for (searchPoly = FirstItem ();searchPoly != (DBObjRecord *) NULL;searchPoly = NextItem ())
 		{
 		if (((DBInt) (SymbolFLD->Record (searchPoly) - (DBObjRecord *) NULL)) != 0) continue;
 		DBPause (searchPoly->RowID () * 100 / ItemNum ());
 		symbol = 1;
Restart:
 		for (polyRec = ItemTable->First (&index);polyRec != (DBObjRecord *) NULL;polyRec = ItemTable->Next (&index))
 			{
 			if (strcmp (polyRec->Name (),searchPoly->Name ()) != 0) continue;
 			lineRec = FirstLine (polyRec);
 			for (line = 0;line <= LineNum (polyRec);++line)
				{
				if (LineRightPoly (lineRec) == polyRec)
					{
					neighborPolyRec = LineLeftPoly (lineRec);
					lineRec = LineNextLine (lineRec);
					}
				else
					{
					neighborPolyRec = LineRightPoly (lineRec);
					lineRec = LinePrevLine (lineRec);
					}
				if (neighborPolyRec == (DBObjRecord *) NULL) continue;
				if (SymbolFLD->Record (neighborPolyRec) == (DBObjRecord *) symbol)
					{ symbol++; goto Restart; }
				}
			}
 		for (polyRec = ItemTable->First (&index);polyRec != (DBObjRecord *) NULL;polyRec = ItemTable->Next (&index))
 			if (strcmp (polyRec->Name (),searchPoly->Name ()) == 0) SymbolFLD->Record (polyRec,(DBObjRecord *) symbol);
		maxSymbol = maxSymbol > symbol ? maxSymbol : symbol;
		}
	while (maxSymbol < SymbolTable->ItemNum ()) SymbolTable->Remove (SymbolTable->ItemNum () - 1);
	while (maxSymbol > SymbolTable->ItemNum ()) SymbolTable->Add ();
	for (symbol = 0;symbol < SymbolTable->ItemNum ();++symbol)
		{
		symRec = SymbolTable->Item (symbol);
		sprintf (nameStr,"Symbol:%2d",symbol);
		symRec->Name (nameStr);
		StyleFLD->Int (symRec,0);
		ForegroundFLD->Int (symRec,1);
		BackgroundFLD->Int (symRec,0);
		}
 	for (polyRec = FirstItem ();polyRec != (DBObjRecord *) NULL;polyRec = NextItem ())
 		SymbolFLD->Record (polyRec,SymbolTable->Item ((DBInt) (SymbolFLD->Record (polyRec) - (DBObjRecord *) NULL) - 1));
	ListReset ();
	}
