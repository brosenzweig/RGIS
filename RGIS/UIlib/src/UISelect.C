/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

UISelect.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/SelectioB.h>
#include <UI.H>

static char *_UISelection = NULL;

static void _UINoMatchSelectionCallback (Widget widget,void *data,XmSelectionBoxCallbackStruct *callData)
  { widget = widget; data = data; callData = callData; UIMessage ("Must Select!"); }

static void _UIOkSelectionCallback (Widget widget,void *data,XmSelectionBoxCallbackStruct *callData)

	{
	widget = widget; data = data;
	if (!XmStringGetLtoR (callData->value,XmSTRING_DEFAULT_CHARSET,&_UISelection))	_UISelection = NULL;
	}

Widget UISelectionCreate (char *selectTitle)

	{
	Widget dShell, selection;
	
	dShell = XtVaCreatePopupShell (selectTitle,xmDialogShellWidgetClass,UITopLevel (),
										XmNminWidth,				200,
										XmNallowShellResize,		true,
										XmNtransient,				true,
										XmNkeyboardFocusPolicy,	XmEXPLICIT,
										NULL);
	selection = XtVaCreateWidget ("UISelectionDialog",xmSelectionBoxWidgetClass,dShell,
										XmNnoResize,				true,
										XmNdialogStyle,			XmDIALOG_FULL_APPLICATION_MODAL,
										XmNautoUnmanage,			false,
										XmNmustMatch,				true,
										NULL);
	XtVaSetValues (XmSelectionBoxGetChild (selection,XmDIALOG_OK_BUTTON),XmNdefaultButtonShadowThickness, 0,NULL);
	XtVaSetValues (XmSelectionBoxGetChild (selection,XmDIALOG_CANCEL_BUTTON),XmNdefaultButtonShadowThickness, 0,NULL);
	XtUnmanageChild (XmSelectionBoxGetChild (selection,XmDIALOG_HELP_BUTTON));
	XtUnmanageChild (XmSelectionBoxGetChild (selection,XmDIALOG_APPLY_BUTTON));
	XtUnmanageChild (XmSelectionBoxGetChild (selection,XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild (XmSelectionBoxGetChild (selection,XmDIALOG_TEXT));

	XtAddCallback (selection,XmNokCallback,(XtCallbackProc) _UIOkSelectionCallback,NULL);
	XtAddCallback (selection,XmNokCallback,(XtCallbackProc) UILoopStopCBK,NULL);
	XtAddCallback (selection,XmNcancelCallback,(XtCallbackProc)	UILoopStopCBK,NULL);
	XtAddCallback (selection,XmNdestroyCallback,(XtCallbackProc) UILoopStopCBK,NULL);
	XtAddCallback (selection,XmNnoMatchCallback,(XtCallbackProc) _UINoMatchSelectionCallback,NULL);
	return (selection);
	}


char *UISelection (Widget widget,char *items,int itemSize,int itemNum)

	{
	int i;
	XmString *strings;

	if ((strings = (XmString *) calloc	(itemNum,sizeof (XmString))) == NULL)
		{
		perror ("Memory Allocation Error in: UISelector ()\n");
		return (NULL);
		}
	for (i = 0;i < itemNum;++i) strings [i] = XmStringCreate (items + i * itemSize,UICharSetNormal);
	_UISelection = NULL;
	XtVaSetValues (widget,XmNlistItems, strings,XmNlistItemCount, itemNum,NULL);
	XtManageChild (widget);

	while (UILoop ());
	XtUnmanageChild (widget);

	for (i = 0;i < itemNum;++i) XmStringFree (strings [i]);
	free (strings);
	return (_UISelection);
	}

static DBInt _UISelectObjectAll (const DBObject *obj) { obj = obj; return (true); }
	
char *UISelectObject (Widget widget,DBObjectLIST<DBObject> *objList,DBInt (*condFunc) (const DBObject *))

	{
	int i = 0;
	XmString *strings;
	DBObject *obj;
	
	if (objList->ItemNum () < 1) { UIMessage ("Empty List"); return (NULL); }
	if ((strings = (XmString *) calloc	(objList->ItemNum (),sizeof (XmString))) == NULL)
		{ perror ("Memory Allocation Error in: UISelectObject ()\n"); return (NULL); }

	if (condFunc == (UISelectCondFunc) NULL) condFunc = _UISelectObjectAll;
	for (obj = objList->First ();obj != NULL;obj = objList->Next ())
		if (((obj->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) && (*condFunc) (obj))
			strings [i++] = XmStringCreateSimple (obj->Name ());

	_UISelection = NULL;
	XtVaSetValues (widget,XmNlistItems, strings,XmNlistItemCount, i,NULL);
	XtManageChild (widget);

	while (UILoop ());
	XtUnmanageChild (widget);

	for (;i > 0;--i) XmStringFree (strings [i - 1]);
	free (strings);
	return (_UISelection);
	}

char *UISelectObject (Widget widget,DBObjectLIST<DBObject> *objList)

	{ return (UISelectObject (widget,objList,_UISelectObjectAll)); }
