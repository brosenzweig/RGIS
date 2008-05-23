/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UIGetString.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/LabelG.h>
#include <Xm/TextF.h>
#include <UI.H>

static void _UIGetStringTextValueChangedCBK (Widget widget,int *strLength,XmTextVerifyCallbackStruct *callData)

	{	if (callData->reason != XmCR_VALUE_CHANGED) return; *strLength = strlen (XmTextFieldGetString (widget));	}

char *UIGetString (char *labelText,int length)

	{
	DBInt save = false, strLength = 0;
	XmString string;
	Widget dShell, mainForm, textF;

	dShell = UIDialogForm ("String Input");
	mainForm = UIDialogFormGetMainForm (dShell);
	textF = XtVaCreateManagedWidget ("UIGetStringTextF",xmTextFieldWidgetClass,mainForm,
								XmNuserData,				mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			5,
								XmNmaxLength,				length,
								XmNcolumns,					length,
								NULL);
	XtAddCallback (textF,XmNvalueChangedCallback,(XtCallbackProc) _UIGetStringTextValueChangedCBK,(XtPointer) &strLength);
	string = XmStringCreate (labelText,UICharSetBold);
	XtVaCreateManagedWidget ("UITableFieldEditFieldNameLabel",xmLabelGadgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				textF,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftOffset,				5,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			textF,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			textF,
								XmNlabelString,			string,
								NULL);
	XmStringFree (string);
	XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&save);
	
	UIDialogFormPopup (dShell);
	while (UILoop ())
		XtSetSensitive (UIDialogFormGetOkButton (dShell),strLength > 0);
	UIDialogFormPopdown (dShell);
	XtDestroyWidget (dShell);
	if (strLength > 0) return (XmTextFieldGetString (textF));
	else	return ((char *) NULL);
	}
