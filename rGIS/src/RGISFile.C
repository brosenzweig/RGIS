/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISFile.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

static DBInt _RGISProjectFileState = false;
static char  _RGISProjectFile [DBDataFileNameLen];
static Widget _RGISProjectSelect = NULL;
static Widget _RGISFileSelect = NULL;

static void _RGISFileProjectLoadCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fileName, buffer [DBDataFileNameLen + 2];
	FILE *listFile;
	DBDataset *dataset = UIDataset ();
	DBObjData *data;

	widget = widget; callData = callData;

	if (_RGISProjectSelect == (Widget) NULL) _RGISProjectSelect = UIFileSelectionCreate ("Project file",NULL,"*.gprj",XmFILE_REGULAR);
		
	if ((fileName = UIFileSelection (_RGISProjectSelect,true)) == NULL) return;
	if ((listFile = fopen (fileName,"r")) == (FILE *) NULL)
		{ perror ("Project File Opening Error in: _RGISFileProjectLoadCBK ()"); return; }
	strncpy (_RGISProjectFile,fileName,sizeof (_RGISProjectFile) - 1);
	_RGISProjectFileState = true;
	while (fgets (buffer,sizeof (buffer) - 2,listFile) == buffer)
		{
		if (buffer [strlen (buffer) - 1] == '\n') buffer [strlen (buffer) - 1] = '\0';
		for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
			if (strcmp (data->FileName (),buffer) == 0) break;
		if (data == (DBObjData *) NULL)
			{
			data = new DBObjData ();
			if (data->Read (buffer) == DBSuccess) workspace->CurrentData (data);
			else delete data;
			}
		}
	fclose (listFile);
	}

static void _RGISFileProjectSaveCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	FILE *listFile;
	DBDataset *dataset = UIDataset ();
	DBObjData *data;
	widget = widget; callData = callData;

	if (_RGISProjectSelect == (Widget) NULL) _RGISProjectSelect = UIFileSelectionCreate ("Project file",NULL,"*.gprj",XmFILE_REGULAR);

	if (_RGISProjectFileState == false)
		{
		char *fileName;
		if ((fileName = UIFileSelection (_RGISProjectSelect,false)) == NULL) return;
		strncpy (_RGISProjectFile,fileName,sizeof (_RGISProjectFile) - 1);
		_RGISProjectFileState = true;
		}
	if ((listFile = fopen (_RGISProjectFile,"w")) == (FILE *) NULL)
		{ perror ("Project File Opening Error in: _RGISFileProjectSaveCBK ()"); return; }
	for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
		if (strlen (data->FileName ()) > 0) fprintf (listFile,"%s\n",data->FileName ());
	fclose (listFile);
	}

static void _RGISFileProjectSaveAsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fileName;
	FILE *listFile;
	DBDataset *dataset = UIDataset ();
	DBObjData *data;
	widget = widget; callData = callData;
	
	if (_RGISProjectSelect == (Widget) NULL) _RGISProjectSelect = UIFileSelectionCreate ("Project file",NULL,"*.gprj",XmFILE_REGULAR);

	if ((fileName = UIFileSelection (_RGISProjectSelect,false)) == NULL) return;
	strncpy (_RGISProjectFile,fileName,sizeof (_RGISProjectFile) - 1);
	_RGISProjectFileState = true;

	if ((listFile = fopen (_RGISProjectFile,"w")) == (FILE *) NULL)
		{ perror ("Project File Opening Error in: _RGISFileProjectSaveCBK ()"); return; }
	for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
		if (strlen (data->FileName ()) > 0) fprintf (listFile,"%s\n",data->FileName ());
	fclose (listFile);
	}

static void _RGISFileOpenCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *subjectSTR = strlen (workspace->Subject ()) > 0 ? workspace->Subject () : (char *) NULL;
	char *domainSTR  = strlen (workspace->Domain ())  > 0 ? workspace->Domain ()  : (char *) NULL;
	DBObjData *data;
	widget = widget; callData = callData;

	if ((data = UIDatasetOpenData (subjectSTR,domainSTR,DBFault)) != (DBObjData *) NULL)
		workspace->CurrentData (data);
	}

static void _RGISFileOpenFileCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fileName;
	DBDataset *dataset = UIDataset ();
	DBObjData *data;
	widget = widget; callData = callData;

	if (_RGISFileSelect == NULL) _RGISFileSelect = UIFileSelectionCreate ("GHAAS Data",NULL,"*.gdb?",XmFILE_REGULAR);

	if ((fileName = UIFileSelection (_RGISFileSelect,true)) == NULL) return;

	for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
		if (strcmp (data->FileName (),fileName) == 0) { workspace->CurrentData (data); return; }
	data = new DBObjData ();
	if (data->Read (fileName) == DBSuccess)
		{
		DBDataset *dataset = UIDataset ();
		DBObjectLIST<DBObjMetaEntry>	*metaList = dataset->MetaList ();
		DBObjMetaEntry *metaEntry;

		workspace->CurrentData (data);
		metaEntry = metaList->Item (data->Name ());
		metaEntry->FileName (data->FileName ());
		}
	else delete data;
	}

static void _RGISFileReloadCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();
 	DBUnsigned dataFlags = data->Flags ();
	widget = widget; callData = callData;

	if (data->Read (data->FileName ()) != DBSuccess)
		{
		dataset->RemoveData (data);
		delete data;
		data = dataset->Data ();
		workspace->CurrentData (data);
		}
	data->Flags (data->Flags (),DBClear);
	data->Flags (dataFlags,DBSet);
	}

static void _RGISFileCloseCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();
	DBObjTable *table;
	DBObject *dispObj;
	DBObjectLIST<DBObjTable>	*tableList = data->Tables ();

	widget = widget; workspace = workspace; callData = callData;
	dataset->RemoveData (data);
	for (table = tableList->First ();table != (DBObjTable *) NULL;table = tableList->Next ())
		if ((dispObj = data->Display (UITableName (data,table))) != (DBObject *) NULL)
				{
				data->DispRemove (dispObj);
				delete ((UITable *) dispObj);
				}
	if ((dispObj = data->Display (UIXYGraphShellStr)) != (DBObject *) NULL)
		delete ((UIXYGraphShell *) dispObj);
	delete data;
	data = dataset->Data ();
	workspace->CurrentData (data);
	}

static void _RGISFileSaveCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fileName;
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();
	DBObjectLIST<DBObjMetaEntry>	*metaList = dataset->MetaList ();
	DBObjMetaEntry *metaEntry;
	
	widget = widget; workspace = workspace; callData = callData;
	if (_RGISFileSelect == NULL) _RGISFileSelect = UIFileSelectionCreate ("GHAAS Data",NULL,"*.gdb?",XmFILE_REGULAR);

	fileName = data->FileName ();
	if (strlen (fileName) == 0)
		{
		if ((fileName = UIFileSelection (_RGISFileSelect,false)) == (char *) NULL) return;
		else
			{
			metaEntry = metaList->Item (data->Name ());
			metaEntry->FileName (fileName);
			data->FileName (fileName);
			}
		}
	data->Write (fileName);
	}

static void _RGISFileSaveAsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fileName;
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();
	DBObjectLIST<DBObjMetaEntry>	*metaList = dataset->MetaList ();
	DBObjMetaEntry *metaEntry;

	widget = widget; workspace = workspace; callData = callData;
	if (_RGISFileSelect == NULL) _RGISFileSelect = UIFileSelectionCreate ("GHAAS Data",NULL,"*.gdb?",XmFILE_REGULAR);

	if ((fileName = UIFileSelection (_RGISFileSelect,false)) == NULL) return;
	data->FileName (fileName);
	data->Write (fileName);
	metaEntry = metaList->Item (data->Name ());
	metaEntry->FileName (data->FileName ());
	}

static void _RGISFileSaveAllCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fileName;
	DBDataset *dataset = UIDataset ();
	DBObjData *data, *currentData = dataset->Data ();
	DBObjectLIST<DBObjMetaEntry>	*metaList = dataset->MetaList ();
	DBObjMetaEntry *metaEntry;
	
	widget = widget; workspace = workspace; callData = callData;
	if (_RGISFileSelect == NULL) _RGISFileSelect = UIFileSelectionCreate ("GHAAS Data",NULL,"*.gdb?",XmFILE_REGULAR);

	for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
		{
		fileName = data->FileName ();
		if (strlen (fileName) == 0)
			{
			workspace->CurrentData (data);
			if ((fileName = UIFileSelection (_RGISFileSelect,false)) == (char *) NULL) continue;
			else
				{
				metaEntry = metaList->Item (data->Name ());
				metaEntry->FileName (fileName);
				data->FileName (fileName);
				}
			}
		data->Write (fileName);
		}
	if (currentData != dataset->Data ()) workspace->CurrentData (currentData);
	}

static void _RGISFileHeaderInfoCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();
	DBObjectLIST<DBObjMetaEntry>	*metaList = dataset->MetaList ();
	DBObjMetaEntry *metaEntry;

	widget = widget; workspace = workspace; callData = callData;
	metaEntry = metaList->Item (data->Name ());
	UIDataHeaderForm (data);
	workspace->CurrentDataName ();
	metaEntry->Name (data->Name ());
	metaEntry->FileName (data->FileName ());
	metaEntry->Type (data->Type ());
	metaEntry->Subject (data->Document (DBDocSubject));
	metaEntry->GeoDomain (data->Document (DBDocGeoDomain));
	}

static void _RGISFilePropertiesCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();

	widget = widget; workspace = workspace; callData = callData;
	UIDataPropertiesForm (data);
	}


static void _RGISFileRelateNewCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();

	widget = widget; workspace = workspace; callData = callData;
	UIRelateData (data,(DBObjRecord *) NULL);
	}

static void _RGISFileRelateEditCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	widget = widget; workspace = workspace; callData = callData;
	}

static void _RGISFileRelateDeleteCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	widget = widget; workspace = workspace; callData = callData;
	}


static void _RGISFileLinkDataCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data (), *linkedData;
	
	widget = widget; workspace = workspace; callData = callData;
	if ((linkedData = UIDatasetSelectData ()) != (DBObjData *) NULL)
		{ data->LinkedData (linkedData); workspace->CurrentData (data); return; }
	}

static void _RGISFileLinkAllDataCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *currentData = dataset->Data (), *data, *linkedData, *firstData;
	
	widget = widget; workspace = workspace; callData = callData;
	firstData = data = dataset->FirstData ();
	while ((linkedData = dataset->NextData ()) != (DBObjData *) NULL) { data->LinkedData (linkedData); data = linkedData; }
	if (data != firstData) data->LinkedData (firstData);
	workspace->CurrentData (currentData);
	}

static void _RGISFileUnlinkAllDataCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data, *currentData = dataset->Data ();
	
	widget = widget; workspace = workspace; callData = callData;
	for (data = dataset->FirstData (); data != (DBObjData *) NULL; data = dataset->NextData ())
		data->LinkedData ((DBObjData *) NULL);
	workspace->CurrentData (currentData);
	}

static void _RGISFileUnlinkDataCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data (), *linkedData = data->LinkedData ();
	
	widget = widget; workspace = workspace; callData = callData;
	if (linkedData == (DBObjData *) NULL) return;

	data->LinkedData ((DBObjData *) NULL);
	workspace->CurrentData (data);
	}

static UIMenuItem RGISFileProjectMenu [] = {
	UIMenuItem ("Load",			UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileProjectLoadCBK, "RGIS22MenuSystem.html#File_ProjectLoad"),
	UIMenuItem ("Save",			UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileProjectSaveCBK, "RGIS22MenuSystem.html#File_ProjectSave"),
	UIMenuItem ("Save as",		UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileProjectSaveAsCBK,"RGIS22MenuSystem.html#File_ProjectSaveAs"),
	UIMenuItem ()};

static UIMenuItem RGISFileRelateMenu [] = {
	UIMenuItem ("New",			UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileRelateNewCBK,	"RGIS22MenuSystem.html#File_Relate"),
	UIMenuItem ("Edit",			UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileRelateEditCBK,	"RGIS22MenuSystem.html#File_Relate"),
	UIMenuItem ("Delete",		UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileRelateDeleteCBK,"RGIS22MenuSystem.html#File_Relate"),
	UIMenuItem ()};

UIMenuItem RGISFileMenu [] = {
	UIMenuItem ("Project",		UIMENU_NORULE,		UIMENU_NORULE,	RGISFileProjectMenu),
	UIMenuItem ("Open Data",	UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileOpenCBK,			"RGIS22MenuSystem.html#File_OpenData"),
	UIMenuItem ("Open File",	UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) _RGISFileOpenFileCBK,	"RGIS22MenuSystem.html#File_OpenFile"),
	UIMenuItem ("Reload",		RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileReloadCBK,		"RGIS22MenuSystem.html#File_Reload"),
	UIMenuItem ("Close",			RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileCloseCBK,		"RGIS22MenuSystem.html#File_Close"),
	UIMenuItem ("Save",			RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileSaveCBK,			"RGIS22MenuSystem.html#File_Save"),
	UIMenuItem ("Save as",		RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileSaveAsCBK,		"RGIS22MenuSystem.html#File_SaveAs"),
	UIMenuItem ("Save all",		RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileSaveAllCBK,		"RGIS22MenuSystem.html#File_SaveAll"),
	UIMenuItem (RGISDataGroup,		UIMENU_NORULE),
	UIMenuItem ("Header Info",	RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileHeaderInfoCBK,	"RGIS22MenuSystem.html#File_HeaderInfo"),
	UIMenuItem ("Properties",	RGISVectorGroup,	UIMENU_NORULE,	(UIMenuCBK) _RGISFilePropertiesCBK,	"RGIS22MenuSystem.html#File_VectorProp"),
	UIMenuItem ("Properties",	RGISContinuousGroup,UIMENU_NORULE,(UIMenuCBK)_RGISFilePropertiesCBK,	"RGIS22MenuSystem.html#File_CGridProp"),
	UIMenuItem ("Properties",	RGISDiscreteGroup,UIMENU_NORULE,	(UIMenuCBK) _RGISFilePropertiesCBK,	"RGIS22MenuSystem.html#File_DGridProp"),
	UIMenuItem ("Properties",	RGISNetworkGroup,	UIMENU_NORULE,	(UIMenuCBK) _RGISFilePropertiesCBK,	"RGIS22MenuSystem.html#File_NetProp"),
	UIMenuItem ("History",		RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) UIMenuNotImplemented,	"RGIS22MenuSystem.html#File_History"),
	UIMenuItem ("Relate",		RGISDataGroup,		UIMENU_NORULE,		RGISFileRelateMenu),
	UIMenuItem (RGISDataGroup,		UIMENU_NORULE),
	UIMenuItem ("Link",			RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileLinkDataCBK,	"RGIS22MenuSystem.html#File_Link"),
	UIMenuItem ("Link all",		RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileLinkAllDataCBK,"RGIS22MenuSystem.html#File_LinkAll"),
	UIMenuItem ("Unlink",		RGISDataGroup,		RGISLinkedDataGroup,	(UIMenuCBK) _RGISFileUnlinkDataCBK,"RGIS22MenuSystem.html#File_Unlink"),
	UIMenuItem ("Unlink all",	RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISFileUnlinkAllDataCBK,"RGIS22MenuSystem.html#File_UnlinkAll"),
	UIMenuItem (UIMENU_NORULE,UIMENU_NORULE),
	UIMenuItem ("Exit",			UIMENU_NORULE, 	UIMENU_NORULE,	(UIMenuCBK) UILoopStopCBK,				"RGIS22MenuSystem.html#File_Exit"),
	UIMenuItem ()};


