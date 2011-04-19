/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBVLineMisc.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

void DBVLineIF::Initialize (DBObjData *data,DBObjTable *items)

	{
	NodeTable = data->Table (DBrNNodes);
	FromNodeFLD  = items->Field (DBrNFromNode);
	ToNodeFLD    = items->Field (DBrNToNode);
	VertexesFLD  = items->Field (DBrNVertexes);
	VertexNumFLD = items->Field (DBrNVertexNum);
	ExtentFLD    = items->Field (DBrNRegion);
	PerimeterFLD = items->Field (DBrNPerimeter);
	NodeCoordFLD	= NodeTable->Field (DBrNCoord);
	}

DBObjRecord *DBVLineIF::Node (DBCoordinate coord,DBInt create)

	{
	DBFloat precision = pow ((double) 10.0,(double) Data ()->Precision ());
	DBObjRecord *nodeRec;
	
	for (nodeRec = NodeTable->First ();nodeRec != (DBObjRecord *) NULL;nodeRec = NodeTable->Next ())
		if (DBMathCartesianDistance (coord,NodeCoordFLD->Coordinate (nodeRec)) < precision) return (nodeRec);
	if (create)
		{
		char objName [DBStringLength];

		sprintf (objName,"Node: %5d",NodeTable->ItemNum ());
		nodeRec = NodeTable->Add (objName);
		NodeCoordFLD->Coordinate (nodeRec,coord);
		return (nodeRec);
		}
	return ((DBObjRecord *) NULL);
	}

void DBVLineIF::Vertexes (DBObjRecord *lineRec,DBCoordinate *coord,DBInt vertexNum)

	{
	DBInt vertex;
	DBFloat length = 0;
	DBCoordinate *vertexes, prevCoord;
	DBObjRecord *dataRec = (DBObjRecord *) NULL;
	DBObjectLIST<DBObjRecord> *dataArrays = Data ()->Arrays ();
	DBRegion lineExtent, dataExtent = Data ()->Extent ();
	DBMathDistanceFunction distFunc = DBMathGetDistanceFunction (Data ());

	prevCoord = NodeCoordFLD->Coordinate (FromNodeFLD->Record (lineRec));
	lineExtent.Expand (prevCoord);
	if (vertexNum > 0)
		{
		if ((dataRec = dataArrays->Item (lineRec->RowID ())) == (DBObjRecord *) NULL)
			{
			if ((dataRec = new DBObjRecord (lineRec->Name (),vertexNum * sizeof (DBCoordinate),sizeof (DBFloat))) == (DBObjRecord *) NULL) return;
			dataArrays->Add (dataRec);
			}
		else	dataRec->Realloc (vertexNum * sizeof (DBCoordinate));
		if ((vertexes = (DBCoordinate *) dataRec->Data ()) == NULL)
			{ perror ("Memory Allocation Error in:  DBVLineIF::Vertexes ()"); return; }
		for (vertex = 0;vertex < vertexNum;vertex++)
			{
			lineExtent.Expand (vertexes [vertex] = coord [vertex]);
			length += DBMathCoordinateDistance (distFunc,prevCoord,coord [vertex]);
			prevCoord = coord [vertex];
			}
		}
	VertexesFLD->Record (lineRec,dataRec);
	VertexNumFLD->Int (lineRec,vertexNum);
	length += DBMathCoordinateDistance (distFunc,prevCoord,NodeCoordFLD->Coordinate (ToNodeFLD->Record (lineRec)));
	PerimeterFLD->Float (lineRec,length);
	lineExtent.Expand (NodeCoordFLD->Coordinate (ToNodeFLD->Record (lineRec)));
	ExtentFLD->Region (lineRec,lineExtent);
	dataExtent.Expand (lineExtent);
	Data ()->Extent (dataExtent);
	}

void DBVLineIF::Flip (DBObjRecord *lineRec)

	{
	DBInt vertexNum = VertexNum (lineRec);
	DBObjRecord *node;

	node = ToNodeFLD->Record (lineRec);
	ToNodeFLD->Record (lineRec,FromNodeFLD->Record (lineRec));
	FromNodeFLD->Record (lineRec,node);
	
	if (vertexNum > 0)
		{
		DBInt vertex;
		DBCoordinate *vertexes = Vertexes (lineRec), coord;
		
		for (vertex = 0;vertex < vertexNum >> 0x01; ++vertex)
			{
			coord = vertexes [vertexNum - 1 -vertex];
			vertexes [vertexNum - 1 -vertex] = vertexes [vertex];
			vertexes [vertex] = coord;
			}
		}
	}
