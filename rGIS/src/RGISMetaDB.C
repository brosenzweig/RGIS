/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

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
	UIMenuItem ((char *) "Refresh",	UIMENU_NORULE,		UIMENU_NORULE,	(UIMenuCBK) UIMenuNotImplemented, (char *) "RGIS22MenuSystem.html#MetaDB_Refresh"),
	UIMenuItem ((char *) "Edit",		UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) UIMenuNotImplemented, (char *) "RGIS22MenuSystem.html#MetaDB_Edit"),
	UIMenuItem ((char *) "Delete",	UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISMetaDBDeleteCBK, (char *) "RGIS22MenuSystem.html#MetaDB_Delete"),
	UIMenuItem ()};
