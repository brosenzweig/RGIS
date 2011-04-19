/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

UIAux.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/TextF.h>
#include <UI.H>

void UIAuxSetLabelString (Widget widget,char *text,char *charset)

	{
	XmString string;

	string = XmStringCreate (text,charset);
	XtVaSetValues (widget,XmNlabelString,string,NULL);
	XmStringFree (string);
	}

void UIAuxSetLabelString (Widget widget,char *text)

	{ UIAuxSetLabelString (widget,text,UICharSetNormal); }

char *UIAuxGetLabelString (Widget widget,char *charset)

	{
	char *text;
	XmString string;

	XtVaGetValues (widget,XmNlabelString,&string,NULL);
	XmStringGetLtoR (string,charset,&text);
	return (text);
	}

char *UIAuxGetLabelString (Widget widget)

	{ return (UIAuxGetLabelString (widget, UICharSetNormal)); }

void UIAuxSetBooleanTrueCBK (Widget widget,int *boolean,XmAnyCallbackStruct *callData)

	{
	widget = widget; callData = callData;
	*boolean = True;
	}

void UIAuxSetBooleanFalseCBK (Widget widget,int *boolean,XmAnyCallbackStruct *callData)

	{
	widget = widget; callData = callData;
	*boolean = False;
	}

void UIAuxSetIntegerCBK (Widget widget,DBInt setVal,XmAnyCallbackStruct *callData)

	{
	DBInt *intVal;
	callData = callData;
	XtVaGetValues (widget,XmNuserData, &intVal, NULL);
	*intVal = setVal;
	}

void UIAuxSetToggleCBK (Widget widget,DBInt *toggle,XmToggleButtonCallbackStruct *callData)

	{	widget = widget;	*toggle = callData->set; }


void UIAuxSetDefaultButtonEH (Widget widget,void *data,XEvent *event,Boolean boolVal)

	{
	Arg wargs [1];

	data = data; boolVal = boolVal;

	switch (event->type)
		{
		case EnterNotify: XtSetArg (wargs [0],XmNshowAsDefault,	True);	break;
		case LeaveNotify: XtSetArg (wargs [0],XmNshowAsDefault,	False);	break;
		}
	XtSetValues (widget,wargs,1);
	}


void UIAuxObjectSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = NULL;
	char *objName;
	int (*condFunc) (const DBObject *);
	DBObjectLIST<DBObject> *objects;

	callData = callData;
	if (select == NULL) select = UISelectionCreate ((char *) "Object Selection");
	XtVaGetValues (text,XmNuserData, &objects, NULL);
	XtVaGetValues (widget,XmNuserData, &condFunc, NULL);
	if ((objName = UISelectObject (select,objects,condFunc)) != NULL)
		XmTextFieldSetString (text,objName);
	}

void UIAuxFileSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = (Widget) NULL;
	char *pattern, *fileName;
	DBInt selectMode;
	XmString string;

	callData = callData;
	if (select == (Widget) NULL) select = UIFileSelectionCreate ((char *) "File Selection",(char *) NULL,(char *) NULL,XmFILE_REGULAR);
	XtVaGetValues (text,		XmNuserData, &pattern, 		NULL);
	XtVaGetValues (widget,	XmNuserData, &selectMode,	NULL);
	if (strlen (pattern) > 0)
		{
		string = XmStringCreate (pattern,UICharSetNormal);
		XtVaSetValues (select, XmNpattern, string,NULL);
		XmStringFree (string);
		}
	if ((fileName = UIFileSelection (select,selectMode)) != NULL)
		XmTextFieldSetString (text,fileName);
	}

void UIAuxDirSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = (Widget) NULL;
	char *pattern, *dirName;
	DBInt selectMode;
	XmString string;

	callData = callData;
	if (select == NULL) select = UIFileSelectionCreate ((char *) "Directory Selection",(char *) NULL,(char *) NULL,XmFILE_DIRECTORY);
	XtVaGetValues (text,XmNuserData, 	&pattern,		NULL);
	XtVaGetValues (widget,XmNuserData,	&selectMode,	NULL);
	if (strlen (pattern) > 0)
		{
		string = XmStringCreate (pattern,UICharSetNormal);
		XtVaSetValues (select,XmNpattern,string,NULL);
		XmStringFree (string);
		}
	if ((dirName = UIFileSelection (select,selectMode)) != NULL)
		XmTextFieldSetString (text,dirName);
	}

int UIStrCmp (char **name1,char **name2) { return (strcmp (*name1,*name2));}
