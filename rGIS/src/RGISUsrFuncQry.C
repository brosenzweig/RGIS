/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGISUsrFuncQry.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Xm.h>
#include <rgis.H>

void _RGISUserFuncionQuery (DBObjData *dbData,UI2DView *view,XEvent *event)

	{
	DBInt sX, sY;
	DBCoordinate coord;
	DBObjData *data = dbData;
	UIAttribView *attribView;

	if (event->type != ButtonPress) return;
	sX = event->xbutton.x;
	sY = event->xbutton.y;
	view->Window2Map  (sX,sY, &(coord.X), &(coord.Y));

	for (data = dbData;
		  (data != (DBObjData *) NULL) && ((data->Flags () & DBObjectFlagProcessed) != DBObjectFlagProcessed);
		  data = data->LinkedData ())
		{
		data->Flags (DBObjectFlagProcessed,DBSet);
		if ((data->Flags () & DBDataFlagUserModeFlags) == DBDataFlagUserModeQuery)
			{
			if ((attribView = (UIAttribView *) data->Display (UIAttribViewSTR)) == (UIAttribView *) NULL)
				data->DispAdd (attribView = new UIAttribView (data));
			switch (data->Type ())
				{
				case DBTypeVectorPoint:
				case DBTypeVectorLine:
				case DBTypeVectorPolygon:
					{
					DBVectorIO *vectorIO = new DBVectorIO (data);
					attribView->Draw (vectorIO->Item (coord));
					} break;
				case DBTypeGridDiscrete:
					{
					DBGridIO *gridIO = new DBGridIO (data);
					attribView->Draw (gridIO->GridItem (coord));
					delete gridIO;
					} break;
				case DBTypeGridContinuous:
					{
					DBPosition pos;
					DBGridIO *gridIO = new DBGridIO (data);
					char nameString [DBStringLength * 2];
					DBObjRecord *record = new DBObjRecord ("Noname",sizeof (DBCoordinate));
					gridIO->Coord2Pos (coord,pos);
					gridIO->Pos2Coord (pos,coord);
					sprintf (nameString,"CellCoordinate %10.2f%10.2f",coord.X,coord.Y);
					record->Name (nameString);
					memcpy (record->Data (),(void *) &coord,sizeof (coord));
					attribView->Draw (record);
					delete record;
					delete gridIO;
					} break;
				case DBTypeNetwork:
					{
					DBNetworkIO *netIO = new DBNetworkIO (data);
					attribView->Draw (netIO->Cell (coord));
					delete netIO;
					} break;
				default:
					fprintf (stderr,"Invalid Data Type in: _RGISUserFuncionQuery ()\n"); break;
				}
			}
		}
	for (data = dbData;
		  (data != (DBObjData *) NULL) && ((data->Flags () & DBObjectFlagProcessed) == DBObjectFlagProcessed);
		  data = data->LinkedData ())
		data->Flags (DBObjectFlagProcessed,DBClear);
	}
