/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISToolsGrdExp.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <ctype.h>
#include <rgis.H>

int _RGISToolsGridExportARCInfo (DBObjData *data,char *selection)

	{
	DBInt layerID, i;
	FILE *file;
	char coverName	[11], command    [256]; 
	char asciiGrid   		[FILENAME_MAX];
	char attribDef   		[FILENAME_MAX];
	char attribData   	[FILENAME_MAX];
	char amlFile    [FILENAME_MAX];
	DBGridIO *gridIO = new DBGridIO (data);
	DBObjRecord *layerRec;

	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		sprintf (asciiGrid,"%s/asciigrid%d.tmp",selection,layerID);
		if (DBExportARCGridLayer (data,layerRec,asciiGrid) == DBFault) return (DBFault);
		}
	if (data->Type () == DBTypeGridDiscrete)
		{
		sprintf (attribDef,"%s/attrib.def",selection);
		if (DBExportARCTableDef (data,DBrNItems,attribDef) == DBFault)
			{ unlink (asciiGrid); return (DBFault); }
		sprintf (attribData,"%s/attrib.dat",selection);
		if (DBExportARCTableData (data,DBrNItems,attribData) == DBFault)
			{ unlink (asciiGrid); unlink (attribDef); return (DBFault); }
		}
	
	sprintf (amlFile,"%s/grdcreate.aml",selection);
	if ((file = fopen (amlFile,"w")) == NULL)
		{
		for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
			{
			layerRec = gridIO->Layer (layerID);
			sprintf (asciiGrid,"%s/asciigrid%d.tmp",selection,layerID);
			unlink (asciiGrid);
			}
		if (data->Type () == DBTypeGridDiscrete) { unlink (attribDef); unlink (attribData); }
		fprintf (stderr,"Aml File Creation Error in: RGISToolsPointExportARCInfo ()\n");
		return (DBFault);
		}
	fprintf (file,"&workspace %s\n",selection);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		strncpy (coverName,layerRec->Name (),sizeof (coverName) - 1);
		coverName	[sizeof (coverName) - 1] = '\0';
		for (i = 0;i < (DBInt) strlen (coverName);++i) if (coverName [i] == '.') coverName [i] = '\0';
		for (i = 0;i < (DBInt) strlen (coverName);++i) coverName [i] = tolower (coverName [i]);
		sprintf (asciiGrid,"%s/asciigrid%d.tmp",selection,layerID);
		if (data->Type () == DBTypeGridContinuous)
			fprintf (file,"asciigrid %s g_%s float\n",asciiGrid,coverName);
		else
			fprintf (file,"asciigrid %s g_%s int\n",asciiGrid,coverName);
		if (data->Type () == DBTypeGridDiscrete)
			{
			fprintf (file,"tables\n");
			fprintf (file,"define g_%s.vattr\n",coverName);
			fprintf (file,"&r %s\n",attribDef);
			fprintf (file,"~\n");
			fprintf (file,"alter GridValue\n");
			fprintf (file,"value,,,,\n");
			fprintf (file,"add\n");
			fprintf (file,"&r %s\n",attribData);
			fprintf (file,"~\n");
			fprintf (file,"q stop\n");
			fprintf (file,"joinitem g_%s.vat g_%s.vattr g_%s.vat value count\n",coverName,coverName,coverName);
			fprintf (file,"tables\n");
			fprintf (file,"select g_%s.vattr\n",coverName);
			fprintf (file,"erase g_%s.vattr\ny\n",coverName);
			fprintf (file,"q stop\n");
			}
		}
	fclose (file);

	if (getenv ("GHAAS_ARC") != NULL)
		{
		sprintf (command,getenv ("GHAAS_ARC"),amlFile);

		system (command);
		for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
			{
			layerRec = gridIO->Layer (layerID);
			sprintf (asciiGrid,"%s/asciigrid%d.tmp",selection,layerID);
			unlink (asciiGrid);
			}
		if (data->Type () == DBTypeGridDiscrete) { unlink (attribDef); unlink (attribData); }
		unlink (amlFile);
		}
	return (DBSuccess);
	}
	
