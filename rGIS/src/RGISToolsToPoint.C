/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISToolsToPoint.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <rgis.H>

static DBObjectLIST<DBObjTableField> *_RGISToolsConvertToPointFields = (DBObjectLIST<DBObjTableField> *) NULL;

static void _RGISToolsConvertToPointSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = NULL;
	char *field;
	int (*condFunc) (const DBObject *);

	callData = callData;
	if (select == NULL) select = UISelectionCreate ("Field Selection");
	XtVaGetValues (widget,XmNuserData, &condFunc, NULL);
	if ((field = UISelectObject (select,(DBObjectLIST<DBObject> *) (_RGISToolsConvertToPointFields),condFunc)) != NULL)
		XmTextFieldSetString (text,field);
	}

void RGISToolsConvertToPointCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *nText, *xText, *yText, *selection;
	int allowOk;
	static int convert;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	DBObjTable *itemTable;
	static Widget tableSelect = (Widget) NULL;
	static Widget dShell = (Widget) NULL, mainForm;
	static Widget nameTextF, xCoordTextF, yCoordTextF;
	XmString string;

	widget = widget;	callData = callData;
	if (tableSelect == (Widget) NULL)	tableSelect = UISelectionCreate ("Table Selection");
	selection = UISelectObject (tableSelect,(DBObjectLIST<DBObject> *) dbData->Tables ());
	if (selection == (char *) NULL) return;
	if ((itemTable = dbData->Table (selection)) == (DBObjTable *) NULL)
		{ fprintf (stderr,"Invalid Table in: RGISToolsConvertToPointCBK ()\n"); return; }
	
	_RGISToolsConvertToPointFields = itemTable->Fields ();
	if (dShell == (Widget) NULL)
		{
		Widget button;

		dShell = UIDialogForm ("Convert Table To Point",false);
		mainForm = UIDialogFormGetMainForm (dShell);
		
		string = XmStringCreate ("Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISToolsConvertToPointNameButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsString,
								NULL);
		XmStringFree (string);
		nameTextF = XtVaCreateManagedWidget ("RGISToolsConvertToPointNameTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISToolsConvertToPointSelectCBK,nameTextF);
		string = XmStringCreate ("Name Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISToolsConvertToPointNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			nameTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		
		string = XmStringCreate ("Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISToolsConvertToPointXCoordButton",xmPushButtonWidgetClass,mainForm,
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
		xCoordTextF = XtVaCreateManagedWidget ("RGISToolsConvertToPointXCoordTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISToolsConvertToPointSelectCBK,xCoordTextF);
		string = XmStringCreate ("X Coordinate:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISToolsConvertToPointXCoordLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			nameTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ("Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISToolsConvertToPointYCoordButton",xmPushButtonWidgetClass,mainForm,
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
								XmNuserData,				DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		yCoordTextF = XtVaCreateManagedWidget ("RGISToolsConvertToPointYCoordTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISToolsConvertToPointSelectCBK,yCoordTextF);
		string = XmStringCreate ("Y Coordinate:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISToolsConvertToPointYCoordLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			nameTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&convert);
		}
	convert = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		xText = XmTextFieldGetString (xCoordTextF);
		yText = XmTextFieldGetString (yCoordTextF);
		allowOk = (strlen (xText) > 0) && (strlen (yText) > 0);
		XtFree (xText);	XtFree (yText);	
		XtSetSensitive (UIDialogFormGetOkButton (dShell),allowOk);
		}
	UIDialogFormPopdown (dShell);
	
	if (convert)
		{
		DBObjData *pntData = new DBObjData ("",DBTypeVectorPoint);
		
		pntData->Name (dbData->Name ());
		pntData->Document (DBDocSubject,dbData->Document (DBDocSubject));
		pntData->Document (DBDocGeoDomain,dbData->Document (DBDocGeoDomain));

		if (UIDataHeaderForm (pntData))
			{
			DBCoordinate coord;
			DBObjTableField *nField, *xField, *yField, *tblFLD, *pntFLD;
			DBObjRecord *tblRec, *pntRec, *symRec;
			DBObjTable *pntTable = pntData->Table (DBrNItems);
			DBObjTable *symbols	= pntData->Table (DBrNSymbols);
			DBObjTableField *symbolIDFLD = symbols->Field (DBrNSymbolID);
			DBObjTableField *fgFLD = symbols->Field (DBrNForeground);
			DBObjTableField *bgFLD = symbols->Field (DBrNBackground);
			DBObjTableField *stFLD = symbols->Field (DBrNStyle);
			DBObjTableField *coordFLD = pntTable->Field (DBrNCoord);
			DBObjTableField *symbolFLD = pntTable->Field (DBrNSymbol);
			DBRegion extent;

			symRec = symbols->Add ("Default Symbol");
			symbolIDFLD->Int (symRec,0);
			fgFLD->Int (symRec,1);
			bgFLD->Int (symRec,2);
			stFLD->Int (symRec,0);

			nText = XmTextFieldGetString (nameTextF);
			xText = XmTextFieldGetString (xCoordTextF);
			yText = XmTextFieldGetString (yCoordTextF);
			nField = _RGISToolsConvertToPointFields->Item (nText);
			xField = _RGISToolsConvertToPointFields->Item (xText);
			yField = _RGISToolsConvertToPointFields->Item (yText);
			XtFree (nText);	XtFree (xText);	XtFree (yText);
			for (tblRec = itemTable->First ();tblRec != (DBObjRecord *) NULL;tblRec = itemTable->Next ())
				{
				if (CMmathEqualValues (coord.X = xField->Float (tblRec),xField->FloatNoData ())) continue;
				if (CMmathEqualValues (coord.Y = yField->Float (tblRec),yField->FloatNoData ())) continue;
				if (nField == (DBObjTableField *) NULL) pntRec = pntTable->Add (tblRec->Name ());
				else pntRec = pntTable->Add (nField->String (tblRec));
				coordFLD->Coordinate (pntRec,coord);
				symbolFLD->Record (pntRec,symRec);
				extent.Expand (coord);
				}
			pntData->Extent (extent);
         pntData->Projection (DBMathGuessProjection (extent));
         pntData->Precision  (DBMathGuessPrecision  (extent));
			for (tblFLD = _RGISToolsConvertToPointFields->First ();tblFLD != (DBObjTableField *) NULL;tblFLD = _RGISToolsConvertToPointFields->Next ())
				if ((tblFLD != nField) && (tblFLD != xField) && (tblFLD != yField) && DBTableFieldIsVisible (tblFLD))
					{
					pntTable->AddField (pntFLD = new DBObjTableField (*tblFLD));
					pntFLD->Required (false);
					pntRec = pntTable->First ();
					for (tblRec = itemTable->First ();tblRec != (DBObjRecord *) NULL;tblRec = itemTable->Next ())
						{
						if (CMmathEqualValues (coord.X = xField->Float (tblRec),xField->FloatNoData ())) continue;
						if (CMmathEqualValues (coord.Y = yField->Float (tblRec),yField->FloatNoData ())) continue;
						switch (tblFLD->Type ())
							{
							case DBTableFieldString:	pntFLD->String	(pntRec,tblFLD->String (tblRec));	break;
							case DBTableFieldInt:		pntFLD->Int		(pntRec,tblFLD->Int (tblRec));		break;
							case DBTableFieldFloat:		pntFLD->Float	(pntRec,tblFLD->Float (tblRec));		break;
							case DBTableFieldDate:		pntFLD->Date	(pntRec,tblFLD->Date (tblRec));		break;
							default: fprintf (stderr,"Invalid Field Type in: RGISToolsConvertToPointCBK ()\n");		break;
							}
						pntRec = pntTable->Next ();
						}
					}
			workspace->CurrentData (pntData);
			}
		else delete pntData;
		}
	}

