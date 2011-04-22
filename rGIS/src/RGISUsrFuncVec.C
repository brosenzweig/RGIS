/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

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
		{ CMmsgPrint (CMmsgAppError, "Invalid data Type in: %s %d",__FILE__,__LINE__); }

	sX = event->xbutton.x;
	sY = event->xbutton.y;
	view->Window2Map  (sX,sY, &coord.X, &coord.Y);
	switch (data->Flags () & DBDataFlagUserModeFlags)
		{
		case DBDataFlagUserModeSelect:
			{
			DBInt flags, select;
			DBObjData *linkedData;
			DBVectorIF *vectorIF = new DBVectorIF (data);

			record = vectorIF->Item (coord);
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
			delete vectorIF;
			} break;
		case DBDataFlagUserModeAdd:
			break;
		case DBDataFlagUserModeFlip:
			break;
		case DBDataFlagUserModeMove:
			if (data->Type () == DBTypeVectorPoint)
				{
				DBInt x, y;
				DBVPointIF *pntIF = new DBVPointIF (data);
				int UIGetLine (Widget,XEvent *,int,int,int *,int *);

				if ((record = pntIF->Item (coord)) != (DBObjRecord *) NULL)
					{
					coord = pntIF->Coordinate (record);

					view->Map2Window (coord.X,coord.Y,&sX,&sY);
					if (UIGetLine (view->DrawingArea (),event,sX,sY,&x,&y))
						{
						DBObjData *linkedData;
						DBRegion extent;
						extent.Expand (data->Extent (record));
						view->Window2Map  (x,y,&coord.X, &coord.Y);
						if ((linkedData = data->LinkedData ()) != (DBObjData *) NULL)
							{
							DBNetworkIF *netIF = new DBNetworkIF (linkedData);
							DBPosition pos;
							netIF->Coord2Pos (coord,pos);netIF->Pos2Coord (pos,coord);
							delete netIF;
							}
						pntIF->Coordinate (record,coord);
						UI2DViewRedrawAll (extent);
						extent.Initialize ();
						extent.Expand (data->Extent (record));
						UI2DViewRedrawAll (extent);
						}
					}
				delete pntIF;
				}
			break;
		case DBDataFlagUserModeDelete:
			if (data->Type () == DBTypeVectorPoint)
				{
				DBVPointIF *pntIF = new DBVPointIF (data);
				DBObjTable *items = data->Table (DBrNItems);

				if ((record = pntIF->Item (coord)) != (DBObjRecord *) NULL)
					{
					DBRegion extent;
					extent.Expand (data->Extent (record));
					items->Delete (record);
					UI2DViewRedrawAll (extent);
					}
				delete pntIF;
				}
			break;
		default: printf ("Unknown Mode %lX",data->Flags () & DBDataFlagUserModeFlags); break;
		}
	}
