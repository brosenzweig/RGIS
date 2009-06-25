/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISToolsARCImp.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <rgis.H>

static DBInt _RGISARCDataType;
static DBObjTable *_RGISARCTempTable = NULL;

static void _RGISARCInfoImportFileSelectCBK (Widget widget,Widget coverText,XmAnyCallbackStruct *callData)

	{
	static Widget fileSelect = NULL;
	char *infoFile;
	widget = widget; callData = callData;

	if (fileSelect == NULL)
		fileSelect = UIFileSelectionCreate ((char *) "ARC/Info Coverage",NULL,(char *) "*",XmFILE_DIRECTORY);

	if ((infoFile = UIFileSelection (fileSelect,true)) != NULL)
		{
		if (_RGISARCTempTable  != (DBObjTable *) NULL) delete _RGISARCTempTable;
		_RGISARCTempTable = new DBObjTable ((char *) "ARCImp Temporary Table");
		if (DBInfoGetFields (_RGISARCTempTable,DBInfoFileName (infoFile,_RGISARCDataType)) == DBFault)
			{ UIMessage ((char *) "Missing Attribute Table"); delete _RGISARCTempTable; _RGISARCTempTable = ( DBObjTable *) NULL; return; }
		XmTextFieldSetString (coverText,infoFile);
		}
	else
		{
		XmTextFieldSetString (coverText,(char *) "");
		if (_RGISARCTempTable != NULL) { delete _RGISARCTempTable; _RGISARCTempTable = NULL; }
		}
	}

static void _RGISARCInfoImportClearFieldCBK (Widget widget,Widget text, XmAnyCallbackStruct *callData)

	{ callData = callData; if (strlen (XmTextFieldGetString (widget)) == 0) XmTextFieldSetString (text,(char *) ""); }

static void _RGISARCInfoImportFieldSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = NULL;
	char *field;
	int (*condFunc) (const DBObject *);

	callData = callData;
	if (select == NULL) select = UISelectionCreate ((char *) "ARC/Info Fields");
	XtVaGetValues (widget,XmNuserData, &condFunc, NULL);
	if ((field = UISelectObject (select,(DBObjectLIST<DBObject> *) (_RGISARCTempTable->Fields ()),condFunc)) != NULL)
		XmTextFieldSetString (text,field);
	}

int _RGISARCInfoImport (DBObjData *vecData)

	{
	char *text;
	int allowSave, ret = DBFault;
	static int import;
	static Widget dShell = NULL;
	static Widget coverTextF, nameTextF, nameButton, symbolTextF, symbolButton;

	_RGISARCDataType = vecData->Type ();

	if (dShell == NULL)
		{
		Widget mainForm, label, coverButton;
		XmString string;

		dShell = UIDialogForm ((char *) "ARC/Info Coverage Import");
		mainForm = UIDialogFormGetMainForm (dShell);
		string = XmStringCreate ((char *) "Coverage:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISARCInfoImportFormCoverLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_POSITION,
								XmNrightPosition,			20,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		coverTextF = XtVaCreateManagedWidget ("RGISARCInfoImportFormCoverTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				label,
								XmNleftOffset,				5,
								XmNmaxLength,				64,
								XmNcolumns,					64,
								NULL);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		coverButton = XtVaCreateManagedWidget ("RGISARCInfoImportFormCoverLabel",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				coverTextF,
								XmNleftOffset,				5,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								NULL);
		XtAddCallback (coverButton,XmNactivateCallback,(XtCallbackProc) _RGISARCInfoImportFileSelectCBK,coverTextF);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Name Field:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISARCInfoImportFormSymbolIDLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				coverTextF,
								XmNtopOffset,				5,
								XmNrightAttachment,		XmATTACH_POSITION,
								XmNrightPosition,			20,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		nameTextF = XtVaCreateManagedWidget ("RGISARCInfoImportFormSymbolIDTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				coverTextF,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				label,
								XmNleftOffset,				5,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					16,
								NULL);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		nameButton = XtVaCreateManagedWidget ("RGISARCInfoImportFormSymbolIDButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				coverTextF,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				nameTextF,
								XmNleftOffset,				5,
								XmNuserData,				DBTableFieldIsString,
								XmNlabelString,			string,
								XmNtraversalOn,			False,
								NULL);
		XtAddCallback (nameButton,XmNactivateCallback,(XtCallbackProc) _RGISARCInfoImportFieldSelectCBK,nameTextF);
		XtAddCallback (coverTextF,XmNvalueChangedCallback,(XtCallbackProc) _RGISARCInfoImportClearFieldCBK,nameTextF);
		XmStringFree (string);
		XtSetSensitive (nameButton,False);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		symbolButton = XtVaCreateManagedWidget ("RGISARCInfoImportFormSymbolNameButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				coverTextF,
								XmNtopOffset,				5,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsCategory,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);
		XtSetSensitive (symbolButton,False);
		symbolTextF = XtVaCreateManagedWidget ("RGISARCInfoImportFormSymbolNameTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				coverTextF,
								XmNtopOffset,				5,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			symbolButton,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					16,
								NULL);
		XtAddCallback (symbolButton,XmNactivateCallback,(XtCallbackProc) _RGISARCInfoImportFieldSelectCBK,symbolTextF);
		XtAddCallback (coverTextF,XmNvalueChangedCallback,(XtCallbackProc) _RGISARCInfoImportClearFieldCBK,symbolTextF);
		string = XmStringCreate ((char *) "Symbol Field:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISARCInfoImportFormSymbolNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				coverTextF,
								XmNtopOffset,				5,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			symbolTextF,
								XmNrightOffset,			5,
								XmNlabelString,			string,
								NULL);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&import);
		XmStringFree (string);
		}
	XmProcessTraversal (coverTextF,XmTRAVERSE_CURRENT);
	import = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		text = XmTextFieldGetString (coverTextF);
		allowSave = strlen (text) > 0;
		XtSetSensitive (nameButton,allowSave);
		XtSetSensitive (symbolButton,allowSave);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),allowSave);
		XtFree (text);
		}
	UIDialogFormPopdown (dShell);
	if (import)
		{
		char *infoFile, *nameField, *symbolField;
		infoFile = XmTextFieldGetString (coverTextF);
		nameField = XmTextFieldGetString (nameTextF);
		symbolField = XmTextFieldGetString (symbolTextF);
		UIPauseDialogOpen ((char *) "Loading ARC/Info Coverage");
		ret = DBImportARCVector (vecData, infoFile,
						 strlen (nameField) > 0   ? nameField 	 : (char *) NULL,
						 strlen (symbolField) > 0 ? symbolField : (char *) NULL);
		UIPauseDialogClose ();
		XtFree (infoFile);
		XtFree (nameField );
		XtFree (symbolField);
		}
	if ( _RGISARCTempTable != NULL) {delete _RGISARCTempTable; _RGISARCTempTable = (DBObjTable *) NULL; }
	return (ret);
	}
