/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISEditLine.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

void RGISEditLineDirectionCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBFloat elev0, elev1;
	DBCoordinate coord;
	DBObjRecord *lineRec;
	DBDataset *dataset = UIDataset ();
	DBObjData *lineData = dataset->Data ();
	DBObjData *gridData = lineData->LinkedData ();
	DBVLineIF *lineIF = new DBVLineIF (lineData);
	DBGridIF *gridIF = new DBGridIF (gridData);

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Changing Directionality");
	for (lineRec = lineIF->FirstItem ();lineRec != (DBObjRecord *) NULL;lineRec = lineIF->NextItem ())
		{
		DBPause (lineRec->RowID () * 100 / lineIF->ItemNum ());
		coord = lineIF->FromCoord (lineRec);
		if (gridIF->Value (coord,&elev0) == false) continue;
		coord = lineIF->ToCoord (lineRec);
		if (gridIF->Value (coord,&elev1) == false) continue;
		if (elev0 < elev1) lineIF->Flip (lineRec);
		}
	UIPauseDialogClose ();
	}
