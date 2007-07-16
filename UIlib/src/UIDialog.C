/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

UIDialog.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <UI.H>

void UIMessage (char *message)

	{
	static int answer;
	Widget dShell;
	static Widget dialogWidget = NULL;
		XmString messageString;

	messageString = XmStringCreate (message,UICharSetNormal);
	if (dialogWidget == NULL) 
		{
		dShell =  XtVaCreatePopupShell ("UIQuestionDialogShell",xmDialogShellWidgetClass,UITopLevel (),
									XmNallowShellResize,			true,
									XmNtransient,					true,
									XmNkeyboardFocusPolicy,		XmEXPLICIT,
									NULL);
		dialogWidget = XtVaCreateManagedWidget ("UIWarningDialog",xmMessageBoxWidgetClass,dShell,
									XmNnoResize,			true,
									XmNautoUnmanage,		false,
									XmNdialogTitle,		XmStringCreate ("Warning",UICharSetNormal),
									XmNdialogStyle,		XmDIALOG_FULL_APPLICATION_MODAL,
									XmNokLabelString,		XmStringCreate ("Accept",UICharSetNormal),
									XmNdialogType,			XmDIALOG_WARNING,
									NULL);
		XtAddCallback (dialogWidget,XmNokCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&answer);
		XtAddCallback (dialogWidget,XmNokCallback,(XtCallbackProc) UILoopStopCBK,0);
		XtUnmanageChild (XmMessageBoxGetChild (dialogWidget,XmDIALOG_CANCEL_BUTTON));
		XtUnmanageChild (XmMessageBoxGetChild (dialogWidget,XmDIALOG_HELP_BUTTON));
		}
	messageString = XmStringCreate (message,UICharSetNormal);
	XtVaSetValues (dialogWidget,XmNmessageString, messageString, NULL);
	XtManageChild (dialogWidget);

	while (UILoop ());
	XtUnmanageChild (dialogWidget);
	XmStringFree (messageString);
	}

int UIYesOrNo (char *message)

	{
	static int answer;
	Widget dShell;
	static Widget dialogWidget = NULL;
	XmString messageString;
	
	if (dialogWidget == NULL) 
		{
		dShell =  XtVaCreatePopupShell ("UIQuestionDialogShell",xmDialogShellWidgetClass,UITopLevel (),
									XmNallowShellResize,			true,
									XmNtransient,					true,
									XmNkeyboardFocusPolicy,		XmEXPLICIT,
									NULL);
		dialogWidget = XtVaCreateManagedWidget ("UIQuestionDialog",xmMessageBoxWidgetClass,dShell,
									XmNnoResize,			true,
									XmNautoUnmanage,		false,
									XmNdialogTitle,		XmStringCreate ("Question",UICharSetNormal),
									XmNdialogStyle,		XmDIALOG_FULL_APPLICATION_MODAL,
									XmNokLabelString,		XmStringCreate ("Yes",UICharSetNormal),
									XmNcancelLabelString,XmStringCreate ("No",UICharSetNormal),
									NULL);
		XtAddCallback(dialogWidget,XmNokCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&answer);
		XtAddCallback(dialogWidget,XmNokCallback,(XtCallbackProc) UILoopStopCBK,0);
		XtAddCallback(dialogWidget,XmNcancelCallback,(XtCallbackProc)UIAuxSetBooleanFalseCBK,&answer);
		XtAddCallback(dialogWidget,XmNcancelCallback,(XtCallbackProc)UILoopStopCBK,0);
		XtUnmanageChild (XmMessageBoxGetChild (dialogWidget,XmDIALOG_HELP_BUTTON));
		}
	messageString = XmStringCreate (message,UICharSetNormal);
	XtVaSetValues (dialogWidget,XmNmessageString, messageString, NULL);
	XtManageChild (dialogWidget);
	while (UILoop ());
	XtUnmanageChild (dialogWidget);
	XmStringFree (messageString);
	return (answer);
	}
