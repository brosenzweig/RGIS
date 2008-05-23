/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISUsrFuncNet.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Xm.h>
#include <rgis.H>

void _RGISUserFuncionNetwork (DBObjData *data,UI2DView *view,XEvent *event)

	{
	DBInt sX, sY, redraw = false;
	DBNetworkIO *netIO;
	DBCoordinate coord;
	DBObjRecord *cellRec, *basinRec;
	void _RGISUserFuncionQuery (DBObjData *,UI2DView *,XEvent *);

	if ((data->Flags () & DBDataFlagUserModeFlags) == DBDataFlagUserModeQuery)
		{ _RGISUserFuncionQuery (data,view,event);	return; }

	if (event->type != ButtonPress) return;
	if (DBTypeNetwork != data->Type ())
		{ fprintf (stderr,"Invalid data Type in: _RGISUserFuncionNetwork ()\n"); }
	
	sX = event->xbutton.x;
	sY = event->xbutton.y;
	view->Window2Map  (sX,sY, &coord.X, &coord.Y);

	netIO = new DBNetworkIO (data);
	switch (data->Flags () & DBDataFlagUserModeFlags)
		{
		case DBDataFlagUserModeSelect:
			{
			DBInt basinID, cellID;
			DBRegion extent;
			UITable *tableView;
			
			if ((cellRec = netIO->Cell (coord)) == (DBObjRecord *) NULL)
				{ UIMessage ("Cell Does not Exists!"); return; }
			for (basinID = 0;basinID < netIO->BasinNum ();++basinID)
				{
				basinRec = netIO->Basin (basinID);
				if ((basinRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
					{
					data->SelectObject (basinRec,DBClear);
					extent.Expand (data->Extent (basinRec));
					}
				}
			for (cellID = 0;cellID < netIO->CellNum ();++cellID)
				{
				cellRec = netIO->Cell (cellID);
				if ((cellRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
					{
					extent.Expand (netIO->Center (cellRec) + (netIO->CellSize () / 2.0));
					extent.Expand (netIO->Center (cellRec) - (netIO->CellSize () / 2.0));
					cellRec->Flags (DBObjectFlagSelected,DBClear);
					}
				}
			if ((tableView = (UITable *) data->Display (UITableName (data,data->Table (DBrNItems)))) != (UITable *) NULL)
				tableView->Draw ();
			if ((tableView = (UITable *) data->Display (UITableName (data,data->Table (DBrNCells)))) != (UITable *) NULL)
				tableView->Draw ();
			UI2DViewRedrawAll (extent);
			cellRec = netIO->Cell (coord);
			switch (data->Flags () & DBDataFlagSelectMode)
				{
				case DBDataFlagSelectMode:	netIO->DownStreamSearch	(cellRec,(DBNetworkACTION) DBNetworkSelect);	break;
				default:							netIO->UpStreamSearch	(cellRec,(DBNetworkACTION) DBNetworkSelect);	break;
				}
			extent.Initialize ();
			for (cellID = 0;cellID < netIO->CellNum ();++cellID)
				{
				cellRec = netIO->Cell (cellID);
				if ((cellRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
					{
					extent.Expand (netIO->Center (cellRec) + (netIO->CellSize () / 2.0));
					extent.Expand (netIO->Center (cellRec) - (netIO->CellSize () / 2.0));
					}
				}
			UI2DViewRedrawAll (extent);
			if ((tableView = (UITable *) data->Display (UITableName (data,data->Table (DBrNCells)))) != (UITable *) NULL)
				tableView->Draw ();
			} break;
		case DBDataFlagUserModeAdd:
			if (netIO->CellAdd (coord) == (DBObjRecord *) NULL) UIMessage ("Cell Creation Error");
			else redraw = true;				
			break;
		case DBDataFlagUserModeDelete:
			if (netIO->CellDelete (coord) == DBFault) UIMessage ("Cell Does not Exists!");				
			else redraw = true;				
			break;
		case DBDataFlagUserModeRotate:
			{
			DBObjRecord *cellRec = netIO->Cell (coord);
			
			if (cellRec != (DBObjRecord *) NULL)
				{
				switch (event->xbutton.button)
					{
					case Button1:	netIO->CellRotate (cellRec,DBForward);		break;
					case Button2:
						if (netIO->CellDirection (cellRec) == DBNull)
								netIO->CellDirection (cellRec,DBNetDirN);
						else	netIO->CellDirection (cellRec,DBNull);
						break;
					case Button3:	netIO->CellRotate (cellRec,DBBackward);	break;
					}
				redraw = true;
				}
			else	UIMessage ("Cell Does not Exists!");
			} break;
		default: printf ("Unknown Mode %lX",data->Flags () & DBDataFlagUserModeFlags); break;
		}
	if (redraw)
		{
		DBPosition pos;
		
		if (netIO->Coord2Pos (coord,pos) == DBSuccess)
			{
			DBRegion extent;
			DBCoordinate delta (netIO->CellWidth () * 1.25, netIO->CellHeight () * 1.25);
			
			netIO->Pos2Coord (pos,coord); coord = coord + delta; extent.Expand (coord);
			netIO->Pos2Coord (pos,coord); coord = coord - delta; extent.Expand (coord);
			UI2DViewRedrawAll (extent); 
			}
		}
	delete netIO;
	}
