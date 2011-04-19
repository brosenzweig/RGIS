/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISEditPoly.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

void RGISEditPolyFourColorCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBVPolyIF *polyIF = new DBVPolyIF (dataset->Data ());
	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Four Coloring Polygons");
	polyIF->FourColoring ();
	UIPauseDialogClose ();
	}
