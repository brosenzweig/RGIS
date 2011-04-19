/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

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
					DBVectorIF *vectorIF = new DBVectorIF (data);
					attribView->Draw (vectorIF->Item (coord));
					} break;
				case DBTypeGridDiscrete:
					{
					DBGridIF *gridIF = new DBGridIF (data);
					attribView->Draw (gridIF->GridItem (coord));
					delete gridIF;
					} break;
				case DBTypeGridContinuous:
					{
					DBPosition pos;
					DBGridIF *gridIF = new DBGridIF (data);
					char nameString [DBStringLength * 2];
					DBObjRecord *record = new DBObjRecord ("Noname",sizeof (DBCoordinate));
					gridIF->Coord2Pos (coord,pos);
					gridIF->Pos2Coord (pos,coord);
					sprintf (nameString,"CellCoordinate %10.2f%10.2f",coord.X,coord.Y);
					record->Name (nameString);
					memcpy (record->Data (),(void *) &coord,sizeof (coord));
					attribView->Draw (record);
					delete record;
					delete gridIF;
					} break;
				case DBTypeNetwork:
					{
					DBNetworkIF *netIF = new DBNetworkIF (data);
					attribView->Draw (netIF->Cell (coord));
					delete netIF;
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
