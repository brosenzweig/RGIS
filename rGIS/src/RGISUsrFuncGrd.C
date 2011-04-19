/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISUsrFuncGrd.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Xm.h>
#include <rgis.H>

void _RGISUserFuncionGrid (DBObjData *data,UI2DView *view,XEvent *event)

	{
	void _RGISUserFuncionQuery (DBObjData *,UI2DView *,XEvent *);

	if ((data->Flags () & DBDataFlagUserModeFlags) == DBDataFlagUserModeQuery)
		{ _RGISUserFuncionQuery (data,view,event);	return; }
	}
