/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISDisplay.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

static void _RGIS2DViewCBK (Widget widget,void *workspace,XmAnyCallbackStruct *callData)

	{
	UI2DView *view = new UI2DView ();

	widget = widget; workspace = workspace; callData = callData; view = view;
	}

static void _RGIS2DViewRedrawCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	widget = widget; data = data; callData = callData;
	UI2DViewRedrawAll ();
	}

static void _RGISAttribViewOpenCBK (Widget widget,void *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	UIAttribView *attribView;

	widget = widget; workspace = workspace; callData = callData;
	if ((attribView = (UIAttribView *) dbData->Display (UIAttribViewSTR)) == (UIAttribView *) NULL)
		dbData->DispAdd (attribView = new UIAttribView (dbData));
	else	attribView->Raise ();
	}

static void _RGISAttribViewCloseCBK (Widget widget,void *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	UIAttribView *attribView;

	widget = widget; workspace = workspace; callData = callData;
	if ((attribView = (UIAttribView *) dbData->Display (UIAttribViewSTR)) != (UIAttribView *) NULL)
		{ dbData->DispRemove (attribView); delete attribView; }
	}

static void _RGISDispTableOpenCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;
	UITable *tableCLS;
	static Widget  displayTableSelect = (Widget) NULL;

	widget = widget; data = data; callData = callData;
	if (displayTableSelect == (Widget) NULL) displayTableSelect = UISelectionCreate ((char *) "Table Selection");
	table = dbData->Table (UISelectObject (displayTableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ()));
	if (table == (DBObjTable *) NULL) return;
	if ((tableCLS = (UITable *) dbData->Display (UITableName (dbData,table))) == (UITable *) NULL)
		dbData->DispAdd (tableCLS = new UITable (dbData,table));
	else	tableCLS->Raise ();
	}

static void _RGISDispGraphOpenCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	UIXYGraphShell *graphCLS;

	widget = widget; data = data; callData = callData;
	if ((graphCLS = (UIXYGraphShell *) dbData->Display (UIXYGraphShellStr)) == (UIXYGraphShell *) NULL)
		dbData->DispAdd (graphCLS = new UIXYGraphShell (dbData));
	else	graphCLS->Raise ();
	}

static void _RGISDispCloseCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObject *dispObj;
	static Widget  displayTableSelect = (Widget) NULL;

	widget = widget; data = data; callData = callData;
	if (displayTableSelect == (Widget) NULL) displayTableSelect = UISelectionCreate ((char *) "Table Selection");
	if ((selection = UISelectObject (displayTableSelect,(DBObjectLIST<DBObject> *) dbData->Displays ())) == (char *) NULL) return;
	if ((dispObj = dbData->Display (selection)) != (DBObject *) NULL)
		{
		dbData->DispRemove (dispObj);
		switch (dispObj->Size ())
			{
			case sizeof (UITable):		delete ((UITable *) dispObj); break;
			case sizeof (UIAttribView):delete ((UIAttribView *) dispObj); break;
			case sizeof (UIXYGraph):	delete ((UIXYGraph *) dispObj); break;
			default:	break;
			}
		}
	}

static void _RGISDispCloseAllCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;
	DBObject *dispObj;
	DBObjectLIST<DBObjTable>	*tableList = dbData->Tables ();

	widget = widget; data = data; callData = callData;
	for (table = tableList->First ();table != (DBObjTable *) NULL;table = tableList->Next ())
		if ((dispObj = dbData->Display (table->Name ())) != (DBObject *) NULL)
			{
			dbData->DispRemove (dispObj);
			switch (dispObj->Size ())
				{
				case sizeof (UITable):		delete ((UITable *) dispObj); break;
				case sizeof (UIAttribView):delete ((UIAttribView *) dispObj); break;
				case sizeof (UIXYGraph):	delete ((UIXYGraph *) dispObj); break;
				default:	break;
				}
			}
	}

UIMenuItem RGISDisplayMenu [] = {
	UIMenuItem ((char *) "2DView",			UIMENU_NORULE, 	RGISDataGroup,	_RGIS2DViewCBK,				(char *) "RGIS22MenuSystem.html#Display_2DView"),
	UIMenuItem ((char *) "Redraw All",		UIMENU_NORULE,		RGISDataGroup,	_RGIS2DViewRedrawCBK,		(char *) "RGIS22MenuSystem.html#Display_RedrawAll"),
	UIMenuItem (UIMENU_NORULE,		UIMENU_NORULE),
	UIMenuItem ((char *) "Attributes",		RGISGeoDataGroup,	UIMENU_NORULE,	_RGISAttribViewOpenCBK,		(char *) "RGIS22MenuSystem.html#Display_Attributes"),
	UIMenuItem ((char *) "Attributes Close",RGISGeoDataGroup,UIMENU_NORULE,	_RGISAttribViewCloseCBK,	(char *) "RGIS22MenuSystem.html#Display_AttribsClose"),
	UIMenuItem (RGISGeoDataGroup,	UIMENU_NORULE),
	UIMenuItem ((char *) "Open Table",		UIMENU_NORULE,		RGISDataGroup,	_RGISDispTableOpenCBK,		(char *) "RGIS22MenuSystem.html#Display_OpenTable"),
	UIMenuItem ((char *) "Open Graph",		UIMENU_NORULE,		RGISDataGroup,	_RGISDispGraphOpenCBK,		(char *) "RGIS22MenuSystem.html#Display_OpenGraph"),
	UIMenuItem ((char *) "Close",				UIMENU_NORULE,		RGISDataGroup,	_RGISDispCloseCBK,			(char *) "RGIS22MenuSystem.html#Display_Close"),
	UIMenuItem ((char *) "Close All",		UIMENU_NORULE,		RGISDataGroup,	_RGISDispCloseAllCBK,		(char *) "RGIS22MenuSystem.html#Display_CloseAll"),
	UIMenuItem ()};
