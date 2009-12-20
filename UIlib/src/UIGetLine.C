/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

UIGetLine.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <UI.H>

static int _UIGetLineRet;
static Widget _UIGetLineWidget;
static int _UIGetLineX0;
static int _UIGetLineY0;
static int *_UIGetLineX1;
static int *_UIGetLineY1;

static void _UIGetLineUserFunc (DBObjData *data,UI2DView *view,XEvent *event)

	{
	int UIInquireVector (Widget,XEvent *,int,int,int *,int *);

	data = data; view = view;
	_UIGetLineRet = UIInquireVector (_UIGetLineWidget,event,_UIGetLineX0,_UIGetLineY0,_UIGetLineX1,_UIGetLineY1);
	}

int UIGetLine (Widget widget,XEvent *event,int x0,int y0,int *x,int *y)

	{
	UI2DView *view;
	UI2DViewUserFunction userFunction =  UI2DViewGetUserFunction ();

	UI2DViewChangeUserFunction ((UI2DViewUserFunction) _UIGetLineUserFunc);
	XtVaGetValues (widget,XmNuserData, &view, NULL);
	_UIGetLineWidget = widget;
	_UIGetLineX0 = x0;
	_UIGetLineY0 = y0;
	_UIGetLineX1 = x;
	_UIGetLineY1 = y;
	
	_UIGetLineUserFunc (UI2DViewGetUserData (),view,event);
	UI2DViewChangeUserFunction (userFunction);
	return (_UIGetLineRet);
	}
