/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

DBExpARCNet.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

DBInt DBNetworkExportASCIIGridDir (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt row, col;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportASCIIGridDir ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	fprintf (file,"ncols         %d\n",netIO->ColNum ());
	fprintf (file,"nrows         %d\n",netIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %d\n",DBFault);

	for (row = netIO->RowNum () - 1;row >= 0; --row)
		{
		for (col = 0; col < netIO->ColNum (); ++col)
			{
			pos.Row = row;
			pos.Col = col;
			fprintf (file," %d",netIO->ToCellDir (netIO->Cell (pos)));
			}
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridBasin (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt row, col;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportASCIIGridBasin ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	fprintf (file,"ncols         %d\n",netIO->ColNum ());
	fprintf (file,"nrows         %d\n",netIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %d\n",DBFault);

	for (row = netIO->RowNum () - 1;row >= 0; --row)
		{
		for (col = 0; col < netIO->ColNum (); ++col)
			{
			pos.Row = row;
			pos.Col = col;
			fprintf (file," %d",netIO->CellBasinID (netIO->Cell (pos)));
			}
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridOrder (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt row, col;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportASCIIGridOrder ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	fprintf (file,"ncols         %d\n",netIO->ColNum ());
	fprintf (file,"nrows         %d\n",netIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %d\n",DBFault);

	for (row = netIO->RowNum () - 1;row >= 0; --row)
		{
		for (col = 0; col < netIO->ColNum (); ++col)
			{
			pos.Row = row;
			pos.Col = col;
			fprintf (file," %d",netIO->CellOrder (netIO->Cell (pos)));
			}
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridArea (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt row, col;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportASCIIGridArea ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	fprintf (file,"ncols         %d\n",netIO->ColNum ());
	fprintf (file,"nrows         %d\n",netIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (row = netIO->RowNum () - 1;row >= 0; --row)
		{
		for (col = 0; col < netIO->ColNum (); ++col)
			{
			pos.Row = row;
			pos.Col = col;
			if ((cellRec = netIO->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIO->CellBasinArea (cellRec));
			}
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridLength (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt row, col;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportASCIIGridLength ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	fprintf (file,"ncols         %d\n",netIO->ColNum ());
	fprintf (file,"nrows         %d\n",netIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (row = netIO->RowNum () - 1;row >= 0; --row)
		{
		for (col = 0; col < netIO->ColNum (); ++col)
			{
			pos.Row = row;
			pos.Col = col;
			if ((cellRec = netIO->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIO->CellBasinLength (cellRec));
			}
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridDistToMouth (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt row, col;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *cellRec;

	if (netIO->DistToMouth () != true) return (DBFault);
	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportASCIIGridLength ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	fprintf (file,"ncols         %d\n",netIO->ColNum ());
	fprintf (file,"nrows         %d\n",netIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (row = netIO->RowNum () - 1;row >= 0; --row)
		{
		for (col = 0; col < netIO->ColNum (); ++col)
			{
			pos.Row = row;
			pos.Col = col;
			if ((cellRec = netIO->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIO->CellDistToMouth (cellRec));
			}
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridDistToOcean (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt row, col;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *cellRec;

	if (netIO->DistToOcean () != true) return (DBFault);
	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportASCIIGridLength ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	fprintf (file,"ncols         %d\n",netIO->ColNum ());
	fprintf (file,"nrows         %d\n",netIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (row = netIO->RowNum () - 1;row >= 0; --row)
		{
		for (col = 0; col < netIO->ColNum (); ++col)
			{
			pos.Row = row;
			pos.Col = col;
			if ((cellRec = netIO->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIO->CellDistToOcean (cellRec));
			}
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportArcGenARC (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt cellID;
	DBFloat length;
	DBCoordinate coord, delta, cellSize;
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportArcGenARC ()"); return (DBFault); }
	cellSize = netIO->CellSize ();

	for (cellID = 0;cellID < netIO->CellNum ();++cellID)
		{
		cellRec = netIO->Cell (cellID);
		fprintf (file,"%8d\n", cellRec->RowID ());
		coord = netIO->Center (cellRec);
		fprintf (file,"%g %g\n",coord.X,coord.Y);
		delta = netIO->Delta (cellRec);
		length = sqrt (delta.X * delta.X + delta.Y * delta.Y);
		if (length < pow ((double) 10.0,(double) netData->Precision ()))
			delta.X = delta.Y = pow ((double) 10.0,(double) netData->Precision ());
		coord = coord + delta;
		fprintf (file,"%g %g\n",coord.X,coord.Y);
		fprintf (file,"END\n");
		}
	fprintf (file,"END\n");
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}

DBInt DBNetworkExportArcGenNODE (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt basinID;
	DBCoordinate coord;
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *basinRec, *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBNetworkExportArcGenNODE ()"); return (DBFault); }

	for (basinID = 0;basinID < netIO->BasinNum ();++basinID)
		{
		basinRec = netIO->Basin (basinID);
		cellRec = netIO->MouthCell (basinRec);
		coord = netIO->Center (cellRec) + netIO->Delta (cellRec);
		fprintf (file,"%8d %g %g\n",basinRec->RowID (),coord.X,coord.Y);
		}
	fprintf (file,"END\n");
	fclose (file);
	delete netIO;
	return (DBSuccess);
	}
