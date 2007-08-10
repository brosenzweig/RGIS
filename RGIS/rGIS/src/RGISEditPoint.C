/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISEditPoint.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <rgis.H>

#define RGISEditPointXCoord "XCoord"
#define RGISEditPointYCoord "YCoord"

void RGISEditPointAddXYCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData =dataset->Data ();
	DBVPointIO *pointIO = new DBVPointIO (dbData);
	DBObjTable *pointTable = dbData->Table (DBrNItems);
	DBObjTableField *xCoordFLD	= pointTable->Field (RGISEditPointXCoord);
	DBObjTableField *yCoordFLD = pointTable->Field (RGISEditPointYCoord);
	DBCoordinate coord;
	DBObjRecord *pointRec;
	UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,pointTable));

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ("Adding XY Coordinates");
	if (xCoordFLD == NULL)
		{
		xCoordFLD = new DBObjTableField (RGISEditPointXCoord,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4));
		pointTable->AddField (xCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (xCoordFLD);
		UIPause (40);
		}
	if (yCoordFLD == NULL)
		{
		yCoordFLD = new DBObjTableField (RGISEditPointYCoord,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4));
		pointTable->AddField (yCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (yCoordFLD);
		UIPause (80);
		}
	for (pointRec = pointIO->FirstItem (); pointRec != (DBObjRecord *) NULL; pointRec = pointIO->NextItem ())
		{
		if (UIPause (80 + pointRec->RowID () * 20 / pointIO->ItemNum ())) goto Stop;
		coord = pointIO->Coordinate  (pointRec);
		xCoordFLD->Float (pointRec,coord.X);
		yCoordFLD->Float (pointRec,coord.Y);
		}
Stop:
	UIPauseDialogClose ();
	if (tableCLS != (UITable *) NULL) tableCLS->Draw ();
	}

static void _RGISEditPointSTNCoordsSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = NULL;
	char *field;
	DBObjectLIST<DBObjTableField> *fields;

	callData = callData;
	if (select == NULL) select = UISelectionCreate ("Field Selection");
	XtVaGetValues (text,XmNuserData, &fields, NULL);
	if ((field = UISelectObject (select,(DBObjectLIST<DBObject> *) fields,DBTableFieldIsFloat)) != NULL)
			XmTextFieldSetString (text,field);
	else	XmTextFieldSetString (text,"");
	}

void RGISEditPointSTNCoordsCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *text;
	static DBInt cont;
	DBDataset *dataset;
	DBObjData *dbData;
	DBObjTable *table;
	DBObjTableField *field = (DBObjTableField *) NULL;
	static Widget dShell = NULL, mainForm, button, textF;

	widget = widget; data = data; callData = callData;
	
	if (dShell == (Widget) NULL)
		{
		XmString string;
		
		dShell = UIDialogForm ("Area Field",false);
		mainForm = UIDialogFormGetMainForm (dShell);
	
		string = XmStringCreate ("Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISEditPointSTNBestCoordsButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsCategory,
								NULL);
		XmStringFree (string);
		textF = XtVaCreateManagedWidget ("RGISEditPointSTNBestCoordsTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISEditPointSTNCoordsSelectCBK,textF);
		string = XmStringCreate ("Area Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISEditJoinTableNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			textF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&cont);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),true);
		}
	
	dataset = UIDataset ();
	dbData = dataset->Data ();
	table = dbData->Table (DBrNItems);
	XtVaSetValues (textF,XmNuserData,table->Fields (),NULL);
	UIDialogFormPopup (dShell);
	cont = false;
	while (UILoop ());
		{
		if (strlen (text = XmTextFieldGetString (textF)) > 0)
		field = table->Field (text);
/*		if ((strlen (text = XmTextFieldGetString (textF)) > 0) &&
			 ((field = table->Field (text)) != (DBObjTableField *) NULL))
				XtSetSensitive (UIDialogFormGetOkButton (dShell),True);
		else	XtSetSensitive (UIDialogFormGetOkButton (dShell),False);
*/		XtFree (text);
		}

	UIDialogFormPopdown (dShell);
	if (cont)
		{
		UIPauseDialogOpen ("Moving Points");
		RGlibPointSTNCoordinates (dbData,field);
		UIPauseDialogClose ();
		UI2DViewRedrawAll ();
		}
	}
