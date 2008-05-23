/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISWorkspace.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <rgis.H>

static RGISControlMode _RGISUserModes [] = {
	RGISControlMode ("Query",			DBTypeVectorPoint,	DBDataFlagUserModeQuery,		false),
	RGISControlMode ("Select",			DBTypeVectorPoint,	DBDataFlagUserModeSelect,		false),
	RGISControlMode ("Add",				DBTypeVectorPoint,	DBDataFlagUserModeAdd,			true),
	RGISControlMode ("Move",			DBTypeVectorPoint,	DBDataFlagUserModeMove,			true),
	RGISControlMode ("Delete",			DBTypeVectorPoint,	DBDataFlagUserModeDelete,		true),

	RGISControlMode ("Query",			DBTypeVectorLine,		DBDataFlagUserModeQuery,		false),
	RGISControlMode ("Select",			DBTypeVectorLine,		DBDataFlagUserModeSelect,		false),
	RGISControlMode ("Flip",			DBTypeVectorLine,		DBDataFlagUserModeFlip,			true),

	RGISControlMode ("Query",			DBTypeVectorPolygon,	DBDataFlagUserModeQuery,		false),
	RGISControlMode ("Select",			DBTypeVectorPolygon,	DBDataFlagUserModeSelect,		false),
			
	RGISControlMode ("Query",			DBTypeGridDiscrete,	DBDataFlagUserModeQuery,		false),
			
	RGISControlMode ("Query",			DBTypeGridContinuous,DBDataFlagUserModeQuery,		false),

	RGISControlMode ("Query",			DBTypeNetwork,			DBDataFlagUserModeQuery,		false),
	RGISControlMode ("Select",			DBTypeNetwork,			DBDataFlagUserModeSelect,		false),
	RGISControlMode ("Add",				DBTypeNetwork,			DBDataFlagUserModeAdd,			true),
	RGISControlMode ("Delete",			DBTypeNetwork,			DBDataFlagUserModeDelete,		true),
	RGISControlMode ("Rotate",			DBTypeNetwork,			DBDataFlagUserModeRotate,		true),
			
	RGISControlMode ("Query",			DBTypeTable,			DBDataFlagUserModeQuery,		false)};

static RGISControlMode _RGISSelectModes [] = {
	RGISControlMode ("One",				DBTypeVectorPoint,	(int) 0x0L,							false),
	RGISControlMode ("Many",			DBTypeVectorPoint,	DBDataFlagSelectMode,			false),

	RGISControlMode ("One",				DBTypeVectorLine,		(int) 0x0L,							false),
	RGISControlMode ("Many",			DBTypeVectorLine,		DBDataFlagSelectMode,			false),

	RGISControlMode ("One",				DBTypeVectorPolygon,	(int) 0x0L,							false),
	RGISControlMode ("Many",			DBTypeVectorPolygon,	DBDataFlagSelectMode,			false),

	RGISControlMode ("Up Stream",		DBTypeNetwork,			(int) 0x0L,							false),
	RGISControlMode ("Down Stream",	DBTypeNetwork,			DBDataFlagSelectMode,			false)};

static void _RGISUserModeCBK (Widget widget,void *mode,XmAnyCallbackStruct *callData)

	{
	callData = callData;
	DBObjData *data;
	
	XtVaGetValues (widget,XmNuserData, &data, NULL);
	data->Flags (DBDataFlagUserModeFlags,DBClear);
	data->Flags ((DBUnsigned) ((char *) mode - (char *) NULL),DBSet);
	}

static void _RGISSelectModeCBK (Widget widget,void *mode,XmAnyCallbackStruct *callData)

	{
	callData = callData;
	DBObjData *data;
	
	XtVaGetValues (widget,XmNuserData, &data, NULL);
	data->Flags (DBDataFlagSelectMode,(DBInt) ((char *) mode - (char *) NULL) == DBDataFlagSelectMode ? true : false);
	}

static void _RGISDisplayModeCBK (Widget widget,void *dummy,XmToggleButtonCallbackStruct *callData)

	{
	DBObjData *data;
	
	dummy = dummy;
	XtVaGetValues (widget,XmNuserData, &data, NULL);
	data->Flags (DBDataFlagDisplay,callData->set);
	}

static void _RGISSmartSortCBK (Widget widget,DBObjectLIST<DBObjData> *dataList,XmToggleButtonCallbackStruct *callData)

	{
	widget = widget;
	dataList->Flags (DBDataLISTFlagSmartSort,callData->set);
	}

int RGISControlMode::CreateButton (Widget menu,XtCallbackProc controlModeCBK)

	{
	Dimension width;
	XmString string = XmStringCreate (NameSTR,(EditFLAG ? (char *) UICharSetItalic : (char *) UICharSetNormal));
	ButtonWDT = XtVaCreateManagedWidget ("RGISControlModesButton",xmPushButtonGadgetClass,menu,
								XmNlabelString,	string, NULL);
	XtAddCallback (ButtonWDT,XmNactivateCallback,controlModeCBK,(void *) ModeVAL);
	XmStringFree (string);
	XtVaGetValues (ButtonWDT,XmNwidth, &width, NULL);
	return ((int) width);
	}

void RGISControlMode::ShowButton (Widget menu, DBObjData *data,DBUnsigned flags)

	{
	if ((data == (DBObjData *) NULL) || (data->Type () != DataTypeVAL))
		XtUnmanageChild (ButtonWDT);
	else
		{
		XtManageChild (ButtonWDT);
		XtVaSetValues (ButtonWDT,XmNuserData, data, NULL);
		if ((data->Flags () & flags) == ModeVAL) XtVaSetValues (menu, XmNmenuHistory, ButtonWDT,NULL);
		}
	}

static void _RGISControlPageCBK (Widget widget,DBInt page,XmToggleButtonCallbackStruct *callData)

	{
	RGISWorkspace *workspace;
	
	XtVaGetValues (widget,XmNuserData, &workspace, NULL);
	if (callData->set) workspace->ControlPage (page);
	}

void RGISWorkspace::ControlPage (DBInt page)

	{
	DBInt i;
	
	for (i = 0;i < (int) (sizeof (Pages) / sizeof (Widget));++i)
		if ((Pages [i] != (Widget) NULL) && XtIsManaged (Pages [i])) XtUnmanageChild (Pages [i]);
	if ((page < i) && (Pages [page] != (Widget) NULL)) XtManageChild (Pages [page]);
	}

RGISWorkspace::~RGISWorkspace ()

	{
	}

static void _RGISTextFieldValueChangedCBK (Widget widget,char *nameSTR,XmAnyCallbackStruct *callData)

	{
	char *text = XmTextFieldGetString (widget);
	
	callData = callData;

	strcpy (nameSTR,text);
	XtFree (text);
	}

static void _RGISSelectButtonCBK (Widget widget,Widget textF,XmAnyCallbackStruct *callData)

	{
	char *(*function) (void), *string, *nameSTR;
	
	callData = callData;
	XtVaGetValues (widget,XmNuserData, (int *) (&function), NULL);
	XtVaGetValues (textF,XmNuserData, 	&nameSTR, NULL);
	if ((string = (*function) ()) != NULL)
			strcpy (nameSTR,string);
	else	strcpy (nameSTR,"");
	XmTextFieldSetString (textF,nameSTR);
	}

static void _RGISWorkspaceSetCurrentDataCBK (Widget widget, RGISWorkspace *workSpace,XmListCallbackStruct *callData)

	{
	char *dataName;
	DBDataset *dataset = UIDataset ();
	DBObjData *data;

	widget = widget;
	XmStringGetLtoR (callData->item,UICharSetNormal,&dataName);
	if ((data = dataset->Data (dataName)) == (DBObjData *) NULL)
		{ fprintf (stderr,"Data not found in : _RGISMainSetCurrentDataCBK ()\n"); return; }
	workSpace->CurrentData (data);
	}

void RGISWorkspace::Initialize (Widget mainForm)

	{
	int i;
	int maxWidth = 0, width;
	int argNum;
	Arg wargs [20];
	Widget selectionForm, textF, button, currentDataForm, dataLabel, label, controlRowCol, toggle;
	WidgetList buttons;
	XmString string;
	DBDataset *dataset = UIDataset ();
	DBObjData *data;

	Pages [0] = (Widget) NULL;
	UserModeNum = (int) (sizeof (_RGISUserModes) / sizeof (RGISControlMode));
	UserModeARR = _RGISUserModes;
	SelectModeNum = (int) (sizeof (_RGISSelectModes) / sizeof (RGISControlMode));
	SelectModeARR = _RGISSelectModes;
	strcpy (SubjectSTR,"");
	strcpy (DomainSTR,"");
	selectionForm = XtVaCreateManagedWidget ("RGISSeletionForm",xmFormWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNshadowThickness,		1,
											NULL);
	textF = XtVaCreateManagedWidget ("RGISWorkspaceSubjectTextF",xmTextFieldWidgetClass,selectionForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			12,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNmaxLength,				DBStringLength - 1,
											XmNcolumns,					DBStringLength * 2 / 3,
											XmNuserData,				SubjectSTR,
											NULL);
	XtAddCallback (textF,XmNvalueChangedCallback,(XtCallbackProc) _RGISTextFieldValueChangedCBK,(XtPointer) SubjectSTR);
	
	string = XmStringCreate ("Subject:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabel",xmLabelWidgetClass,selectionForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				textF,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			textF,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			textF,
											XmNlabelString,			string,
											NULL);
	string = XmStringCreate ("Select",UICharSetBold);
	button = XtVaCreateManagedWidget ("RGISWorkspaceSubjectSelectB",xmPushButtonGadgetClass,selectionForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				textF,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				textF,
											XmNleftOffset,				5,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			textF,
											XmNlabelString,			string,
											XmNuserData,				(XtArgVal) UIDatasetSubject,
											NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISSelectButtonCBK,textF);

	string = XmStringCreate ("Domain:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceDomainLabel",xmLabelWidgetClass,selectionForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				textF,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				button,
											XmNleftOffset,				15,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			textF,
											XmNlabelString,			string,
											NULL);

	textF = XtVaCreateManagedWidget ("RGISWorkspaceDomainTextF",xmTextFieldWidgetClass,selectionForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNmaxLength,				DBStringLength - 1,
											XmNcolumns,					DBStringLength * 2 / 3,
											XmNuserData,				DomainSTR,
											NULL);
	XtAddCallback (textF,XmNvalueChangedCallback,(XtCallbackProc) _RGISTextFieldValueChangedCBK,(XtPointer) DomainSTR);
	
	string = XmStringCreate ("Select",UICharSetBold);
	button = XtVaCreateManagedWidget ("RGISWorkspaceDomainSelectB",xmPushButtonGadgetClass,selectionForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				textF,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				textF,
											XmNleftOffset,				5,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			textF,
											XmNlabelString,			string,
											XmNuserData,				(XtArgVal) UIDatasetGeoDomain,
											NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISSelectButtonCBK,textF);

	currentDataForm = XtVaCreateManagedWidget ("RGISCurrentDataForm",xmFormWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				selectionForm,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNshadowThickness,		1,
											NULL);

	string = XmStringCreate ("Current Data:",UICharSetBold);
	dataLabel = XtVaCreateManagedWidget ("RGISWorkspaceCurrentDataLabel",xmLabelWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	
	string = XmStringCreate (" ",UICharSetNormal);
	CurrentDataLabel = XtVaCreateManagedWidget ("RGISWorkspaceCurrentDataLabelField",xmLabelGadgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				dataLabel,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			75,
											XmNrightOffset,			5,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											XmNlabelString,			string,
											NULL);
	
	XmStringFree (string);
	string = XmStringCreate ("Type:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceDataTypeLabel",xmLabelWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				CurrentDataLabel,
											XmNleftOffset,				5,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	string = XmStringCreate (" ",UICharSetNormal);
	DataTypeLabel = XtVaCreateManagedWidget ("RGISWorkspaceDataTypeLabelField",xmLabelGadgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);

	string = XmStringCreate ("Domain:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceDomainLabel",xmLabelWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				CurrentDataLabel,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			CurrentDataLabel,
											XmNrightOffset,			5,
											XmNalignment,				XmALIGNMENT_END,
											XmNmarginHeight,			8,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	string = XmStringCreate (" ",UICharSetNormal);
	GeoDomainLabel = XtVaCreateManagedWidget ("RGISWorkspaceDomainLabelField",xmLabelGadgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				label,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			35,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			label,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);

	string = XmStringCreate ("Version:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabel",xmLabelWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				GeoDomainLabel,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				GeoDomainLabel,
											XmNleftOffset,				10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			GeoDomainLabel,
											XmNalignment,				XmALIGNMENT_END,	
											XmNrecomputeSize,			False,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	string = XmStringCreate (" ",UICharSetNormal);
	VersionLabel = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabelField",xmLabelGadgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				label,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			label,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);

	UserModeMenu = XmCreatePulldownMenu (currentDataForm,"RGISSelectModeMenu",NULL,0);
	for (i = 0;i < UserModeNum;++i)
		{
		width = UserModeARR [i].CreateButton (UserModeMenu,(XtCallbackProc) _RGISUserModeCBK);
		maxWidth = width > maxWidth ? width : maxWidth;
		}
	XtVaGetValues (UserModeMenu,XmNchildren,	&buttons, NULL);
	string = XmStringCreate ("User Mode:",UICharSetBold);
	UserModeMenu = XtVaCreateManagedWidget ("RGISUserModeMenu",xmRowColumnWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				VersionLabel,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			VersionLabel,
											XmNsubMenuId,				UserModeMenu,
											XmNlabelString,			string,
											XmNrowColumnType,			XmMENU_OPTION,
											XmNtraversalOn,			False,
											NULL);
	XmStringFree (string);

	string = XmStringCreate ("Subject:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceDomainLabel",xmLabelWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				GeoDomainLabel,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			CurrentDataLabel,
											XmNrightOffset,			5,
											XmNalignment,				XmALIGNMENT_END,
											XmNmarginHeight,			8,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	string = XmStringCreate (" ",UICharSetNormal);
	SubjectLabel = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabelField",xmLabelGadgetClass,currentDataForm,						
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				label,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			35,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			label,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);


	string = XmStringCreate ("Display",UICharSetBold);
	DisplayToggle = XtVaCreateManagedWidget ("RGISWorkspaceDisplayToggle",xmToggleButtonGadgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				SubjectLabel,
											XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNleftWidget,				VersionLabel,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			SubjectLabel,
											XmNlabelString,			string,
											XmNshadowThickness,		0,
											NULL);
	XtAddCallback (DisplayToggle,XmNvalueChangedCallback,(XtCallbackProc) _RGISDisplayModeCBK,(void *) NULL);
	XmStringFree (string);

	SelectModeMenu = XmCreatePulldownMenu (currentDataForm,"RGISSelectModeMenu",NULL,0);
	for (i = 0;i < SelectModeNum;++i)
		{
		width = SelectModeARR [i].CreateButton (SelectModeMenu,(XtCallbackProc) _RGISSelectModeCBK);
		maxWidth = width > maxWidth ? width : maxWidth;
		}
	for (i = 0;i < UserModeNum;++i) XtVaSetValues (buttons [i],XmNwidth, maxWidth, XmNrecomputeSize, False, NULL);
	XtVaGetValues (SelectModeMenu,XmNchildren,	&buttons, NULL);
	for (i = 0;i < SelectModeNum;++i) XtVaSetValues (buttons [i],XmNwidth, maxWidth, XmNrecomputeSize, False, NULL);
	
	string = XmStringCreate ("Select Mode:",UICharSetBold);
	SelectModeMenu = XtVaCreateManagedWidget ("RGISelectModeMenu",xmRowColumnWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				SubjectLabel,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			SubjectLabel,
											XmNsubMenuId,				SelectModeMenu,
											XmNlabelString,			string,
											XmNrowColumnType,			XmMENU_OPTION,
											XmNtraversalOn,			False,
											NULL);
	XmStringFree (string);

	string = XmStringCreate ("Linked Data:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabel",xmLabelWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				SubjectLabel,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			CurrentDataLabel,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNalignment,				XmALIGNMENT_END,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);

	string = XmStringCreate (" ",UICharSetNormal);
	LinkedDataLabel = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabelField",xmLabelGadgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				SubjectLabel,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			80,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
								
	string = XmStringCreate ("Type:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabel",xmLabelWidgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				SubjectLabel,
											XmNtopOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			DataTypeLabel,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNalignment,				XmALIGNMENT_END,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	string = XmStringCreate (" ",UICharSetNormal);
	LinkedTypeLabel = XtVaCreateManagedWidget ("RGISWorkspaceSubjectLabelField",xmLabelGadgetClass,currentDataForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				SubjectLabel,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											XmNlabelString,			string,
											NULL);
	XmStringFree (string);
	
	string = XmStringCreate ("Active Data:",UICharSetBold);
	label = XtVaCreateManagedWidget ("RGISPointInfoItemNumLabel",xmLabelWidgetClass, mainForm,
											XmNtopAttachment,				XmATTACH_WIDGET,
											XmNtopWidget,					currentDataForm,
											XmNtopOffset,					5,
											XmNleftAttachment,			XmATTACH_FORM,
											XmNleftOffset,					5,
											XmNlabelString,				string,
											NULL);
	XmStringFree (string);

	string = XmStringCreate ("Smart Sort",UICharSetBold);
	SmartSortToggle = XtVaCreateManagedWidget ("RGISWorkspaceDisplayToggle",xmToggleButtonGadgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				currentDataForm,
											XmNtopOffset,				5,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			60,
											XmNlabelString,			string,
											XmNshadowThickness,		0,
											XmNset,						((dataset->DataList ())->Flags () & DBDataLISTFlagSmartSort) == DBDataLISTFlagSmartSort,
											NULL);
	XtAddCallback (SmartSortToggle,XmNvalueChangedCallback,(XtCallbackProc) _RGISSmartSortCBK,(void *) (dataset->DataList ()));
	XmStringFree (string);
	
	argNum = 0;
	XtSetArg (wargs [argNum],	XmNtopAttachment,				XmATTACH_WIDGET);	++argNum;
	XtSetArg (wargs [argNum],	XmNtopWidget,					label);				++argNum;
	XtSetArg (wargs [argNum],	XmNtopOffset,					5);					++argNum;
	XtSetArg (wargs [argNum],	XmNleftAttachment,			XmATTACH_FORM);	++argNum;
	XtSetArg (wargs [argNum],	XmNleftOffset,					5); 					++argNum;
	XtSetArg (wargs [argNum],	XmNrightAttachment,			XmATTACH_POSITION);	++argNum;
	XtSetArg (wargs [argNum],	XmNrightPosition,				60);					++argNum;
	XtSetArg (wargs [argNum],	XmNbottomAttachment,			XmATTACH_FORM);	++argNum;
	XtSetArg (wargs [argNum],	XmNbottomOffset,				5);					++argNum;
	XtSetArg (wargs [argNum],	XmNscrollingPolicy,			XmAUTOMATIC);		++argNum;
	XtSetArg (wargs [argNum],	XmNscrollBarDisplayPolicy,	XmAS_NEEDED);		++argNum;
	XtSetArg (wargs [argNum],	XmNvisualPolicy,				XmVARIABLE);		++argNum;
	XtSetArg (wargs [argNum],	XmNshadowThickness,			2);					++argNum;
	XtSetArg (wargs [argNum],	XmNselectionPolicy,			XmSINGLE_SELECT);	++argNum;
	DataList = XmCreateScrolledList (mainForm,"RGISCurrentDataList",wargs,argNum);
	XtAddCallback (DataList,XmNsingleSelectionCallback,	(XtCallbackProc) _RGISWorkspaceSetCurrentDataCBK,this);
	XtManageChild (DataList);

	VecPNL = new RGISVecPanel (mainForm,DataList);
	GrdPNL = new RGISGrdPanel (mainForm,DataList);
	NetPNL = new RGISNetPanel (mainForm,DataList);
	TblPNL = new RGISTblPanel (mainForm,DataList);
	GrpPNL = new RGISGrpPanel (mainForm,DataList);	

	controlRowCol = XtVaCreateManagedWidget ("RGISGroupMenuRowColumn",xmRowColumnWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				SmartSortToggle,
											XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNrightWidget,			currentDataForm,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			SmartSortToggle,
											XmNorientation,			XmHORIZONTAL,
											XmNpacking,					XmPACK_COLUMN,
											XmNspacing,					0,
											XmNmarginWidth,			0,
											XmNmarginHeight,			0,
											XmNnumColumns,				1,
											XmNradioBehavior,			True,
											XmNradioAlwaysOne,		True,
											NULL);
	string = XmStringCreate ("Display Modes",UICharSetBold);
	toggle = XtVaCreateManagedWidget ("RGISWorkspaceDisplayControlToggleWGT",xmToggleButtonGadgetClass,controlRowCol,
											XmNuserData,				this,
											XmNlabelString,			string,
											XmNhighlightThickness,	0,
											XmNshadowThickness,		0,
											XmNmarginWidth,			5,
											XmNset,						True,
											NULL);
	XmStringFree (string);
	XtAddCallback (toggle,XmNvalueChangedCallback,(XtCallbackProc) _RGISControlPageCBK,(XtPointer) 0);
	string = XmStringCreate ("Groups",UICharSetBold);
	toggle = XtVaCreateManagedWidget ("RGISWorkspaceGroupControlToggleWGT",xmToggleButtonGadgetClass,controlRowCol,
											XmNuserData,				this,
											XmNlabelString,			string,
											XmNhighlightThickness,	0,
											XmNshadowThickness,		0,
											XmNmarginWidth,			5,
											XmNset,						False,
											NULL);
	XmStringFree (string);
	XtAddCallback (toggle,XmNvalueChangedCallback,(XtCallbackProc) _RGISControlPageCBK,(XtPointer) 1);
	
	for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
		{
		string = XmStringCreate (data->Name (),UICharSetNormal);
		XmListAddItem (DataList,string,0);
		XmStringFree (string);
		}
	if ((data = dataset->Data ()) != (DBObjData *) NULL)
		{
		CurrentData (data);
		string = XmStringCreate (data->Name (),UICharSetNormal);
		XmListSelectItem (DataList,string,False);
		}
	}

void RGISWorkspace::CurrentDataName ()

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *data = dataset->Data ();
	XmString string = XmStringCreate (data->Name (),UICharSetNormal);
	
	XmListReplaceItems (DataList,&DataNameString,1,&string);
	XmStringFree (DataNameString);
	DataNameString = string;
	XmListSelectItem (DataList,DataNameString,False);
	}

void RGISWorkspace::CurrentData (DBObjData *data)

	{
	int i, panelIsManaged;
	DBDataset *dataset = UIDataset ();
	DBObjData *linkedData = data != (DBObjData *) NULL ? data->LinkedData () : (DBObjData *) NULL;
	DBRegion extent;
	void _RGISUserFuncionVector  (DBObjData *,UI2DView *,XEvent *);
	void _RGISUserFuncionGrid    (DBObjData *,UI2DView *,XEvent *);
	void _RGISUserFuncionNetwork (DBObjData *,UI2DView *,XEvent *);

	if (DataTypeVAR != DBFault)
		{
		char *dataName;

		XmStringGetLtoR (DataNameString,UICharSetNormal,&dataName);
		if (dataset->Data (dataName) == (DBObjData *) NULL) XmListDeleteItem (DataList,DataNameString);
		XmStringFree (DataNameString);
		}

	XtUnmanageChild (UserModeMenu);
	XtUnmanageChild (SelectModeMenu);
	if (Pages [0] != (Widget) NULL)
		{
		panelIsManaged = XtIsManaged (Pages [0]);
		XtUnmanageChild (Pages [0]);
		Pages [0] = (Widget) NULL;
		Pages [1] = (Widget) NULL;
		}

	if (DataTypeVAR != DBFault)
		{
		switch (DataTypeVAR)
			{
			case DBTypeVectorPoint:
			case DBTypeVectorLine:
			case DBTypeVectorPolygon:
			case DBTypeGridDiscrete:		UIMenuVisible (RGISSymbolGroup,	false);	break;
			default: break;
			}
		switch (DataTypeVAR)
			{
			case DBTypeVectorPoint:
				UIMenuVisible 	(RGISPointGroup,		false);
				break;
			case DBTypeVectorLine:
				UIMenuSensitive(RGISLineGroup,		false);
				UIMenuVisible 	(RGISLineGroup,		false);
				break;
			case DBTypeVectorPolygon:
				UIMenuVisible 	(RGISPolyGroup,		false);
				break;
			case DBTypeGridDiscrete:
				UIMenuVisible 	(RGISDiscreteGroup,	false);
				break;
			case DBTypeGridContinuous:
				UIMenuVisible 	(RGISContinuousGroup,	false);
				break;
			case DBTypeNetwork:			UIMenuVisible (RGISNetworkGroup,	false);	break;
			case DBTypeTable:				UIMenuVisible (RGISTableGroup,	false);	break;
			}

		if (LinkedTypeVAR != DBFault)
			{
			UIAuxSetLabelString (LinkedDataLabel,"");
			UIAuxSetLabelString (LinkedTypeLabel,"");
			switch (LinkedTypeVAR)
				{
				case DBTypeVectorPoint:		UIMenuSensitive (RGISLinkedPointGroup,		false);		break;
				case DBTypeVectorLine:		UIMenuSensitive (RGISLinkedLineGroup,		false);		break;
				case DBTypeVectorPolygon:	UIMenuSensitive (RGISLinkedPolyGroup,		false);		break;
				case DBTypeGridDiscrete:	UIMenuSensitive (RGISLinkedDiscreteGroup,	false);		break;
				case DBTypeGridContinuous:	UIMenuSensitive (RGISLinkedContinuousGroup,false);		break;
				case DBTypeNetwork:			UIMenuSensitive (RGISLinkedNetworkGroup,	false);		break;
				case DBTypeTable:				break;
				}
			}
		}

	UI2DViewChangeUserData (data);	
	if (data == (DBObjData *) NULL)
		{
		UI2DViewChangeUserFunction ((UI2DViewUserFunction) NULL);
		UIAuxSetLabelString (CurrentDataLabel,"");
		UIAuxSetLabelString (VersionLabel,"");
		UIAuxSetLabelString (GeoDomainLabel,"");
		UIAuxSetLabelString (SubjectLabel,"");
		UIAuxSetLabelString (DataTypeLabel,"");
		UIAuxSetLabelString (LinkedDataLabel,""); 
		UIAuxSetLabelString (LinkedTypeLabel,"");
		DataTypeVAR = DBFault;
		LinkedTypeVAR = DBFault;
		UIMenuVisible 	(RGISDataGroup,			false);
		UIMenuSensitive(RGISLinkedDataGroup,	false);
		return;
		}
	extent = data->Extent ();
	UIMenuSensitive (RGISDataGroup,	true);


	if (dataset->Data (data->Name (),true) == (DBObjData *) NULL)
		{
		dataset->Data (data);
		DataNameString = XmStringCreate (data->Name (),UICharSetNormal);
		XmListAddItem (DataList,DataNameString,0);
		XmStringFree (DataNameString);
		}
	DataNameString = XmStringCreate (data->Name (),UICharSetNormal);
	XmListSelectItem (DataList,DataNameString,False);
	UIAuxSetLabelString (CurrentDataLabel,data->Name ());
	UIAuxSetLabelString (VersionLabel, data->Document (DBDocVersion));
	UIAuxSetLabelString (GeoDomainLabel,data->Document (DBDocGeoDomain));
	UIAuxSetLabelString (SubjectLabel,data->Document (DBDocSubject));
	UIAuxSetLabelString (DataTypeLabel,DBDataTypeString (data->Type ()));
	DataTypeVAR = data->Type ();
	if (DataTypeVAR != DBTypeTable)
		{
		XmToggleButtonGadgetSetState (DisplayToggle,(data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay,False);
		XtVaSetValues (DisplayToggle,XmNuserData, data, NULL);
		}

	if ((linkedData = data->LinkedData ()) == NULL)
		{
		UIAuxSetLabelString (LinkedDataLabel," "); 
		UIAuxSetLabelString (LinkedTypeLabel," ");
		UIMenuSensitive (RGISLinkedDataGroup,	false);
		}
	else
		{
		UIAuxSetLabelString (LinkedDataLabel,linkedData->Name ());
		UIAuxSetLabelString (LinkedTypeLabel,DBDataTypeString (linkedData->Type ()));
		LinkedTypeVAR = linkedData->Type ();
		switch (LinkedTypeVAR)
			{
			case DBTypeVectorPoint:		UIMenuSensitive (RGISLinkedPointGroup,			true);	break;
			case DBTypeVectorLine:		UIMenuSensitive (RGISLinkedLineGroup,			true);	break;
			case DBTypeVectorPolygon:	UIMenuSensitive (RGISLinkedPolyGroup,			true);	break;
			case DBTypeGridDiscrete:	UIMenuSensitive (RGISLinkedDiscreteGroup,		true);	break;
			case DBTypeGridContinuous:	UIMenuSensitive (RGISLinkedContinuousGroup,	true);	break;
			case DBTypeNetwork:			UIMenuSensitive (RGISLinkedNetworkGroup,		true);	break;
			case DBTypeTable:				UIMenuSensitive (RGISLinkedTableGroup,			true);	break;
			}
		}

	for (i = 0;i < UserModeNum;++i) 		UserModeARR [i].ShowButton (UserModeMenu,data,DBDataFlagUserModeFlags);
	for (i = 0;i < SelectModeNum;++i) 	SelectModeARR [i].ShowButton (SelectModeMenu,data,DBDataFlagSelectMode);

	switch (DataTypeVAR)
		{
		case DBTypeVectorPoint:
			Pages [0] = VecPNL->Load (data);
			UIMenuVisible (RGISPointGroup,		true);
			UIMenuSensitive (RGISGeoDataGroup,	true);
			XtManageChild (SelectModeMenu);
			UI2DViewChangeUserFunction (_RGISUserFuncionVector);
			break;
		case DBTypeVectorLine:
			Pages [0] = VecPNL->Load (data);
			UIMenuVisible (RGISLineGroup,			true);
			UIMenuSensitive (RGISGeoDataGroup,	true);
			XtManageChild (SelectModeMenu);
			UI2DViewChangeUserFunction (_RGISUserFuncionVector);
			break;
		case DBTypeVectorPolygon:
			Pages [0] = VecPNL->Load (data);
			UIMenuVisible (RGISPolyGroup,			true);
			UIMenuSensitive (RGISGeoDataGroup,	true);
			XtManageChild (SelectModeMenu);
			UI2DViewChangeUserFunction (_RGISUserFuncionVector);
			break;
		case DBTypeGridDiscrete:
			Pages [0] = GrdPNL->Load (data);
			UIMenuVisible (RGISDiscreteGroup,	true);
			UIMenuSensitive (RGISGeoDataGroup,	true);
			UI2DViewChangeUserFunction (_RGISUserFuncionGrid);
			break;
		case DBTypeGridContinuous:
			Pages [0] = GrdPNL->Load (data);
			UIMenuVisible (RGISContinuousGroup,	true);
			UIMenuSensitive (RGISGeoDataGroup,	true);
			UI2DViewChangeUserFunction (_RGISUserFuncionGrid);
			break;
		case DBTypeNetwork:
			Pages [0] = NetPNL->Load (data);
			UIMenuVisible (RGISNetworkGroup,		true);
			UIMenuSensitive (RGISGeoDataGroup,	true);
			XtManageChild (SelectModeMenu);
			UI2DViewChangeUserFunction (_RGISUserFuncionNetwork);
			break;
		case DBTypeTable:
			Pages [0] = TblPNL->Load (data);
			UIMenuVisible (RGISTableGroup,		true);
			UIMenuSensitive (RGISDataGroup,	true);
			UI2DViewChangeUserFunction ((UI2DViewUserFunction) NULL);
			break;
		}
	Pages [1] = GrpPNL->Load (data);
	if (panelIsManaged) XtManageChild (Pages [0]); else XtManageChild (Pages [1]);
	XtManageChild (UserModeMenu);
	}
