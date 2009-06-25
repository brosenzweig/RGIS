/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISPanels.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <rgis.H>

RGISPanel::RGISPanel (Widget parent,Widget leftWidget)

	{
	FormWGT = XtVaCreateWidget ("RGISPanelForm",xmFormWidgetClass,parent,
									XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
									XmNtopWidget,				leftWidget,
									XmNleftAttachment,		XmATTACH_WIDGET,
									XmNleftWidget,				leftWidget,
									XmNleftOffset,				5,
									XmNrightAttachment,		XmATTACH_FORM,
									XmNrightOffset,			5,
									XmNbottomAttachment,		XmATTACH_FORM,
									XmNbottomOffset,			5,
									XmNshadowThickness,		1,
									NULL);
	}

static void _RGISPanelFlagsToggleCBK (Widget widget,void *Flags,XmToggleButtonCallbackStruct *callData)

	{
	DBObjData *data;

	XtVaGetValues (widget,XmNuserData, &data, NULL);
	data->Flags ((DBUnsigned) ((char *) Flags - (char *) NULL),callData->set ? DBSet : DBClear);
	}

static DBUnsigned _RGISVectorControlCodes [] = { DBDataFlagDispModeVecAnnotNone, DBDataFlagDispModeVecAnnotate, DBDataFlagDispModeVecAnnotSelect };

RGISVecPanel::RGISVecPanel (Widget parent, Widget leftWidget) : RGISPanel (parent,leftWidget)

	{
	int toggle;
	char *toggleNames [] = { (char *) "None", (char *) "Selected", (char *) "All" };
	XmString string;
	Widget label, toggleB;

	string = XmStringCreate ((char *) "Annotation:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISVecPanelVerticalLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				25,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				20,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	RowColumnWGT = XtVaCreateManagedWidget ("RGISVecPanelRowColumn",xmRowColumnWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				25,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			20,
											XmNorientation,			XmVERTICAL,
											XmNpacking,					XmPACK_COLUMN,
											XmNspacing,					0,
											XmNmarginWidth,			0,
											XmNmarginHeight,			0,
											XmNnumColumns,				1,
											XmNradioBehavior,			True,
											XmNradioAlwaysOne,		True,
											NULL);
	for (toggle = 0;toggle < (int) (sizeof (toggleNames) / sizeof (char *));++toggle)
		{
		string = XmStringCreate (toggleNames [toggle],UICharSetNormal);
		toggleB = XtVaCreateManagedWidget ("RGISVecPanelToggleWGT",xmToggleButtonGadgetClass,RowColumnWGT,
											XmNlabelString,			string,
											XmNhighlightThickness,	0,
											XmNmarginHeight,			1,
											XmNset,						True,
											NULL);
		XmStringFree (string);
		XtAddCallback (toggleB,XmNvalueChangedCallback,(XtCallbackProc) _RGISPanelFlagsToggleCBK,(void *) _RGISVectorControlCodes [toggle]);
		}

	string = XmStringCreate ((char *) "Display",UICharSetNormal);
	ActiveToggleWGT = XtVaCreateManagedWidget ("RGISVecPanelActiveToggleWGT",xmToggleButtonGadgetClass,Form (),
											XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNleftWidget,				RowColumnWGT,
											XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNrightWidget,			RowColumnWGT,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			25,
											XmNlabelString,			string,
											XmNhighlightThickness,	0,
											XmNset,						True,
											NULL);
	XmStringFree (string);
	XtAddCallback (ActiveToggleWGT,XmNvalueChangedCallback,(XtCallbackProc) _RGISPanelFlagsToggleCBK,(void *) DBDataFlagHideIdle);
	string = XmStringCreate ((char *) "Idle Items:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISVecPanelVerticalLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				ActiveToggleWGT,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				20,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			ActiveToggleWGT,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	}

Widget RGISVecPanel::Load (DBObjData *data)

	{
	int toggle, toggleNum;
	WidgetList toggles;

	XtVaGetValues (RowColumnWGT,XmNchildren, &toggles, XmNnumChildren, &toggleNum, NULL);
	for (toggle = 0;toggle < toggleNum; ++toggle)
		{
		XtVaSetValues (toggles [toggle],XmNuserData, data, NULL);
		XmToggleButtonGadgetSetState (toggles [toggle],(data->Flags () & DBDataFlagDispModeFlags) == _RGISVectorControlCodes [toggle],False);
		}
	XtVaSetValues (ActiveToggleWGT,XmNuserData, data, NULL);
	XmToggleButtonGadgetSetState (ActiveToggleWGT,(data->Flags () & DBDataFlagHideIdle) == DBDataFlagHideIdle,False);
	return (Form ());
	}

static void _RGISGridLayerListCBK (Widget widget,void *dummy, XmListCallbackStruct *callData)

	{
	int item = 0;
	DBObjData *data;
	DBObjTable *layerTable;
	DBObjRecord *record;

	dummy = dummy;
	XtVaGetValues (widget,XmNuserData, &data, NULL);
	layerTable = data->Table (DBrNLayers);
	for (record = layerTable->First ();record != (DBObjRecord *) NULL;record = layerTable->Next ())
		if (++item == callData->item_position)	layerTable->Item (record->RowID (),true);
	}

RGISGrdPanel::RGISGrdPanel (Widget parent, Widget leftWidget) : RGISPanel (parent,leftWidget)

	{
	int argNum;
	Arg wargs [16];
	Widget label;
	XmString string;

	string = XmStringCreate ((char *) "Layers:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISGrdPanelLayersLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				20,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);

	argNum = 0;
	XtSetArg (wargs [argNum],	XmNtopAttachment,				XmATTACH_WIDGET);	++argNum;
	XtSetArg (wargs [argNum],	XmNtopWidget,					label);				++argNum;
	XtSetArg (wargs [argNum],	XmNtopOffset,					5);					++argNum;
	XtSetArg (wargs [argNum],	XmNleftAttachment,			XmATTACH_FORM);	++argNum;
	XtSetArg (wargs [argNum],	XmNleftOffset,					20); 					++argNum;
	XtSetArg (wargs [argNum],	XmNrightAttachment,			XmATTACH_FORM);	++argNum;
	XtSetArg (wargs [argNum],	XmNrightOffset,				20); 					++argNum;
	XtSetArg (wargs [argNum],	XmNscrollingPolicy,			XmAUTOMATIC);		++argNum;
	XtSetArg (wargs [argNum],	XmNscrollBarDisplayPolicy,	XmAS_NEEDED);		++argNum;
	XtSetArg (wargs [argNum],	XmNvisualPolicy,				XmVARIABLE);		++argNum;
	XtSetArg (wargs [argNum],	XmNvisibleItemCount,			5);					++argNum;
	XtSetArg (wargs [argNum],	XmNshadowThickness,			2);					++argNum;
	XtSetArg (wargs [argNum],	XmNselectionPolicy,			XmSINGLE_SELECT);	++argNum;
	XtSetArg (wargs [argNum],	XmNtextColumns,				DBStringLength);	++argNum;
	XtManageChild (LayerListWGT = XmCreateScrolledList (Form (),(char *) "RGISGrdPanelLayerList",wargs,argNum));
	XtAddCallback (LayerListWGT,XmNsingleSelectionCallback,	(XtCallbackProc) _RGISGridLayerListCBK,(void *) NULL);

	string = XmStringCreate ((char *) "Dynamic Shades",UICharSetBold);
	DynamicToggleWGT = XtVaCreateManagedWidget ("RGISGrdPanelDynamicToggleWGT",xmToggleButtonGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				LayerListWGT,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				20,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNlabelString,			string,
											XmNhighlightThickness,	0,
											XmNshadowThickness,		0,
											XmNset,						False,
											NULL);
	XmStringFree (string);

	string = XmStringCreate ((char *) "Annotate",UICharSetBold);
	AnnotateToggleWGT = XtVaCreateManagedWidget ("RGISNetGrdPanelAnnotToggleWGT",xmToggleButtonGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				LayerListWGT,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			20,
											XmNlabelString,			string,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNhighlightThickness,	0,
											XmNshadowThickness,		0,
											XmNset,						False,
											NULL);
	XmStringFree (string);
	}

Widget RGISGrdPanel::Load (DBObjData *data)

	{
	int itemPosition = 0;
	DBObjTable *layerTable = data->Table (DBrNLayers);
	DBObjRecord *record;
	XmString string;

	XmListDeleteAllItems (LayerListWGT);
	for (record = layerTable->First ();record != (DBObjRecord *) NULL;record = layerTable->Next ())
		{
		string = XmStringCreate (record->Name (),UICharSetNormal);
		XmListAddItem (LayerListWGT, string, ++itemPosition);
		XmStringFree (string);
		}
	XmListDeselectAllItems (LayerListWGT);
	record = layerTable->Item ();
	string = XmStringCreate (record->Name (),UICharSetNormal);
	XmListSelectItem (LayerListWGT,string,False);
	XmStringFree (string);
	XtVaSetValues (LayerListWGT,XmNuserData,	data, NULL);
	return (Form ());
	}

static void _RGISNetPanelColorCBK (Widget widget,void *Flags,XmAnyCallbackStruct *callData)

	{
	DBObjData *data;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &data, NULL);
	data->Flags (DBDataFlagDispModeNetColors,DBClear);
	data->Flags ((DBUnsigned) ((char *) Flags - (char *) NULL),DBSet);
	}

class RGISNetPanelColor

	{
	private:
		char *NameSTR;
		DBUnsigned FlagVAR;
	public:
		RGISNetPanelColor (char *name,DBUnsigned flag) { NameSTR = name; FlagVAR = flag; }
		void MakeButton (Widget widget)
			{
			XmString string;
			Widget button;

			string = XmStringCreate (NameSTR,UICharSetNormal);
			button = XtVaCreateManagedWidget ("RGISNetworkInfoColorMenuButton",xmPushButtonGadgetClass,widget,
														XmNlabelString,			string, NULL);
			XmStringFree (string);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISNetPanelColorCBK,(void *) FlagVAR);
			}
		DBInt IsColor (DBUnsigned flag) { return (FlagVAR == flag ? true : false); }
	};

static RGISNetPanelColor _RGISNetPanelColors [] = {
	RGISNetPanelColor ((char *) "Uniform", DBDataFlagDispModeNetColorUniform),
	RGISNetPanelColor ((char *) "Basins",	DBDataFlagDispModeNetColorBasin),
	RGISNetPanelColor ((char *) "Symbol",	DBDataFlagDispModeNetColorSymbol)};

RGISNetPanel::RGISNetPanel (Widget parent, Widget leftWidget) : RGISPanel (parent,leftWidget)

	{
	int button;
	Widget rowCol, label;
	XmString string;

	ColorMenuWGT = XmCreatePulldownMenu (Form (),(char *) "RGISNetPanelColorMenu",NULL,0);

	for (button = 0;button < (int) (sizeof (_RGISNetPanelColors) / sizeof (RGISNetPanelColor)); ++button)
		_RGISNetPanelColors [button].MakeButton (ColorMenuWGT);

	string = XmStringCreate ((char *) "Color Code:",UICharSetBold);
	ColorMenuWGT = XtVaCreateManagedWidget ("RGISNetPanelColorMenu",xmRowColumnWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				25,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			15,
											XmNsubMenuId,				ColorMenuWGT,
											XmNlabelString,			string,
											XmNrowColumnType,			XmMENU_OPTION,
											XmNtraversalOn,			False,
											NULL);
	XmStringFree (string);

	string = XmStringCreate ((char *) "Stick Symbol:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISNetPanelSymbolLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				ColorMenuWGT,
											XmNtopOffset,				25,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				20,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	rowCol = XtVaCreateManagedWidget ("RGISNetPanelRowColumn",xmRowColumnWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				ColorMenuWGT,
											XmNtopOffset,				25,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			20,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNorientation,			XmVERTICAL,
											XmNpacking,					XmPACK_COLUMN,
											XmNspacing,					0,
											XmNmarginWidth,			0,
											XmNmarginHeight,			0,
											XmNnumColumns,				1,
											XmNradioBehavior,			False,
											XmNradioAlwaysOne,		False,
											NULL);
	string = XmStringCreate ((char *) "Direction",UICharSetNormal);
	FlowDirToggleWGT = XtVaCreateManagedWidget ("RGISNetPanelFlowDirToggleWGT",xmToggleButtonGadgetClass,rowCol,
											XmNlabelString,			string,
											XmNhighlightThickness,	0,
											XmNset,						True,
											NULL);
	XmStringFree (string);
	XtAddCallback (FlowDirToggleWGT,XmNvalueChangedCallback,(XtCallbackProc) _RGISPanelFlagsToggleCBK,(void *) DBDataFlagDispModeNetDirection);
	string = XmStringCreate ((char *) "Active Only",UICharSetNormal);
	ActiveToggleWGT = XtVaCreateManagedWidget ("RGISNetPanelActiveToggleWGT",xmToggleButtonGadgetClass,rowCol,
											XmNlabelString,			string,
											XmNhighlightThickness,	0,
											XmNset,						True,
											NULL);
	XmStringFree (string);
	XtAddCallback (ActiveToggleWGT,XmNvalueChangedCallback,(XtCallbackProc) _RGISPanelFlagsToggleCBK,(void *) DBDataFlagHideIdle);
	}

Widget RGISNetPanel::Load (DBObjData *data)

	{
	int button;
	Widget menu;
	WidgetList buttons;

	XtVaGetValues (ColorMenuWGT,XmNsubMenuId,	 &menu, NULL);
	XtVaGetValues (menu,XmNchildren,	&buttons, NULL);
	for (button = 0;button < (int) (sizeof (_RGISNetPanelColors) / sizeof (RGISNetPanelColor)); ++button)
		{
		XtVaSetValues (buttons [button],XmNuserData, data, NULL);
		if (_RGISNetPanelColors [button].IsColor (data->Flags () & DBDataFlagDispModeNetColors))
			XtVaSetValues (ColorMenuWGT,XmNmenuHistory, buttons [button], NULL);
		}
	XtVaSetValues (FlowDirToggleWGT,	XmNuserData, data, NULL);
	XmToggleButtonGadgetSetState (FlowDirToggleWGT,(data->Flags () & DBDataFlagDispModeNetDirection) == DBDataFlagDispModeNetDirection,False);
	XtVaSetValues (ActiveToggleWGT,	XmNuserData, data, NULL);
	XmToggleButtonGadgetSetState (ActiveToggleWGT,((data->Flags () & DBDataFlagHideIdle) == DBDataFlagHideIdle),False);
	return (Form ());
	}

RGISTblPanel::RGISTblPanel (Widget parent, Widget leftWidget) : RGISPanel (parent,leftWidget)

	{
	}

Widget RGISTblPanel::Load (DBObjData *data)

	{
	data = data;
	return (Form ());
	}



static void _RGISGrpPanelNewCBK (Widget widget,RGISGrpPanel *grpPanel,XmAnyCallbackStruct *callData)

	{
	char *groupName;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	DBObjTable *groupTable = dbData->Table (DBrNGroups);
	DBObjTableField *group;
	DBObjRecord *record;

	widget = widget; callData = callData;
	if ((groupName = UIGetString ((char *) "Group Name",16)) == (char *) NULL) return;
	if (groupTable == (DBObjTable *) NULL)
		{
		DBObjTable *items  = dbData->Table (DBrNItems);
		DBObjectLIST<DBObjTable> *tables  = dbData->Tables ();

		DBTableFieldDefinition fieldDefs [] =	{
					DBTableFieldDefinition (groupName,	DBTableFieldInt,(char *) 	"%1d",sizeof (DBByte)),
					DBTableFieldDefinition () };
		tables->Add (groupTable = new DBObjTable (DBrNGroups,fieldDefs));
		grpPanel->Groups (groupTable->Fields ());
		group = groupTable->Field (groupName);
		for (record = items->First ();record != (DBObjRecord *) NULL;record = items->Next ())
			group->Int (groupTable->Add (record->Name ()),true);
		}
	else
		{
		groupTable->AddField (group = new DBObjTableField (groupName,DBTableFieldInt,"%1d",sizeof (DBByte)));
		for (record = groupTable->First ();record != (DBObjRecord *) NULL;record = groupTable->Next ())
			group->Int (record,true);
		}
	grpPanel->Add (group);
	}

static void _RGISGrpPanelDeleteCBK (Widget widget,RGISGrpPanel *grpPanel,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	DBObjTable *groupTable = dbData->Table (DBrNGroups);
	DBObjTableField *group = grpPanel->Current ();
	static Widget fieldSelect = (Widget) NULL;

	widget = widget; callData = callData;
	if (fieldSelect == (Widget) NULL) fieldSelect = UISelectionCreate ((char *) "Group Selection");

	if ((groupTable == (DBObjTable *) NULL) || (group == (DBObjTableField *) NULL)) return;
	if (groupTable->FieldNum () == 1) grpPanel->Groups ((DBObjectLIST<DBObjTableField> *) NULL);
	grpPanel->Remove (group);
	groupTable->DeleteField (group);
	if (groupTable->FieldNum () == 0)
		{
		DBObjectLIST<DBObjTable> *tables  = dbData->Tables ();
		tables->Remove (groupTable);
		delete groupTable;
		}
	}

static void _RGISGrpPanelLoadCBK (Widget widget,RGISGrpPanel *grpPanel,XmAnyCallbackStruct *callData)

	{
	DBInt rowID;
	DBDataset  *dataset= UIDataset ();
	DBObjData  *dbData = dataset->Data ();
	DBObjTable *itemTable  = dbData->Table (DBrNItems);
	DBObjTable *groupTable = dbData->Table (DBrNGroups);
	DBObjTableField *group;
	DBObjRecord *itemRec, *groupRec;
	UITable *tableCLS;

	widget = widget; callData = callData;
	if (groupTable == (DBObjTable *) NULL) return;
	group = grpPanel->Current ();

	for (rowID = 0;rowID < groupTable->ItemNum ();++rowID)
		{
		groupRec = groupTable->Item (rowID);
		itemRec = itemTable->Item (rowID);
		itemRec->Flags (DBObjectFlagSelected,group->Int (groupRec));
		}
	if ((tableCLS = (UITable *) dbData->Display (UITableName (dbData,itemTable))) != (UITable *) NULL)
		tableCLS->Draw ();
	}

static void _RGISGrpPanelSaveCBK (Widget widget,RGISGrpPanel *grpPanel,XmAnyCallbackStruct *callData)

	{
	DBInt rowID;
	DBDataset  *dataset= UIDataset ();
	DBObjData  *dbData = dataset->Data ();
	DBObjTable *itemTable  = dbData->Table (DBrNItems);
	DBObjTable *groupTable = dbData->Table (DBrNGroups);
	DBObjTableField *group;
	DBObjRecord *itemRec, *groupRec;
	UITable *tableCLS;

	widget = widget; callData = callData;
	if (groupTable == (DBObjTable *) NULL) return;
	group = grpPanel->Current ();
	for (rowID = 0;rowID < groupTable->ItemNum ();++rowID)
		{
		groupRec = groupTable->Item (rowID);
		itemRec = itemTable->Item (rowID);
		group->Int (groupRec,(itemRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ? true : false);
		}
	if ((tableCLS = (UITable *) dbData->Display (UITableName (dbData,groupTable))) != (UITable *) NULL)
		tableCLS->Draw ();
	}

static void _RGISGridGroupListCBK (Widget widget,RGISGrpPanel *grpPanel, XmListCallbackStruct *callData)

	{
	DBObjData *data;
	DBObjTable *groupTable;
	DBObjectLIST<DBObjTableField> *groups;
	DBObjTableField *group = (DBObjTableField *) NULL;

	XtVaGetValues (widget,XmNuserData, &data, NULL);
	if (data == (DBObjData *) NULL) return;
	if ((groupTable = data->Table (DBrNGroups)) != (DBObjTable *) NULL)
		{
		groups = groupTable->Fields ();
		if ((group = groups->Item (callData->item_position - 1)) != (DBObjTableField *) NULL)
			grpPanel->Current (group);
		}
	}

RGISGrpPanel::RGISGrpPanel (Widget parent, Widget leftWidget) : RGISPanel (parent,leftWidget)

	{
	int argNum;
	Arg wargs [18];
	XmString string;
	Widget newButton, subForm;

	subForm = XtVaCreateManagedWidget ("RGISGrpPanelSubForm",xmFormWidgetClass,Form (),
									XmNleftAttachment,		XmATTACH_FORM,
									XmNleftOffset,				15,
									XmNrightAttachment,		XmATTACH_FORM,
									XmNrightOffset,			15,
									XmNbottomAttachment,		XmATTACH_FORM,
									XmNbottomOffset,			15,
									XmNshadowThickness,		0,
									NULL);
	string = XmStringCreate ((char *) "New",UICharSetBold);
	newButton = XtVaCreateManagedWidget ("RGISGrpPanelNewButtonWGT",xmPushButtonGadgetClass,subForm,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			25,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	XtAddCallback (newButton,XmNactivateCallback,(XtCallbackProc) _RGISGrpPanelNewCBK,(void *) this);
	string = XmStringCreate ((char *) "Delete",UICharSetBold);
	DeleteButtonWGT = XtVaCreateManagedWidget ("RGISGrpPanelLoadButtonWGT",xmPushButtonGadgetClass,subForm,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			25,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			50,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNlabelString,			string,
											NULL);
	XtSetSensitive (DeleteButtonWGT,False);
	XmStringFree (string);
	XtAddCallback (DeleteButtonWGT,XmNactivateCallback,(XtCallbackProc) _RGISGrpPanelDeleteCBK,(void *) this);
	string = XmStringCreate ((char *) "Load",UICharSetBold);
	LoadButtonWGT = XtVaCreateManagedWidget ("RGISGrpPanelLoadButtonWGT",xmPushButtonGadgetClass,subForm,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			50,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			75,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNlabelString,			string,
											NULL);
	XtSetSensitive (LoadButtonWGT,False);
	XmStringFree (string);
	XtAddCallback (LoadButtonWGT,XmNactivateCallback,(XtCallbackProc) _RGISGrpPanelLoadCBK,(void *) this);
	string = XmStringCreate ((char *) "Save",UICharSetBold);
	SaveButtonWGT = XtVaCreateManagedWidget ("RGISGrpPanelSaveButtonWGT",xmPushButtonGadgetClass,subForm,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			75,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNlabelString,			string,
											NULL);
	XtSetSensitive (SaveButtonWGT,False);
	XmStringFree (string);
	XtAddCallback (SaveButtonWGT,XmNactivateCallback,(XtCallbackProc) _RGISGrpPanelSaveCBK,(void *) this);
	argNum = 0;
	XtSetArg (wargs [argNum],	XmNtopAttachment,				XmATTACH_FORM);	++argNum;
	XtSetArg (wargs [argNum],	XmNtopOffset,					15);					++argNum;
	XtSetArg (wargs [argNum],	XmNleftAttachment,			XmATTACH_FORM);	++argNum;
	XtSetArg (wargs [argNum],	XmNleftOffset,					15); 					++argNum;
	XtSetArg (wargs [argNum],	XmNrightAttachment,			XmATTACH_FORM);	++argNum;
	XtSetArg (wargs [argNum],	XmNrightOffset,				15); 					++argNum;
	XtSetArg (wargs [argNum],	XmNbottomAttachment,			XmATTACH_WIDGET);	++argNum;
	XtSetArg (wargs [argNum],	XmNbottomWidget,				subForm); 			++argNum;
	XtSetArg (wargs [argNum],	XmNbottomOffset,				10); 					++argNum;
	XtSetArg (wargs [argNum],	XmNscrollingPolicy,			XmAUTOMATIC);		++argNum;
	XtSetArg (wargs [argNum],	XmNscrollBarDisplayPolicy,	XmAS_NEEDED);		++argNum;
	XtSetArg (wargs [argNum],	XmNvisualPolicy,				XmVARIABLE);		++argNum;
	XtSetArg (wargs [argNum],	XmNshadowThickness,			2);					++argNum;
	XtSetArg (wargs [argNum],	XmNselectionPolicy,			XmSINGLE_SELECT);	++argNum;
	XtSetArg (wargs [argNum],	XmNtextColumns,				DBStringLength);	++argNum;
	XtManageChild (GroupListWGT = XmCreateScrolledList (Form (),(char *) "RGISGrpPanelList",wargs,argNum));
	XtAddCallback (GroupListWGT,XmNsingleSelectionCallback,	(XtCallbackProc) _RGISGridGroupListCBK,(void *) this);
	GroupLIST = (DBObjectLIST<DBObjTableField> *) NULL;
	}

Widget RGISGrpPanel::Load (DBObjData *data)

	{
	DBObjTable *groupTable = data->Table (DBrNGroups);

	XmListDeleteAllItems (GroupListWGT);
	if (groupTable != (DBObjTable *) NULL)
		{
		DBObjTableField *group;

		GroupLIST = groupTable->Fields ();
		for (group = GroupLIST->First ();group != (DBObjTableField *) NULL;group = GroupLIST->Next ())
			Add (group);
		XtSetSensitive (DeleteButtonWGT,True);
		XtSetSensitive (LoadButtonWGT,True);
		XtSetSensitive (SaveButtonWGT,True);
		XtVaSetValues (GroupListWGT,XmNuserData, data, NULL);
		}
	else
		{
		XtSetSensitive (DeleteButtonWGT,False);
		XtSetSensitive (LoadButtonWGT,False);
		XtSetSensitive (SaveButtonWGT,False);
		}
	return (Form ());
	}

void RGISGrpPanel::Add (DBObjTableField *group)

	{
	XmString string;
	string = XmStringCreate (group->Name (),UICharSetNormal);
	XmListAddItem (GroupListWGT,string,group->RowID () + 1);
	XmListSelectItem (GroupListWGT,string,True);
	XmStringFree (string);
	XtSetSensitive (DeleteButtonWGT,True);
	XtSetSensitive (LoadButtonWGT,True);
	XtSetSensitive (SaveButtonWGT,True);
	}

void RGISGrpPanel::Remove (DBObjTableField *group)

	{
	int *selectPos, selectCount;

	if(!XmListGetSelectedPos (GroupListWGT,&selectPos,&selectCount)) return;
	if (selectCount > 1) { XtFree ((char *) selectPos); return; }
	XmListDeleteItemsPos (GroupListWGT,1,selectPos [0]);
	if (GroupLIST != (DBObjectLIST<DBObjTableField> *) NULL)
		XmListSelectPos (GroupListWGT,selectPos [0],True);
	}
