/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISMetaDB.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

static void _RGISMetaDBDeleteCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *subjectSTR = strlen (workspace->Subject ()) > 0 ? workspace->Subject () : (char *) NULL;
	char *domainSTR  = strlen (workspace->Domain ())  > 0 ? workspace->Domain ()  : (char *) NULL;
	DBObjMetaEntry *metaEntry;
	DBDataset *dataset = UIDataset ();

	widget = widget; callData = callData;

	if ((metaEntry = UIDatasetMetaData (subjectSTR,domainSTR,DBFault)) != (DBObjMetaEntry *) NULL)
		dataset->DeleteMetaEntry (metaEntry);
	}

UIMenuItem RGISMetaDBMenu [] = {
	UIMenuItem ("Refresh",	UIMENU_NORULE,		UIMENU_NORULE,	(UIMenuCBK) UIMenuNotImplemented, "RGIS22MenuSystem.html#MetaDB_Refresh"),
	UIMenuItem ("Edit",		UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) UIMenuNotImplemented, "RGIS22MenuSystem.html#MetaDB_Edit"),
	UIMenuItem ("Delete",	UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISMetaDBDeleteCBK, "RGIS22MenuSystem.html#MetaDB_Delete"),
	UIMenuItem ()};