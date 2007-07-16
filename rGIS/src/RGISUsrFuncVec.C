/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISUsrFuncVec.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Xm.h>
#include <rgis.H>

void _RGISUserFuncionVector (DBObjData *data,UI2DView *view,XEvent *event)

	{
	DBCoordinate coord;
	DBShort sX, sY;
	DBObjRecord *record;
	void _RGISUserFuncionQuery (DBObjData *,UI2DView *,XEvent *);
	void _RGISUserFuncionNetwork (DBObjData *,UI2DView *,XEvent *);

	if ((data->Flags () & DBDataFlagUserModeFlags) == DBDataFlagUserModeQuery)
		{ _RGISUserFuncionQuery (data,view,event);	return; }

	if (event->type != ButtonPress) return;
	if (DBTypeVector != (data->Type () & DBTypeVector))
		{ fprintf (stderr,"Invalid data Type in: _RGISUserFuncionVector ()\n"); }
	
	sX = event->xbutton.x;
	sY = event->xbutton.y;
	view->Window2Map  (sX,sY, &coord.X, &coord.Y);
	switch (data->Flags () & DBDataFlagUserModeFlags)
		{
		case DBDataFlagUserModeSelect:
			{
			DBInt flags, select;
			DBObjData *linkedData;
			DBVectorIO *vectorIO = new DBVectorIO (data);

			record = vectorIO->Item (coord);
			select = (record->Flags () & DBObjectFlagSelected) !=  DBObjectFlagSelected ? DBSet : DBClear;
			record->Flags (DBObjectFlagSelected,select);
			if ((select == DBSet) && ((linkedData = data->LinkedData ()) != (DBObjData *) NULL) && (linkedData->Type () == DBTypeNetwork))
				{
				flags = linkedData->Flags () & DBDataFlagUserModeFlags;
				linkedData->Flags (DBDataFlagUserModeFlags,DBClear);
				linkedData->Flags (DBDataFlagUserModeSelect,DBSet);
				_RGISUserFuncionNetwork (linkedData,view,event);
				linkedData->Flags (DBDataFlagUserModeFlags,DBClear);
				linkedData->Flags (flags,DBSet);
				}
			if (record != (DBObjRecord *) NULL) UI2DViewRedrawAll (data->Extent (record));
			delete vectorIO;
			} break;
		case DBDataFlagUserModeAdd:
			break;
		case DBDataFlagUserModeFlip:
			break;
		case DBDataFlagUserModeMove:
			if (data->Type () == DBTypeVectorPoint)
				{
				DBInt x, y;
				DBVPointIO *pointIO = new DBVPointIO (data);
				int UIGetLine (Widget,XEvent *,int,int,int *,int *);

				if ((record = pointIO->Item (coord)) != (DBObjRecord *) NULL)
					{
					coord = pointIO->Coordinate (record);
				
					view->Map2Window (coord.X,coord.Y,&sX,&sY);
					if (UIGetLine (view->DrawingArea (),event,sX,sY,&x,&y))
						{
						DBObjData *linkedData;
						DBRegion extent;
						extent.Expand (data->Extent (record));
						view->Window2Map  (x,y,&coord.X, &coord.Y);
						if ((linkedData = data->LinkedData ()) != (DBObjData *) NULL)
							{
							DBNetworkIO *netIO = new DBNetworkIO (linkedData);
							DBPosition pos;
							netIO->Coord2Pos (coord,pos);netIO->Pos2Coord (pos,coord);
							delete netIO;
							}
						pointIO->Coordinate (record,coord);
						UI2DViewRedrawAll (extent);
						extent.Initialize ();
						extent.Expand (data->Extent (record));
						UI2DViewRedrawAll (extent);
						}
					}
				delete pointIO;
				}
			break;
		case DBDataFlagUserModeDelete:
			if (data->Type () == DBTypeVectorPoint)
				{
				DBVPointIO *pointIO = new DBVPointIO (data);
				DBObjTable *items = data->Table (DBrNItems);

				if ((record = pointIO->Item (coord)) != (DBObjRecord *) NULL)
					{
					DBRegion extent;
					extent.Expand (data->Extent (record));
					items->Delete (record);
					UI2DViewRedrawAll (extent);
					}
				delete pointIO;
				}
			break;
		default: printf ("Unknown Mode %lX",data->Flags () & DBDataFlagUserModeFlags); break;
		}
	}
