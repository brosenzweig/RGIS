/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBExpARCGrd.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

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
	DBGridIF *gridIF = new DBGridIF (data);

	fprintf (file,"ncols         %d\n",gridIF->ColNum ());
	fprintf (file,"nrows         %d\n",gridIF->RowNum ());
	fprintf (file,"xllcorner     %f\n",(data->Extent ()).LowerLeft.X);
	fprintf (file,"yllcorner     %f\n",(data->Extent ()).LowerLeft.Y);
	fprintf (file,"cellsize      %f\n",gridIF->CellWidth ());
	if (data->Type () == DBTypeGridContinuous)
		{
		DBFloat value;
		fprintf (file,"NODATA_value  %f\n",gridIF->MissingValue ());
		for (row = gridIF->RowNum () - 1;row >= 0;row--)
			{
			for (col = 0;col < gridIF->ColNum ();col++)
				{
				pos.Row = row;
				pos.Col = col;
				if (gridIF->Value (layerRec,pos,&value))	fprintf (file," %f",value);
				else	fprintf (file," %f",gridIF->MissingValue ());
				}
			fprintf (file,"\n");
			}
		}
	else
		{
		fprintf (file,"NODATA_value  %d\n",DBDefaultMissingIntVal);
		for (row = gridIF->RowNum () - 1;row >= 0;row--)
			{
			for (col = 0;col < gridIF->ColNum ();col++)
				{
				pos.Row = row;
				pos.Col = col;
				fprintf (file," %d",gridIF->GridValue (layerRec,pos));
				}
			fprintf (file,"\n");
			}
		}
	delete gridIF;
	return (DBSuccess);
	}
