/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

UIFieldEdit.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Frame.h>
#include <UI.H>

#define UITableFieldEditNameTextF			"*UITableFieldEditNameTextF"

#define UITableFieldEditTypeMenu				"*UITableFieldEditTypeMenu"
#define UITableFieldEditTypeCharButton		"*UITableFieldEditTypeCharacterButton"
#define UITableFieldEditTypeIntButton		"*UITableFieldEditTypeIntButton"
#define UITableFieldEditTypeFloatButton	"*UITableFieldEditTypeFloatButton"
#define UITableFieldEditTypeOptionMenu		"*UITableFieldEditTypeOptionMenu"

#define UITableFieldEditFieldNodataTextF	"*UITableFieldEditFieldNodataTextF"
#define UITableFieldEditFieldNodataLabel	"*UITableFieldEditFieldNodataLabel"

#define UITableFieldEditSizeMenu				"*UITableFieldEditSizeMenu"
#define UITableFieldEditSizeByteButton		"*UITableFieldEditSizeByteButton"
#define UITableFieldEditSizeShortButton	"*UITableFieldEditSizeShortButton"
#define UITableFieldEditSizeLongButton		"*UITableFieldEditSizeLongButton"
#define UITableFieldEditSizeOptionMenu		"*UITableFieldEditSizeOptionMenu"

#define UITableFieldEditWidthLabelF			"*UITableFieldEditWidthLabelF"
#define UITableFieldEditWidthScale			"*UITableFieldEditWidthScale"

#define UITableFieldEditDecimalsScale		"*UITableFieldEditDecimalsScale"
#define UITableFieldEditDecimalsLabel		"*UITableFieldEditDecimalsLabel"

static void _UITableFieldEditWidthScale (Widget widget,DBInt value)

	{
	char numberString [13];

	sprintf (numberString,"%d",value);
	UIAuxSetLabelString (widget,numberString);
	}

static void _UITableFieldEditDisplayWidgets (Widget mainForm, DBObjTableField *field)

	{
	DBInt minimum, currentMin, maximum, currentMax, value;
	char noDataString [DBStringLength];

	switch (field->Type ())
		{
		case DBTableFieldFloat:
			minimum = 4;	maximum = 20;
			value = field->FormatWidth ();
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditDecimalsLabel));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditDecimalsScale));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditFieldNodataLabel));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditFieldNodataTextF));
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditSizeByteButton));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditSizeShortButton));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditSizeLongButton));
			XmScaleSetValue (XtNameToWidget (mainForm,UITableFieldEditDecimalsScale),field->FormatDecimals ());
			switch (field->Length ())
				{
				case sizeof (DBFloat4):
					XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditSizeOptionMenu),
							XmNmenuHistory,	XtNameToWidget (mainForm,UITableFieldEditSizeShortButton),NULL);
					break;
				case sizeof (DBFloat):
					XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditSizeOptionMenu),
							XmNmenuHistory,	XtNameToWidget (mainForm,UITableFieldEditSizeLongButton),NULL);
					break;
				}
			sprintf (noDataString,"%f",field->FloatNoData ());
			XmTextFieldSetString (XtNameToWidget (mainForm,UITableFieldEditFieldNodataTextF),noDataString);
			break;
		case DBTableFieldInt:
			minimum = 1;	maximum = 16;
			value = field->FormatWidth ();
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditDecimalsLabel));
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditDecimalsScale));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditFieldNodataLabel));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditFieldNodataTextF));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditSizeByteButton));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditSizeShortButton));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditSizeLongButton));
			switch (field->Length ())
				{
				case sizeof (DBByte):
					XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditSizeOptionMenu),
							XmNmenuHistory,	XtNameToWidget (mainForm,UITableFieldEditSizeByteButton),NULL);
					break;
				case sizeof (DBShort):
					XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditSizeOptionMenu),
							XmNmenuHistory,	XtNameToWidget (mainForm,UITableFieldEditSizeShortButton),NULL);
					break;
				case sizeof (DBInt):
					XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditSizeOptionMenu),
							XmNmenuHistory,	XtNameToWidget (mainForm,UITableFieldEditSizeLongButton),NULL);
					break;
				}
			sprintf (noDataString,"%d",field->IntNoData ());
			XmTextFieldSetString (XtNameToWidget (mainForm,UITableFieldEditFieldNodataTextF),noDataString);
			break;
		case DBTableFieldString:
			minimum = 0;	maximum = 10;
			for (value = 0;((0x01 << value) != field->FormatWidth ()) && (value < maximum);++value);
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditDecimalsLabel));
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditDecimalsScale));
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditFieldNodataLabel));
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditFieldNodataTextF));
			XtManageChild (XtNameToWidget (mainForm,UITableFieldEditSizeByteButton));
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditSizeShortButton));
			XtUnmanageChild (XtNameToWidget (mainForm,UITableFieldEditSizeLongButton));
			XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditSizeOptionMenu),
							XmNmenuHistory,	XtNameToWidget (mainForm,UITableFieldEditSizeByteButton),NULL);
			break;
		}
	XtVaGetValues (XtNameToWidget (mainForm,UITableFieldEditWidthScale),	XmNminimum,	&currentMin,	XmNmaximum,	&currentMax,	NULL);
	XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditWidthScale),
						XmNminimum,	currentMin < minimum ? currentMin : minimum,
						XmNmaximum,	currentMax > maximum ? currentMax : maximum,	NULL);
	_UITableFieldEditWidthScale (XtNameToWidget (mainForm,UITableFieldEditWidthLabelF),field->FormatWidth ());
	XmScaleSetValue (XtNameToWidget (mainForm,UITableFieldEditWidthScale),value);
	XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditWidthScale),	XmNminimum,	minimum, XmNmaximum,	maximum,	NULL);
	}

static void _UITableFieldEditWidthScaleValueCBK (Widget widget,Widget label,XmScaleCallbackStruct *callData)

	{
	Widget mainForm;
	DBObjTableField *field;

	XtVaGetValues (widget, XmNuserData, &mainForm, NULL);
	XtVaGetValues (mainForm, XmNuserData, &field,  NULL);
	switch (field->Type ())
		{
		case DBTableFieldString:	field->Length (0x01 << callData->value);	break;
		case DBTableFieldInt:		field->FormatWidth (callData->value);		break;
		case DBTableFieldFloat:
			field->FormatWidth (callData->value);
			XmScaleSetValue (XtNameToWidget (mainForm,UITableFieldEditDecimalsScale),field->FormatDecimals ());
			XtVaSetValues (XtNameToWidget (mainForm,UITableFieldEditDecimalsScale),
								XmNmaximum,					callData->value - 3,
								NULL);
			break;
		}
	_UITableFieldEditWidthScale (label,field->FormatWidth ());
	}

static void _UITableFieldEditDecimalScaleValueCBK (Widget widget,void *data,XmScaleCallbackStruct *callData)

	{
	Widget mainForm;
	DBObjTableField *field;

	data = data;
	XtVaGetValues (widget, XmNuserData, &mainForm, NULL);
	XtVaGetValues (mainForm, XmNuserData, &field,  NULL);
	if (field->Type () != DBTableFieldFloat) return;
	field->FormatDecimals (callData->value);
	}

static void _UITableFieldEditTypeButtonCBK (Widget widget,DBInt type, XmAnyCallbackStruct *callData)

	{
	Widget mainForm;
	DBObjTableField *field;
	
	callData = callData;
	XtVaGetValues (widget,	XmNuserData, &mainForm,	NULL);
	XtVaGetValues (mainForm,XmNuserData, &field, 	NULL);
	
	field->Type (type); 
	_UITableFieldEditDisplayWidgets (mainForm,field);
	}

static void _UITableFieldEditSizeButtonCBK (Widget widget,DBInt size, XmAnyCallbackStruct *callData)

	{
	Widget mainForm;
	DBObjTableField *field;
	
	callData = callData;
	XtVaGetValues (widget, XmNuserData, &mainForm, NULL);
	XtVaGetValues (mainForm, XmNuserData, &field,  NULL);
	if ((field->Type () != DBTableFieldInt) || (field->Type () != DBTableFieldFloat)) return;

	if (field->Type () == DBTableFieldFloat) size = size << 0x01;
	field->Length (size);	
	}

static void _UITableFieldEditTextValueChangedCBK (Widget widget,void *data,XmTextVerifyCallbackStruct *callData)

	{
	char *string;
	Widget mainForm;
	DBObjTableField *field;

	data = data;
	if (callData->reason != XmCR_VALUE_CHANGED) return;
	XtVaGetValues (widget, XmNuserData, &mainForm, NULL);
	XtVaGetValues (mainForm, XmNuserData, &field,  NULL);
	if ((string = XmTextFieldGetString (widget)) != (char *) NULL)
		{ field->Name (string); XtFree (string); }
	}

static void _UITableFieldEditNoDataValueChangedCBK (Widget widget,void *data,XmTextVerifyCallbackStruct *callData)

	{
	char *string;
	Widget mainForm;
	DBObjTableField *field;

	data = data;
	if (callData->reason != XmCR_VALUE_CHANGED) return;
	XtVaGetValues (widget, XmNuserData, &mainForm, NULL);
	XtVaGetValues (mainForm, XmNuserData, &field,  NULL);
	if ((string = XmTextFieldGetString (widget)) != (char *) NULL)
		{
		switch (field->Type ())
			{
			case DBTableFieldInt:
				{
				DBInt noData;
				if (sscanf (string,"%d",&noData) == 1) field->IntNoData (noData);
				} break;
			case DBTableFieldFloat:
				{
				DBFloat noData;
				if (sscanf (string,"%lf",&noData) == 1) field->FloatNoData (noData);
				} break;
			}
		XtFree (string); }
	}

DBObjTableField *UITableFieldEdit ()

	{
	DBObjTableField *field = new DBObjTableField (), *retField;
	
	retField = UITableFieldEdit (field);
	delete field;
	return (retField);
	}

DBObjTableField *UITableFieldEdit (DBObjTableField *field)

	{
	static DBInt changed, save;
	DBObjTableField *retField = new DBObjTableField (*field);
	Dimension width, maxWidth;
	static Widget dShell = (Widget) NULL, mainForm = (Widget) NULL;

	if (dShell == (Widget) NULL)
		{
		XmString string;
		Widget label, textF, subMenu, menu, button, frame, scale;
		dShell = UIDialogForm ("Field Edit");
		mainForm = UIDialogFormGetMainForm (dShell);

		textF = XtVaCreateManagedWidget (UITableFieldEditNameTextF + 1,xmTextFieldWidgetClass,mainForm,
								XmNuserData,				mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNleftAttachment,		XmATTACH_POSITION,
								XmNleftPosition,			25,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			5,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					DBStringLength,
								NULL);
		XtAddCallback (textF,XmNvalueChangedCallback,(XtCallbackProc) _UITableFieldEditTextValueChangedCBK,(XtPointer) NULL);
		string = XmStringCreate ("Field Name:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UITableFieldEditFieldNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				textF,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			textF,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			textF,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		subMenu = XmCreatePulldownMenu (mainForm,(char *) UITableFieldEditTypeMenu + 1,NULL,0);
		string = XmStringCreate ("Character",UICharSetNormal);
		button = XtVaCreateManagedWidget (UITableFieldEditTypeCharButton + 1,xmPushButtonGadgetClass,subMenu,
								XmNuserData,				mainForm,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldEditTypeButtonCBK,(XtPointer) DBTableFieldString);
		XtVaGetValues (button, XmNwidth, &width, NULL);
		maxWidth = width;
		string = XmStringCreate ("Integer",UICharSetNormal);
		button = XtVaCreateManagedWidget (UITableFieldEditTypeIntButton + 1,xmPushButtonGadgetClass,subMenu,
								XmNuserData,				mainForm,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldEditTypeButtonCBK,(XtPointer) DBTableFieldInt);
		XtVaGetValues (button, XmNwidth, &width, NULL);
		maxWidth = maxWidth > width ? maxWidth : width;
		string = XmStringCreate ("Float",UICharSetNormal);
		button = XtVaCreateManagedWidget (UITableFieldEditTypeFloatButton + 1,xmPushButtonGadgetClass,subMenu,
								XmNuserData,				mainForm,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldEditTypeButtonCBK,(XtPointer) DBTableFieldFloat);
		XtVaGetValues (button, XmNwidth, &width, NULL);
		maxWidth = maxWidth > width ? maxWidth : width;
		string = XmStringCreate ("Data Type:",UICharSetBold);
		menu = XtVaCreateManagedWidget (UITableFieldEditTypeOptionMenu + 1,xmRowColumnWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				textF,
								XmNtopOffset,				10,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				5,
								XmNsubMenuId,				subMenu,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);

		textF = XtVaCreateManagedWidget (UITableFieldEditFieldNodataTextF + 1,xmTextFieldWidgetClass,mainForm,
								XmNuserData,				mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				menu,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			menu,
								XmNmaxLength,				10 ,
								XmNcolumns,					10 ,
								NULL);
		XtAddCallback (textF,XmNvalueChangedCallback,(XtCallbackProc) _UITableFieldEditNoDataValueChangedCBK,(XtPointer) NULL);
		string = XmStringCreate ("Nodata:",UICharSetBold);
		label = XtVaCreateManagedWidget (UITableFieldEditFieldNodataLabel + 1,xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				textF,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			textF,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			textF,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		subMenu = XmCreatePulldownMenu (mainForm,(char *) UITableFieldEditSizeMenu + 1,NULL,0);
		string = XmStringCreate ("Byte",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableFieldEditSizeByteButton",xmPushButtonGadgetClass,subMenu,
								XmNuserData,				mainForm,
								XmNwidth, 					maxWidth,
								XmNrecomputeSize, 		False,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldEditSizeButtonCBK,(XtPointer) sizeof (DBByte));
		string = XmStringCreate ("Short",UICharSetNormal);
		button = XtVaCreateManagedWidget (UITableFieldEditSizeShortButton + 1,xmPushButtonGadgetClass,subMenu,
								XmNuserData,				mainForm,
								XmNwidth, 					maxWidth,
								XmNrecomputeSize, 		False,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldEditSizeButtonCBK,(XtPointer) sizeof (DBShort));
		string = XmStringCreate ("Long",UICharSetNormal);
		button = XtVaCreateManagedWidget (UITableFieldEditSizeLongButton + 1,xmPushButtonGadgetClass,subMenu,
								XmNuserData,				mainForm,
								XmNwidth, 					maxWidth,
								XmNrecomputeSize, 		False,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldEditSizeButtonCBK,(XtPointer) sizeof (DBInt));
		string = XmStringCreate ("Size:",UICharSetBold);
		menu = XtVaCreateManagedWidget (UITableFieldEditSizeOptionMenu + 1,xmRowColumnWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				menu,
								XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNrightWidget,			menu,
								XmNsubMenuId,				subMenu,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);
	
		frame = XtVaCreateManagedWidget ("UITableFieldEditWidthFrame",xmFrameWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				menu,
								XmNtopOffset,				5,
								XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNrightWidget,			menu,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			5,
								NULL);
		string = XmStringCreate ("data",UICharSetNormal);
		label = XtVaCreateManagedWidget (UITableFieldEditWidthLabelF + 1,xmLabelWidgetClass,frame,
								XmNmarginWidth,			5,
								XmNalignment,				XmALIGNMENT_END,
								XmNlabelString,			string,
								XmNrecomputeSize,			False,
								NULL);
		XmStringFree (string);
		scale = XtVaCreateManagedWidget (UITableFieldEditWidthScale + 1,xmScaleWidgetClass,mainForm,
								XmNuserData,				mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				frame,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			frame,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			frame,
								XmNorientation,			XmHORIZONTAL,
								XmNminimum,					0,
								XmNmaximum,					9,
								XmNvalue,					6,
								XmNscaleWidth,				60,
								XmNtraversalOn,			False,
								NULL);
		XtAddCallback (scale,XmNdragCallback,(XtCallbackProc) _UITableFieldEditWidthScaleValueCBK,(XtPointer) label);
		string = XmStringCreate ("Width:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UITableFieldEditWidthLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				frame,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			scale,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			frame,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		scale = XtVaCreateManagedWidget (UITableFieldEditDecimalsScale + 1,xmScaleWidgetClass,mainForm,
								XmNuserData,				mainForm,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			frame,
								XmNorientation,			XmHORIZONTAL,
								XmNminimum,					0,
								XmNmaximum,					9,
								XmNvalue,					3,
								XmNshowValue,				True,
								XmNscaleWidth,				60,
								XmNtraversalOn,			False,
								NULL);
		XtAddCallback (scale,XmNdragCallback,(XtCallbackProc) _UITableFieldEditDecimalScaleValueCBK,(XtPointer) NULL);
		string = XmStringCreate ("Decimals:",UICharSetBold);
		label = XtVaCreateManagedWidget (UITableFieldEditDecimalsLabel + 1,xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				frame,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			scale,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			frame,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&save);
		}
	UIDialogFormPopup (dShell);
	XtVaSetValues (mainForm,	XmNuserData,	retField,	NULL);
	_UITableFieldEditDisplayWidgets (mainForm,retField);
	XmTextFieldSetString (XtNameToWidget (mainForm,UITableFieldEditNameTextF),retField->Name ());

	save = false;
	while (UILoop ())
		{
		changed = false;
		if (strlen (retField->Name ()) == 0) continue;
		if (retField->Type () != field->Type ())								changed = true;
		else if (strcmp (retField->Name (),field->Name ()) != 0)			changed = true;
		else if (strcmp (retField->Format (),field->Format ()) != 0)	changed = true;
		else if (retField->Length () != field->Length ())					changed = true;
		else switch (retField->Type ())
				{
				case DBTableFieldInt:
					if (retField->IntNoData () != field->IntNoData ())		changed = true;
					break;
				case DBTableFieldFloat:
					if (DBMathEqualValues (retField->FloatNoData (),field->FloatNoData ()))	changed = true;
					break;
				}
		XtSetSensitive (UIDialogFormGetOkButton (dShell),changed);
		}
	UIDialogFormPopdown (dShell);
	if (save) 	return (retField);
	else delete retField;
	return ((DBObjTableField *) NULL);
	}
