/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISAnLine.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <rgis.H>

static DBObjectLIST<DBObjTableField> *_RGISAnLineSampleGridFields = (DBObjectLIST<DBObjTableField> *) NULL;

static void _RGIAnalyseLineSSampleGridSSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = NULL;
	char *field;
	int (*condFunc) (const DBObject *);

	callData = callData;
	if (select == NULL) select = UISelectionCreate ("Field Selection");
	XtVaGetValues (widget,XmNuserData, &condFunc, NULL);
	if ((field = UISelectObject (select,(DBObjectLIST<DBObject> *) (_RGISAnLineSampleGridFields),condFunc)) != NULL)
		XmTextFieldSetString (text,field);
	}

void RGISAnalyseLineSSampleGridCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *fText;
	int allowOk;
	static int sample;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData  = dataset->Data (), *grdData = dbData->LinkedData ();
	DBObjTable *itemTable = dbData->Table (DBrNItems);
	static Widget dShell = (Widget) NULL, mainForm;
	static Widget fromNameTextF, toNameTextF;
	XmString string;

	widget = widget;	workspace = workspace; callData = callData;
	_RGISAnLineSampleGridFields = itemTable->Fields ();
	if (dShell == (Widget) NULL)
		{
		Widget button;

		dShell = UIDialogForm ("Single Layer Grid Sampling",false);
		mainForm = UIDialogFormGetMainForm (dShell);
		
		string = XmStringCreate ("Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseLineSSampleNameButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				grdData->Type () == DBTypeGridContinuous ? DBTableFieldIsNumeric : DBTableFieldIsCategory,
								NULL);
		XmStringFree (string);
		fromNameTextF = XtVaCreateManagedWidget ("RGISAnalyseLineSSampleNameTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			button,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGIAnalyseLineSSampleGridSSelectCBK,fromNameTextF);
		string = XmStringCreate ("From Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseLineSSampleNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			fromNameTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		
		string = XmStringCreate ("Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalyseLineSSampleNameButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				button,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				grdData->Type () == DBTypeGridContinuous ? DBTableFieldIsNumeric : DBTableFieldIsCategory,
								NULL);
		XmStringFree (string);
		toNameTextF = XtVaCreateManagedWidget ("RGISAnalyseLineSSampleNameTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			button,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					DBStringLength / 2,
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGIAnalyseLineSSampleGridSSelectCBK,toNameTextF);
		string = XmStringCreate ("To Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalyseLineSSampleNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			toNameTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&sample);
		}

	sample = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		allowOk = false;
		fText = XmTextFieldGetString (fromNameTextF);
		if (strlen (fText) > 0) allowOk = true;
		XtFree (fText);	
		fText = XmTextFieldGetString (toNameTextF);
		if (strlen (fText) > 0) allowOk = true;
		XtFree (fText);	
		XtSetSensitive (UIDialogFormGetOkButton (dShell),allowOk);
		}
	UIDialogFormPopdown (dShell);
	
	if (sample)
		{
		DBInt ret;
		DBFloat value;
		DBCoordinate coord;
		DBGridIO *gridIO = new DBGridIO (grdData);
		DBVLineIO *linIO = new DBVLineIO (dbData);
		DBObjTableField *fromField;
		DBObjTableField *toField;
		DBObjRecord *record;
		
		fText = XmTextFieldGetString (fromNameTextF);
		if (strlen (fText) > 0)
			{
			if ((fromField = itemTable->Field (fText)) == (DBObjTableField *) NULL)
				itemTable->AddField (fromField = new DBObjTableField (fText,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4)));
			}
		else	fromField = (DBObjTableField *) NULL;
		XtFree (fText);
		fText = XmTextFieldGetString (toNameTextF);
		if (strlen (fText) > 0)
			{
			if ((toField = itemTable->Field (fText)) == (DBObjTableField *) NULL)
				itemTable->AddField (toField = new DBObjTableField (fText,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4)));
			}
		else	toField = (DBObjTableField *) NULL;
		XtFree (fText);

		UIPauseDialogOpen ("Sampling Grid");
		for (record = itemTable->First ();record != (DBObjRecord *) NULL;record = itemTable->Next ())
			{
			if (UIPause (record->RowID () * 100 / itemTable->ItemNum ())) goto Stop;
			if (fromField != (DBObjTableField *) NULL)
				{
				coord = linIO->FromCoord (record);
				ret = gridIO->Value (coord,&value);
				if (fromField->Type () == DBTableFieldFloat)
					{
					if (ret)	fromField->Float (record,value);
					else		fromField->Float (record,fromField->FloatNoData ());
					}
				else
					{
					if (ret)	fromField->Int (record,(DBInt) value);
					else		fromField->Int (record,fromField->IntNoData ());
					}
				}
			if (toField != (DBObjTableField *) NULL)
				{
				coord = linIO->ToCoord (record);
				ret = gridIO->Value (coord,&value);
				if (toField->Type () == DBTableFieldFloat)
					{
					if (ret)	toField->Float (record,value);
					else		toField->Float (record,toField->FloatNoData ());
					}
				else
					{
					if (ret)	toField->Int (record,(DBInt) value);
					else		toField->Int (record,toField->IntNoData ());
					}
				}
			}
Stop:
		UIPauseDialogClose ();
		delete linIO;
		delete gridIO;
		}
	}

#define RGISLineFromNodeValue	"FromNodeValue"
#define RGISLineToNodeValue	"ToNodeValue"

void RGISAnalyseLineMSampleGridCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt layerID, layerNum;
	DBCoordinate coord;
	DBFloat realValue;
	DBDataset *dataset  = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	DBObjData *grdData  = dbData->LinkedData ();
	DBObjData *tblData;
	DBObjTable *table, *itemTable = dbData->Table (DBrNItems);
	DBObjTableField *lineIDFLD;
	DBObjTableField *layerIDFLD;
	DBObjTableField *layerNameFLD;
	DBObjTableField *fromValueFLD = (DBObjTableField *) NULL;
	DBObjTableField *toValueFLD = (DBObjTableField *) NULL;
	DBVLineIO	*linIO = (DBVLineIO *)	 NULL;
	DBGridIO *gridIO;
	DBObjRecord *record, *layerRec, *tblRec;
	DBObjectLIST<DBObjTableField> *fields;

	widget = widget; callData = callData;

	gridIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGISAnalyseLineMSampleGridCBK ()\n"); delete gridIO; return; }


	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,dbData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,grdData->Document (DBDocSubject));

	if (UIDataHeaderForm (tblData) == false) { delete gridIO; delete tblData; return; }
	table = tblData->Table (DBrNItems);

	linIO = new DBVLineIO (dbData);

	table->AddField (lineIDFLD =		new DBObjTableField ("GHAASPointID",DBTableFieldInt,	"%8d",sizeof (DBInt)));
	table->AddField (layerIDFLD =		new DBObjTableField ("LayerID",		DBTableFieldInt,	"%4d",sizeof (DBShort)));
	table->AddField (layerNameFLD =	new DBObjTableField ("LayerName",	DBTableFieldString,"%s",DBStringLength));
	table->AddField (fromValueFLD =	new DBObjTableField (RGISLineFromNodeValue,DBTableFieldFloat,gridIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (toValueFLD =		new DBObjTableField (RGISLineToNodeValue,DBTableFieldFloat,gridIO->ValueFormat (),sizeof (DBFloat4)));

	grdData->Flags (DBObjectFlagProcessed,DBSet);
	UIPauseDialogOpen ("Sampling Grid(s)");
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (record = itemTable->First ();record != (DBObjRecord *) NULL;record = itemTable->Next ())
			{
			if (UIPause ((layerRec->RowID () * itemTable->ItemNum () + record->RowID ()) * 100 / (itemTable->ItemNum () * gridIO->LayerNum ()))) goto Stop;
			if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			tblRec = table->Add (record->Name ());
			lineIDFLD->Int (tblRec,record->RowID () + 1);
			layerIDFLD->Int (tblRec,layerRec->RowID ());
			layerNameFLD->String (tblRec,layerRec->Name ());
			coord = linIO->FromCoord (record);
			if (gridIO->Value (layerRec,coord,&realValue))
				fromValueFLD->Float (tblRec,realValue);
			coord = linIO->ToCoord (record);
			if (gridIO->Value (layerRec,coord,&realValue))
				toValueFLD->Float (tblRec,realValue);
			}
		}
Stop:
	UIPauseDialogClose ();
	delete gridIO;
	delete linIO;

	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*lineIDFLD));
	fields->Add (new DBObjTableField (*layerIDFLD));
	table->ListSort (fields);
	workspace->CurrentData  (tblData);
	delete fields;
	}
