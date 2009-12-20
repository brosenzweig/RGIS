/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGISAnGNUXYPlot.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <rgis.H>

void RGISAnGNUXYPlotCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	FILE *file;
	char command [DBDataFileNameLen * 2 + DBStringLength];
	const char *ghaasDir;
	char *f0Text, *f1Text, fileName [12];
	int allowOk, fd;
	static int plot = false, logScale = false;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	DBObjTable *itemTable = dbData->Table (DBrNItems);
	static Widget dShell = (Widget) NULL, mainForm;
	static Widget field0TextF, field1TextF;
	Widget toggle;
	XmString string;

	widget = widget;	workspace = workspace; callData = callData;
	if (dShell == (Widget) NULL)
		{
		Widget button;

		dShell = UIDialogForm ((char *) "Compare Fields",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISEdiCompFieldsButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		field0TextF = XtVaCreateManagedWidget ("RGISEditCompField0TextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,field0TextF);
		string = XmStringCreate ((char *) "X Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISEditCompFieldNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			field0TextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISEditCompField0Button",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				button,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		field1TextF = XtVaCreateManagedWidget ("RGISEditCompField1TextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,field1TextF);
		string = XmStringCreate ((char *) "Y Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISEditCompField1Label",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			field1TextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Logarithmic Scale",UICharSetBold);
		toggle = XtVaCreateManagedWidget ("RGISEditCompLogScaleToggle",xmToggleButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				button,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNlabelString,			string,
								XmNhighlightThickness,	0,
								XmNshadowThickness,		0,
								XmNmarginWidth,			5,
								XmNset,						False,
								NULL);
 		XmStringFree (string);
 		XtAddCallback (toggle,XmNvalueChangedCallback,(XtCallbackProc) UIAuxSetToggleCBK, (XtPointer) &logScale);

		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&plot);
		}
	plot = false;
	XtVaSetValues (field0TextF,XmNuserData, itemTable->Fields (), NULL);
	XtVaSetValues (field1TextF,XmNuserData, itemTable->Fields (), NULL);
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		f0Text = XmTextFieldGetString (field0TextF);
		f1Text = XmTextFieldGetString (field1TextF);
		allowOk = (strlen (f0Text) > 0) && (strlen (f1Text) > 0);
		XtFree (f0Text);	XtFree (f1Text);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),allowOk);
		}
	UIDialogFormPopdown (dShell);

	if (plot)
		{
		char buffer [256];
		DBInt intVal, rowID;
		DBFloat val;
		DBObjTableField *field0 = itemTable->Field (f0Text = XmTextFieldGetString (field0TextF));
		DBObjTableField *field1 = itemTable->Field (f1Text = XmTextFieldGetString (field1TextF));
		DBObjRecord *record;

		XtFree (f0Text);	XtFree (f1Text);
		if ((field0 == (DBObjTableField *) NULL) || (field1 == (DBObjTableField *) NULL))
			{ fprintf (stderr,"Invalid Compare Fields in: _RGISEditCompareFieldsCBK ()\n");	return; }

		strcpy (fileName,"ghaasXXXXXX");
		if ((fd = mkstemp (fileName)) == DBFault)
			{ perror ("File Opening Error in: RGISAnGNUXYPlotCBK ()"); return; }

		if ((file = fdopen (fd,"w")) == (FILE *) NULL)
			{ perror ("File Opening Error in: RGISAnGNUXYPlotCBK ()"); return; }

		fprintf (file,"\"GHAASId\"\t\"RecordName\"\t\"Selected\"\t\"%s\"\t\"%s\"\n",field0->Name (),field1->Name ());
		for (rowID = 0;rowID < itemTable->ItemNum ();++rowID)
			{
			record = itemTable->Item (rowID);
			if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			sprintf (buffer,"%d\t\"%s\"\t%d",record->RowID () + 1,record->Name (),
						(record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ? true : false);
			if (field0->Type () == DBTableFieldInt)
				{
				intVal = field0->Int (record);
				if (intVal == field0->IntNoData ()) continue;
				sprintf (buffer + strlen (buffer),"\t%d",intVal);
				}
			else
				{
				val = field0->Float (record);
				if (CMmathEqualValues (val,field0->FloatNoData ())) continue;
				sprintf (buffer + strlen (buffer),"\t%f",val);
				}
			if (field1->Type () == DBTableFieldInt)
				{
				intVal = field1->Int (record);
				if (intVal == field1->IntNoData ()) continue;
				sprintf (buffer + strlen (buffer),"\t%d",intVal);
				}
			else
				{
				val = field1->Float (record);
				if (CMmathEqualValues (val,field1->FloatNoData ())) continue;
				sprintf (buffer + strlen (buffer),"\t%f",val);
				}
			fprintf (file,"%s\n",buffer);
			}
		fclose (file);
		ghaasDir = getenv ("GHAAS_DIR");
		sprintf (command,"%s/Scripts/xy-plot.sh %s %s",ghaasDir != (const char *) NULL ? ghaasDir : ".",fileName,logScale ? "log" : "normal");
		system (command);
		unlink (fileName);
		}
	}
