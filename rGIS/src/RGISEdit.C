/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISEdit.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <rgis.H>

static void _RGISEditNameFieldCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *fieldName;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table = dbData->Table (DBrNItems);
	DBObjTableField *field;
	DBObjRecord *record;
	static Widget fieldSelect = (Widget) NULL;
	widget = widget; data = data; callData = callData;
	
	if (table == (DBObjTable *) NULL) return;
	if (fieldSelect == (Widget) NULL) fieldSelect = UISelectionCreate ("Field Selection");
	fieldName = UISelectObject (fieldSelect,(DBObjectLIST<DBObject> *)(table->Fields ()),DBTableFieldIsString);
	if (fieldName == (char *) NULL) return;
	if ((field = table->Field (fieldName)) == (DBObjTableField *) NULL) return;
	UIPauseDialogOpen ("Changing Record Names");
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		{
		UIPause (record->RowID () * 100 / table->ItemNum ());
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		record->Name (field->String (record));		
		}
	UIPauseDialogClose ();
	}

static void _RGISEditJoinTableSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = NULL;
	char *field;
	DBObjectLIST<DBObjTableField> *fields;
	int (*condFunc) (const DBObject *);

	callData = callData;
	if (select == NULL) select = UISelectionCreate ("Field Selection");
	XtVaGetValues (widget,XmNuserData, &condFunc, NULL);
	XtVaGetValues (text,XmNuserData, &fields, NULL);
	if ((field = UISelectObject (select,(DBObjectLIST<DBObject> *) fields,condFunc)) != NULL)
		XmTextFieldSetString (text,field);
	}

static void _RGISEditJoinTableCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *selection;
	char *f0Text, *f1Text;
	static DBInt join;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjData *lnkData = dbData->LinkedData () != (DBObjData *) NULL ? dbData->LinkedData () : dbData;
	DBObjTable *itemTable = dbData->Table (DBrNItems);
	DBObjTable *joinTable;
	DBObjTableField *joinField = (DBObjTableField *) NULL, *relateField = (DBObjTableField *) NULL;
	static Widget tableSelect = (Widget) NULL;
	static Widget dShell = (Widget) NULL, mainForm;
	static Widget field0TextF, field1TextF;
	static Widget field0Button, field1Button;

	widget = widget; data = data; callData = callData;
	if (tableSelect == (Widget) NULL)
		{
		tableSelect = UISelectionCreate ("Table Selection");
		XmString string;

		dShell = UIDialogForm ("Compare Fields",false);
		mainForm = UIDialogFormGetMainForm (dShell);
		
		string = XmStringCreate ("Select",UICharSetBold);
		field0Button = XtVaCreateManagedWidget ("RGISEditJoinTableField0Button",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsCategory,
								NULL);
		XmStringFree (string);
		field0TextF = XtVaCreateManagedWidget ("RGISEditJoinTableField0TextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				field0Button,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			field0Button,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			field0Button,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					DBStringLength / 2,
								NULL);
		XtAddCallback (field0Button,XmNactivateCallback,(XtCallbackProc) _RGISEditJoinTableSelectCBK,field0TextF);
		string = XmStringCreate ("Relate Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISEditJoinTableField0NameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				field0Button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			field0TextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			field0Button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		
		string = XmStringCreate ("Select",UICharSetBold);
		field1Button = XtVaCreateManagedWidget ("RGISEditJoinTableField1Button",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				field0Button,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsCategory,
								NULL);
		XmStringFree (string);
		field1TextF = XtVaCreateManagedWidget ("RGISEditJoinTableField1TextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				field1Button,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			field1Button,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			field1Button,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					DBStringLength / 2,
								NULL);
		XtAddCallback (field1Button,XmNactivateCallback,(XtCallbackProc) _RGISEditJoinTableSelectCBK,field1TextF);
		string = XmStringCreate ("Join Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISEditJoinTableField1Label",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				field1Button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			field1TextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			field1Button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		XtSetSensitive (UIDialogFormGetOkButton (dShell),true);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&join);
		}
	selection = UISelectObject (tableSelect,(DBObjectLIST<DBObject> *) lnkData->Tables ());
	if (selection == (char *) NULL)	return;
	if ((joinTable = lnkData->Table (selection)) == (DBObjTable *) NULL)
		{ fprintf (stderr,"Invalid Table in: _RGISEditJoinTableCBK ()\n"); return; }
	
	XtVaSetValues (field0TextF,XmNuserData, itemTable->Fields (), NULL);
	XtVaSetValues (field1TextF,XmNuserData, joinTable->Fields (), NULL);
	join = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		f0Text = XmTextFieldGetString (field0TextF);
		f1Text = XmTextFieldGetString (field1TextF);
		if ((strlen (f0Text) > 0) && ((relateField = itemTable->Field (f0Text)) != (DBObjTableField *) NULL))
			switch (relateField->Type ())
				{
				case DBTableFieldString:
					XtVaSetValues (field1Button,	XmNuserData, DBTableFieldIsString, NULL); break;
				case DBTableFieldInt:
					XtVaSetValues (field1Button,	XmNuserData, DBTableFieldIsInteger, NULL); break;
				default:
					fprintf (stderr,"Invalid Data Type in: _RGISEditJoinTableCBK ()\n"); break;
				}
		else	XtVaSetValues (field1Button,	XmNuserData, DBTableFieldIsCategory, NULL);

		if ((strlen (f1Text) > 0) && ((joinField = joinTable->Field (f1Text)) != (DBObjTableField *) NULL))
			switch (joinField->Type ())
				{
				case DBTableFieldString:
					XtVaSetValues (field0Button,	XmNuserData, DBTableFieldIsString, NULL); break;
				case DBTableFieldInt:
					XtVaSetValues (field0Button,	XmNuserData, DBTableFieldIsInteger, NULL); break;
				default:
					fprintf (stderr,"Invalid Data Type in: _RGISEditJoinTableCBK ()\n"); break;
				}
		else	XtVaSetValues (field0Button,	XmNuserData, DBTableFieldIsCategory, NULL);
		
		XtFree (f0Text);	XtFree (f1Text);	
		}
	UIDialogFormPopdown (dShell);

	if (join)
		{
		UIPauseDialogOpen ("Joining Tables");
		RGLibTableJoin (itemTable, relateField, joinTable, joinField);
		UIPauseDialogClose ();
		}
	}

static void _RGISEditAddFieldCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;
	DBObjTableField *field;
	UITable *tableCLS;
	static Widget tableSelect = (Widget) NULL;

	widget = widget; data = data; callData = callData;
	if (tableSelect == (Widget) NULL) tableSelect = UISelectionCreate ("Table Selection");
	if ((selection = UISelectObject (tableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ())) == (char *) NULL) return;
	if ((table = dbData->Table (selection)) == (DBObjTable *) NULL) return;
	if ((field = UITableFieldEdit ()) != (DBObjTableField *) NULL)
		table->AddField (field); 
	if ((tableCLS = (UITable *) dbData->Display (UITableName (dbData,table))) != (UITable *) NULL)
		{
		tableCLS->AddField (field);
		tableCLS->Draw ();
		}
	}

static void _RGISEditRedefineFieldCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;
	UITable *tableCLS;
	static Widget tableSelect = (Widget) NULL;

	widget = widget; data = data; callData = callData;
	if (tableSelect == (Widget) NULL) tableSelect = UISelectionCreate ("Table Selection");
	selection = UISelectObject (tableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ());
	if (selection == (char *) NULL)	return;
	if ((table = dbData->Table (selection)) == (DBObjTable *) NULL)
		{ fprintf (stderr,"Invalid Table in: _RGISEditFieldsCBK ()\n"); return; }
	
	if ((UITableRedefineFields (table) == DBFault) && 
		 ((tableCLS = (UITable *) dbData->Display (UITableName (dbData,table))) != (UITable *) NULL))
		{
		dbData->DispRemove (tableCLS);
		delete tableCLS;
		}
	}

static void _RGISEditDeleteFieldCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;
	static Widget tableSelect = (Widget) NULL;
	static Widget fieldSelect = (Widget) NULL;

	widget = widget; data = data; callData = callData;
	if (tableSelect == (Widget) NULL) tableSelect = UISelectionCreate ("Table Selection");
	if (fieldSelect == (Widget) NULL) fieldSelect = UISelectionCreate ("Field Selection");
	selection = UISelectObject (tableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ());
	if (selection == (char *) NULL) return;
	if ((table = dbData->Table (selection)) == (DBObjTable *) NULL) return;
	selection = UISelectObject(fieldSelect,(DBObjectLIST<DBObject> *)(table->Fields()),DBTableFieldIsOptional);
	if (selection != (char *) NULL) table->DeleteField (table->Field (selection)); 
	}

static void _RGISEditDeleteSelectionCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBInt recID;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table, *groups;
	DBObjRecord *record;
	static Widget tableSelect = (Widget) NULL;
	widget = widget; data = data; callData = callData;
	
	if (tableSelect == (Widget) NULL) tableSelect = UISelectionCreate ("Table Selection");
	selection = UISelectObject (tableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ());
	if (selection == (char *) NULL) return;
	if (strcmp (selection,DBrNGroups) == 0) selection = DBrNItems;
	if ((table = dbData->Table (selection)) == (DBObjTable *) NULL) return;
	if (strcmp (selection,DBrNCells) != 0)
		{
		if (strcmp (selection,DBrNItems) == 0)
				groups = dbData->Table (DBrNGroups);
		else	groups = (DBObjTable *) NULL;

		for (recID = 0;recID < table->ItemNum ();++recID)
			{
			record = table->Item (recID);
			if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
				{
				table->Delete (record);
				if (groups != (DBObjTable *) NULL) groups->Delete (groups->Item (recID));
				recID--;
				}
			}
		}
	else
		{
		DBNetworkIO *netIO = new DBNetworkIO (dbData);
		for (recID = 0;recID < table->ItemNum ();++recID)
			{
			record = table->Item (recID);
			if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
				netIO->CellDelete (table->Item (recID));
			}
		UIPauseDialogOpen ("Re-building Network");
		netIO->Build ();
		UIPauseDialogClose ();
		delete netIO;
		}
	}

static void _RGISEditSymbolCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *symData = dataset->Data ();
	DBObjTable *symbols = symData->Table (DBrNSymbols);
	
	widget = widget; data = data; callData = callData;
	switch (symData->Type ())
		{
		case DBTypeVectorPoint: UISymbolEdit (symbols,UISymbolMarker);	break;
		case DBTypeVectorLine:  UISymbolEdit (symbols,UISymbolLine);	break;
		case DBTypeVectorPolygon: 
		case DBTypeGridDiscrete:UISymbolEdit (symbols,UISymbolShade);	break;
		case DBTypeNetwork:		UISymbolEdit (symbols,UISymbolStick);	break;
		}
	}

static void _RGISEditSymbolFieldCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *fieldName;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table = dbData->Table (DBrNItems);
	static Widget fieldSelect = (Widget) NULL;
	widget = widget; data = data; callData = callData;
	
	if (table == (DBObjTable *) NULL) return;
	if (fieldSelect == (Widget) NULL) fieldSelect = UISelectionCreate ("Field Selection");
	if ((fieldName = UISelectObject (fieldSelect,(DBObjectLIST<DBObject> *) (table->Fields ()),DBTableFieldIsCategory)) == (char *) NULL) return;
	UIPauseDialogOpen ("Building Symbols");
	RGlibGenFuncSymbolField (dbData,fieldName);
	UIPauseDialogClose ();
	}

static UIMenuItem _RGISEditFieldsMenu [] = {
	UIMenuItem ("Add",				RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISEditAddFieldCBK,				"RGIS22MenuSystem.html#EditAll_Fields_Add"),
	UIMenuItem ("Redefine",			RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISEditRedefineFieldCBK,			"RGIS22MenuSystem.html#EditAll_Fields_Redefine"),
	UIMenuItem ("Delete",			RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISEditDeleteFieldCBK,			"RGIS22MenuSystem.html#EditAll_Fields_Delete"),
	UIMenuItem ()};

extern void RGISEditPointAddXYCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditPointSTNCoordsCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISEditPointSTNCoordsAttrCBK (Widget,void *,XmAnyCallbackStruct *);

extern void RGISEditLineDirectionCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISEditPolyFourColorCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISEditGridDateLayersCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditGridRenameLayerCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditGridStatsCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditGridNetFilterCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditGridRemovePitsCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISEditNetBuildCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditNetTrimCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditNetAddCellXYCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditNetAddBasinXYCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditNetMagnitudeCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditNetDistToMouthCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditNetDistToOceanCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISEditAdjustNetworkCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

UIMenuItem RGISEditMenu [] = {
	UIMenuItem ("Name Field",		RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISEditNameFieldCBK,				"RGIS22MenuSystem.html#EditAll_NameField"),
	UIMenuItem ("Join Table",		RGISDataGroup,		UIMENU_NORULE,	(UIMenuCBK) _RGISEditJoinTableCBK,				"RGIS22MenuSystem.html#EditAll_JoinTable"),
	UIMenuItem ("Fields",			RGISDataGroup,		UIMENU_NORULE,	_RGISEditFieldsMenu),
	UIMenuItem ("Delete Selection",RGISDataGroup,	UIMENU_NORULE,	(UIMenuCBK) _RGISEditDeleteSelectionCBK,		"RGIS22MenuSystem.html#EditAll_DeleteSelection"),
	UIMenuItem ("Symbol Field",	RGISSymbolGroup,	UIMENU_NORULE,	(UIMenuCBK) _RGISEditSymbolFieldCBK,			"RGIS22MenuSystem.html#EditAll_SymbolField"),
	UIMenuItem ("Symbols",			RGISSymbolGroup,	UIMENU_NORULE,	(UIMenuCBK) _RGISEditSymbolCBK,					"RGIS22MenuSystem.html#EditAll_Symbols"),
	UIMenuItem (RGISPointGroup,	UIMENU_NORULE),
	UIMenuItem ("AddXY",				RGISPointGroup,	UIMENU_NORULE,	(UIMenuCBK) RGISEditPointAddXYCBK,				"RGIS22MenuSystem.html#EditPoint_AddXY"),
	UIMenuItem ("STN Coordinates",RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISEditPointSTNCoordsCBK,"RGIS22MenuSystem.html#EditPoint_STNCoord"),
	UIMenuItem (RGISLineGroup,		UIMENU_NORULE),
	UIMenuItem ("River Directions",RGISLineGroup,	RGISLinkedContinuousGroup,(UIMenuCBK) RGISEditLineDirectionCBK,"RGIS22MenuSystem.html#EditLine_RiverDirection"),
	UIMenuItem (RGISPolyGroup,		UIMENU_NORULE),
	UIMenuItem ("Four Color",		RGISPolyGroup,		UIMENU_NORULE,	(UIMenuCBK) RGISEditPolyFourColorCBK,			"RGIS22MenuSystem.html#EditPoly_FourColor"),
	UIMenuItem (RGISGridGroup,		UIMENU_NORULE),
	UIMenuItem ("Date Layers",		RGISGridGroup,		UIMENU_NORULE,	(UIMenuCBK) RGISEditGridDateLayersCBK,			"RGIS22MenuSystem.html#EditGrid_DateLayers"),
	UIMenuItem ("Rename Layer",	RGISGridGroup,		UIMENU_NORULE,	(UIMenuCBK) RGISEditGridRenameLayerCBK,		"RGIS22MenuSystem.html#EditGrid_RenameLayer"),
	UIMenuItem ("Grid Statistics",RGISDiscreteGroup,UIMENU_NORULE,	(UIMenuCBK) RGISEditGridStatsCBK,				"RGIS22MenuSystem.html#EditGrid_GridStats"),
	UIMenuItem ("Remove Pits",		RGISContinuousGroup,RGISLinkedNetworkGroup,(UIMenuCBK) RGISEditGridRemovePitsCBK,"RGIS22MenuSystem.html#EditGrid_RemovePits"),
	UIMenuItem ("Network Filter",	RGISContinuousGroup,RGISLinkedNetworkGroup,(UIMenuCBK) RGISEditGridNetFilterCBK,"RGIS22MenuSystem.html#EditGrid_NetworkFilter"),
	UIMenuItem (RGISNetworkGroup,	UIMENU_NORULE),
	UIMenuItem ("Build",				RGISNetworkGroup, UIMENU_NORULE, (UIMenuCBK) RGISEditNetBuildCBK,					"RGIS22MenuSystem.html#EditNet_Build"),
	UIMenuItem ("Trim",				RGISNetworkGroup, UIMENU_NORULE, (UIMenuCBK) RGISEditNetTrimCBK,					"RGIS22MenuSystem.html#EditNet_Trim"),
	UIMenuItem ("AddCellXY",		RGISNetworkGroup,	UIMENU_NORULE,	(UIMenuCBK) RGISEditNetAddCellXYCBK,			"RGIS22MenuSystem.html#EditNet_AddCellXY"),
	UIMenuItem ("AddBasinXY",		RGISNetworkGroup,	UIMENU_NORULE,	(UIMenuCBK) RGISEditNetAddBasinXYCBK,			"RGIS22MenuSystem.html#EditNet_AddBasinXY"),
	UIMenuItem ("Magnitude",		RGISNetworkGroup,	UIMENU_NORULE,	(UIMenuCBK)	RGISEditNetMagnitudeCBK,			"RGIS22MenuSystem.html#EditNet_Magnitude"),
	UIMenuItem ("Distance to Mouth",RGISNetworkGroup,UIMENU_NORULE,(UIMenuCBK)	RGISEditNetDistToMouthCBK,			"RGIS22MenuSystem.html#EditNet_DistMouth"),
	UIMenuItem ("Distance to Ocean",RGISNetworkGroup,UIMENU_NORULE,(UIMenuCBK)	RGISEditNetDistToOceanCBK,			"RGIS22MenuSystem.html#EditNet_DistOcean"),
	UIMenuItem ("Adjust Network",	RGISNetworkGroup,	RGISLinkedLineGroup,		(UIMenuCBK) RGISEditAdjustNetworkCBK,"RGIS22MenuSystem.html#EditNet_Adjust"),
	UIMenuItem ()};