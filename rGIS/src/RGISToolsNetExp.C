/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGISToolsNetExp.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <ctype.h>
#include <rgis.H>

DBInt DBNetworkExportASCIIGridDir		(DBObjData *,char *);
DBInt DBNetworkExportASCIIGridBasin		(DBObjData *,char *); 
DBInt DBNetworkExportASCIIGridOrder		(DBObjData *,char *); 
DBInt DBNetworkExportASCIIGridArea		(DBObjData *,char *); 
DBInt DBNetworkExportASCIIGridLength	(DBObjData *,char *); 
DBInt DBNetworkExportASCIIGridDistToMouth	(DBObjData *,char *); 
DBInt DBNetworkExportASCIIGridDistToOcean	(DBObjData *,char *); 
DBInt DBNetworkExportArcGenARC			(DBObjData *,char *);
DBInt DBNetworkExportArcGenNODE			(DBObjData *,char *);
DBInt DBNetworkExportArcAttributes		(DBObjData *,char *);
DBInt DBNetworkExportBasinAttributes	(DBObjData *,char *);
DBInt DBNetworkExportArcGenMouse			(DBObjData *,char *);

int _RGISToolsNetworkExportARCInfo (DBObjData *netData,char *selection)

	{
	FILE *file;
	char command    [256]; 
	char asciiGridNet		[FILENAME_MAX]; 
	char asciiGridBasin	[FILENAME_MAX]; 
	char asciiGridOrder	[FILENAME_MAX]; 
	char asciiGridArea	[FILENAME_MAX]; 
	char asciiGridLength	[FILENAME_MAX]; 
	char asciiGridDistToMouth [FILENAME_MAX]; 
	char asciiGridDistToOcean [FILENAME_MAX]; 
	char arcGenArc  		[FILENAME_MAX]; 
	char arcGenNode 		[FILENAME_MAX]; 
	char arcAttribDef		[FILENAME_MAX]; 
	char arcAttribData	[FILENAME_MAX]; 
	char nodeAttribDef	[FILENAME_MAX]; 
	char nodeAttribData	[FILENAME_MAX]; 
	char amlFile    		[FILENAME_MAX]; 

	sprintf (asciiGridNet,"%s/asciigrid.tmp",selection);
	if (DBNetworkExportASCIIGridDir (netData,asciiGridNet) == DBFault) return (DBFault);

	sprintf (asciiGridBasin,"%s/asciibasin.tmp",selection);
	if (DBNetworkExportASCIIGridBasin (netData,asciiGridBasin) == DBFault)
		{
		unlink (asciiGridNet);
		return (DBFault);
		}

	sprintf (asciiGridOrder,"%s/asciiorder.tmp",selection);
	if (DBNetworkExportASCIIGridOrder (netData,asciiGridOrder) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		return (DBFault);
		}

	sprintf (asciiGridArea,"%s/asciiarea.tmp",selection);
	if (DBNetworkExportASCIIGridArea (netData,asciiGridArea) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		return (DBFault);
		}

	sprintf (asciiGridLength,"%s/asciilength.tmp",selection);
	if (DBNetworkExportASCIIGridLength (netData,asciiGridLength) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		return (DBFault);
		}

	sprintf (asciiGridDistToMouth,"%s/asciidist2mouth.tmp",selection);
	if (DBNetworkExportASCIIGridDistToMouth (netData,asciiGridDistToMouth) == DBFault)
		strcpy (asciiGridDistToMouth,"");

	sprintf (asciiGridDistToOcean,"%s/asciidist2ocean.tmp",selection);
	if (DBNetworkExportASCIIGridDistToOcean (netData,asciiGridDistToOcean) == DBFault)
		strcpy (asciiGridDistToOcean,"");

	sprintf (arcGenArc,"%s/arcgenarc.tmp",selection);
	if (DBNetworkExportArcGenARC (netData,arcGenArc) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		return (DBFault);
		}

	sprintf (arcGenNode,"%s/arcgennode.tmp",selection);
	if (DBNetworkExportArcGenNODE (netData,arcGenNode) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		unlink (arcGenArc);
		return (DBFault);
		}

	sprintf (arcAttribDef,"%s/arc_def.tmp",selection);
	if (DBExportARCTableDef (netData,DBrNCells,arcAttribDef) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		unlink (arcGenArc);
		unlink (arcGenNode);
		return (DBFault);
		}

	sprintf (arcAttribData,"%s/arc_data.tmp",selection);
	if (DBExportARCTableData (netData,DBrNCells,arcAttribData) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		unlink (arcGenArc);
		unlink (arcGenNode);
		unlink (arcAttribDef);
		return (DBFault);
		}
	sprintf (nodeAttribDef,"%s/node_def.tmp",selection);
	if (DBExportARCTableDef (netData,DBrNItems,nodeAttribDef) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		unlink (arcGenArc);
		unlink (arcGenNode);
		unlink (arcAttribDef);
		unlink (arcAttribData);
		return (DBFault);
		}
	sprintf (nodeAttribData,"%s/node_data.tmp",selection);
	if (DBExportARCTableData (netData,DBrNItems,nodeAttribData) == DBFault)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		unlink (arcGenArc);
		unlink (arcGenNode);
		unlink (arcAttribDef);
		unlink (arcAttribData);
		unlink (nodeAttribDef);
		return (DBFault);
		}

	sprintf (amlFile,"%s/netcreate.aml",selection);
	if ((file = fopen (amlFile,"w")) == NULL)
		{
		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		unlink (arcGenArc);
		unlink (arcGenNode);
		unlink (arcAttribDef);
		unlink (arcAttribData);
		unlink (nodeAttribData);
		unlink (nodeAttribDef);
		perror ("File Creation Error in: _RGISToolsNetworkExportARCInfo ()"); 
		return (DBFault);
		}

	fprintf (file,"&workspace %s\n",selection);
	fprintf (file,"asciigrid %s g_network\n",asciiGridNet);
	fprintf (file,"asciigrid %s g_basin\n",asciiGridBasin);
	fprintf (file,"asciigrid %s g_order\n",asciiGridOrder);
	fprintf (file,"asciigrid %s g_cumularea float\n",asciiGridArea);
	fprintf (file,"asciigrid %s g_length float\n",asciiGridLength);
	if (strlen (asciiGridDistToMouth) > 1)
		fprintf (file,"asciigrid %s g_distmouth float\n",asciiGridDistToMouth);
	if (strlen (asciiGridDistToOcean) > 1)
		fprintf (file,"asciigrid %s g_distocean float\n",asciiGridDistToOcean);

	fprintf (file,"generate c_network\n");
	fprintf (file,"input %s\nlines\n",arcGenArc);
	fprintf (file,"input %s\npoints\n",arcGenNode);
	fprintf (file,"quit\n");
	fprintf (file,"build c_network arc\n");
	fprintf (file,"build c_network node\n");
	fprintf (file,"build c_network point\n");
	fprintf (file,"gridpoly g_basin c_basin\n");
	fprintf (file,"build c_basin arc\n");
	fprintf (file,"build c_basin node\n");

	fprintf (file,"tables\n");
	fprintf (file,"define c_network.aattr\n");
	fprintf (file,"&r %s\n",arcAttribDef);
	fprintf (file,"~\n");
	fprintf (file,"add\n");
	fprintf (file,"&r %s\n",arcAttribData);
	fprintf (file,"~\n");

	fprintf (file,"define c_network.pattr\n");
	fprintf (file,"&r %s\n",nodeAttribDef);
	fprintf (file,"~\n");
	fprintf (file,"alter\n");
	fprintf (file,"ID basinID,,,,\n");
	fprintf (file,"~\n");
	fprintf (file,"add\n");
	fprintf (file,"&r %s\n",nodeAttribData);
	fprintf (file,"~\n");

	fprintf (file,"additem G_BASIN.VAT row 4 8 b\n");
	fprintf (file,"select G_BASIN.VAT\n");
	fprintf (file,"calculate row = value + 1\n");
	fprintf (file,"select C_BASIN.PAT\n");
	fprintf (file,"alter\n");
	fprintf (file,"grid-code basinID,,,,\n");
	fprintf (file,"~\n");
	
	fprintf (file,"q stop\n");

	fprintf (file,"joinitem C_NETWORK.AAT C_NETWORK.AATTR C_NETWORK.AAT c_network# c_network-id link\n");
	fprintf (file,"joinitem C_NETWORK.PAT C_NETWORK.PATTR C_NETWORK.PAT c_network# c_network-id link\n");
	fprintf (file,"joinitem G_BASIN.VAT   C_NETWORK.PATTR G_BASIN.VAT   VALUE      count        link\n");
	fprintf (file,"joinitem C_BASIN.PAT   C_NETWORK.PATTR C_BASIN.PAT   BASINID      BASINID\n");
/*	fprintf (file,"arcedit\nec c_network\nef arc\n");
	fprintf (file,"select LENGTH < %f\ndelete\n",pow ((double) 10.0,(double) netData->Precision ()) * 2.0);
	fprintf (file,"save\nq\n");
*/	fprintf (file,"tables\n");
	fprintf (file,"dropitem G_BASIN.VAT row\n");
	fprintf (file,"select C_NETWORK.AATTR\nerase C_NETWORK.AATTR\ny\n");
	fprintf (file,"select C_NETWORK.PATTR\nerase C_NETWORK.PATTR\ny\n");
	fprintf (file,"q stop\n");
	fprintf (file,"addxy C_NETWORK point\n"); 
	fclose  (file);

	if (getenv ("GHAAS_ARC") != NULL)
		{
		sprintf (command,getenv ("GHAAS_ARC"),amlFile);

		system (command);

		unlink (asciiGridNet);
		unlink (asciiGridBasin);
		unlink (asciiGridOrder);
		unlink (asciiGridArea);
		unlink (asciiGridLength);
		if (strlen (asciiGridDistToMouth) > 1) unlink (asciiGridDistToMouth);
		if (strlen (asciiGridDistToOcean) > 1) unlink (asciiGridDistToOcean);
		unlink (arcGenArc);
		unlink (arcGenNode);
		unlink (arcAttribDef);
		unlink (arcAttribData);
		unlink (nodeAttribData);
		unlink (nodeAttribDef);
		unlink (amlFile);
		}
	return (DBSuccess);
	}
