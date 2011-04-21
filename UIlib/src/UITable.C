/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

UITable.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/Protocols.h>
#include <Xm/TextF.h>
#include <cm.h>
#include <UI.H>

#define UITableTitle "GHAAS Table"
#define UITableSpacing 1
#define UITableMargin  4
#define UITableRecordSpace 200

char *UITableName (const DBObjData *dbData,const DBObjTable *table)

	{
	static char tableName [DBDataFileNameLen + sizeof (UITableTitle) + 3];
	sprintf (tableName,"%s:  %s of %s",UITableTitle,table->Name (),dbData->Name ());
	return (tableName);
	}

static int _UITableSortFunc (const DBObjRecord **obj0,const DBObjRecord **obj1)

	{
	DBInt ret = ((*obj1)->Flags () & DBObjectFlagSelected) - ((*obj0)->Flags () & DBObjectFlagSelected);

	if (ret != 0) return (ret);
	ret = ((DBObject *) (*obj0))->ListPos () - ((DBObject *) (*obj1))->ListPos ();
	return (ret);
	}

static void _UITableFieldSortCBK (Widget widget,DBInt dir,XmAnyCallbackStruct *callData)

	{
	DBInt flags;
	UITable *uiTable;
	UITableField *uiField;
	DBObjTable *table;
	DBObjTableField *field;

	callData = callData;
	XtVaGetValues (XtParent (XtParent (XtParent (widget))),XmNuserData, &uiTable,NULL);
	XtVaGetValues (widget,XmNuserData, &uiField,NULL);
	table = uiTable->Table ();
	if (dir != DBFault)
		{
		if (uiField == (UITableField *) NULL)	table->ListSort (dir);
		else
			{
			field = uiField->Field ();
			flags = field->Flags ();
			field->Flags (DBObjectFlagSortReversed,dir);
			table->ListSort (field);
			field->Flags (flags);
			}
		}
	else	table->ListSort (_UITableSortFunc);
	uiTable->Draw ();
	}

static void _UITableFieldConditionCBK (Widget widget,DBInt value,XmAnyCallbackStruct *callData)

	{
	DBInt *cond;

	XtVaGetValues (widget,XmNuserData, &cond,NULL);
	*cond = value;
	}

static void _UITableFieldSelectCBK (Widget widget,DBInt select,XmAnyCallbackStruct *callData)

	{
	char *text;
	static int cond = 0, proc, toggle = 0;
	static Widget condMenu, button;
	static Widget fieldText;
	static Widget dShell = (Widget) NULL;
	XmString string;
	DBObjTable *dbTable;
	DBObjTableField *dbField;
	UITable *uiTable;
	UITableField *uiField;

	callData = callData;
	XtVaGetValues (XtParent (XtParent (XtParent (widget))),XmNuserData, &uiTable,NULL);
	XtVaGetValues (widget,XmNuserData, &uiField,NULL);
	dbTable = uiTable->Table ();
	dbField = uiField->Field ();

	if (dShell == (Widget) NULL)
		{
		Widget mainForm, toggleButton;

		dShell = UIDialogForm ((char *) "Select Records");
		mainForm = UIDialogFormGetMainForm (dShell);
		XtVaSetValues (mainForm,XmNuserData, uiTable, NULL);

		condMenu = XmCreatePulldownMenu (mainForm,(char *) "UITableSelectCondMenu",NULL,0);
		string = XmStringCreate ((char *) ">",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,condMenu,
								XmNlabelString,	string,
								XmNuserData,		&cond,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldConditionCBK,(XtPointer) cond++);
		XmStringFree (string);
		string = XmStringCreate ((char *) ">=",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,condMenu,
								XmNlabelString,	string,
								XmNuserData,		&cond,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldConditionCBK,(XtPointer) cond++);
		XmStringFree (string);
		string = XmStringCreate ((char *) "==",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,condMenu,
								XmNlabelString,	string,
								XmNuserData,		&cond,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldConditionCBK,(XtPointer) cond++);
		XmStringFree (string);
		string = XmStringCreate ((char *) "!=",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,condMenu,
								XmNlabelString,	string,
								XmNuserData,		&cond,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldConditionCBK,(XtPointer) cond);
		XmStringFree (string);
		string = XmStringCreate ((char *) "<>",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,condMenu,
								XmNlabelString,	string,
								XmNuserData,		&cond,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldConditionCBK,(XtPointer) cond++);
		XmStringFree (string);
		string = XmStringCreate ((char *) "<=",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,condMenu,
								XmNlabelString,	string,
								XmNuserData,		&cond,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldConditionCBK,(XtPointer) cond++);
		XmStringFree (string);
		string = XmStringCreate ((char *) "<",UICharSetNormal);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,condMenu,
								XmNlabelString,	string,
								XmNuserData,		&cond,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldConditionCBK,(XtPointer) cond++);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Condition:",UICharSetBold);
		condMenu = XtVaCreateManagedWidget ("UITableSelectCondMenu",xmRowColumnWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				10,
											XmNsubMenuId,				condMenu,
											XmNlabelString,			string,
											XmNrowColumnType,			XmMENU_OPTION,
											XmNtraversalOn,			false,
											NULL);
		XmStringFree (string);
		fieldText = XtVaCreateManagedWidget ("UITableSelectCondTextF",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				condMenu,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				condMenu,
											XmNleftOffset,				5,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			condMenu,
											XmNmaxLength,				DBStringLength - 1,
											XmNcolumns,					DBStringLength / 2,
											NULL);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&proc);
		XtAddCallback (UIDialogFormGetCancelButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanFalseCBK,&proc);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("UITableSelectCondButton",xmPushButtonGadgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				fieldText,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				fieldText,
											XmNleftOffset,				10,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			fieldText,
											XmNlabelString,			string,
											XmNmarginWidth,			5,
											NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,fieldText);
		string = XmStringCreate ((char *) "From Selection",UICharSetBold);
		toggleButton = XtVaCreateManagedWidget ("UITableSelectToggleButton",xmToggleButtonWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				condMenu,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNleftWidget,				fieldText,
											XmNleftOffset,				10,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNlabelString,			string,
											XmNtraversalOn,			false,
											XmNshadowThickness,		0,
											NULL);
		XmStringFree (string);
		XtAddCallback (toggleButton,XmNvalueChangedCallback,(XtCallbackProc) UIAuxSetToggleCBK,&toggle);
		cond = 0;
		}
	proc = false;
	string = XmStringCreate (uiField->Name (),UICharSetBold);
	XtVaSetValues (condMenu,XmNlabelString,	string,	NULL);
	XmStringFree (string);
	XtVaSetValues (fieldText,XmNuserData,	dbTable->Fields (),NULL);
	if (dbField->Type () == DBTableFieldString)
			XtVaSetValues (button,XmNuserData, DBTableFieldIsString,  NULL);
	else	XtVaSetValues (button,XmNuserData, DBTableFieldIsNumeric, NULL);

	UIDialogFormTitle (dShell,select ? (char *) "Select Records" : (char *) "Unselect Records");
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		text = XmTextFieldGetString (fieldText);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),strlen (text) > 0);
		XtFree (text);
		}
	UIDialogFormPopdown (dShell);
	if (proc)
		{
		DBInt rowID, ret, intVal, nullVal;
		DBFloat floatVal;
		DBDate dateVal;
		DBObjData *dbData = (DBObjData *) NULL;
		DBObjRecord *record;
		DBObjTableField *field;

		if (strcmp (dbTable->Name (),DBrNItems) == 0) dbData = uiTable->Data ();
		text = XmTextFieldGetString (fieldText);
		if ((field = dbTable->Field (text)) == (DBObjTableField *) NULL)
			switch (dbField->Type ())
				{
				case DBTableFieldInt:
					if (sscanf (text,"%d",&intVal) != 1) { UIMessage ((char *) "Invalid Integer Value"); goto Stop; }
					break;
				case DBTableFieldFloat:
					if (sscanf (text,"%lf",&floatVal) != 1) { UIMessage ((char *) "Invalid Float Value"); goto Stop; }
					break;
				case DBTableFieldDate: dateVal.Set (text); break;
				default:	break;
				}
		for (rowID = 0;rowID < dbTable->ItemNum ();++rowID)
			{
			record = dbTable->Item (rowID);
			nullVal = false;
			if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
				{
				if (toggle) { if (select == DBClear) continue; }
				else			{ if (select == DBSet) continue; }
				}
			else
				{
				if (toggle) { if (select == DBSet) continue; }
				else			{ if (select == DBClear) continue; }
				}

			switch (dbField->Type ())
				{
				default:
				case DBTableFieldString:
					if (field == (DBObjTableField *) NULL)
						ret = strncmp (dbField->String (record),text,strlen(text));
					else
						ret = strncmp (dbField->String (record),field->String (record),strlen (field->String (record)));
					break;
				case DBTableFieldInt:
					if (field != (DBObjTableField *) NULL)
						{
						intVal = field->Int (record);
						if (intVal == field->IntNoData ()) nullVal = true;
						}
					if (dbField->Int (record) == dbField->IntNoData ()) nullVal = true;
					else	ret = dbField->Int (record) - intVal;
					break;
				case DBTableFieldFloat:
					if (field != (DBObjTableField *) NULL )
						{
						floatVal = field->Float (record);
						if (CMmathEqualValues (floatVal,field->FloatNoData ()))	nullVal = true;
						}
					if (CMmathEqualValues (dbField->Float (record),dbField->FloatNoData ())) nullVal = true;
					else
						{
						if (dbField->Float (record) > floatVal)		ret = 1;
						else if (dbField->Float (record) < floatVal) ret = -1;
						else ret = 0;
						}
					break;
				case DBTableFieldDate:
					if (field != (DBObjTableField *) NULL ) dateVal = field->Date (record);
					if (dbField->Date (record) > dateVal)			ret = 1;
					else if (dbField->Date (record) < dateVal)	ret = -1;
					else ret = 0;
					break;
				}
			if (nullVal)	ret = toggle;
			else
				switch (cond)
					{
					case 0:	ret = ret > 0;		break;
					case 1:	ret = ret >= 0;	break;
					case 2:	ret = ret == 0;	break;
					case 3:	ret = ret != 0;	break;
					case 4:	ret = ret <= 0;	break;
					case 5:	ret = ret < 0;		break;
					default:	ret = false;		break;
					}
			if (toggle)
				{
				if (!ret)
					{
					if (dbData != (DBObjData *) NULL) dbData->SelectObject (record,select == DBSet ? DBClear : DBSet);
					else	record->Flags (DBObjectFlagSelected,select == DBSet ? DBClear : DBSet);
					}
				}
			else
				{
				if (ret)
					{
					if (dbData != (DBObjData *) NULL) dbData->SelectObject (record,select);
					else	record->Flags (DBObjectFlagSelected,select);
					}
				}
			}
		uiTable->Draw ();
		if (dbData != (DBObjData *) NULL) UI2DViewRedrawAll ();
Stop:	XtFree (text);
		}
	}

static void _UITableFieldUniqueCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *strVal;
	DBInt select, intVal;
	DBFloat floatVal;
	DBDate dateVal;
	DBObjTable *dbTable;
	DBObjTableField *dbField;
	DBObjData *dbData;
	DBObjRecord *record;
	UITable *uiTable;
	UITableField *uiField;

	XtVaGetValues (XtParent (XtParent (XtParent (widget))),XmNuserData, &uiTable,NULL);
	XtVaGetValues (widget,XmNuserData, &uiField,NULL);
	dbTable = uiTable->Table ();
	dbField = uiField->Field ();
	if (strcmp (dbTable->Name (),DBrNItems) == 0) dbData = uiTable->Data ();

	switch (dbField->Type ())
		{
		default:
		case DBTableFieldString:	strVal = (char *) NULL;					break;
		case DBTableFieldInt:		intVal = dbField->IntNoData ();		break;
		case DBTableFieldFloat:		floatVal = dbField->FloatNoData ();	break;
		case DBTableFieldDate:														break;
		}
	dbField->Flags (DBObjectFlagSortReversed,false);
	dbTable->ListSort (dbField);
	for (record = dbTable->First ();record != (DBObjRecord *) NULL; record = dbTable->Next ())
		{
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		switch (dbField->Type ())
			{
			case DBTableFieldString:
				if ((strVal == (char *) NULL) || (strcmp (strVal,dbField->String (record)) != 0))
					{
					strVal = dbField->String (record);
					select = true;
					}
				else	select = false;
				break;
			case DBTableFieldInt:
				if (intVal != dbField->Int (record))
					{
					intVal = dbField->Int (record);
					select = true;
					}
				else	select = false;
				break;
			case DBTableFieldFloat:
				if (CMmathEqualValues (dbField->Float (record),floatVal))
					{
					floatVal = dbField->Float (record);
					select = true;
					}
				else	select = false;
				break;
			case DBTableFieldDate:
				if (dateVal != dbField->Date (record))
					{
					dateVal = dbField->Date (record);
					select = true;
					}
				else	select = false;
				break;
			}
		if (dbData != (DBObjData *) NULL)
				dbData->SelectObject (record,select);
		else	record->Flags (DBObjectFlagSelected,select);
		}
	uiTable->Draw ();
	if (dbData != (DBObjData *) NULL) UI2DViewRedrawAll ();
	}


static void _UITableRecordIDSortCBK (Widget widget,DBInt dir,XmAnyCallbackStruct *callData)

	{
	UITable *uiTable;
	DBObjTable *table;

	dir = dir; callData = callData;
	XtVaGetValues (XtParent (XtParent (XtParent (widget))),XmNuserData, &uiTable,NULL);
	table = uiTable->Table ();
	table->ListReset ();
	uiTable->Draw ();
	}

static Widget _UITableCreateHeaderMenu (Widget widget)

	{
	Arg wargs [2];
	int argNum = 0;
	Widget button, subMenu;
	XmString string;

	subMenu = XmCreatePulldownMenu (widget,(char *) "UITablePullDownMenu",wargs,argNum);
	string = XmStringCreateSimple ((char *) "Ascending");
	button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableRecordIDSortCBK,(XtPointer) false);
	XmStringFree (string);
	string = XmStringCreateSimple ((char *) "Descending");
	button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableRecordIDSortCBK,(XtPointer) true);
	XmStringFree (string);
	return (subMenu);
	}

static Widget _UITableCreateHeaderMenu (Widget widget,UITableField *uiField)

	{
	Arg wargs [2];
	int argNum = 0;
	Widget button, subMenu;
	XmString string;

	subMenu = XmCreatePulldownMenu (widget,(char *) "UITablePullDownMenu",wargs,argNum);
	string = XmStringCreateSimple ((char *) "Ascending");
	button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiField,
																NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldSortCBK,(XtPointer) false);
	XmStringFree (string);
	string = XmStringCreateSimple ((char *) "Descending");
	button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiField,
																NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldSortCBK,(XtPointer) true);
	XmStringFree (string);
	if (uiField == (UITableField *) NULL)
		{
		string = XmStringCreateSimple ((char *) "Selection");
		button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiField,
																NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldSortCBK,(XtPointer) DBFault);
		XmStringFree (string);
		}
	else
		{
		string = XmStringCreateSimple ((char *) "Select");
		button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiField,
																NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldSelectCBK,(XtPointer) DBSet);
		XmStringFree (string);
		string = XmStringCreateSimple ((char *) "Unselect");
		button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiField,
																NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldSelectCBK,(XtPointer) DBClear);
		XmStringFree (string);
		string = XmStringCreateSimple ((char *) "Unique");
		button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiField,
																NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldUniqueCBK,(XtPointer) NULL);
		XmStringFree (string);
		}
	return (subMenu);
	}

UITableField::UITableField (Widget menuBar,DBObjTableField *field) : DBObject (field->Name (),sizeof (UITableField))

	{
	Pixel foreground, background;
	XmString string;
	DBObjTable *table;
	DBObjRecord *record;
	UITable *uiTable;

	string = XmStringCreate (field->Name (),UICharSetBold);

	XtVaGetValues (XtParent (menuBar),XmNforeground,				&foreground,
												XmNbackground,					&background,
												XmNuserData,					&uiTable,
												NULL);
	table = uiTable->Table ();
	FieldPTR = field;
	string = XmStringCreate (field->Name (),UICharSetBold);
	FieldButtonWGT = XtVaCreateManagedWidget ("UIClearSelectionButton",xmCascadeButtonWidgetClass,menuBar,
												XmNlabelString,				string,
												XmNhighlightThickness,		0,
												XmNshadowThickness,			1,
												XmNsubMenuId,					_UITableCreateHeaderMenu (menuBar,this),
												XmNforeground,					foreground,
												XmNbackground,					background,
												XmNuserData,					this,
												NULL);
	XmStringFree (string);
	XtVaGetValues (FieldButtonWGT, XmNwidth,	&WidthVAR, NULL);
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ()) Width (record);
	XtVaSetValues (FieldButtonWGT,XmNwidth,	WidthVAR + 2 * UITableMargin, XmNrecomputeSize,	false,NULL);
	}

void UITableField::Width (DBObjRecord *record)

	{
	DBInt width;
	char *string;
	XmString xmString;

	string = FieldPTR->String (record);
	if (strlen (string) > 0)
		{
		xmString = XmStringCreate (string,UICharSetNormal);
		width = XmStringWidth (UIFixedFontList (),xmString);
		WidthVAR = WidthVAR > width ? WidthVAR : width;
		XmStringFree (xmString);
		}
	}

static void _UITableFieldSelectRecordCBK (Widget widget,UITableRecord *uiRecord,XmAnyCallbackStruct *callData)

	{
	DBInt select;
	DBObjRecord *record = uiRecord->Record ();
	DBObjData *dbData;
	DBObjTable *table;
	UITable *uiTable;
	UIXYGraphShell *graphCLS;

	callData = callData;
	XtVaGetValues (XtParent (XtParent (widget)),XmNuserData,		&uiTable, NULL);
	dbData = uiTable->Data ();
	table  = uiTable->Table ();
	select = (record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ? DBClear : DBSet;
	if (strcmp (table->Name (),DBrNItems) == 0)
		{
		if (select && ((dbData->Flags () & DBDataFlagSelectMode) != DBDataFlagSelectMode))
			{
			DBObjRecord *itemRecord;
			for (itemRecord = table->First ();itemRecord != (DBObjRecord *) NULL;itemRecord = table->Next ())
				if ((itemRecord->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
					{
					dbData->SelectObject (itemRecord,DBClear);
					uiTable->Draw (itemRecord);
					UI2DViewRedrawAll (dbData->Extent (itemRecord));
					}
			}
		dbData->SelectObject (record,select);
		UI2DViewRedrawAll (dbData->Extent (record));
		if ((graphCLS = (UIXYGraphShell *) dbData->Display (UIXYGraphShellStr)) != (UIXYGraphShell *) NULL)
			graphCLS->Configure (record);
		}
	else	record->Flags (DBObjectFlagSelected,select);
	uiTable->Draw (record);
	}

void UITableRecord::AddField (UITableField *uiField,DBInt cellHeight)

	{
	Pixel foreground, background;
	Widget button;

	XtVaGetValues (RecordRightMenuBarWGT, XmNforeground,	&foreground, XmNbackground,	&background,NULL);
	button = XtVaCreateManagedWidget ("UITableFieldButton",xmCascadeButtonWidgetClass,RecordRightMenuBarWGT,
												XmNfontList,					UIFixedFontList (),
												XmNrecomputeSize,  			false,
												XmNhighlightThickness,		0,
												XmNshadowThickness,			1,
												XmNwidth,						uiField->Width () + 2 * UITableMargin,
												XmNheight,						cellHeight,
												XmNalignment,  				(DBTableFieldIsNumeric (uiField->Field ()) ?
																					XmALIGNMENT_END : XmALIGNMENT_BEGINNING),
												XmNforeground,					background,
												XmNbackground,					foreground,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldSelectRecordCBK,(XtPointer) this);
	}

static Widget _UITableCreateRecordNameMenu (Widget widget,UITableRecord *uiRecord)

	{
	Arg wargs [2];
	int argNum = 0;
	Widget button, subMenu;
	XmString string;

	XtSetArg (wargs [0], XmNuserData,	uiRecord); argNum++;
	subMenu = XmCreatePulldownMenu (widget,(char *) "UITablePullDownMenu",wargs,argNum);
	string = XmStringCreateSimple ((char *) "(Un)Select");
	button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiRecord,
																NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableFieldSelectCBK,(XtPointer) uiRecord);
	XmStringFree (string);
	string = XmStringCreateSimple ((char *) "Idle");
	button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiRecord,
																NULL);
	XmStringFree (string);
	string = XmStringCreateSimple ((char *) "Edit");
	button = XtVaCreateManagedWidget ("UIFunctionButton",xmCascadeButtonWidgetClass, subMenu,
																XmNlabelString,		string,
																XmNuserData,			uiRecord,
																NULL);
	XmStringFree (string);
	return (subMenu);
	}

UITableRecord::UITableRecord (Widget leftWidget,Widget  rightWidget,DBInt numWidth, DBInt nameWidth, DBObjectLIST<UITableField> *uiFields,DBInt cellHeight)

	{
	Pixel foreground, background;
	UITableField *uiField;
	UITable *uiTable;

	XtVaGetValues (leftWidget,			XmNforeground,					&foreground,
												XmNbackground,					&background,
												XmNuserData,					&uiTable,
												NULL);
	RecordLeftMenuBarWGT = XtVaCreateWidget ("UITableRecordLefMenuBar",xmRowColumnWidgetClass,leftWidget,
												XmNmarginWidth,				0,
												XmNmarginHeight,				0,
												XmNorientation,				XmHORIZONTAL,
												XmNspacing,						UITableSpacing,
												XmNrowColumnType,				XmMENU_BAR,
												XmNshadowThickness,			0,
												XmNforeground,					foreground,
												XmNbackground,					background,
												XmNuserData,					uiTable,
												NULL);
	XtVaCreateManagedWidget ("UITableRowButton",xmCascadeButtonWidgetClass,RecordLeftMenuBarWGT,
												XmNrecomputeSize,  			false,
												XmNhighlightThickness,		0,
												XmNshadowThickness,			1,
												XmNwidth,						numWidth,
												XmNheight,						cellHeight,
												XmNforeground,					background,
												XmNbackground,					foreground,
												XmNuserData,					uiTable,
												NULL);
	XtVaCreateManagedWidget ("UITableNameButton",xmCascadeButtonWidgetClass,RecordLeftMenuBarWGT,
												XmNrecomputeSize,  			false,
												XmNhighlightThickness,		0,
												XmNshadowThickness,			1,
												XmNwidth,						nameWidth,
												XmNheight,						cellHeight,
												XmNforeground,					background,
												XmNbackground,					foreground,
												XmNsubMenuId,					_UITableCreateRecordNameMenu (RecordLeftMenuBarWGT,this),
												XmNuserData,					uiTable,
												NULL);

	RecordRightMenuBarWGT = XtVaCreateWidget ("UITableRecordRightMenuBar",xmRowColumnWidgetClass,rightWidget,
												XmNmarginWidth,				0,
												XmNmarginHeight,				0,
												XmNorientation,				XmHORIZONTAL,
												XmNspacing,						UITableSpacing,
												XmNrowColumnType,				XmMENU_BAR,
												XmNshadowThickness,			0,
												XmNforeground,					foreground,
												XmNbackground,					background,
												XmNuserData,					this,
												NULL);
	FieldsPTR = uiFields;
	for (uiField = FieldsPTR->First ();uiField != (UITableField *) NULL;uiField = FieldsPTR->Next ())
		AddField (uiField,cellHeight);
	XtManageChild (RecordLeftMenuBarWGT);
	XtManageChild (RecordRightMenuBarWGT);
	}

void UITableRecord::Draw (UITableField *uiField)

	{
	DBObjTableField *field = uiField->Field ();
	Pixel background;
	WidgetList buttons;

	XtVaGetValues (RecordRightMenuBarWGT,	XmNchildren,	&buttons,
														XmNforeground,	&background,
														NULL);
	background = (RecordPTR->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ?
					 UIColor (UIColorStandard,7) : background;
	XtVaSetValues (buttons [uiField->RowID ()],	XmNbackground,	background,	NULL);

	UIAuxSetLabelString (buttons [uiField->RowID ()],field->String (RecordPTR));
	XtSetSensitive (buttons [uiField->RowID ()],(RecordPTR->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle);
	}

void UITableRecord::Draw ()

	{
	char numberString [DBStringLength];
	UITableField *uiField;
	Pixel background;
	WidgetList buttons;

	XtVaGetValues (RecordLeftMenuBarWGT,XmNchildren,	&buttons,
													XmNforeground,	&background,
													NULL);
	sprintf (numberString,"%9d",RecordPTR->RowID () + 1);
	background = (RecordPTR->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ?
					 UIColor (UIColorStandard,7) : background;
	UIAuxSetLabelString (buttons [0],numberString);
	XtVaSetValues (buttons [0],	XmNbackground,	background,	NULL);
	UIAuxSetLabelString (buttons [1],RecordPTR->Name ());
	XtVaSetValues (buttons [1],	XmNbackground,	background,	NULL);
	XtSetSensitive(buttons [1],(RecordPTR->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle);
	for (uiField = FieldsPTR->First ();uiField != (UITableField *) NULL;uiField = FieldsPTR->Next ())
		Draw (uiField);
	}

static void _UITableResizeACTION (Widget widget, XEvent *event, String *args,unsigned int * argNum)

	{
	UITable *tableCLS;

	args = args; argNum = argNum;
	XtVaGetValues (widget, XmNuserData, &tableCLS, NULL);
	tableCLS->Resize (event->xconfigure.height);
	}

static void _UITableVerScrollBarCBK (Widget widget,UITable *tableCLS,XmScrollBarCallbackStruct *callData)

	{	widget = widget;	tableCLS->Draw (callData->value); }

static Widget _UITableSelectionFileSelect = NULL;
static Widget _UITableSelectionFieldSelect = (Widget) NULL;

static void _UITableLoadSelectionCBK (Widget widget,UITable *uiTable,XmAnyCallbackStruct *callData)

	{
	DBInt selectInt;
	FILE *inFile;
	char *selection, selectString [256];
	DBObjTable *table = uiTable->Table ();
	DBObjTableField *field;
	DBObjRecord *record;

	widget = widget; callData = callData;
	if (_UITableSelectionFieldSelect == (Widget) NULL) _UITableSelectionFieldSelect = UISelectionCreate ((char *) "Table Field Selection");
	if (_UITableSelectionFileSelect == NULL)	_UITableSelectionFileSelect = UIFileSelectionCreate ((char *) "Selection File",NULL,(char *) "*.*",XmFILE_REGULAR);
	if ((selection = UISelectObject (_UITableSelectionFieldSelect,(DBObjectLIST<DBObject> *) (table->Fields ()),DBTableFieldIsCategory)) == (char *) NULL) return;
	if ((field = table->Field (selection)) == (DBObjTableField *) NULL) return;
	if ((selection = UIFileSelection (_UITableSelectionFileSelect,false)) == NULL) return;
	if ((inFile = fopen (selection,"r")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in:%s %d",__FILE__,__LINE__); return; }
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
			record->Flags (DBObjectFlagSelected,false);
	switch (field->Type ())
		{
		case DBTableFieldString:
			while (fgets (selectString,sizeof (selectString),inFile) != (char *) NULL)
				{
				selectString [strlen (selectString) - 1] = '\0';
				for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
					if (strcmp (field->String (record),selectString) == 0)
						record->Flags (DBObjectFlagSelected,true);
				}
			break;
		case DBTableFieldInt:
			while (fscanf (inFile,"%d",&selectInt) == 1)
				for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
					if (field->Int (record) == selectInt)
						record->Flags (DBObjectFlagSelected,true);
			break;
		default:		break;
		}
	fclose (inFile);
	uiTable->Draw ();
	if (strcmp (table->Name (),DBrNItems) == 0) UI2DViewRedrawAll ();
	}

static void _UITableSaveSelectionCBK (Widget widget,UITable *uiTable,XmAnyCallbackStruct *callData)

	{
	FILE *outFile;
	char *selection;
	DBObjTable *table = uiTable->Table ();
	DBObjTableField *field;
	DBObjRecord *record;

	widget = widget; callData = callData;
	if (_UITableSelectionFieldSelect == (Widget) NULL) _UITableSelectionFieldSelect = UISelectionCreate ((char *) "Table Field Selection");
	if (_UITableSelectionFileSelect == NULL)	_UITableSelectionFileSelect = UIFileSelectionCreate ((char *) "Selection File",NULL,(char *) "*.*",XmFILE_REGULAR);
	if ((selection = UISelectObject (_UITableSelectionFieldSelect,(DBObjectLIST<DBObject> *) (table->Fields ()),DBTableFieldIsCategory)) == (char *) NULL) return;
	if ((field = table->Field (selection)) == (DBObjTableField *) NULL) return;
	if ((selection = UIFileSelection (_UITableSelectionFileSelect,false)) == NULL) return;
	if ((outFile = fopen (selection,"w")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in:%s %d",__FILE__,__LINE__); return; }
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		{
		if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
			fprintf (outFile,"%s\n",field->String (record));
		}
	fclose (outFile);
	}

static void _UITableClearSelectionCBK (Widget widget,UITable *uiTable,XmAnyCallbackStruct *callData)

	{
	DBObjData *data = uiTable->Data ();
	DBObjTable *table = uiTable->Table ();
	DBObjRecord *record;

	widget = widget; callData = callData;
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
			record->Flags (DBObjectFlagSelected,false);
	uiTable->Draw ();
	if (strcmp (table->Name (),DBrNItems) == 0) UI2DViewRedrawAll ();
	if (data->Type () == DBTypeNetwork)
		{
		UITable *cellTable = (UITable *) data->Display (DBrNCells);
		if (cellTable != (UITable *) NULL) cellTable->Draw ();
		}
	}

static void _UITableInvertSelectionCBK (Widget widget,UITable *uiTable,XmAnyCallbackStruct *callData)

	{
	DBObjData *data = uiTable->Data ();
	DBObjTable *table = uiTable->Table ();
	DBObjRecord *record;

	widget = widget; callData = callData;
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		record->Flags (DBObjectFlagSelected,((record->Flags () & DBObjectFlagSelected) != DBObjectFlagSelected));
	uiTable->Draw ();
	if (strcmp (table->Name (),DBrNItems) == 0) UI2DViewRedrawAll ();
	if (data->Type () == DBTypeNetwork)
		{
		UITable *cellTable = (UITable *) data->Display (DBrNCells);
		if (cellTable != (UITable *) NULL) cellTable->Draw ();
		}
	}

static void _UITableSetIdleCBK (Widget widget,UITable *uiTable,XmAnyCallbackStruct *callData)

	{
	DBObjTable *table = uiTable->Table ();
	DBObjRecord *record;

	widget = widget; callData = callData;
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
			{
			record->Flags (DBObjectFlagSelected,DBClear);
			record->Flags (DBObjectFlagIdle,DBSet);
			}
		else	record->Flags (DBObjectFlagIdle,DBClear);
	uiTable->Draw ();
	if (strcmp (table->Name (),DBrNItems) == 0) UI2DViewRedrawAll ();
	}

static void _UITableGetIdleCBK (Widget widget,UITable *uiTable,XmAnyCallbackStruct *callData)

	{
	DBObjTable *table = uiTable->Table ();
	DBObjRecord *record;

	widget = widget; callData = callData;
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			{
			record->Flags (DBObjectFlagIdle,DBClear);
			record->Flags (DBObjectFlagSelected,DBSet);
			}
		else	record->Flags (DBObjectFlagSelected,DBClear);
	uiTable->Draw ();
	if (strcmp (table->Name (),DBrNItems) == 0) UI2DViewRedrawAll ();
	}

static void _UITableDeleteCBK (Widget widget,UITable *uiTable,XmAnyCallbackStruct *callData)

	{
	DBObjData *dbData = uiTable->Data ();
	widget = widget; callData = callData;
	dbData->DispRemove (uiTable);
	delete uiTable;
	}

UITable::UITable (DBObjData *dbData, DBObjTable *table) : DBObjectLIST<UITableRecord> (table->Name (),sizeof (UITable))

	{
	char *title = UITableName (dbData,table);
	int row;
	DBObjTableField *field;
	DBObjectLIST<DBObjTableField> *fields;
	DBObjRecord *record;
	Pixel foreground, background;
	Dimension clipHeight, menuWidth;
	static Pixmap iconPixmap = (Pixmap) NULL;
	static Pixmap loadPixmap = (Pixmap) NULL, savePixmap = (Pixmap) NULL;
	static Pixmap setIdlePixmap = (Pixmap) NULL, getIdlePixmap = (Pixmap) NULL;
	static Pixmap clearPixmap = (Pixmap) NULL,invertPixmap = (Pixmap) NULL;
	Widget mainForm, menuBar, leftScrolledW, rightScrolledW, recordMenuBar, clipWindow, button;
	XmString string;
	XmFontList fontList;
	XtActionsRec actionRec = { (char *) "resize",_UITableResizeACTION };
	Atom deleteWindowAtom = XmInternAtom(XtDisplay(UITopLevel ()),(char *) "WM_DELETE_WINDOW",FALSE);

	if (table == (DBObjTable *) NULL) return;
	XtAppAddActions (UIApplicationContext (), &actionRec,1);
	DataPTR = dbData;
	TablePTR = table;

	fields = TablePTR->Fields ();
	StartRowVAR = 0;

	Name (title);
	DShellWGT = XtVaCreateWidget ("UTableDialogShell",xmDialogShellWidgetClass,UITopLevel (),
												XmNkeyboardFocusPolicy,		XmPOINTER,
												XmNtitle,						title,
												XmNtransient,					false,
												XmNallowShellResize,			false,
												NULL);
	XmAddWMProtocolCallback (DShellWGT,deleteWindowAtom,(XtCallbackProc) _UITableDeleteCBK,(XtPointer) this);
	mainForm = XtVaCreateWidget ("UITableMainForm",xmFormWidgetClass,DShellWGT,
												XmNuserData,					this,
												XmNshadowThickness,			0,
												NULL);
	XtOverrideTranslations (mainForm,XtParseTranslationTable ("<Configure>: resize()"));
	XtVaGetValues (mainForm,XmNforeground,	&foreground,XmNbackground,	&background,NULL);
	iconPixmap = iconPixmap != (Pixmap) NULL ? iconPixmap :
					 XmGetPixmap(XtScreen (UITopLevel()),(char *) "GHAAStable",foreground,background);
	XtVaSetValues (DShellWGT,XmNiconPixmap, iconPixmap,NULL);
	menuBar = XtVaCreateManagedWidget ("UITableMenuBar",xmRowColumnWidgetClass,mainForm,
												XmNtopAttachment,				XmATTACH_FORM,
												XmNtopOffset,					2,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNleftOffset,					2,
												XmNrightAttachment, 			XmATTACH_FORM,
												XmNrightOffset,				2,
												XmNorientation,				XmHORIZONTAL,
												XmNspacing,						10,
												XmNrowColumnType,				XmWORK_AREA,
												XmNshadowThickness,			2,
												NULL);

	loadPixmap = loadPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASload.bmp",  foreground,background) : loadPixmap;
	savePixmap = savePixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASsave.bmp",  foreground,background) : savePixmap;
	clearPixmap = clearPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASclear.bmp", foreground,background) : clearPixmap;
	invertPixmap = invertPixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASinvert.bmp", foreground,background) : invertPixmap;
	setIdlePixmap = setIdlePixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASsetIdle.bmp", foreground,background) : setIdlePixmap;
	getIdlePixmap = getIdlePixmap == (Pixmap) NULL ?
					XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASsetIdle.bmp", foreground,background) : getIdlePixmap;

	button = XtVaCreateManagedWidget ("UILoadSelectionButton",xmPushButtonWidgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				loadPixmap,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableLoadSelectionCBK,this);
	button = XtVaCreateManagedWidget ("UISaveSelectionButton",xmPushButtonWidgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				savePixmap,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableSaveSelectionCBK,this);
	button = XtVaCreateManagedWidget ("UIClearSelectionButton",xmPushButtonWidgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				clearPixmap,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableClearSelectionCBK,this);
	button = XtVaCreateManagedWidget ("UIInvertSelectionButton",xmPushButtonWidgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				invertPixmap,
												NULL);
	XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableInvertSelectionCBK,this);
	if ((strcmp (table->Name (),DBrNItems) == 0) ||
		 (strcmp (table->Name (),DBrNCells) == 0) ||
		 (strcmp (table->Name (),DBrNLayers) == 0))
		{
		button = XtVaCreateManagedWidget ("UISetIdleButton",xmPushButtonWidgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				setIdlePixmap,
												NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableSetIdleCBK,this);
		button = XtVaCreateManagedWidget ("UIGetIdleButton",xmPushButtonWidgetClass,menuBar,
												XmNlabelType,					XmPIXMAP,
												XmNlabelPixmap,				getIdlePixmap,
												NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UITableGetIdleCBK,this);
		}
	VerScrollBarWGT = XtVaCreateManagedWidget ("UITableVerScrollBar",xmScrollBarWidgetClass,mainForm,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					menuBar,
												XmNtopOffset,					4,
												XmNrightAttachment,			XmATTACH_FORM,
												XmNrightOffset,				4,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				26,
												XmNorientation,				XmVERTICAL,
												NULL);
	XtAddCallback (VerScrollBarWGT,XmNvalueChangedCallback,(XtCallbackProc) _UITableVerScrollBarCBK,this);
	leftScrolledW = XtVaCreateManagedWidget ("UITableLeftScrolledWindow",xmScrolledWindowWidgetClass,mainForm,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					menuBar,
												XmNtopOffset,					4,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNleftOffset,					5,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				4,
												XmNscrollBarDisplayPolicy,	XmAS_NEEDED,
												XmNscrollingPolicy,			XmAUTOMATIC,
												XmNspacing,						UITableSpacing,
												XmNshadowThickness,			2,
												XmNwidth,						UITableRecordSpace,
												NULL);
	rightScrolledW = XtVaCreateManagedWidget ("UITableRightScrolledWindow",xmScrolledWindowWidgetClass,mainForm,
												XmNtopAttachment,				XmATTACH_WIDGET,
												XmNtopWidget,					menuBar,
												XmNtopOffset,					4,
												XmNleftAttachment,			XmATTACH_WIDGET,
												XmNleftWidget,					leftScrolledW,
												XmNleftOffset,					5,
												XmNrightAttachment,			XmATTACH_WIDGET,
												XmNrightWidget,				VerScrollBarWGT,
												XmNrightOffset,				2,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				4,
												XmNscrollBarDisplayPolicy,	XmAS_NEEDED,
												XmNscrollingPolicy,			XmAUTOMATIC,
												XmNspacing,						UITableSpacing,
												XmNshadowThickness,			2,
												NULL);
	LeftRowColWGT = XtVaCreateManagedWidget ("UITableLeftRowCol",xmRowColumnWidgetClass,leftScrolledW,
												XmNorientation,				XmVERTICAL,
												XmNspacing,						UITableSpacing,
												XmNallowShellResize,			true,
												XmNmarginHeight,				UITableSpacing,
												XmNmarginWidth,				UITableSpacing,
												XmNforeground,					background,
												XmNbackground,					foreground,
												XmNtraversalOn,				true,
												XmNuserData,					this,
												NULL);
	RightRowColWGT = XtVaCreateManagedWidget ("UITableRightRowCol",xmRowColumnWidgetClass,rightScrolledW,
												XmNorientation,				XmVERTICAL,
												XmNspacing,						UITableSpacing,
												XmNallowShellResize,			true,
												XmNmarginHeight,				UITableSpacing,
												XmNmarginWidth,				UITableSpacing,
												XmNforeground,					background,
												XmNbackground,					foreground,
												XmNtraversalOn,				true,
												XmNuserData,					this,
												NULL);
	XmScrolledWindowSetAreas (rightScrolledW,(Widget) NULL,(Widget) NULL,RightRowColWGT);
	recordMenuBar = XtVaCreateManagedWidget ("UITableRecordMenuBar",xmRowColumnWidgetClass,LeftRowColWGT,
												XmNmarginHeight,				0,
												XmNmarginWidth,				0,
												XmNorientation,				XmHORIZONTAL,
												XmNspacing,						UITableSpacing,
												XmNrowColumnType,				XmMENU_BAR,
												XmNshadowThickness,			0,
												XmNforeground,					foreground,
												XmNbackground,					background,
												XmNuserData,					this,
												NULL);
	string = XmStringCreate ((char *) "RecordID",UICharSetBold);
	RecordIDButtonWGT = XtVaCreateManagedWidget ("UITableRecordNumberButton",xmCascadeButtonWidgetClass,recordMenuBar,
												XmNlabelString,				string,
												XmNhighlightThickness,		0,
												XmNshadowThickness,			1,
												XmNsubMenuId,					_UITableCreateHeaderMenu (recordMenuBar),
												XmNforeground,					background,
												XmNbackground,					foreground,
												NULL);
	XmStringFree (string);
	string = XmStringCreate ((char *) "Record Name",UICharSetBold);
	NameButtonWGT = XtVaCreateManagedWidget ("UITableRecordNameButton",xmCascadeButtonWidgetClass,recordMenuBar,
												XmNlabelString,				string,
												XmNhighlightThickness,		0,
												XmNshadowThickness,			1,
												XmNsubMenuId,					_UITableCreateHeaderMenu (recordMenuBar,(UITableField *) NULL),
												XmNforeground,					background,
												XmNbackground,					foreground,
												NULL);
	XmStringFree (string);
	FieldMenuBarWGT = XtVaCreateManagedWidget ("UITableFieldMenuBar",xmRowColumnWidgetClass,RightRowColWGT,
												XmNmarginHeight,				0,
												XmNmarginWidth,				0,
												XmNorientation,				XmHORIZONTAL,
												XmNspacing,						UITableSpacing,
												XmNrowColumnType,				XmMENU_BAR,
												XmNshadowThickness,			0,
												XmNforeground,					foreground,
												XmNbackground,					background,
												XmNuserData,					this,
												NULL);
	FieldsPTR = new DBObjectLIST<UITableField> ("Table Fields");
	for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
		{
		if ((DBTableFieldIsVisible (field) == false) ||
			 ((field->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)) continue;
		AddField (field);
		}
	StartRowVAR = 0;
	ResizingVAR = false;
	NameWidthVAR = 0;
	XtRealizeWidget (mainForm);
	XtVaGetValues (rightScrolledW,	XmNclipWindow, &clipWindow, 	NULL);
	XtVaGetValues (mainForm,			XmNheight,		&BaseHeightVAR,NULL);
	XtVaGetValues (clipWindow,			XmNheight,		&clipHeight, 	NULL);
	XtVaGetValues (FieldMenuBarWGT,	XmNheight,		&CellHeightVAR,NULL);
	XtVaGetValues (button,				XmNwidth,		&menuWidth,		NULL);
	menuWidth = (menuWidth + 10) * 8;
	XtVaSetValues (DShellWGT,			XmNminWidth,	menuWidth,		NULL);
	BaseHeightVAR -= clipHeight  - CellHeightVAR - 4 * UITableSpacing;
	row = TablePTR->ItemNum () > 20 ? 20 :TablePTR->ItemNum ();
	XtVaSetValues (mainForm,			XmNheight, 		BaseHeightVAR + row * (CellHeightVAR + UITableSpacing), NULL);
	XtVaGetValues (RecordIDButtonWGT, XmNfontList, &fontList, XmNwidth, &RecordIDWidthVAR, NULL);
	string = XmStringCreate ((char *) "9999999",UICharSetNormal);
	RecordIDWidthVAR = RecordIDWidthVAR > (XmStringWidth (fontList,string) + 2 * UITableMargin) ?
							 RecordIDWidthVAR : (XmStringWidth (fontList,string) + 2 * UITableMargin);
	NameWidthVAR = UITableRecordSpace - (RecordIDWidthVAR + 4 * UITableSpacing + 4);
	XmStringFree (string);
	for (record = TablePTR->First ();record != (DBObjRecord *) NULL;record = TablePTR->Next ())
		AddRecord (record);
	XtVaSetValues (RecordIDButtonWGT,XmNwidth,	RecordIDWidthVAR,	XmNrecomputeSize, false, NULL);
	XtVaSetValues (NameButtonWGT,		XmNwidth,	NameWidthVAR,	XmNrecomputeSize, false, NULL);
	XtManageChild (mainForm);
	}

void UITable::Resize (int height)

	{
	DBInt newRowNum, row;
	DBObjRecord *record;

	if (ResizingVAR) return;
	ResizingVAR = true;
	newRowNum = (int) floor ((double) (abs (height - BaseHeightVAR) / (CellHeightVAR + UITableSpacing)));
	if (newRowNum < 1) { ResizingVAR = false; return; }
	if ((newRowNum -= ItemNum ()) == 0) { ResizingVAR = false; return; }
	record = TablePTR->First (ItemNum ());
	if (newRowNum > 0)
		for (row = 0;row < newRowNum;++row)
			{
			Add (new UITableRecord (LeftRowColWGT,RightRowColWGT,RecordIDWidthVAR,NameWidthVAR,FieldsPTR,CellHeightVAR));
			Draw (ItemNum () - 1,record);
			record = TablePTR->Next ();
			}
	else
		{
		UITableRecord *uiRecord;
		for (row = newRowNum;row < 0;++row)
			if ((uiRecord = Item (ItemNum () - 1)) != (UITableRecord *) NULL)
				{ Remove (uiRecord); delete uiRecord; }
		}
	XtVaSetValues (VerScrollBarWGT,	XmNpageIncrement,		ItemNum () - 1,
												XmNsliderSize,			ItemNum (),
												XmNmaximum,				TablePTR->ItemNum (),
												NULL);
	ResizingVAR = false;
	}

void UITable::AddField (DBObjTableField *field)

	{
	UITableField *uiField = new UITableField (FieldMenuBarWGT,field);
	UITableRecord *uiRecord;

	FieldsPTR->Add (uiField);
	for (uiRecord = First ();uiRecord != (UITableRecord *) NULL;uiRecord = Next ())
		uiRecord->AddField (uiField,CellHeightVAR);
	}

void UITable::AddRecord (DBObjRecord *record)

	{
	DBInt width, row;
	XmString string;
	XmFontList fontList;

	XtVaGetValues (NameButtonWGT, XmNfontList, &fontList, NULL);
	string = XmStringCreate (record->Name (),UICharSetNormal);
	width = XmStringWidth (fontList,string);
	NameWidthVAR = NameWidthVAR > width ? NameWidthVAR : width;
	XmStringFree (string);
	row = TablePTR->ItemNum () > 36 ? 36 : TablePTR->ItemNum ();
	XtVaSetValues (DShellWGT,			XmNbaseHeight,	BaseHeightVAR,
												XmNheightInc,	CellHeightVAR + UITableSpacing,
												XmNminHeight,	BaseHeightVAR + CellHeightVAR + UITableSpacing,
												XmNmaxHeight,	BaseHeightVAR + row * (CellHeightVAR + UITableSpacing),
												NULL);
	}

void UITable::Draw (DBInt startRow)

	{
	DBInt row = 0;
	DBObjRecord *record;

	StartRowVAR = startRow;
	for (record = TablePTR->First (StartRow ());(row < ItemNum ()) && (record != (DBObjRecord *) NULL);record = TablePTR->Next ())
		Draw (row++,record);
	}

void UITable::Draw (DBObjRecord *record)

	{
	UITableRecord *uiRecord;

	for (uiRecord = First ();uiRecord != (UITableRecord *) NULL;uiRecord =Next ())
		if (uiRecord->Record () == record) break;
	if (uiRecord != (UITableRecord *) NULL) uiRecord->Draw ();
	}

void UITable::Raise () const

	{
	}
