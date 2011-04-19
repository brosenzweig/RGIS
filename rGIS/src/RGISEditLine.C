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
	DBVLineIO *lineIO = new DBVLineIO (lineData);
	DBGridIO *gridIO = new DBGridIO (gridData);

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Changing Directionality");
	for (lineRec = lineIO->FirstItem ();lineRec != (DBObjRecord *) NULL;lineRec = lineIO->NextItem ())
		{
		DBPause (lineRec->RowID () * 100 / lineIO->ItemNum ());
		coord = lineIO->FromCoord (lineRec);
		if (gridIO->Value (coord,&elev0) == false) continue;
		coord = lineIO->ToCoord (lineRec);
		if (gridIO->Value (coord,&elev1) == false) continue;
		if (elev0 < elev1) lineIO->Flip (lineRec);
		}
	UIPauseDialogClose ();
	}
