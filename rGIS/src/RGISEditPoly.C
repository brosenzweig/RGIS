/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISEditPoly.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

void RGISEditPolyFourColorCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBVPolyIO *polyIO = new DBVPolyIO (dataset->Data ());
	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ("Four Coloring Polygons");
	polyIO->FourColoring ();
	UIPauseDialogClose ();
	}
