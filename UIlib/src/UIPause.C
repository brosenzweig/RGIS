/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UIPause.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/DrawingA.h>
#include <Xm/Separator.h>
#include <UI.H>

static bool   _UIPauseOn = true;
static Widget _UIPauseDShell = NULL;
static Widget _UIPauseButton, _UIContButton, _UIStopButton, _UIDrawingArea;

static void _UIPauseCallback (Widget widget,int data,XmAnyCallbackStruct *callData)

	{
	widget = widget; data = data; callData = callData;
	XtSetSensitive (_UIContButton,true);
	XtSetSensitive (_UIPauseButton,false);
	while (UILoop ()); 
	XtSetSensitive (_UIContButton,false);
	XtSetSensitive (_UIPauseButton,true);
	}

void UIPauseDisplay (bool display) { _UIPauseOn = display; }

void UIPauseDialogOpen (char *title) { UIPauseDialogOpen (title,true); }

void UIPauseDialogOpen (char *title,DBInt dbPause)

	{
	static int cont;
	Widget mainForm, menuForm, separator;
	XmString string;

	if (_UIPauseOn == false) return;
	if (_UIPauseDShell == NULL)
		{
		_UIPauseDShell = XtVaCreatePopupShell ("UIPauseWidgetShell",xmDialogShellWidgetClass,UITopLevel (),
											XmNallowShellResize,		true,
											XmNtransient,				true,
											XmNminWidth,				400,
											XmNminHeight,				100,
											XmNkeyboardFocusPolicy,	XmEXPLICIT,
											NULL);
		mainForm = XtVaCreateManagedWidget ("UIPauseWidget",xmFormWidgetClass,_UIPauseDShell,
											XmNnoResize,				true,
											XmNdialogStyle,			XmDIALOG_FULL_APPLICATION_MODAL,
											XmNshadowThickness,		0,
											NULL);
		menuForm = XtVaCreateManagedWidget ("UIPauseMenuForm",xmFormWidgetClass,mainForm, 
											XmNleftAttachment,		XmATTACH_FORM,
											XmNrightAttachment, 		XmATTACH_FORM,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNmarginWidth,			30,
											XmNmarginHeight,			10,
											XmNspacing,					20,
											XmNpacking,					XmPACK_COLUMN,
											XmNorientation,			XmHORIZONTAL,
											XmNentryAlignment,		XmALIGNMENT_CENTER,
											XmNrowColumnType,			XmMENU_BAR,
											XmNshadowThickness,		0,
											NULL);
		separator = XtVaCreateManagedWidget ("UIPauseSeparator",xmSeparatorWidgetClass,mainForm,
											XmNorientation,			XmHORIZONTAL,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNbottomAttachment,		XmATTACH_WIDGET,
											XmNbottomWidget,			menuForm,
											NULL);

		_UIDrawingArea = XtVaCreateManagedWidget ("UIPauseDisplay",xmDrawingAreaWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				15,
											XmNleftOffset,				30,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNrightOffset,			30,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			15,
											XmNbottomAttachment,		XmATTACH_WIDGET,
											XmNbottomWidget,			separator,
											XmNmaxWidth,				400,
											XmNmaxHeight,				30,
											XmNminWidth,				400,
											XmNminHeight,				30,
											XmNwidth,					400,
											XmNheight,					30,
											XmNbackground,				UIColor (UIColorStandard,0),
											NULL);
		string = XmStringCreate ("Pause",UICharSetNormal);
		_UIPauseButton = XtVaCreateManagedWidget ("UIPauseButton",xmPushButtonWidgetClass,menuForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			10,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			30,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNlabelString,			string,
											NULL);
		XmStringFree (string);
		string = XmStringCreate ("Stop",UICharSetNormal);
		XtAddCallback (_UIPauseButton,XmNactivateCallback,(XtCallbackProc) _UIPauseCallback,0);

		_UIStopButton = XtVaCreateManagedWidget ("UIPauseStopButton",xmPushButtonWidgetClass,menuForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			40,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			60,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNlabelString,			string,
											NULL);
		XmStringFree (string);
		XtAddCallback (_UIStopButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanFalseCBK,&cont);
		XtAddCallback (_UIStopButton,XmNactivateCallback,(XtCallbackProc) UILoopStopCBK,NULL);
		string = XmStringCreate ("Continue",UICharSetNormal);
		_UIContButton = XtVaCreateManagedWidget ("UIPauseContButton",xmPushButtonWidgetClass,menuForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			70,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			90,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNlabelString,			string,
											NULL);
		XmStringFree (string);
		XtAddCallback (_UIContButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanFalseCBK,&cont);
		}
	XtVaSetValues (_UIPauseDShell,XmNtitle, title, NULL);
	XtPopup (_UIPauseDShell,XtGrabNone);
	XtSetSensitive (_UIContButton,false);
	XtSetSensitive (_UIPauseButton,true);
	if (dbPause) DBPauseSetFunction (UIPause);
	UIPause (0);
	}

int UIPause (int perCent)

	{
	static int prevPerCent = 0;
	static GC gc = NULL;
	XWindowAttributes xwa;
	
	if (_UIPauseDShell == NULL) return (false);

	if (gc == NULL)
		{
		XGCValues xgcv;
		xgcv.foreground = UIColor (UIColorStandard,4);
		gc = XtGetGC (_UIDrawingArea,GCForeground,&xgcv);
		}
	if (prevPerCent != perCent)
		{
		prevPerCent = perCent;
		XGetWindowAttributes (XtDisplay (_UIDrawingArea),XtWindow (_UIDrawingArea),&xwa);
		XFillRectangle (XtDisplay (_UIDrawingArea),XtWindow (_UIDrawingArea),gc, 0,0,
							 xwa.width * prevPerCent / 100,xwa.height);
		XmUpdateDisplay (_UIPauseDShell);
		if (QLength (XtDisplay (UITopLevel ())) > 0) return (!UILoop ());
		}
	return (false);
	}

void UIPauseDialogClose ()

	{ if (_UIPauseDShell != NULL) XtPopdown (_UIPauseDShell); DBPauseSetFunction (NULL);}
