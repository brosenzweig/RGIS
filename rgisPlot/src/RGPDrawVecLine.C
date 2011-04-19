/******************************************************************************

GHAAS RiverGIS Plot Utility V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGPDrawVecLine.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<rgisPlot.H>
#include<DBif.H>

DBInt RGPDrawVecLine (DBInt mode, DBInt *entryNum, DBObjData *lineData)

	{
	DBInt ret, vertex, symbolMode, maxVertexNum = 100, lineStyle, lineColor, lineWidth;
	char charBuffer [RGPBufferSIZE], errorMsg [RGPBufferSIZE];
	const char *symbolModes [] = { "default", "uniform", "custom", NULL };
	float *xCoord, *yCoord;
	DBCoordinate nodeCoord, *vertexCoords;
	DBObjRecord *record;
	DBVLineIF *lineIF = new DBVLineIF (lineData);

	cpgqls (&lineStyle);
	cpgqlw (&lineWidth);
	cpgqci (&lineColor);
	
	if ((xCoord = (float *) calloc (maxVertexNum, sizeof (float))) == (float *) NULL)
		{ perror ("Memory Allocation Error in: RGPDrawLine ()"); delete lineIF; return (DBFault); }
	if ((yCoord = (float *) calloc (maxVertexNum, sizeof (float))) == (float *) NULL)
		{ perror ("Memory Allocation Error in: RGPDrawLine ()"); free (xCoord); delete lineIF; return (DBFault); }
	
	do	{
		RGPPrintMessage (mode,entryNum,"Symbol mode [default|uniform|custom]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((symbolMode = CMoptLookup (symbolModes,charBuffer,true)) != DBFault) break;
		sprintf (errorMsg,"Invalid Symbol Mode [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
		} while (true);

	switch (symbolMode)
		{
		default:
		case 0:	break;
		case 1:	break;
		case 2:	break;
		}
	for (record = lineIF->FirstItem ();record != (DBObjRecord *) NULL;record = lineIF->NextItem ())
		{
		switch (lineIF->ItemStyle (record) >> 0x02)
			{
			default:
			case 0:	cpgsls (1);	break;
			case 1:	cpgsls (2);	break;
			case 2:	cpgsls (4); break;
			}
		cpgslw ((lineIF->ItemStyle (record) & 0x03) + 1);
		cpgsci (lineIF->ItemForeground (record));

		if (maxVertexNum < lineIF->VertexNum (record) + 2)
			{
			maxVertexNum = lineIF->VertexNum (record) + 2;
			if ((xCoord = (float *) realloc (xCoord, maxVertexNum * sizeof (float))) == (float *) NULL)
				{ perror ("Memory Reallocation Error in: RGPDrawLine ()"); free (yCoord); delete lineIF; return (DBFault); }
			if ((yCoord = (float *) realloc (yCoord, maxVertexNum * sizeof (float))) == (float *) NULL)
				{ perror ("Memory Reallocation Error in: RGPDrawLine ()"); free (xCoord); delete lineIF; return (DBFault); }
			}
		vertex = 0;
		nodeCoord = lineIF->FromCoord (record);
		xCoord [vertex] = nodeCoord.X;
		yCoord [vertex] = nodeCoord.Y;
		vertex++;
		if (lineIF->VertexNum (record) > 0)
			{
			vertexCoords = lineIF->Vertexes (record);
			while (vertex - 1 < lineIF->VertexNum (record))
				{
				xCoord [vertex] = vertexCoords [vertex - 1].X;
				yCoord [vertex] = vertexCoords [vertex - 1].Y;
				vertex++;
				}
			}
		nodeCoord = lineIF->ToCoord (record);
		xCoord [vertex] = nodeCoord.X;
		yCoord [vertex] = nodeCoord.Y;
		vertex++;
		cpgline (vertex,xCoord,yCoord);
		}

Stop:
	cpgsls (lineStyle);
	cpgslw (lineWidth);
	cpgsci (lineColor);
	free (xCoord), free (yCoord);
	delete lineIF;
	return (ret);
	}
