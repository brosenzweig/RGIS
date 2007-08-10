/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

DBImpARCLine.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

class DBARCRecord
	{
	private:
		int     IntIDVAR;
		int     RecLengthVAR;
		int     UserIDVAR;
		int     FromNodeVAR;
		int     ToNodeVAR;
		int     LeftPolyVAR;
		int     RightPolyVAR;
		int     NumOfPntsVAR;
		void Swap ()
			{
			DBByteOrderSwapWord (&IntIDVAR);
			DBByteOrderSwapWord (&RecLengthVAR);
			DBByteOrderSwapWord (&UserIDVAR);
			DBByteOrderSwapWord (&FromNodeVAR);
			DBByteOrderSwapWord (&ToNodeVAR);
			DBByteOrderSwapWord (&LeftPolyVAR);
			DBByteOrderSwapWord (&RightPolyVAR);
			DBByteOrderSwapWord (&NumOfPntsVAR);
			}
	public:
		int Read (FILE *file,int swap)
			{
			if (fread (this,sizeof (DBARCRecord),1,file) != 1) return (DBFault);
			if (swap) Swap ();
			return (DBSuccess);
			}
		DBInt ID () const				 { return (IntIDVAR); }
		DBInt RecordLength () const { return (RecLengthVAR); }
		DBInt NumOfPnts () const 	 { return (NumOfPntsVAR); }
		DBInt FromNode () const 	 { return (FromNodeVAR); }
		DBInt ToNode () const 		 { return (ToNodeVAR); }
		DBObjRecord *LeftPoly  () const { return ((DBObjRecord *) (LeftPolyVAR  > 1 ? LeftPolyVAR  - 2 : DBFault)); }
		DBObjRecord *RightPoly () const { return ((DBObjRecord *) (RightPolyVAR > 1 ? RightPolyVAR - 2 : DBFault)); }
	};

int DBImportARCLine (DBObjData *vecData,const char *arcCov)

	{
	FILE *inFile;
	DBInt arcNum, vertex, swap = DBByteOrder (DBByteOrderLITTLE), floatCov;
	char fileName [DBDataFileNameLen], objName [DBStringLength];
	short infoHeader [50];
	DBARCRecord arcRecord;
	DBObjTable  *lines;
	DBObjTable  *nodes;
	DBObjectLIST<DBObjRecord> *data;
	DBObjRecord *lineRec, *nodeRec, *dataRec;
	DBRegion dataExtent = vecData->Extent (), itemExtent;

	DBObjTableField *fromNodeFLD;
	DBObjTableField *toNodeFLD;
	DBObjTableField *leftPolyFLD;
	DBObjTableField *rightPolyFLD;
	DBObjTableField *vertexesFLD;
	DBObjTableField *vertexNumFLD;
	DBObjTableField *extentFLD;
	
	DBObjTableField *coordFLD;
	DBObjTableField *linkNumFLD;
	DBCoordinate *vertexes, nodeCoord;
	DBFloat4 floatVAR [2];

	switch (vecData->Type ())
		{
		case DBTypeVectorLine:		lines = vecData->Table (DBrNItems);		break;
		case DBTypeVectorPolygon:	lines = vecData->Table (DBrNContours);	break;
		default: fprintf (stderr,"Invalide Vector Data Type in: DBImportARCLine ()\n"); return (DBFault);
		}

	fromNodeFLD	= lines->Field (DBrNFromNode);
	toNodeFLD	= lines->Field (DBrNToNode);
	leftPolyFLD	= lines->Field (DBrNLeftPoly);
	rightPolyFLD= lines->Field (DBrNRightPoly);
	vertexesFLD	= lines->Field (DBrNVertexes);
	vertexNumFLD= lines->Field (DBrNVertexNum);
	extentFLD	= lines->Field (DBrNRegion);

	nodes = vecData->Table (DBrNNodes);	
	coordFLD		= nodes->Field (DBrNCoord);
	linkNumFLD	= nodes->Field (DBrNLinkNum);
	data = vecData->Arrays ();
	
	sprintf (fileName,"%s/arc",arcCov);	
	if (access (fileName,R_OK) == DBFault) sprintf (fileName,"%s/arc.adf",arcCov);

	if ((inFile = fopen (fileName,"r")) == NULL)
		{ perror ("File Opening Error in: DBImportARCLine ()"); return (DBFault); }
	if (fread (infoHeader,sizeof (short),50,inFile) != 50)
		{ perror ("File Reading Error in: DBImportARCLine ()"); return (DBFault); }
	
	arcNum = 0;			
	for (lineRec = lines->First ();arcRecord.Read (inFile,swap) != DBFault;lineRec = lines->Next ())
		{
		if (lineRec == NULL)
			{
			sprintf (objName,"Line: %5d",arcRecord.ID () + 1);
			if ((lineRec = lines->Add (objName)) == (DBObjRecord *) NULL)	return (DBFault);
			}
		else DBPause ((++arcNum * 100) / lines->ItemNum ());
		
		floatCov = arcRecord.RecordLength () - 12 == arcRecord.NumOfPnts () * (DBInt) sizeof (float) ? true : false;
		while (arcRecord.FromNode () > nodes->ItemNum ())
			{
			sprintf (objName,"Node: %5d",nodes->ItemNum () + 1);
			nodes->Add (objName);
			if ((nodeRec = nodes->Item ()) == NULL) return (DBFault);
			linkNumFLD->Int (nodeRec,0);
			}
		if ((nodeRec = nodes->Item (arcRecord.FromNode () - 1)) == (DBObjRecord *) NULL)
			{ fprintf (stderr,"Node Not Found in: DBImportARCLine ()\n"); return (DBFault); }
		if (floatCov)
			{
			if (fread (floatVAR,sizeof (floatVAR),1,inFile) != 1)
				{ perror ("File Reading Error in: DBImportARCLine ()"); return (DBFault); }
			if (swap)	{ DBByteOrderSwapWord (floatVAR); DBByteOrderSwapWord (floatVAR + 1); }
			nodeCoord.X = (DBFloat) floatVAR [0];
			nodeCoord.Y = (DBFloat) floatVAR [1];
			}
		else
			{
			if (fread (&nodeCoord,sizeof (DBCoordinate),1,inFile) != 1)
				{ perror ("File Reading Error in: DBImportARCLine ()"); return (DBFault); }
			if (swap) nodeCoord.Swap ();
			}
		coordFLD->Coordinate (nodeRec,nodeCoord);
		fromNodeFLD->Record	(lineRec,nodeRec);
		if (leftPolyFLD  != (DBObjTableField *) NULL) leftPolyFLD->Record  (lineRec,arcRecord.LeftPoly  ());
		if (rightPolyFLD != (DBObjTableField *) NULL) rightPolyFLD->Record (lineRec,arcRecord.RightPoly ());
		itemExtent.LowerLeft	 = nodeCoord;
		itemExtent.UpperRight = nodeCoord;
		linkNumFLD->Int (nodeRec,linkNumFLD->Int (nodeRec) + 1);
		if (arcRecord.NumOfPnts () > 2)
			{
			if ((dataRec = data->Item (lineRec->RowID ())) == (DBObjRecord *) NULL)
				{
				if ((dataRec = new DBObjRecord ("LineData",0,sizeof (DBFloat))) == (DBObjRecord *) NULL) return (DBFault);
				data->Add (dataRec);
				}
			dataRec->Realloc ((arcRecord.NumOfPnts () - 2) * sizeof (DBCoordinate));
			if ((vertexes = (DBCoordinate *) dataRec->Data ()) == NULL)
				{ perror ("Memory Allocation Error in: DBImportARCLine ()"); return (DBFault); }
		
			if (floatCov)
				for (vertex = 0;vertex < arcRecord.NumOfPnts () - 2;++vertex)
					{
					if (fread (floatVAR,sizeof (floatVAR),1,inFile) != 1)
					{ perror ("File Reading Error in: DBImportARCLine ()"); return (DBFault); }
					if (swap) { DBByteOrderSwapWord (floatVAR); DBByteOrderSwapWord (floatVAR + 1); }
					vertexes [vertex].X = (DBFloat) floatVAR [0];
					vertexes [vertex].Y = (DBFloat) floatVAR [1];
					dataExtent.Expand (vertexes [vertex]);
					itemExtent.Expand (vertexes [vertex]);
					}
			else
				for (vertex = 0;vertex < arcRecord.NumOfPnts () - 2;++vertex)
					{
					if (fread (vertexes + vertex,sizeof (DBCoordinate),1,inFile) != 1)
						{ perror ("File Reading Error in: DBImportARCLine ()"); return (DBFault); }
					if (swap) vertexes [vertex].Swap ();
					dataExtent.Expand (vertexes [vertex]);
					itemExtent.Expand (vertexes [vertex]);
					}
			vertexesFLD->Record (lineRec,dataRec);
			}
		else	vertexesFLD->Record (lineRec,(DBObjRecord *) NULL);
		vertexNumFLD->Int	  (lineRec,arcRecord.NumOfPnts () - 2);
		while (arcRecord.ToNode () > nodes->ItemNum ())
			{
			sprintf (objName,"Node: %5d",nodes->ItemNum () + 1);
			nodes->Add (objName);
			if ((nodeRec = nodes->Item ()) == NULL) return (DBFault);
			linkNumFLD->Int (nodeRec,0);
			}
		if ((nodeRec = nodes->Item (arcRecord.ToNode () - 1)) == (DBObjRecord *) NULL)
			{ fprintf (stderr,"Node Not Found in: DBImportARCLine ()\n"); return (DBFault); }
		if (floatCov)
			{
			if (fread (floatVAR,sizeof (floatVAR),1,inFile) != 1)
				{ perror ("File Reading Error in: DBImportARCLine ()"); return (DBFault); }
			if (swap)	{ DBByteOrderSwapWord (floatVAR); DBByteOrderSwapWord (floatVAR + 1); }
			nodeCoord.X = (DBFloat) floatVAR [0];
			nodeCoord.Y = (DBFloat) floatVAR [1];
			}
		else
			{
			if (fread (&nodeCoord,sizeof (DBCoordinate),1,inFile) != 1)
				{ perror ("File Reading Error in: DBImportARCLine ()"); return (DBFault); }
			if (swap) nodeCoord.Swap ();
			}
		itemExtent.Expand (nodeCoord);
		coordFLD->Coordinate (nodeRec,nodeCoord);
		linkNumFLD->Int (nodeRec,linkNumFLD->Int (nodeRec) + 1);
		toNodeFLD->Record		(lineRec,nodeRec);
		extentFLD->Region		(lineRec,itemExtent);
		}
	fclose (inFile);

	vertexesFLD->RecordProp ((DBInt) sizeof (DBFloat));
	vecData->Extent (dataExtent);
   vecData->Projection (DBMathGuessProjection (dataExtent));
	return (DBSuccess);
	}
