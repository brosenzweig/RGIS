/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISToolsPntExp.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <ctype.h>
#include <rgis.H>

int _RGISToolsPointExportARCInfo (DBObjData *data,char *selection)

	{
	FILE *file;
	DBInt pos;
	char coverName	[11], command    [FILENAME_MAX + 20]; 
	char asciiGenerate   [FILENAME_MAX];
	char asciiAttribDef  [FILENAME_MAX];
	char asciiAttribData [FILENAME_MAX];
	char amlFile    [FILENAME_MAX];

	sprintf (asciiGenerate,"%s/asciigen.tmp",selection);
	if (DBExportARCGenPoint (data,asciiGenerate) == DBFault) return (DBFault);
	sprintf (asciiAttribDef,"%s/asciiattrib.def",selection);
	if (DBExportARCTableDef (data,DBrNItems,asciiAttribDef) == DBFault)
		{ unlink (asciiGenerate); return (DBFault); }
	sprintf (asciiAttribData,"%s/asciiattrib.dat",selection);
	if (DBExportARCTableData (data,DBrNItems,asciiAttribData) == DBFault)
		{ unlink (asciiGenerate); unlink (asciiAttribDef); return (DBFault); }

	for (pos = strlen (data->FileName ());pos > 0;pos--) if ((data->FileName ()) [pos] == '/') break;
	strncpy (coverName,(data->FileName ()) + pos + 1,sizeof (coverName) - 1);
	coverName	[sizeof (coverName) - 1] = '\0';
	for (pos = 0;pos < (DBInt) strlen (coverName);++pos) if (coverName [pos] == '.') coverName [pos] = '\0';
	for (pos = 0;pos < (DBInt) strlen (coverName);++pos) coverName [pos] = tolower (coverName [pos]);
	sprintf (amlFile,"%s/pntcreate.aml",selection);
	if ((file = fopen (amlFile,"w")) == NULL)
		{
		unlink (asciiGenerate);
		unlink (asciiAttribDef);
		unlink (asciiAttribData);
		perror ("Aml File Creation Error in: RGISToolsPointExportARCInfo ()");
		return (DBFault);
		}
	fprintf (file,"&workspace %s\n",selection);
	fprintf (file,"generate c_%s\n",coverName);
	fprintf (file,"input %s\n",asciiGenerate);
	fprintf (file,"points\n");
	fprintf (file,"quit\n");
	fprintf (file,"build c_%s point\n",coverName);
	fprintf (file,"tables\n");
	fprintf (file,"define c_%s.pattr\n",coverName);
	fprintf (file,"&r %s\n",asciiAttribDef);
	fprintf (file,"~\n");
	fprintf (file,"alter id\n");
	fprintf (file,"c_%s-id,,,,\n",coverName);
	fprintf (file,"add\n");
	fprintf (file,"&r %s\n",asciiAttribData);
	fprintf (file,"~\n");
	fprintf (file,"q stop\n");
	fprintf (file,"joinitem c_%s.pat c_%s.pattr c_%s.pat c_%s-id c_%s-id\n",coverName,coverName,coverName,coverName,coverName);
	fprintf (file,"addxy C_%s point\n",coverName); 
	fprintf (file,"tables\n");
	fprintf (file,"select c_%s.pattr\n",coverName);
	fprintf (file,"erase c_%s.pattr\ny\n",coverName);
	fprintf (file,"q stop\n");
	fclose (file);

	if (getenv ("GHAAS_ARC") != NULL)
		{
		sprintf (command,getenv ("GHAAS_ARC"),amlFile);
		system (command);
		unlink (asciiGenerate);
		unlink (asciiAttribDef);
		unlink (asciiAttribData);
		unlink (amlFile);
		}
	return (DBSuccess);
	}
