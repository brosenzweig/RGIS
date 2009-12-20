/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGISAnField.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <rgis.H>

static void _RGISAnFieldCalculate (DBObjTable *table,DBInt oper)

	{
	char *rText, *f0Text, *f1Text;
	int allowOk;
	static int calc;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	static Widget dShell = (Widget) NULL, mainForm;
	static Widget field0TextF, field1TextF, resultTextF;
	XmString string;

	if (dShell == (Widget) NULL)
		{
		Widget button;

		dShell = UIDialogForm ((char *) "Calculate",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISFieldCalcButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_FORM,
								XmNtopOffset,           10,
								XmNrightAttachment,     XmATTACH_FORM,
								XmNrightOffset,         10,
								XmNmarginHeight,        5,
								XmNtraversalOn,         False,
								XmNlabelString,         string,
								XmNuserData,            DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		field0TextF = XtVaCreateManagedWidget ("RGISFieldCalcOpTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           button,
								XmNrightAttachment,     XmATTACH_WIDGET,
								XmNrightWidget,         button,
								XmNrightOffset,         10,
								XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,        button,
								XmNmaxLength,           DBStringLength,
								XmNcolumns,             DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,field0TextF);
		string = XmStringCreate ((char *) "First Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISFieldCalcOpLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           button,
								XmNleftAttachment,      XmATTACH_FORM,
								XmNleftOffset,          10,
								XmNrightAttachment,     XmATTACH_WIDGET,
								XmNrightWidget,         field0TextF,
								XmNrightOffset,         10,
								XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,        button,
								XmNlabelString,         string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISFieldCalcOpButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_WIDGET,
								XmNtopWidget,           button,
								XmNtopOffset,           10,
								XmNrightAttachment,     XmATTACH_FORM,
								XmNrightOffset,         10,
								XmNmarginHeight,        5,
								XmNtraversalOn,         False,
								XmNlabelString,         string,
								XmNuserData,            DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		field1TextF = XtVaCreateManagedWidget ("RGISFieldCalcOpTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           button,
								XmNrightAttachment,     XmATTACH_WIDGET,
								XmNrightWidget,         button,
								XmNrightOffset,         10,
								XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,        button,
								XmNmaxLength,           DBStringLength,
								XmNcolumns,             DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,field1TextF);
		string = XmStringCreate ((char *) "Second Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISFieldCalcOpLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,          10,
								XmNrightAttachment,     XmATTACH_WIDGET,
								XmNrightWidget,         field1TextF,
								XmNrightOffset,         10,
								XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,        button,
								XmNlabelString,         string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISFieldCalcResButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_WIDGET,
								XmNtopWidget,           button,
								XmNtopOffset,           10,
								XmNrightAttachment,     XmATTACH_FORM,
								XmNrightOffset,         10,
								XmNmarginHeight,        5,
								XmNtraversalOn,         False,
								XmNlabelString,         string,
								XmNuserData,            DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		resultTextF = XtVaCreateManagedWidget ("RGISFieldCalcResTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           button,
								XmNrightAttachment,     XmATTACH_WIDGET,
								XmNrightWidget,         button,
								XmNrightOffset,         10,
								XmNmaxLength,           DBStringLength,
								XmNcolumns,             DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,resultTextF);
		string = XmStringCreate ((char *) "Result Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISFieldCalcResLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,           button,
								XmNleftAttachment,      XmATTACH_FORM,
								XmNleftOffset,          10,
								XmNrightAttachment,     XmATTACH_WIDGET,
								XmNrightWidget,         resultTextF,
								XmNrightOffset,         10,
								XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,        button,
								XmNlabelString,         string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&calc);
		}
	calc = false;
	XtVaSetValues (field0TextF,XmNuserData, table->Fields (), NULL);
	XtVaSetValues (field1TextF,XmNuserData, table->Fields (), NULL);
	XtVaSetValues (resultTextF,XmNuserData, table->Fields (), NULL);
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		f0Text = XmTextFieldGetString (field0TextF);
		f1Text = XmTextFieldGetString (field1TextF);
		rText  = XmTextFieldGetString (resultTextF);
		allowOk = (strlen (f0Text) > 0) && (strlen (f1Text) > 0) && (rText > 0);
		XtFree (f0Text);	XtFree (f1Text);	XtFree (rText);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),allowOk);
		}
	UIDialogFormPopdown (dShell);

	if (calc)
		{
		UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,table));
		DBObjTableField *field;

		f0Text = XmTextFieldGetString (field0TextF);
		f1Text = XmTextFieldGetString (field1TextF);
		rText  = XmTextFieldGetString (resultTextF);
		field = table->Field (rText);
		if (RGlibGenFuncFieldCalculate (table,f0Text,f1Text,rText,oper) != DBSuccess)
			UIMessage ((char *) "Invalid Fields");
		if (tableCLS != (UITable *) NULL)
			{
			if (field == (DBObjTableField *) NULL)
				tableCLS->AddField (table->Field (rText));
			tableCLS->Draw ();
			}
		XtFree (f0Text);
		XtFree (f1Text);
		XtFree (rText);
		}
	}

static Widget _RGISAnalyseFieldTableSelect = (Widget) NULL;

void RGISAnalyseFieldCalcAddCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;
	widget = widget; data = data; callData = callData;

	if (_RGISAnalyseFieldTableSelect == (Widget) NULL) _RGISAnalyseFieldTableSelect = UISelectionCreate ((char *) "Table Selection");
	table = dbData->Table (UISelectObject (_RGISAnalyseFieldTableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ()));
	if (table == (DBObjTable *) NULL) return;
	_RGISAnFieldCalculate (table,DBMathOperatorAdd);
	}

void RGISAnalyseFieldCalcSubtractCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;

	widget = widget; data = data; callData = callData;
	if (_RGISAnalyseFieldTableSelect == (Widget) NULL) _RGISAnalyseFieldTableSelect = UISelectionCreate ((char *) "Table Selection");
	table = dbData->Table (UISelectObject (_RGISAnalyseFieldTableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ()));
	if (table == (DBObjTable *) NULL) return;
	_RGISAnFieldCalculate (table,DBMathOperatorSub);
	}

void RGISAnalyseFieldCalcMultiplyCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;

	widget = widget; data = data; callData = callData;
	if (_RGISAnalyseFieldTableSelect == (Widget) NULL) _RGISAnalyseFieldTableSelect = UISelectionCreate ((char *) "Table Selection");
	table = dbData->Table (UISelectObject (_RGISAnalyseFieldTableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ()));
	if (table == (DBObjTable *) NULL) return;
	_RGISAnFieldCalculate (table,DBMathOperatorMul);
	}

void RGISAnalyseFieldCalcDivideCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;

	widget = widget; data = data; callData = callData;
	if (_RGISAnalyseFieldTableSelect == (Widget) NULL) _RGISAnalyseFieldTableSelect = UISelectionCreate ((char *) "Table Selection");
	table = dbData->Table (UISelectObject (_RGISAnalyseFieldTableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ()));
	if (table == (DBObjTable *) NULL) return;
	_RGISAnFieldCalculate (table,DBMathOperatorDiv);
	}

void RGISAnalyseFieldSetValueCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	static DBInt cont;
	char *selection, *srcText;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *table;
	DBObjTableField *dstFLD;
	static Widget selectWidget = (Widget) NULL;
	static Widget dShell = NULL, mainForm, button, srcTextF;

	widget = widget; data = data; callData = callData;
	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ((char *) "Select Field");
	if (dShell == (Widget) NULL)
		{
		XmString string;

		dShell = UIDialogForm ((char *) "Fields",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseFieldCalcSetValueButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_FORM,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_FORM,
								XmNbottomOffset,            10,
								XmNmarginHeight,            5,
								XmNtraversalOn,             False,
								XmNlabelString,             string,
								XmNuserData,                DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		srcTextF = XtVaCreateManagedWidget ("RGISAnalyseFieldCalcSetValueTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             button,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNmaxLength,               DBStringLength,
								XmNcolumns,                 DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,srcTextF);
		string = XmStringCreate ((char *) "Source Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseFieldCalcSetValueLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNleftAttachment,          XmATTACH_FORM,
								XmNleftOffset,              10,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             srcTextF,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNlabelString,             string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&cont);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),True);
		}
	if (_RGISAnalyseFieldTableSelect == (Widget) NULL) _RGISAnalyseFieldTableSelect = UISelectionCreate ((char *) "Table Selection");
	table = dbData->Table (UISelectObject (_RGISAnalyseFieldTableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ()));
	if (table == (DBObjTable *) NULL) return;
	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) table->Fields (),DBTableFieldIsNumeric)) == NULL)
		return;
	if ((dstFLD = table->Field (selection)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Corrupt Table in: _RGISAnalyseFieldCalcSetValueCBK ()\n"); return; }
	if (dstFLD->Required ()) { UIMessage ((char *) "Protected Field!"); return; }
	XtVaSetValues (srcTextF,XmNuserData,table->Fields (),NULL);
	cont = false;
	UIDialogFormPopup (dShell);
	while (UILoop ());

	UIDialogFormPopdown (dShell);
 	if (cont)
		{
		DBInt recID, constant;
		DBFloat value;
		DBObjTableField *srcFLD;
		DBObjRecord *record;
		UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,table));

		srcText = XmTextFieldGetString (srcTextF);
		if ((srcFLD = table->Field (srcText)) == (DBObjTableField *) NULL)
			{
			if (DBTableFieldIsNumeric (dstFLD))
				constant = sscanf (srcText,"%lf",&value) == 1 ? true : false;
			else
				constant = strlen (srcText) > 0 ? true : false;
			}
		for (recID = 0;recID < table->ItemNum ();++recID)
			{
			record = table->Item (recID);
			if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if (srcFLD != (DBObjTableField *) NULL)
				switch (srcFLD->Type ())
					{
					case DBTableFieldString:	dstFLD->String	(record,srcFLD->String (record));	break;
					case DBTableFieldInt:		dstFLD->Int		(record,srcFLD->Int (record));		break;
					case DBTableFieldFloat:		dstFLD->Float	(record,srcFLD->Float (record));		break;
					}
			else
				{
				if (DBTableFieldIsNumeric (dstFLD))
					dstFLD->Float (record,constant ? value : record->RowID () + 1);
				else
					dstFLD->String (record,constant ? srcText : record->Name ());
				}
			}
		XtFree (srcText);
		if (tableCLS != (UITable *) NULL) tableCLS->Draw ();
		}
	}

void RGISAnalyseFieldsCompareCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *rText, *f0Text, *f1Text;
	int allowOk;
	static int compare = false, diffMethod = 0;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	DBObjTable *itemTable = dbData->Table (DBrNItems);
	static Widget dShell = (Widget) NULL, mainForm;
	static Widget field0TextF, field1TextF, resultTextF, menu, mButton;
	XmString string;

	widget = widget;	data = data; callData = callData;
	if (dShell == (Widget) NULL)
		{
		Widget button;

		dShell = UIDialogForm ((char *) "Compare Fields",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISEdiCompFieldsButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_FORM,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNmarginHeight,            5,
								XmNtraversalOn,             False,
								XmNlabelString,             string,
								XmNuserData,                DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		field0TextF = XtVaCreateManagedWidget ("RGISAnalyseCompField0TextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             button,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNmaxLength,               DBStringLength,
								XmNcolumns,                 DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,field0TextF);
		string = XmStringCreate ((char *) "First Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseCompFieldNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNleftAttachment,          XmATTACH_FORM,
								XmNleftOffset,              10,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             field0TextF,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNlabelString,             string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseCompField0Button",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_WIDGET,
								XmNtopWidget,               button,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNmarginHeight,            5,
								XmNtraversalOn,             False,
								XmNlabelString,             string,
								XmNuserData,                DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		field1TextF = XtVaCreateManagedWidget ("RGISAnalyseCompField1TextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             button,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNmaxLength,               DBStringLength,
								XmNcolumns,                 DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,field1TextF);
		string = XmStringCreate ((char *) "Second Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseCompField1Label",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNleftAttachment,          XmATTACH_FORM,
								XmNleftOffset,              10,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             field1TextF,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNlabelString,             string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseCompResultFieldsButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_WIDGET,
								XmNtopWidget,               button,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNmarginHeight,            5,
								XmNtraversalOn,             False,
								XmNlabelString,             string,
								XmNuserData,                DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		resultTextF = XtVaCreateManagedWidget ("RGISAnalyseCompFieldsResultTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             button,
								XmNrightOffset,             10,
								XmNmaxLength,               DBStringLength,
								XmNcolumns,                 DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,resultTextF);
		string = XmStringCreate ((char *) "Result Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseCompFieldsResultLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNleftAttachment,          XmATTACH_FORM,
								XmNleftOffset,              10,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             resultTextF,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNlabelString,             string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&compare);

		menu = XmCreatePulldownMenu (mainForm,(char *) "RGISAnalyseCompFieldsMenu",NULL,0);
		string = XmStringCreate ((char *) "Plain",UICharSetNormal);
		mButton = XtVaCreateManagedWidget ("RGISAnalyseCompFieldsMenuButton",xmPushButtonWidgetClass,menu,
								XmNlabelString,             string,
								XmNuserData,                &diffMethod,
								NULL);
		XmStringFree (string);
		XtAddCallback (mButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(XtPointer) 0);
		string = XmStringCreate ((char *) "Simple %",UICharSetNormal);
		mButton = XtVaCreateManagedWidget ("RGISAnalyseCompFieldsMenuButton",xmPushButtonWidgetClass,menu,
								XmNlabelString,             string,
								XmNuserData,                &diffMethod,
								NULL);
		XmStringFree (string);
		XtAddCallback (mButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(XtPointer) 1);
		string = XmStringCreate ((char *) "Symmetric %",UICharSetNormal);
		mButton = XtVaCreateManagedWidget ("RGISAnalyseCompFieldsMenuButton",xmPushButtonWidgetClass,menu,
								XmNlabelString,             string,
								XmNuserData,                &diffMethod,
								NULL);
		XmStringFree (string);
		XtAddCallback (mButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(XtPointer) 2);
		string = XmStringCreate ((char *) "Difference Method:",UICharSetBold);
		menu = XtVaCreateManagedWidget ("RGISAnalyseCompFieldsMenu",xmRowColumnWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_WIDGET,
								XmNtopWidget,               button,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_FORM,
								XmNbottomOffset,            10,
								XmNrowColumnType,           XmMENU_OPTION,
								XmNlabelString,             string,
								XmNsubMenuId,               menu,
								NULL);
		XmStringFree (string);
		}
	compare = false;
	XtVaSetValues (field0TextF,XmNuserData, itemTable->Fields (), NULL);
	XtVaSetValues (field1TextF,XmNuserData, itemTable->Fields (), NULL);
	XtVaSetValues (resultTextF,XmNuserData, itemTable->Fields (), NULL);
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		f0Text = XmTextFieldGetString (field0TextF);
		f1Text = XmTextFieldGetString (field1TextF);
		rText  = XmTextFieldGetString (resultTextF);
		allowOk = (strlen (f0Text) > 0) && (strlen (f1Text) > 0) && (rText > 0);
		XtFree (f0Text);	XtFree (f1Text);	XtFree (rText);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),allowOk);
		}
	UIDialogFormPopdown (dShell);

	if (compare)
		{
		UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,itemTable));
		DBObjTableField *field;

		f0Text = XmTextFieldGetString (field0TextF);
		f1Text = XmTextFieldGetString (field1TextF);
		rText  = XmTextFieldGetString (resultTextF);
		field = itemTable->Field (rText);
		RGlibGenFuncFieldCompare (itemTable,f0Text,f1Text,rText,diffMethod);
		if (tableCLS != (UITable *) NULL)
			{
			if (field == (DBObjTableField *) NULL)
				tableCLS->AddField (itemTable->Field (rText));
			tableCLS->Draw ();
			}
		XtFree (f0Text);
		XtFree (f1Text);
		XtFree (rText);
		}
	}

static void _RGISAnFieldTopoDialog (DBInt (*function) (DBObjTable *,char *,char *,char *))

	{
	char *nextStnText, *srcText, *dstText;
	static DBInt cont;
	DBDataset *dataset;
	DBObjData *dbData;
	DBObjTable *table;
	static Widget dShell = NULL, mainForm, button, nextStnTextF, srcTextF, dstTextF;

	if (dShell == (Widget) NULL)
		{
		XmString string;

		dShell = UIDialogForm ((char *) "Fields",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseTopoDialogButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_FORM,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNmarginHeight,            5,
								XmNtraversalOn,             False,
								XmNlabelString,             string,
								XmNuserData,                DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		nextStnTextF = XtVaCreateManagedWidget ("RGISAnalyseTopoDialogTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             button,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNmaxLength,               DBStringLength,
								XmNcolumns,                 DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,nextStnTextF);
		string = XmStringCreate ((char *) "Next Station Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseTopoDialogLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNleftAttachment,          XmATTACH_FORM,
								XmNleftOffset,              10,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             nextStnTextF,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNlabelString,             string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseTopoDialogButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_WIDGET,
								XmNtopWidget,               nextStnTextF,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNmarginHeight,            5,
								XmNtraversalOn,             False,
								XmNlabelString,             string,
								XmNuserData,                DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		srcTextF = XtVaCreateManagedWidget ("RGISAnalyseTopoDialogTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             button,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNmaxLength,               DBStringLength,
								XmNcolumns,                 DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,srcTextF);
		string = XmStringCreate ((char *) "Source Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseTopoDialogLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNleftAttachment,          XmATTACH_FORM,
								XmNleftOffset,              10,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             srcTextF,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNlabelString,             string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseTopoSubtractButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_WIDGET,
								XmNtopWidget,               srcTextF,
								XmNtopOffset,               10,
								XmNrightAttachment,         XmATTACH_FORM,
								XmNrightOffset,             10,
								XmNmarginHeight,            5,
								XmNtraversalOn,             False,
								XmNlabelString,             string,
								XmNuserData,                DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		dstTextF = XtVaCreateManagedWidget ("RGISAnalyseTopoSubtractTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             button,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNmaxLength,               DBStringLength,
								XmNcolumns,                 DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,dstTextF);
		string = XmStringCreate ((char *) "Target Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseTopoSubtractLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,           XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,               button,
								XmNleftAttachment,          XmATTACH_FORM,
								XmNleftOffset,              10,
								XmNrightAttachment,         XmATTACH_WIDGET,
								XmNrightWidget,             dstTextF,
								XmNrightOffset,             10,
								XmNbottomAttachment,        XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,            button,
								XmNlabelString,             string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&cont);
		}
	dataset = UIDataset ();
	dbData = dataset->Data ();
	if (_RGISAnalyseFieldTableSelect == (Widget) NULL) _RGISAnalyseFieldTableSelect = UISelectionCreate ((char *) "Table Selection");
	table = dbData->Table (UISelectObject (_RGISAnalyseFieldTableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ()));
	if (table == (DBObjTable *) NULL) return;

	XtVaSetValues (nextStnTextF,XmNuserData,table->Fields (),NULL);
	XtVaSetValues (srcTextF,XmNuserData,table->Fields (),NULL);
	XtVaSetValues (dstTextF,XmNuserData,table->Fields (),NULL);
	cont = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		nextStnText = XmTextFieldGetString (nextStnTextF);
		srcText = XmTextFieldGetString (srcTextF);
		if ((strlen (nextStnText) > 0) && table->Field (nextStnText) != (DBObjTableField *) NULL &&
			 (strlen (srcText) > 0) && table->Field (srcText) != (DBObjTableField *) NULL)
				XtSetSensitive (UIDialogFormGetOkButton (dShell),True);
		else	XtSetSensitive (UIDialogFormGetOkButton (dShell),False);
		XtFree (nextStnText);
		XtFree (srcText);
		}
	UIDialogFormPopdown (dShell);

	if (cont)
		{
		UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,table));
		DBObjTableField *dstFLD;

		UIPauseDialogOpen ((char *) "Calculating");
		nextStnText = XmTextFieldGetString (nextStnTextF);
		srcText = XmTextFieldGetString (srcTextF);
		dstText = XmTextFieldGetString (dstTextF);
		if (tableCLS != (UITable *) NULL) dstFLD = table->Field (dstText);
		(*function) (table,nextStnText,srcText,dstText);
		if (tableCLS != (UITable *) NULL)
			{
			if (dstFLD == (DBObjTableField *) NULL) tableCLS->AddField (table->Field (dstText));
			tableCLS->Draw ();
			}
		XtFree (nextStnText);
		XtFree (srcText);
		XtFree (dstText);
		UIPauseDialogClose ();
		}
	}

void RGISAnalyseFieldTopoAccumCBK (Widget widget, void *userData,XmAnyCallbackStruct *callData)

	{
	widget = widget; userData = userData; callData = callData;
	_RGISAnFieldTopoDialog (RGlibGenFuncTopoAccum);
	}

void RGISAnalyseFieldTopoSubtractCBK (Widget widget, void *userData,XmAnyCallbackStruct *callData)

	{
	widget = widget; userData = userData; callData = callData;
	_RGISAnFieldTopoDialog (RGlibGenFuncTopoSubtract);
	}
