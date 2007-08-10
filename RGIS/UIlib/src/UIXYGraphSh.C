/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

UIXYGraphSh.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/BulletinB.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <UI.H>

#define UIXYGraphShellMinWidth  500
#define UIXYGraphShellMinHeight 350

static void _UIXYGraphShellDeleteCBK (Widget widget,UIXYGraphShell *xyGraph, XmAnyCallbackStruct *callData)

	{ widget = widget; callData = callData; delete xyGraph; }

static void _UIXYGraphShellSetDataCBK (Widget widget, UIXYGraphShell *xyGraph, XmAnyCallbackStruct *callData)

	{ widget = widget; callData = callData; xyGraph->SetData (); }

static void _UIXYGraphShellConfigureCBK (Widget widget, UIXYGraphShell *xyGraph, XmAnyCallbackStruct *callData)

	{ widget = widget; callData = callData; xyGraph->Configure (); }

static void _UIXYGraphShellSetListCBK (Widget widget, UIXYGraphShell *xyGraph, XmListCallbackStruct *callData)

	{ widget = widget; xyGraph->SetList (callData->item_position); }

static void _UIXYGraphShellSetSeriesCBK (Widget widget, UIXYGraphShell *xyGraph, XmAnyCallbackStruct *callData)

	{ widget = widget; callData = callData; xyGraph->SetSeries (); }

static void _UIXYGraphShellAddSeriesCBK (Widget widget, UIXYGraphShell *xyGraph, XmAnyCallbackStruct *callData)

	{ widget = widget; callData = callData; xyGraph->AddSeries (); }

static void _UIXYGraphShellRemoveSeriesCBK (Widget widget, UIXYGraphShell *xyGraph, XmAnyCallbackStruct *callData)

	{ widget = widget; callData = callData; xyGraph->RemoveSeries (); }

UIXYGraphShell::UIXYGraphShell (DBObjData *dbData) : DBObject (UIXYGraphShellStr,sizeof (UIXYGraphShell))

	{
	char title [DBDataNameLen + 12];
	Widget mainForm, form, label, button, subForm;
	XmString string;
	DBInt argNum;
	Arg wargs [20];
	Atom deleteWindowAtom = XmInternAtom(XtDisplay(UITopLevel ()),"WM_DELETE_WINDOW",FALSE);

	sprintf (title,"XYGraph: %s",dbData->Name ());
	DShell = XtVaCreatePopupShell ("UIXYGraphShellPopupShell",xmDialogShellWidgetClass,UITopLevel (),
												XmNkeyboardFocusPolicy,		XmPOINTER,
												XmNallowShellResize,			false,
												XmNtitle,						title,
												XmNtransient,					false,
												XmNminWidth,					UIXYGraphShellMinWidth,
												XmNminHeight,					UIXYGraphShellMinHeight,
												NULL);
	XmAddWMProtocolCallback (DShell,deleteWindowAtom,(XtCallbackProc) _UIXYGraphShellDeleteCBK,(XtPointer) this); 

	mainForm = XtVaCreateWidget ("UIXYGraphShellMainForm",xmFormWidgetClass,DShell,
												XmNshadowThickness,			0,
												NULL);
	form = XtVaCreateManagedWidget ("UIXYGraphShellForm",xmFormWidgetClass,mainForm,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNleftOffset,					10,
												XmNrightAttachment,			XmATTACH_FORM,
												XmNrightOffset,				10,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				10,
												XmNshadowThickness,			0,
												NULL);
	string = XmStringCreate ("Select",UICharSetBold);
	button = XtVaCreateManagedWidget ("UIXYGraphShellTSButton",xmPushButtonWidgetClass,form,
												XmNtopAttachment,				XmATTACH_FORM,
												XmNtopOffset,					10,
												XmNrightAttachment,			XmATTACH_FORM,
												XmNrightOffset,				10,
												XmNmarginHeight,				5,
												XmNtraversalOn,				false,
												XmNlabelString,				string,
												XmNuserData,					(XtPointer) NULL,
												NULL);
	XmStringFree (string);
	string = XmStringCreate ("Time Series:",UICharSetBold);
	label = XtVaCreateManagedWidget ("UIXYGraphShellTSLabel",xmLabelWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					button,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNleftOffset,					10,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				button,
												XmNlabelString,				string,
												NULL);
	XmStringFree (string);
	TSDataFieldWGT = XtVaCreateManagedWidget ("UIXYGraphShellTSTextF",xmTextFieldWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					button,
												XmNleftAttachment,			XmATTACH_WIDGET,
												XmNleftWidget,					label,
												XmNleftOffset,					10,
												XmNrightAttachment,			XmATTACH_WIDGET,
												XmNrightWidget,				button,
												XmNrightOffset,				10,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				button,
												XmNmaxLength,					DBStringLength,
												XmNcolumns,						DBStringLength,
												XmNuserData,					dbData->Table (DBrNRelations),
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,TSDataFieldWGT);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIXYGraphShellSetDataCBK,this);
	string = XmStringCreate ("Select",UICharSetBold);
	XAxisButtonWGT = XtVaCreateManagedWidget ("UIXYGraphShellXAxisButton",xmPushButtonWidgetClass,form,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					button,
												XmNtopOffset,					10,
												XmNleftAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNleftWidget,					button,
												XmNrightAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNrightWidget,				button,
												XmNmarginHeight,				5,
												XmNtraversalOn,				false,
												XmNlabelString,				string,
												XmNsensitive,					false,
												XmNuserData,					DBTableFieldIsAxisVal,
												NULL);
	XmStringFree (string);
	XAxisFieldWGT = XtVaCreateManagedWidget ("UIXYGraphShellXAxisTextF",xmTextFieldWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					XAxisButtonWGT,
												XmNrightAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNrightWidget,				TSDataFieldWGT,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				XAxisButtonWGT,
												XmNmaxLength,					DBStringLength,
												XmNcolumns,						DBStringLength / 2,
												XmNsensitive,					false,
												NULL);
	XtAddCallback (XAxisButtonWGT,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,XAxisFieldWGT);
	XtAddCallback (XAxisButtonWGT,XmNactivateCallback,(XtCallbackProc) _UIXYGraphShellConfigureCBK,this);
	string = XmStringCreate ("X-Axis:",UICharSetBold);
	label = XtVaCreateManagedWidget ("UIXYGraphShellXAxisLabel",xmLabelWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					XAxisButtonWGT,
												XmNrightAttachment,			XmATTACH_WIDGET,
												XmNrightWidget,				XAxisFieldWGT,
												XmNrightOffset,				5,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				XAxisButtonWGT,
												XmNlabelString,				string,
												NULL);
	XmStringFree (string);
	string = XmStringCreate ("Select",UICharSetBold);
	YAxisButtonWGT = XtVaCreateManagedWidget ("UIXYGraphShellYAxisButton",xmPushButtonWidgetClass,form,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					XAxisButtonWGT,
												XmNleftAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNleftWidget,					button,
												XmNrightAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNrightWidget,				XAxisButtonWGT,
												XmNmarginHeight,				5,
												XmNtraversalOn,				false,
												XmNlabelString,				string,
												XmNsensitive,					false,
												XmNuserData,					DBTableFieldIsNumeric,
												NULL);
	XmStringFree (string);
	YAxisFieldWGT = XtVaCreateManagedWidget ("UIXYGraphShellYAxisTextF",xmTextFieldWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					YAxisButtonWGT,
												XmNrightAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNrightWidget,				TSDataFieldWGT,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				YAxisButtonWGT,
												XmNmaxLength,					DBStringLength,
												XmNcolumns,						DBStringLength / 2,
												XmNsensitive,					false,
												NULL);
	XtAddCallback (YAxisButtonWGT,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,YAxisFieldWGT);
	XtAddCallback (YAxisButtonWGT,XmNactivateCallback,(XtCallbackProc) _UIXYGraphShellSetSeriesCBK,this);
	string = XmStringCreate ("Y-Axis:",UICharSetBold);
	label = XtVaCreateManagedWidget ("UIXYGraphShellYAxisLabel",xmLabelWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					YAxisButtonWGT,
												XmNrightAttachment,			XmATTACH_WIDGET,
												XmNrightWidget,				YAxisFieldWGT,
												XmNrightOffset,				5,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				YAxisButtonWGT,
												XmNlabelString,				string,
												NULL);
	XmStringFree (string);
	subForm = XtVaCreateManagedWidget ("UIXYGraphShellSubForm",xmFormWidgetClass,form,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					YAxisButtonWGT,
												XmNtopOffset,					5,
												XmNleftAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNleftWidget,					label,
												XmNrightAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNrightWidget,				YAxisButtonWGT,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				10,
												XmNshadowThickness,			0,
												NULL);
	string = XmStringCreate ("<- Add Series",UICharSetBold);
	AddButtonWGT = XtVaCreateManagedWidget ("UIXYGraphShellYAxisButton",xmPushButtonWidgetClass,subForm,
												XmNtopAttachment,				XmATTACH_FORM,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNrightAttachment,			XmATTACH_POSITION,
												XmNrightPosition,				50,
												XmNrightOffset,				5,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNmarginHeight,				5,
												XmNtraversalOn,				false,
												XmNlabelString,				string,
												XmNsensitive,					false,
												NULL);
	XmStringFree (string);
	XtAddCallback (AddButtonWGT,XmNactivateCallback,(XtCallbackProc) _UIXYGraphShellAddSeriesCBK,this);
	XtAddCallback (AddButtonWGT,XmNactivateCallback,(XtCallbackProc) _UIXYGraphShellConfigureCBK,this);
	string = XmStringCreate ("Remove Series",UICharSetBold);
	RemoveButtonWGT = XtVaCreateManagedWidget ("UIXYGraphShellYAxisButton",xmPushButtonWidgetClass,subForm,
												XmNtopAttachment,				XmATTACH_FORM,
												XmNleftAttachment,			XmATTACH_POSITION,
												XmNleftPosition,				50,
												XmNleftOffset,					5,
												XmNrightAttachment,			XmATTACH_FORM,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNmarginHeight,				5,
												XmNtraversalOn,				false,
												XmNlabelString,				string,
												XmNsensitive,					false,
												NULL);
	XmStringFree (string);
	XtAddCallback (RemoveButtonWGT,XmNactivateCallback,(XtCallbackProc) _UIXYGraphShellRemoveSeriesCBK,this);
	XtAddCallback (RemoveButtonWGT,XmNactivateCallback,(XtCallbackProc) _UIXYGraphShellConfigureCBK,this);
	argNum = 0;
	XtSetArg (wargs [argNum],	XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET);	++argNum;
	XtSetArg (wargs [argNum],	XmNtopWidget,					XAxisFieldWGT);				++argNum;
	XtSetArg (wargs [argNum],	XmNleftAttachment,			XmATTACH_FORM);				++argNum;
	XtSetArg (wargs [argNum],	XmNleftOffset,					5); 								++argNum;
	XtSetArg (wargs [argNum],	XmNrightAttachment,			XmATTACH_WIDGET);				++argNum;
	XtSetArg (wargs [argNum],	XmNrightWidget,				subForm);						++argNum;
	XtSetArg (wargs [argNum],	XmNrightOffset,				10);								++argNum;
	XtSetArg (wargs [argNum],	XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET);	++argNum;
	XtSetArg (wargs [argNum],	XmNbottomWidget,				subForm);						++argNum;
	XtSetArg (wargs [argNum],	XmNshadowThickness,			2);								++argNum;
	XtSetArg (wargs [argNum],	XmNselectionPolicy,			XmSINGLE_SELECT);				++argNum;
	XtManageChild (SeriesListWGT = XmCreateScrolledList (form,"UIXYGraphShellSeriesList",wargs,argNum));
	XtAddCallback (SeriesListWGT,XmNsingleSelectionCallback,	(XtCallbackProc) _UIXYGraphShellSetListCBK,this);
	
	argNum = 0;
	XtSetArg (wargs [argNum],	XmNtopAttachment,				XmATTACH_FORM);				++argNum;
	XtSetArg (wargs [argNum],	XmNtopOffset,					10);								++argNum;
	XtSetArg (wargs [argNum],	XmNleftAttachment,			XmATTACH_FORM);				++argNum;
	XtSetArg (wargs [argNum],	XmNleftOffset,					10);								++argNum;
	XtSetArg (wargs [argNum],	XmNrightAttachment,			XmATTACH_FORM);				++argNum;
	XtSetArg (wargs [argNum],	XmNrightOffset,				10);								++argNum;
	XtSetArg (wargs [argNum],	XmNbottomAttachment,			XmATTACH_WIDGET);				++argNum;
	XtSetArg (wargs [argNum],	XmNbottomWidget,				form);							++argNum;
	XtSetArg (wargs [argNum],	XmNbottomOffset,				5);								++argNum;
	Graph = new UIXYGraph (mainForm,"Text",wargs,argNum);
	XtVaSetValues (mainForm,XmNwidth, UIXYGraphShellMinWidth, XmNheight, UIXYGraphShellMinHeight, NULL);
	XtManageChild (mainForm);
	XtPopup (DShell,XtGrabNone);
	YFieldsPTR = new DBObjectLIST<DBObjTableField> ("YAxisFields");
	DataPTR = dbData;
	RecordPTR = (DataPTR->Table (DBrNItems))->Item ();
	RelDataPTR = (DBObjData *) NULL;
	}

UIXYGraphShell::~UIXYGraphShell ()

	{
	if (RelDataPTR == (DBObjData *) NULL) delete RelDataPTR;
	delete Graph; 
	XtDestroyWidget (DShell);
	}

void UIXYGraphShell::Configure (DBObjRecord *record)

	{
	DBInt i;
	char *fieldName = XmTextFieldGetString (XAxisFieldWGT);
	DBObjTableField *field;
	
	if (DataPTR == (DBObjData *) NULL) return;
	if (RelDataPTR == (DBObjData *) NULL) return;
	if (((XAxisFLD = (RelDataPTR->Table (DBrNItems))->Field (fieldName)) == (DBObjTableField *) NULL) ||
		 (YFieldsPTR->ItemNum () < 1)) return;
	XtFree (fieldName);

	RecordPTR = record;
	Graph->ChangeTitle (record->Name ());
	while (Graph->ItemNum () > 0) Graph->Remove (Graph->Item ());
	for (i = 0;i < YFieldsPTR->ItemNum ();++i)
		{
		field = YFieldsPTR->Item (i);
		Graph->Add (new UIDataSeries (field->Name (),RelDataPTR->Table (DBrNItems),JoinFLD,XAxisFLD,field,RelFLD->String (record),i + 1));
		}
	Graph->Draw (true);
	}

void UIXYGraphShell::SetData ()

	{
	char *tsName = XmTextFieldGetString (TSDataFieldWGT);
	DBDataset *dataset = UIDataset ();
	DBObjTable 	*relTable = DataPTR->Table (DBrNRelations);
	DBObjRecord *relRecord = relTable->Item (tsName);
	DBObjTableField *relDataFLD;
	DBObjMetaEntry *metaEntry;
	
	if (relRecord == (DBObjRecord *) NULL)	return;
	XtFree (tsName);
	
	relDataFLD = relTable->Field (DBrNRelateData);
	RelFLD = relTable->Field (DBrNRelateField);
	JoinFLD = relTable->Field (DBrNRelateJoinField);
	if (RelDataPTR != (DBObjData *) NULL)
		{
		if (strcmp (RelDataPTR->Name (),relDataFLD->String (relRecord)) == 0)
			return;
		else
			{
			if (dataset->Data (relDataFLD->String (relRecord)) == (DBObjData *) NULL)
				delete RelDataPTR;
			}
		}
	YFieldsPTR->DeleteAll ();
	XmTextFieldSetString (XAxisFieldWGT,"");
	XmTextFieldSetString (YAxisFieldWGT,"");
	XmListDeleteAllItems (SeriesListWGT);
	if ((RelDataPTR = dataset->Data (relDataFLD->String (relRecord))) == (DBObjData *) NULL)
		{
		if ((metaEntry = dataset->Meta (relDataFLD->String (relRecord))) == (DBObjMetaEntry *) NULL)
			{
			fprintf (stderr,"Meta Enrty Finding Error in:  UIXYGraphShell::ConfigureTS ()\n");
			XtSetSensitive (XAxisButtonWGT,false);
			XtSetSensitive (XAxisFieldWGT,false);
			XtSetSensitive (YAxisButtonWGT,false);
			XtSetSensitive (YAxisFieldWGT,false);
			XtSetSensitive (AddButtonWGT,false);
			XtSetSensitive (RemoveButtonWGT,false);
			return;
			}
		RelDataPTR = new DBObjData ();
		if (RelDataPTR->Read (metaEntry->FileName ()) != DBSuccess)
			{
			XtSetSensitive (XAxisButtonWGT,false);
			XtSetSensitive (XAxisFieldWGT,false);
			XtSetSensitive (YAxisButtonWGT,false);
			XtSetSensitive (YAxisFieldWGT,false);
			XtSetSensitive (AddButtonWGT,false);
			XtSetSensitive (RemoveButtonWGT,false);
			delete RelDataPTR;
			RelDataPTR = (DBObjData *) NULL;
			return;
			}
		}
	RelFLD  = (DataPTR->Table (DBrNItems))->Field (RelFLD->String (relRecord));
	JoinFLD = (RelDataPTR->Table (DBrNItems))->Field (JoinFLD->String (relRecord));
	XtVaSetValues (XAxisFieldWGT, XmNuserData,	(RelDataPTR->Table (DBrNItems))->Fields (), NULL);
	XtVaSetValues (YAxisFieldWGT, XmNuserData,	(RelDataPTR->Table (DBrNItems))->Fields (), NULL);
	XtSetSensitive (XAxisButtonWGT,true);
	XtSetSensitive (XAxisFieldWGT,true);
	XtSetSensitive (YAxisButtonWGT,true);
	XtSetSensitive (YAxisFieldWGT,true);
	}

void UIXYGraphShell::SetList (int itemPos)

	{
	DBObjTableField *field = YFieldsPTR->Item (itemPos - 1,true);
	
	if (field == (DBObjTableField *) NULL) return;
	XmTextFieldSetString (YAxisFieldWGT,field->Name ());
	XtSetSensitive (RemoveButtonWGT,true);
	}

void UIXYGraphShell::SetSeries ()

	{
	char *fieldName = XmTextFieldGetString (YAxisFieldWGT);
	XmString string;
	DBObjTableField *field = (RelDataPTR->Table (DBrNItems))->Field (fieldName);

	if (field != (DBObjTableField *) NULL)
		{
		string = XmStringCreate (field->Name (),UICharSetNormal);
		
		if (XmListItemExists (SeriesListWGT,string))
			{
			XtSetSensitive (AddButtonWGT,false);
			XtSetSensitive (RemoveButtonWGT,true);
			XmListSelectItem (SeriesListWGT,string,false);
			YFieldsPTR->Item (fieldName,true);
			}
		else
			{
			XtSetSensitive (AddButtonWGT,true);
			XtSetSensitive (RemoveButtonWGT,false);
			}
		XmStringFree (string);
		}
	XtFree (fieldName);
	}

void UIXYGraphShell::AddSeries ()

	{
	char *fieldName = XmTextFieldGetString (YAxisFieldWGT);
	XmString string;
	DBObjectLIST<DBObjTableField> *fields = (RelDataPTR->Table (DBrNItems))->Fields ();
	DBObjTableField *field = new DBObjTableField (*(fields->Item (fieldName)));
	
	XtFree (fieldName);
	string = XmStringCreate (field->Name (),UICharSetNormal);
	XmListAddItem (SeriesListWGT,string,0);
	XmListSelectItem (SeriesListWGT,string,false);
	XmStringFree (string);
	YFieldsPTR->Add (field);
	XtSetSensitive (AddButtonWGT,false);
	XtSetSensitive (RemoveButtonWGT,true);
	}

void UIXYGraphShell::RemoveSeries ()

	{
	XmString string;
	DBObjTableField *field = YFieldsPTR->Item ();
	
	string = XmStringCreate (field->Name (),UICharSetNormal);
	XmListDeleteItem (SeriesListWGT,string);
	XmStringFree (string);
	YFieldsPTR->Delete (field);
	if ((field = YFieldsPTR->Item ()) != (DBObjTableField *) NULL)
		{
		string = XmStringCreate (field->Name (),UICharSetNormal);
		XmListSelectItem (SeriesListWGT,string,false);
		XmStringFree (string);
		}
	XtSetSensitive (RemoveButtonWGT,false);
	XtSetSensitive (AddButtonWGT,true);
	}

void UIXYGraphShell::Raise () { XtPopup (DShell,XtGrabNone); }
