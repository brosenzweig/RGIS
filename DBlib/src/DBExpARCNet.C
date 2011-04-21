/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBExpARCNet.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

DBInt DBNetworkExportASCIIGridDir (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIF *netIF = new DBNetworkIF (netData);

	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	fprintf (file,"ncols         %d\n",netIF->ColNum ());
	fprintf (file,"nrows         %d\n",netIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %d\n",DBFault);

	for (pos.Row = netIF->RowNum () - 1;pos.Row >= 0; --pos.Row)
		{
		for (pos.Col = 0; pos.Col < netIF->ColNum (); ++pos.Col)
			fprintf (file," %d",netIF->ToCellDir (netIF->Cell (pos)));
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridBasin (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIF *netIF = new DBNetworkIF (netData);

	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	fprintf (file,"ncols         %d\n",netIF->ColNum ());
	fprintf (file,"nrows         %d\n",netIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %d\n",DBFault);

	for (pos.Row = netIF->RowNum () - 1;pos.Row >= 0; --pos.Row)
		{
		for (pos.Col = 0; pos.Col < netIF->ColNum (); ++pos.Col)
			fprintf (file," %d",netIF->CellBasinID (netIF->Cell (pos)));
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridOrder (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIF *netIF = new DBNetworkIF (netData);

	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	fprintf (file,"ncols         %d\n",netIF->ColNum ());
	fprintf (file,"nrows         %d\n",netIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %d\n",DBFault);

	for (pos.Row = netIF->RowNum () - 1;pos.Row >= 0; --pos.Row)
		{
		for (pos.Col = 0; pos.Col < netIF->ColNum (); ++pos.Col)
			fprintf (file," %d",netIF->CellOrder (netIF->Cell (pos)));
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridArea (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBObjRecord *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	fprintf (file,"ncols         %d\n",netIF->ColNum ());
	fprintf (file,"nrows         %d\n",netIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (pos.Row = netIF->RowNum () - 1;pos.Row >= 0; --pos.Row)
		{
		for (pos.Col = 0; pos.Col < netIF->ColNum (); ++pos.Col)
			if ((cellRec = netIF->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIF->CellBasinArea (cellRec));
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridLength (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBObjRecord *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	fprintf (file,"ncols         %d\n",netIF->ColNum ());
	fprintf (file,"nrows         %d\n",netIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (pos.Row = netIF->RowNum () - 1;pos.Row >= 0; --pos.Row)
		{
		for (pos.Col = 0; pos.Col < netIF->ColNum (); ++pos.Col)
			if ((cellRec = netIF->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIF->CellBasinLength (cellRec));
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridDistToMouth (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBObjRecord *cellRec;

	if (netIF->DistToMouth () != true) return (DBFault);
	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	fprintf (file,"ncols         %d\n",netIF->ColNum ());
	fprintf (file,"nrows         %d\n",netIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (pos.Row = netIF->RowNum () - 1;pos.Row >= 0; --pos.Row)
		{
		for (pos.Col = 0; pos.Col < netIF->ColNum (); ++pos.Col)
			if ((cellRec = netIF->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIF->CellDistToMouth (cellRec));
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportASCIIGridDistToOcean (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBPosition pos;
	DBCoordinate cellSize;
	DBRegion extent = netData->Extent ();
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBObjRecord *cellRec;

	if (netIF->DistToOcean () != true) return (DBFault);
	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	fprintf (file,"ncols         %d\n",netIF->ColNum ());
	fprintf (file,"nrows         %d\n",netIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",extent.LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",extent.LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",cellSize.X);
	fprintf (file,"NODATA_value  %f\n",-9999.0);

	for (pos.Row = netIF->RowNum () - 1;pos.Row >= 0; --pos.Row)
		{
		for (pos.Col = 0; pos.Col < netIF->ColNum (); ++pos.Col)
			if ((cellRec = netIF->Cell (pos)) == (DBObjRecord *) NULL)	fprintf (file," %f",-9999.0);
			else	fprintf (file," %f",netIF->CellDistToOcean (cellRec));
		fprintf (file,"\n");
		}
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportArcGenARC (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt cellID;
	DBFloat length;
	DBCoordinate coord, delta, cellSize;
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBObjRecord *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	cellSize = netIF->CellSize ();

	for (cellID = 0;cellID < netIF->CellNum ();++cellID)
		{
		cellRec = netIF->Cell (cellID);
		fprintf (file,"%8d\n", cellRec->RowID ());
		coord = netIF->Center (cellRec);
		fprintf (file,"%g %g\n",coord.X,coord.Y);
		delta = netIF->Delta (cellRec);
		length = sqrt (delta.X * delta.X + delta.Y * delta.Y);
		if (length < pow ((double) 10.0,(double) netData->Precision ()))
			delta.X = delta.Y = pow ((double) 10.0,(double) netData->Precision ());
		coord = coord + delta;
		fprintf (file,"%g %g\n",coord.X,coord.Y);
		fprintf (file,"END\n");
		}
	fprintf (file,"END\n");
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}

DBInt DBNetworkExportArcGenNODE (DBObjData *netData,char *fileName)

	{
	FILE *file;
	DBInt basinID;
	DBCoordinate coord;
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBObjRecord *basinRec, *cellRec;

	if ((file = fopen (fileName,"w")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }

	for (basinID = 0;basinID < netIF->BasinNum ();++basinID)
		{
		basinRec = netIF->Basin (basinID);
		cellRec = netIF->MouthCell (basinRec);
		coord = netIF->Center (cellRec) + netIF->Delta (cellRec);
		fprintf (file,"%8d %g %g\n",basinRec->RowID (),coord.X,coord.Y);
		}
	fprintf (file,"END\n");
	fclose (file);
	delete netIF;
	return (DBSuccess);
	}
