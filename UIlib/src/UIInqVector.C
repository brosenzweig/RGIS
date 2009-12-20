/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

UIInqVector.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <X11/cursorfont.h>
#include <UI.H>

int UIInquireVector (Widget widget,XEvent *event,int x0,int y0,int *x1,int *y1)

	{
	XSetWindowAttributes xswa;
	XGCValues xgcv;
	static GC gc = (GC) NULL;
	static Cursor cursor [2] = { (Cursor) NULL,(Cursor) NULL};
	static int inLoop,cont, ret;

	if (cursor [0] == (Cursor) NULL) cursor [0] = XCreateFontCursor (XtDisplay (widget),XC_hand2);
	if (cursor [1] == (Cursor) NULL) cursor [1] = XCreateFontCursor (XtDisplay (widget),XC_top_left_arrow);
	if (gc == (GC) NULL) { xgcv.function = GXinvert; gc = XtGetGC (widget,GCFunction,&xgcv); }
	if (inLoop == false)
		{
		if (event->type != ButtonPress) return (false);
		inLoop = cont = true;
		ret = false;
		xswa.cursor = cursor [0];
		XChangeWindowAttributes (XtDisplay (widget),XtWindow (widget),CWCursor,&xswa);
		*x1 = event->xbutton.x; *y1 = event->xbutton.y;
		XDrawLine (XtDisplay (widget),XtWindow (widget), gc, x0, y0, *x1,*y1); 
		do	{
			XtAppNextEvent (UIApplicationContext (),event);
			XtDispatchEvent (event);
			} while (cont);
		XDrawLine (XtDisplay (widget),XtWindow (widget), gc, x0, y0, *x1,*y1); 
		xswa.cursor = cursor [1];
		XChangeWindowAttributes (XtDisplay (widget),XtWindow (widget),CWCursor,&xswa);
		inLoop = false;
		return (ret);
		}
	else switch (event->type)
			{
			case ButtonRelease:
				xswa.cursor = cursor [1];
				XChangeWindowAttributes (XtDisplay (widget),XtWindow (widget),CWCursor,&xswa);
				ret  = true;
				cont = false;
				break;
			case MotionNotify:
				XDrawLine (XtDisplay (widget),XtWindow (widget), gc, x0, y0, *x1,*y1); 
				*x1  = event->xmotion.x;
				*y1 = event->xmotion.y;
				XDrawLine (XtDisplay (widget),XtWindow (widget), gc, x0, y0, *x1,*y1); 
				break;
			case LeaveNotify:
				ret  = false;
				cont = false;
				break;	
			}
	return (false);
	} 
