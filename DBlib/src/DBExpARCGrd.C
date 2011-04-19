/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBExpARCGrd.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

int DBExportARCGridLayer (DBObjData *data,DBObjRecord *layerRec,const char *fileName)

	{
	FILE *file;
	DBInt ret;

	if ((file = fopen (fileName,"w")) == (FILE *) NULL)
		{ perror ("File Opening Error in: DBExportARCGridLayer ()"); return (DBFault); }
	ret = DBExportARCGridLayer (data,layerRec,file);
	fclose (file);
	return (ret);
	}

int DBExportARCGridLayer (DBObjData *data,DBObjRecord *layerRec,FILE *file)

	{
	DBInt row, col;
	DBPosition pos;
	DBGridIO *gridIO = new DBGridIO (data);

	fprintf (file,"ncols         %d\n",gridIO->ColNum ());
	fprintf (file,"nrows         %d\n",gridIO->RowNum ());
	fprintf (file,"xllcorner     %f\n",(data->Extent ()).LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",(data->Extent ()).LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",gridIO->CellWidth ());
	if (data->Type () == DBTypeGridContinuous)
		{
		DBFloat value;
		fprintf (file,"NODATA_value  %f\n",gridIO->MissingValue ());
		for (row = gridIO->RowNum () - 1;row >= 0;row--)
			{
			for (col = 0;col < gridIO->ColNum ();col++)
				{
				pos.Row = row;
				pos.Col = col;
				if (gridIO->Value (layerRec,pos,&value))	fprintf (file," %f",value);
				else	fprintf (file," %f",gridIO->MissingValue ());
				}
			fprintf (file,"\n");
			}
		}
	else
		{
		fprintf (file,"NODATA_value  %d\n",DBDefaultMissingIntVal);
		for (row = gridIO->RowNum () - 1;row >= 0;row--)
			{
			for (col = 0;col < gridIO->ColNum ();col++)
				{
				pos.Row = row;
				pos.Col = col;
				fprintf (file," %d",gridIO->GridValue (layerRec,pos));
				}
			fprintf (file,"\n");
			}
		}
	delete gridIO;
	return (DBSuccess);
	}
