/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

DBImpASCIINet.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <ctype.h>
#include <DB.H>
#include <DBio.H>

#define NCOLS "ncols"
#define NROWS "nrows"
#define XLLCORNER "xllcorner"
#define YLLCORNER "yllcorner"
#define XLLCENTER	"xllcenter"
#define YLLCENTER	"yllcenter"
#define CELLSIZE	"cellsize"
#define NODATA		"nodata_value"
#define CORNER 0
#define CENTER 1

int DBImportASCIINet (DBObjData *netData,const char *fileName)

	{
	FILE *file;
	char buffer [81];
	char nameSTR [DBStringLength];
	DBInt i, j, row, col, rowNum, colNum, cornerType, noData, gridVal;
	DBFloat cellSize;
	DBPosition pos;
	DBCoordinate coord;
	DBRegion extent;
	DBObjTable *basinTable = netData->Table (DBrNItems);
	DBObjTable *cellTable  = netData->Table (DBrNCells);
	DBObjTable *layerTable = netData->Table (DBrNLayers);

	DBObjTableField *mouthPosFLD  = basinTable->Field (DBrNMouthPos);
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

	DBObjRecord *layerRec, *dataRec, *cellRec, *basinRec;
	DBNetworkIO *netIO;

	if ((file = fopen (fileName,"r")) == NULL)
		{ perror ("File Opening Error in: DBImportASCIINet ()"); return (DBFault); }

	for (i = 0;i < 6;++i)
		if (fgets (buffer,sizeof (buffer),file) == (char *) NULL)
			{ perror ("File Reading Error in: DBImportASCIINet ()"); return (DBFault); }
		else
			{
			for (j = 0;(j < (int) strlen (buffer)) && (buffer [j] != ' ');++j)
				buffer [j] = tolower (buffer [j]);
			if (strncmp (buffer,NCOLS,strlen (NCOLS)) == 0)
				sscanf (buffer + strlen (NCOLS),"%d",&colNum);
			if (strncmp (buffer,NROWS,strlen (NROWS)) == 0)
				sscanf (buffer + strlen (NROWS),"%d",&rowNum);
			if (strncmp (buffer,XLLCORNER,strlen (XLLCORNER)) == 0)
				{ sscanf (buffer + strlen (XLLCORNER),"%lf",&(coord.X)); cornerType = CORNER;}
			if (strncmp (buffer,YLLCORNER,strlen (YLLCORNER)) == 0)
				{ sscanf (buffer + strlen (YLLCORNER),"%lf",&(coord.Y)); cornerType = CORNER;}
			if (strncmp (buffer,XLLCENTER,strlen (XLLCENTER)) == 0)
				{ sscanf (buffer + strlen (XLLCENTER),"%lf",&(coord.X)); cornerType = CENTER;}
			if (strncmp (buffer,YLLCENTER,strlen (YLLCENTER)) == 0)
				{ sscanf (buffer + strlen (YLLCENTER),"%lf",&(coord.Y)); cornerType = CENTER;}
			if (strncmp (buffer,CELLSIZE,strlen (CELLSIZE)) == 0)
				sscanf (buffer + strlen (CELLSIZE),"%lf",&cellSize);
			if (strncmp (buffer,NODATA,strlen (NODATA)) == 0)
				sscanf (buffer + strlen (NODATA),"%d",&noData);
			}
	layerTable->Add (DBrNLookupGrid);
	if ((layerRec = layerTable->Item (DBrNLookupGrid)) == (DBObjRecord *) NULL)
		{ fprintf (stderr,"Network Layer Creation Error in: DBImportASCIINet ()\n"); return (DBFault); }
	 printf ("%f %f\n",coord.X,coord.Y);
	cellWidthFLD->Float  (layerRec,cellSize);
	cellHeightFLD->Float (layerRec,cellSize);
	valueTypeFLD->Int (layerRec,DBTableFieldInt);
	valueSizeFLD->Int (layerRec,sizeof (DBInt));
	rowNumFLD->Int (layerRec,rowNum);
	colNumFLD->Int (layerRec,colNum);
	if ((dataRec = new DBObjRecord ("NetLookupGridRecord",rowNum * colNum * sizeof (DBInt),sizeof (DBInt))) == (DBObjRecord *) NULL)
		return (DBFault);
	layerFLD->Record (layerRec,dataRec);
	(netData->Arrays ())->Add (dataRec);
	for (row = rowNum - 1;row >= 0;--row)
		for (col = 0;col < colNum;++col)
			{
			pos.Row = row;
			pos.Col = col;
			if (fscanf (file,"%d", &gridVal) != 1)
				{ perror ("File Reading Error in: DBImportASCIINet ()"); return (DBFault); }
			else
				{
				if (gridVal == noData)
					((DBInt *) dataRec->Data ()) [pos.Row * colNum + pos.Col] = DBFault;
				else
					{
					sprintf (nameSTR,"GHAASCell:%d",cellTable->ItemNum ());
					cellRec = cellTable->Add (nameSTR);
					positionFLD->Position(cellRec,pos);
					toCellFLD->Int			(cellRec,gridVal);
					fromCellFLD->Int		(cellRec,(DBInt) 0);
					orderFLD->Int			(cellRec,(DBInt) 0);
					basinFLD->Int			(cellRec,(DBInt) 0);
					basinCellsFLD->Int	(cellRec,(DBInt) 0);
					travelFLD->Int			(cellRec,(DBInt) 0);
					upCellPosFLD->Position	(cellRec,pos);
					cellAreaFLD->Float	(cellRec,(DBFloat) 0.0);
					subbasinLengthFLD->Float(cellRec,(DBFloat) 0.0);
					subbasinAreaFLD->Float	(cellRec,(DBFloat) 0.0);

					((DBInt *) dataRec->Data ()) [pos.Row * colNum + pos.Col] = cellRec->RowID ();
					}
				}
			}
	sprintf (nameSTR,"GHAASBasin%d",(DBInt) 0);
	basinRec = basinTable->Add (nameSTR);
	mouthPosFLD->Position	(basinRec,positionFLD->Position (cellTable->Item (0)));
	colorFLD->Int				(basinRec,0);
	netData->Precision (cellSize / 25.0);
	coord.X = coord.X - cornerType * cellSize / 2.0;
	coord.Y = coord.Y - cornerType * cellSize / 2.0;
	extent.Expand (coord);
	coord.X += colNum * cellSize;
	coord.Y += rowNum * cellSize;
	extent.Expand (coord);
	netData->Extent (extent);
   netData->Projection (DBMathGuessProjection (extent));
   netData->Precision  (DBMathGuessPrecision  (extent));
	netIO = new DBNetworkIO (netData);
	netIO->Build ();
	delete netIO;
	fclose (file);
	return (DBSuccess);
	}
