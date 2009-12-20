/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

UI2DView.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <X11/cursorfont.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/DrawingA.h>
#include <Xm/Protocols.h>
#include <UI.H>

#define ZOOM_MODE 0
#define PANE_MODE 1
#define USER_MODE 2

static DBObjectLIST<UI2DView> _UI2DViewLIST = DBObjectLIST<UI2DView> ("2DView LIST");

DBObjectLIST<UI2DView> *UI2DViewList () { return (&_UI2DViewLIST); }

static UI2DViewUserFunction _UI2DViewUserFunction = (UI2DViewUserFunction) NULL;
static DBObjData *_UI2DViewUserData = NULL;

void UI2DViewChangeUserFunction (UI2DViewUserFunction func)
	{
	DBInt userCond =  func != (UI2DViewUserFunction) NULL ? true : false;
	UI2DView *view;

	_UI2DViewUserFunction = userCond ? func :  (UI2DViewUserFunction) NULL;

	for (view = UI2DViewFirst ();view != (UI2DView *) NULL;view = UI2DViewNext ())
		view->SetUserToggleMode (userCond);
	}
UI2DViewUserFunction UI2DViewGetUserFunction () { return (_UI2DViewUserFunction); }
void UI2DViewChangeUserData (DBObjData *data) { _UI2DViewUserData = data; }
DBObjData *UI2DViewGetUserData () { return (_UI2DViewUserData); }

static Cursor _UI2DViewViewCursor = (Cursor) NULL;
static Cursor _UI2DViewUserCursor = (Cursor) NULL;

void UI2DView::InputMode (int mode)

	{
	XSetWindowAttributes xswa;

	InputModeVAR = mode;
	ActiveCursor = xswa.cursor = mode == USER_MODE ? _UI2DViewUserCursor : _UI2DViewViewCursor;
	if (XtIsRealized (DrawingAreaW))
		XChangeWindowAttributes (XtDisplay (DrawingAreaW), XtWindow (DrawingAreaW), CWCursor, &xswa);
	}


void UI2DView::Set (DBRegion region)

	{
	int value, sliderSize;
	double freeLength, dVal;
	DBRegion dataEXT = (UIDataset ())->Extent ();

	SetExtent (region);
	SetActiveExtent (ViewEXT);

	sliderSize = (int) ((ViewEXT.UpperRight.X - ViewEXT.LowerLeft.X) * 100.0 /
							  (dataEXT.UpperRight.X - dataEXT.LowerLeft.X));
	sliderSize = sliderSize < 100 ? sliderSize : 100;
	sliderSize = sliderSize > 	 1 ? sliderSize :   1;
	freeLength = (dataEXT.UpperRight.X - dataEXT.LowerLeft.X) - (ViewEXT.UpperRight.X - ViewEXT.LowerLeft.X);
	if (freeLength == (double) 0.0) value = 0;
	else
		{
		value = (int) ((ViewEXT.LowerLeft.X - dataEXT.LowerLeft.X) * (100.0 - sliderSize) / freeLength);
		value = value < 100 - sliderSize ? value : 100 - sliderSize;
		value = value > 0 ? value : 0;
		}
	XmScrollBarSetValues (HorScrollBarW,value,sliderSize,(int) 0x0L,sliderSize,false);

	sliderSize = (int) ((ViewEXT.UpperRight.Y - ViewEXT.LowerLeft.Y) * 100.0 /
							  (dataEXT.UpperRight.Y - dataEXT.LowerLeft.Y));
	sliderSize = sliderSize < 100 ? sliderSize : 100;
	sliderSize = sliderSize > 	 1 ? sliderSize :   1;

	freeLength = (dataEXT.UpperRight.Y - dataEXT.LowerLeft.Y) - (ViewEXT.UpperRight.Y - ViewEXT.LowerLeft.Y);
	if (freeLength == (double) 0.0) value = 0;
	else
		{
		value = (int) ((dataEXT.UpperRight.Y - ViewEXT.UpperRight.Y) * (100.0 - sliderSize) / freeLength);
		value = value < 100 - sliderSize ? value : 100 - sliderSize;
		value = value > 0 ? value : 0;
		}
	XmScrollBarSetValues (VerScrollBarW,value,sliderSize,(int) 0x0L,sliderSize,false);

	dVal = (ViewEXT.UpperRight.X - ViewEXT.LowerLeft.X) / (dataEXT.UpperRight.X - dataEXT.LowerLeft.X) <
			 (ViewEXT.UpperRight.Y - ViewEXT.LowerLeft.Y) / (dataEXT.UpperRight.Y - dataEXT.LowerLeft.Y) ?
			 (ViewEXT.UpperRight.X - ViewEXT.LowerLeft.X) / (dataEXT.UpperRight.X - dataEXT.LowerLeft.X) :
			 (ViewEXT.UpperRight.Y - ViewEXT.LowerLeft.Y) / (dataEXT.UpperRight.Y - dataEXT.LowerLeft.Y);
	value = (int) (dVal * 100.0);
	value = value < 100 ? value : 100;
	value = value > 0 ? value : 0;

	XmScaleSetValue (ScaleW,value);
	Draw ();
	}

void UI2DView::SetUserToggleMode (DBInt visible)

	{
	if (visible) XtManageChild (UserToggle);
	else
		{
		if (XmToggleButtonGadgetGetState (UserToggle))
			XmToggleButtonGadgetSetState (ZoomToggle,true,true);
		XtUnmanageChild (UserToggle);
		}
	}

static void _UI2DViewResizeCBK (Widget widget,UI2DView *view,XmDrawingAreaCallbackStruct *callData)

	{
	widget = widget;

	if (callData->reason != XmCR_RESIZE) return;
	view->Size ();
	view->Set ();
	}

static void _UI2DViewHorScrollBarValueChangedCBK (Widget widget,UI2DView *view,XmScrollBarCallbackStruct *callData)

	{
	int value, slider, incr,pgIncr;
	double deltaX, freeLength;
	DBRegion dataEXT = (UIDataset ())->Extent (), viewEXT = view->Extent ();

	callData = callData;
	XmScrollBarGetValues (widget,&value,&slider,&incr,&pgIncr);

	freeLength = (dataEXT.UpperRight.X - dataEXT.LowerLeft.X) - (viewEXT.UpperRight.X - viewEXT.LowerLeft.X);

	deltaX  = slider == 100 ? 0 : freeLength * value / (100.0 - slider);
	deltaX -= (viewEXT.LowerLeft.X - dataEXT.LowerLeft.X);

	viewEXT.LowerLeft.X  = viewEXT.LowerLeft.X  + deltaX;
	viewEXT.UpperRight.X = viewEXT.UpperRight.X + deltaX;

	view->Set  (viewEXT);
	}

static void _UI2DViewVerScrollBarValueChangedCBK (Widget widget,UI2DView *view,XmScrollBarCallbackStruct *callData)

	{
	int value, slider, incr,pgIncr;
	double deltaY, freeLength;
	DBRegion dataEXT = (UIDataset ())->Extent (), viewEXT = view->Extent ();

	callData = callData;
	XmScrollBarGetValues (widget,&value,&slider,&incr,&pgIncr);

	freeLength = (dataEXT.UpperRight.Y - dataEXT.LowerLeft.Y) - (viewEXT.UpperRight.Y - viewEXT.LowerLeft.Y);

	deltaY = slider == 100 ? 0 : freeLength * value / (100.0 - slider);
	deltaY -= dataEXT.UpperRight.Y - viewEXT.UpperRight.Y;

	viewEXT.LowerLeft.Y  = viewEXT.LowerLeft.Y - deltaY;
	viewEXT.UpperRight.Y = viewEXT.UpperRight.Y - deltaY;

	view->Set  (viewEXT);
	}

static void _UI2DViewScaleValueChangedCBK (Widget widget,UI2DView *view,XmScaleCallbackStruct *callData)

	{
	int value;
	double dval;
	DBCoordinate center, size;
	DBRegion extent, dataEXT = (UIDataset ())->Extent (), viewEXT = view->Extent ();

	widget = widget;
	if (callData->reason != XmCR_VALUE_CHANGED) return;

	value = callData->value;
	value = value > 0 ? value : 1;
	dval = (double) value / 100.0;

	center.X = (viewEXT.UpperRight.X + viewEXT.LowerLeft.X) / 2.0;
	center.Y = (viewEXT.UpperRight.Y + viewEXT.LowerLeft.Y) / 2.0;
	size.X	= dataEXT.UpperRight.X - dataEXT.LowerLeft.X;
	size.Y	= dataEXT.UpperRight.Y - dataEXT.LowerLeft.Y;

	size.X = size.X * dval;
	size.Y = size.Y * dval;

	center.X = dataEXT.LowerLeft.X  + size.X / 2.0 > center.X ? dataEXT.LowerLeft.X  + size.X / 2.0 : center.X;
	center.X = dataEXT.UpperRight.X - size.X / 2.0 < center.X ? dataEXT.UpperRight.X - size.X / 2.0 : center.X;

	center.Y = dataEXT.LowerLeft.Y + size.Y / 2.0 > center.Y ? dataEXT.LowerLeft.Y + size.Y / 2.0 : center.Y ;
	center.Y = dataEXT.UpperRight.Y - size.Y / 2.0 < center.Y ? dataEXT.UpperRight.Y - size.Y / 2.0 : center.Y ;

	viewEXT.LowerLeft.X = center.X - size.X / 2.0; viewEXT.LowerLeft.Y = center.Y - size.Y / 2.0;
	extent.LowerLeft  = viewEXT.LowerLeft;
	viewEXT.UpperRight.X = center.X + size.X / 2.0; viewEXT.UpperRight.Y = center.Y + size.Y / 2.0;
	extent.UpperRight = viewEXT.UpperRight;
	view->Set (extent);
	}

void _UI2DViewFullActivateCBK (Widget widget,UI2DView *view,XmPushButtonCallbackStruct *callData)

	{
	widget = widget;

	if (callData->reason != XmCR_ACTIVATE) return;

	view->Set ((UIDataset ())->Extent ());
	}

static void _UI2DViewExtentActivateCBK (Widget widget,UI2DView *view,XmPushButtonCallbackStruct *callData)

	{
	widget = widget;
	DBObjData *data = (UIDataset ())->Data ();
	if (callData->reason != XmCR_ACTIVATE) return;
	if (data == (DBObjData *) NULL) return;

	view->Set (data->Extent ());
	}

static void _UI2DViewRedrawCBK (Widget widget,UI2DView *view,XmPushButtonCallbackStruct *callData)

	{
	widget = widget;
	if (callData->reason != XmCR_ACTIVATE) return;
	view->Draw ();
	}

static void _UI2DViewZoom (Widget widget,XEvent *event,UI2DView *view)

	{
	static int sX, sY, width, height;
	int UIInquireRegion (Widget,XEvent *,int *,int *,int *,int *);

	if (UIInquireRegion(widget,event,&sX,&sY,&width,&height))
		{
		DBFloat mX, mY;
		DBCoordinate lowerLeft, upperRight;
		DBRegion extent;

		view->Window2Map  (sX,sY + height, &mX, &mY);
		lowerLeft.X = mX; lowerLeft.Y = mY;
		view->Window2Map  (sX + width,sY, &mX, &mY);
		upperRight.X = mX; upperRight.Y = mY;
		extent.LowerLeft  = lowerLeft;
		extent.UpperRight = upperRight;
		view->Set (extent);
		}
	}

static void _UI2DViewPane (Widget widget,XEvent *event,UI2DView *view)

	{
	static int x0, y0, x1, y1;
	int UIInquireVector (Widget,XEvent *,int,int,int *,int *);

	if (event->type == ButtonPress)
		{ x0 = event->xbutton.x; y0 = event->xbutton.y; }
	if (UIInquireVector (widget,event, x0, y0,&x1,&y1))
		{
		DBCoordinate coord0, coord1;
		DBRegion extent = view->Extent ();

	 	view->Window2Map  (x0,y0, &coord0.X, &coord0.Y);
	 	view->Window2Map  (x1,y1, &coord1.X, &coord1.Y);

	 	extent = extent + (coord0 - coord1);
	 	view->Set (extent);
		}
	}

static void _UI2DViewZoomToggleValueCBK (Widget widget,UI2DView *view,XmToggleButtonCallbackStruct *callData)

	{ widget = widget; if (callData->set) view->InputMode (ZOOM_MODE); }

static void _UI2DViewPaneToggleValueCBK (Widget widget,UI2DView *view,XmToggleButtonCallbackStruct *callData)

	{ widget = widget; if (callData->set) view->InputMode (PANE_MODE); }

static void _UI2DViewUserToggleValueCBK (Widget widget,UI2DView *view,XmToggleButtonCallbackStruct *callData)

	{ widget = widget; if (callData->set) view->InputMode (USER_MODE); }

static void _UI2DViewMeshOptionCBK (Widget widget,UI2DView *view,XmToggleButtonCallbackStruct *callData)

	{ widget = widget; if (callData->set) view->DrawMesh (); else	view->Draw (); }

static void _UI2DViewPointerEHR (Widget widget,UI2DView *view,XEvent *event,Boolean *boolean)

	{
	boolean = boolean;

	switch (view->InputMode ())
		{
		case ZOOM_MODE:
			_UI2DViewZoom (widget,event,view);		break;
		case PANE_MODE:
			_UI2DViewPane (widget,event,view);		break;
		case USER_MODE:
			if (_UI2DViewUserFunction != (UI2DViewUserFunction) NULL) (*_UI2DViewUserFunction) (_UI2DViewUserData,view,event);
			break;
		default:	break;
		}
	}

static void _UI2DViewDeleteCBK (Widget widget,UI2DView *view,XmAnyCallbackStruct *callData)

	{ widget = widget; callData = callData; delete view; }

UI2DView::UI2DView () : DBObject ("Noname 2DView",sizeof (UI2DView))

	{
	char name [DBStringLength];
	Widget menuBar, scrolledWindow, radioBox;
	Widget button;
	static Pixmap iconPixmap	 = (Pixmap) NULL, meshPixmap	 = (Pixmap) NULL;
	static Pixmap fullPixmap	 = (Pixmap) NULL, extentPixmap = (Pixmap) NULL;
	static Pixmap capturePixmap = (Pixmap) NULL, redrawPixmap = (Pixmap) NULL;
	static Pixmap zoomPixmap	 = (Pixmap) NULL, panePixmap	 = (Pixmap) NULL, userPixmap	 = (Pixmap) NULL;
	Pixel foreground, background;
	XWindowAttributes xwa;
	XSetWindowAttributes xswa;
	Atom deleteWindowAtom = XmInternAtom(XtDisplay(UITopLevel ()),(char *) "WM_DELETE_WINDOW",FALSE);

	_UI2DViewLIST.Add (this); sprintf (name,"2DView:%2d",RowID ()); Name (name);
	Image = (XImage *) NULL;
	DrawRegion = FullRegion = NULL;
	MaxVertexNumVAR = 0;
	PointARR = (XPoint *) NULL;
	RequiredEXT = (UIDataset ())->Extent ();
	DShell = XtVaCreatePopupShell ("UI2DView",xmDialogShellWidgetClass,UITopLevel (),
												XmNkeyboardFocusPolicy,		XmPOINTER,
												XmNtitle,						Name (),
												XmNtransient,					false,
												XmNminWidth,					600,
												XmNminHeight,					450,
												NULL);
	XmAddWMProtocolCallback (DShell,deleteWindowAtom,(XtCallbackProc) _UI2DViewDeleteCBK,(XtPointer) this);
	MainFormW = XtVaCreateManagedWidget ("UI2DViewForm",xmFormWidgetClass,DShell,
												XmNdialogStyle,				XmDIALOG_WORK_AREA,
												XmNshadowThickness,			0,
												XmNwidth,						600,
												XmNheight,						450,
												NULL);
	menuBar = XtVaCreateManagedWidget ("UI2DViewMenuBar",xmRowColumnWidgetClass,MainFormW,
												XmNtopAttachment,				XmATTACH_FORM,
												XmNtopOffset,					2,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNleftOffset,					2,
												XmNrightAttachment, 			XmATTACH_FORM,
												XmNrightOffset,				2,
												XmNorientation,				XmHORIZONTAL,
												XmNrowColumnType,				XmWORK_AREA,
												NULL);
	XtVaGetValues (MainFormW,XmNforeground,	&foreground,XmNbackground,	&background,NULL);

	iconPixmap = iconPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen (UITopLevel()),(char *) "UNH2Dview",foreground,background): iconPixmap;
	XtVaSetValues (DShell,XmNiconPixmap, iconPixmap,NULL);

	fullPixmap = fullPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHFull",  foreground,background) : fullPixmap;
	extentPixmap = extentPixmap  == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHExtent",foreground,background) : extentPixmap;
	capturePixmap= capturePixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHCapture",foreground,background):capturePixmap;
	redrawPixmap = redrawPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHRedraw", foreground,background) : redrawPixmap;
	zoomPixmap = zoomPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHZoom",  foreground,background) : zoomPixmap;
	panePixmap = panePixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHPane",  foreground,background) : panePixmap;
	userPixmap = userPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHUser",  foreground,background) : userPixmap;
	meshPixmap = meshPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "UNHMesh",  foreground,background) : meshPixmap;

	button = XtVaCreateManagedWidget ("UI2DViewRedrawButton",xmPushButtonGadgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				redrawPixmap,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UI2DViewRedrawCBK,this);
	button = XtVaCreateManagedWidget ("UI2DViewFullButton",xmPushButtonGadgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				fullPixmap,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UI2DViewFullActivateCBK,this);
	button = XtVaCreateManagedWidget ("UI2DViewExtentButton",xmPushButtonGadgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				extentPixmap,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UI2DViewExtentActivateCBK,this);
	button = XtVaCreateManagedWidget ("UI2DViewCaptureButton",xmPushButtonGadgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				capturePixmap,
												NULL);
	radioBox = XtVaCreateManagedWidget ("UI2DViewRadioBox",xmRowColumnWidgetClass,menuBar,
												XmNorientation,				XmHORIZONTAL,
												XmNpacking,						XmPACK_COLUMN,
												XmNisHomogeneous,				true,
												XmNentryClass,					xmToggleButtonGadgetClass,
												XmNradioBehavior,				true,
												NULL);
	ZoomToggle = XtVaCreateManagedWidget ("UI2DViewZoomToggle",xmToggleButtonGadgetClass,radioBox,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				zoomPixmap,
												XmNshadowThickness,			0,
												XmNset,							true,
												NULL);
	XtAddCallback (ZoomToggle,XmNvalueChangedCallback,(XtCallbackProc) _UI2DViewZoomToggleValueCBK,this);
	PaneToggle = XtVaCreateManagedWidget ("UI2DViewPaneToggle",xmToggleButtonGadgetClass,radioBox,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				panePixmap,
												XmNshadowThickness,			0,
												NULL);
	XtAddCallback (PaneToggle,XmNvalueChangedCallback,(XtCallbackProc) _UI2DViewPaneToggleValueCBK,this);
	UserToggle = XtVaCreateWidget ("UI2DViewUserToggle",xmToggleButtonGadgetClass,radioBox,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				userPixmap,
												XmNmappedWhenManaged,		false,
												XmNshadowThickness,			0,
												NULL);
	XtAddCallback (UserToggle,XmNvalueChangedCallback,(XtCallbackProc) _UI2DViewUserToggleValueCBK,this);
	if (_UI2DViewUserFunction != (UI2DViewUserFunction) NULL) XtManageChild (UserToggle);
	MeshOptionW = XtVaCreateManagedWidget ("UI2DViewMeshOption",xmToggleButtonGadgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				meshPixmap,
												XmNshadowThickness,			0,
												NULL);
	XtAddCallback (MeshOptionW,XmNvalueChangedCallback,(XtCallbackProc) _UI2DViewMeshOptionCBK,this);
	XtVaSetValues (menuBar,	XmNmenuHelpWidget, MeshOptionW, NULL);

	ScaleW = XtVaCreateManagedWidget ("UI2DViewScale",xmScaleWidgetClass,MainFormW,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					menuBar,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNleftOffset,					2,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				18,
												XmNorientation,				XmVERTICAL,
												XmNwidth,						16,
												NULL);
	XtAddCallback (ScaleW,XmNvalueChangedCallback,(XtCallbackProc) _UI2DViewScaleValueChangedCBK,this);
	scrolledWindow = XtVaCreateManagedWidget ("UI2DViewScrolledWindow",xmScrolledWindowWidgetClass,MainFormW,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					menuBar,
												XmNtopOffset,					2,
												XmNleftAttachment,			XmATTACH_WIDGET,
												XmNleftWidget,					ScaleW,
												XmNleftOffset,					3,
												XmNrightAttachment,			XmATTACH_FORM,
												XmNrightOffset,				3,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				3,
												XmNspacing,						2,
												NULL);
	HorScrollBarW = XtVaCreateManagedWidget ("UI2DViewHorScrollBar", xmScrollBarWidgetClass, scrolledWindow,
												XmNsliderSize,					100,
												XmNorientation,				XmHORIZONTAL,
												XmNheight,						16,
												NULL);
	XtAddCallback (HorScrollBarW,XmNvalueChangedCallback,(XtCallbackProc) _UI2DViewHorScrollBarValueChangedCBK,this);
	VerScrollBarW = XtVaCreateManagedWidget ("UI2DViewVerScrollBar", xmScrollBarWidgetClass, scrolledWindow,
												XmNsliderSize,					100,
												XmNorientation,				XmVERTICAL,
												XmNwidth,						16,
												NULL);
	XtAddCallback (VerScrollBarW,XmNvalueChangedCallback,(XtCallbackProc) _UI2DViewVerScrollBarValueChangedCBK,this);
	DrawingAreaW = XtVaCreateManagedWidget ("UI2DViewDrawingArea", xmDrawingAreaWidgetClass, scrolledWindow,
												XmNuserData,					this,
												XmNbackground,					UIColor (UIColorStandard,0),
												NULL);
	XtAddCallback (DrawingAreaW,XmNresizeCallback,(XtCallbackProc) _UI2DViewResizeCBK,this);
	XtAddEventHandler (DrawingAreaW,EnterWindowMask|LeaveWindowMask|PointerMotionMask|ButtonPressMask|ButtonReleaseMask, false,(XtEventHandler) _UI2DViewPointerEHR,this);
	XmScrolledWindowSetAreas(scrolledWindow,HorScrollBarW,VerScrollBarW,DrawingAreaW);

	_UI2DViewViewCursor  =  _UI2DViewViewCursor != (Cursor) NULL ? _UI2DViewViewCursor : XCreateFontCursor (XtDisplay (DrawingAreaW),XC_top_left_arrow),
	_UI2DViewUserCursor  =  _UI2DViewUserCursor != (Cursor) NULL ? _UI2DViewUserCursor : XCreateFontCursor (XtDisplay (DrawingAreaW),XC_crosshair);
	RegenCursor =  XCreateFontCursor (XtDisplay (DrawingAreaW),XC_watch);
	ActiveCursor = xswa.cursor = _UI2DViewViewCursor;
	XChangeWindowAttributes (XtDisplay (DrawingAreaW), XtWindow (DrawingAreaW), CWCursor, &xswa);

	XGetWindowAttributes (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),&xwa);
	Background = xwa.backing_pixel;
	xswa.backing_store = Always;
	XChangeWindowAttributes (XtDisplay (DrawingAreaW),  XtWindow (DrawingAreaW),CWBackingStore,&xswa);
	InputMode (ZOOM_MODE);
	Size ();
	Set ();
	}

UI2DView::~UI2DView ()

	{
	_UI2DViewLIST.Remove (this);
	XtDestroyWidget (DShell);
	if (PointARR != (XPoint *) NULL) free (PointARR);
	if (Image != NULL) XDestroyImage (Image);
	}

UI2DView *UI2DViewFirst() { return (_UI2DViewLIST.First ()); }
UI2DView *UI2DViewNext () { return (_UI2DViewLIST.Next ()); }

void UI2DViewRedrawAll ()

	{
	UI2DView *view;

	for (view = UI2DViewFirst();view != (UI2DView *) NULL;view = UI2DViewNext()) view->Draw ();
	}

void UI2DViewRedrawAll (DBRegion extent)

	{
	UI2DView *view;

	for (view = UI2DViewFirst();view != (UI2DView *) NULL;view = UI2DViewNext())
		view->Draw (extent);
	}

