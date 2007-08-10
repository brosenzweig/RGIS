/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

DBImpARCPoly.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

int DBImportARCLine	(DBObjData *,const char *);

int DBImportARCPoly (DBObjData *vecData,const char *arcCov)

	{
	DBInt polyNum = 0, line, lineNum, vertexNum,  polyDir;
	DBObjTable *items = vecData->Table (DBrNItems);
	DBObjTable *lines	= vecData->Table (DBrNContours);
	DBObjTableField *firstLineFLD	= items->Field (DBrNFirstLine);
	DBObjTableField *lineNumFLD	= items->Field (DBrNLineNum);
	DBObjTableField *polyVertexNumFLD = items->Field (DBrNVertexNum);
	DBObjTableField *polyExtentFLD	= items->Field (DBrNRegion);
	DBObjTableField *leftPolyFLD	= lines->Field (DBrNLeftPoly);
	DBObjTableField *rightPolyFLD	= lines->Field (DBrNRightPoly);
	DBObjTableField *nextLineFLD	= lines->Field (DBrNNextLine);
	DBObjTableField *prevLineFLD	= lines->Field (DBrNPrevLine);
	DBObjTableField *fromNodeFLD	= lines->Field (DBrNFromNode);
	DBObjTableField *toNodeFLD		= lines->Field (DBrNToNode);
	DBObjTableField *lineVertextNumFLD	= lines->Field (DBrNVertexNum);
	DBObjTableField *lineExtentFLD	= lines->Field (DBrNRegion);
	DBObjRecord *polyRec, *lineRec, *firstNodeRec, *nodeRec, **lineRecArray;
	DBRegion extent, initRegion;

	if (DBImportARCLine (vecData,arcCov) == DBFault) return (DBFault);

	items->Delete (0);
	for (polyRec = items->First ();polyRec != (DBObjRecord *) NULL;polyRec = items->Next ())
		lineNumFLD->Int (polyRec,0);

	for (lineRec = lines->First ();lineRec != (DBObjRecord *) NULL;lineRec = lines->Next ())
		{
		rightPolyFLD->Record	(lineRec,items->Item ((DBInt) (rightPolyFLD->Record(lineRec) - (DBObjRecord *) NULL)));
		leftPolyFLD->Record  (lineRec,items->Item ((DBInt) (leftPolyFLD->Record (lineRec) - (DBObjRecord *) NULL)));
		nextLineFLD->Record (lineRec,(DBObjRecord *) NULL);
		prevLineFLD->Record (lineRec,(DBObjRecord *) NULL);
		}

	if ((lineRecArray = (DBObjRecord **) calloc (lines->ItemNum (),sizeof (DBObjRecord *))) == (DBObjRecord **) NULL)
		{ perror ("Memory Allocation Error in: DBImportARCPoly ()"); return (DBFault); }

	for (polyRec = items->First ();polyRec != (DBObjRecord *) NULL;polyRec = items->Next ())
		{
		DBPause ((++polyNum * 100) / items->ItemNum ());

		lineNum = vertexNum = 0;
		extent = initRegion;
		for (lineRec = lines->First ();lineRec != (DBObjRecord *) NULL;lineRec = lines->Next ())
			if ((rightPolyFLD->Record (lineRec) == polyRec) || (leftPolyFLD->Record (lineRec) == polyRec))
				{
				lineRecArray [lineNum++] = lineRec;
				vertexNum += lineVertextNumFLD->Int (lineRec);
				extent.Expand (lineExtentFLD->Region (lineRec));
				}
		lineNumFLD->Int (polyRec,lineNum);
		polyExtentFLD->Region (polyRec,extent);
		firstLineFLD->Record (polyRec,lineRecArray [0]);
		firstNodeRec = (rightPolyFLD->Record (lineRecArray [0]) == polyRec) ?
							fromNodeFLD->Record  (lineRecArray [0]) : toNodeFLD->Record(lineRecArray [0]);
		for (lineNum = 1;lineNum < lineNumFLD->Int (polyRec);++lineNum)
			{
			polyDir = (rightPolyFLD->Record (lineRecArray [lineNum - 1]) == polyRec);
			nodeRec = polyDir ?	toNodeFLD->Record  (lineRecArray [lineNum - 1]) :
										fromNodeFLD->Record(lineRecArray [lineNum - 1]);
			for (line = lineNum;line < lineNumFLD->Int (polyRec); ++line)
				if ((nodeRec == fromNodeFLD->Record (lineRecArray [line])) ||
					 (nodeRec == toNodeFLD->Record   (lineRecArray [line]))) break;
			if (line < lineNumFLD->Int (polyRec))
				{
				lineRec = lineRecArray [lineNum];
				lineRecArray [lineNum] = lineRecArray [line];
				lineRecArray [line] = lineRec;
				}
			else
				{
				vertexNum += 1;
				if (nodeRec != firstNodeRec) vertexNum += 1;
				}
			}
		polyDir = (rightPolyFLD->Record (lineRecArray [lineNum - 1]) == polyRec);
		nodeRec = polyDir ?	toNodeFLD->Record  (lineRecArray [lineNum - 1]) :
									fromNodeFLD->Record(lineRecArray [lineNum - 1]);
		polyVertexNumFLD->Int (polyRec,vertexNum + lineNum + 1 + (nodeRec != firstNodeRec ? 1 : 0));
		for (lineNum = 1;lineNum < lineNumFLD->Int (polyRec);++lineNum)
			{
			if (rightPolyFLD->Record  (lineRecArray [lineNum - 1]) == polyRec)
					nextLineFLD->Record (lineRecArray [lineNum - 1],lineRecArray [lineNum]);
			else	prevLineFLD->Record (lineRecArray [lineNum - 1],lineRecArray [lineNum]);
			}
		if (rightPolyFLD->Record  (lineRecArray [lineNum - 1]) == polyRec)
				nextLineFLD->Record (lineRecArray [lineNum - 1],lineRecArray [0]);
		else	prevLineFLD->Record (lineRecArray [lineNum - 1],lineRecArray [0]);
		}
	free (lineRecArray);
	return (DBSuccess);
	}
