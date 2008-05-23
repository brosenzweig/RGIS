/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UIDialogForm.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/BulletinB.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <UI.H>

#define UIDialogFormName			"*UIDialogForm"
#define UIDialogBulletinName		"*UIDialogBulletin"
#define UIDialogPaneName			"*UIDialogPane"
#define UIDialogActionFormName	"*UIDialogActionForm"
#define UIDialogOkButtonName		"*UIDialogOkButton"
#define UIDialogCancelButtonName	"*UIDialogCancelButton"

Widget UIDialogFormGetMainForm (Widget widget)
	{ return (XtNameToWidget (widget,UIDialogFormName)); }

Widget UIDialogFormGetOkButton (Widget widget)
	{ return (XtNameToWidget (widget,UIDialogOkButtonName)); }

Widget UIDialogFormGetCancelButton (Widget widget)
	{ return (XtNameToWidget (widget,UIDialogCancelButtonName)); }

void UIDialogFormTitle (Widget widget,char *title)

	{ XtVaSetValues (widget,XmNtitle, title, NULL); }

void UIDialogFormPopup (Widget widget)

	{
	Dimension actionHeight;

	if (!XtIsManaged (XtNameToWidget (widget,UIDialogBulletinName)))
		{
		Widget *children;
		int numChildren;
		
		XtVaGetValues (XtNameToWidget (widget,UIDialogPaneName),
								XmNchildren, 			&children,
								XmNnumChildren,		&numChildren, NULL);
		XtManageChild (XtNameToWidget (widget,UIDialogBulletinName));
		
		while (numChildren-- > 0)
			if (XmIsTraversable (children [numChildren])) XtVaSetValues (children [numChildren],XmNtraversalOn, false, NULL);
		XtVaGetValues (XtNameToWidget (widget,UIDialogActionFormName),XmNheight, &actionHeight, NULL);
		XtVaSetValues (XtNameToWidget (widget,UIDialogActionFormName),XmNpaneMaximum, actionHeight,
																						  XmNpaneMinimum, actionHeight,
																						  NULL);
		}
	XtPopup (widget,XtGrabNone);
	UILoop ();
	}

void UIDialogFormPopdown (Widget widget) { XtPopdown (widget); }

Widget UIDialogForm (char *dialogTitle) { return (UIDialogForm (dialogTitle,true)); }

Widget UIDialogForm (char *dialogTitle,int resize)

	{
	Widget dShell, bulletin, pane, actionForm, button;
	XmString ok = XmStringCreate ("Accept",UICharSetBold);
	XmString cancel = XmStringCreate ("Discard",UICharSetBold);

	dShell = XtVaCreatePopupShell (dialogTitle,xmDialogShellWidgetClass,UITopLevel (),
								XmNkeyboardFocusPolicy,		XmEXPLICIT,
								XmNallowShellResize,			true,
								NULL);
	bulletin = XtVaCreateWidget (UIDialogBulletinName + 1,xmBulletinBoardWidgetClass,dShell,
								XmNnoResize,					resize,
								XmNdialogStyle,				XmDIALOG_FULL_APPLICATION_MODAL,
								XmNtransient,					true,
								XmNmarginHeight,				5,
								XmNmarginWidth,				5,
								NULL);
	XtAddCallback (bulletin,XmNunmapCallback,(XtCallbackProc) UILoopStopCBK,NULL);
	pane = XtVaCreateManagedWidget (UIDialogPaneName + 1,xmPanedWindowWidgetClass,bulletin,
								XmNsashWidth,					1,
								XmNsashHeight,					1,
								XmNallowResize,				true,
								NULL);
	XtVaCreateManagedWidget (UIDialogFormName + 1, xmFormWidgetClass, pane,
								XmNshadowThickness,			0,
								XmNallowResize,				true,
								NULL);
	actionForm = XtVaCreateManagedWidget (UIDialogActionFormName + 1,xmFormWidgetClass,pane,
								XmNallowResize,				false,
								XmNshadowThickness,			0, NULL);
	button = XtVaCreateManagedWidget (UIDialogOkButtonName + 1,xmPushButtonWidgetClass,actionForm,
								XmNtopAttachment,				XmATTACH_FORM,
								XmNtopOffset,					5,
								XmNbottomAttachment,			XmATTACH_FORM,
								XmNbottomOffset,				5,
								XmNleftAttachment,			XmATTACH_POSITION,
								XmNleftPosition,				20,
								XmNrightAttachment,			XmATTACH_POSITION,
								XmNrightPosition,				40,
								XmNlabelString,				ok,
								NULL);
	XtSetSensitive (button,false);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UILoopStopCBK,NULL);
	button = XtVaCreateManagedWidget (UIDialogCancelButtonName+ 1,xmPushButtonWidgetClass,actionForm,
								XmNtopAttachment,				XmATTACH_FORM,
								XmNtopOffset,					5,
								XmNbottomAttachment,			XmATTACH_FORM,
								XmNbottomOffset,				5,
								XmNleftAttachment,			XmATTACH_POSITION,
								XmNleftPosition,				60,
								XmNrightAttachment,			XmATTACH_POSITION,
								XmNrightPosition,				80,
								XmNlabelString,				cancel,
								NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UILoopStopCBK,NULL);
	XmStringFree (ok);
	XmStringFree (cancel);
	return (dShell);
	}
