/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UIAttribView.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/Protocols.h>
#include <UI.H>

void UIAttribView::NewField (char *fieldName,const char *format)

	{
	char formatSTR [12];
	DBInt textLen;
	Dimension height;
	XmString string;
	Widget label;

	strncpy (formatSTR,format,sizeof (formatSTR));
	if (formatSTR [1] == 's') textLen = DBStringLength;
	else
		{
		for (textLen = 1;(formatSTR [textLen] >= '0') && (formatSTR [textLen] <= '9');++textLen);
		formatSTR [textLen] = '\0';
		sscanf (formatSTR + 1,"%d",&textLen);
		textLen = textLen < DBStringLength ? textLen : DBStringLength;
		}
	label = XtVaCreateManagedWidget (fieldName,xmRowColumnWidgetClass,FieldsRowCol,
								XmNorientation,			XmVERTICAL,
								XmNrowColumnType,			XmWORK_AREA,
								XmNpacking,					XmPACK_NONE,
								XmNspacing,					0,
								NULL);
	label = XtVaCreateManagedWidget ("UIAttribViewTextField",xmTextFieldWidgetClass,label,
								XmNfontList,				UIFixedFontList (),
								XmNcolumns,					textLen,
								NULL);
	XtVaGetValues (label,XmNheight, &height, NULL);
	string = XmStringCreate (fieldName,UICharSetBold);
	label = XtVaCreateManagedWidget (fieldName,xmLabelWidgetClass,NamesRowCol,
								XmNlabelString,			string,
								XmNheight,					height,
								XmNrecomputeSize,			False,
								NULL);
	XmStringFree (string);
	}

static void _UIAttribViewDeleteCBK (Widget widget,UIAttribView *view,XmAnyCallbackStruct *callData)

	{
	DBObjData *dbData = view->Data ();
	
	widget = widget; callData = callData;
	dbData->DispRemove (view);
	delete view;
	}

UIAttribView::UIAttribView (DBObjData *data) : DBObject (data->Name (),sizeof (UIAttribView))

	{
	DBObjTableField *field;
	DBObjectLIST<DBObjTableField> *fields;
	Widget mainForm, scrolledW, rowCol, label;
	XmString string;
	Atom deleteWindowAtom = XmInternAtom(XtDisplay(UITopLevel ()),"WM_DELETE_WINDOW",FALSE);

	DataPTR = data;
	ItemTable = DataPTR->Table (DBrNItems);
	Name (UIAttribViewSTR);
	DShellWGT = XtVaCreatePopupShell ("UIAttribView",xmDialogShellWidgetClass,UITopLevel (),
								XmNkeyboardFocusPolicy,		XmPOINTER,
								XmNtitle,						data->Name (),
								XmNtransient,					False,
								XmNminWidth,					400,
								XmNminHeight,					300,
								NULL);
	XmAddWMProtocolCallback (DShellWGT,deleteWindowAtom,(XtCallbackProc) _UIAttribViewDeleteCBK,(XtPointer) this); 
	mainForm = XtVaCreateWidget ("UIAttribViewForm",xmFormWidgetClass,DShellWGT,
								XmNshadowThickness,			0,
								NULL);
	string = XmStringCreate ("Item:",UICharSetBold);
	label = XtVaCreateManagedWidget ("UIAttribViewNameLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNlabelString,			string,
								XmNrecomputeSize,			False,
								NULL);
	XmStringFree (string);
	string = XmStringCreate ("",UICharSetNormal);
	ItemNameWGT = XtVaCreateManagedWidget ("UIAttribViewNameText",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				label,
								XmNleftOffset,				5,
								XmNlabelString,			string,
								NULL);
	XmStringFree (string);
	scrolledW = XtVaCreateManagedWidget ("UIAttribViewScrolledW",xmScrolledWindowWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				label,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				5,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			 5,
								XmNheight,					150,
								XmNvisualPolicy,			XmCONSTANT,
								XmNscrollingPolicy,		XmAUTOMATIC,
								NULL);
	rowCol = XtVaCreateManagedWidget ("UIAttribViewRowColumn",xmRowColumnWidgetClass,scrolledW,
								XmNnumColumns,				2,
								XmNorientation,			XmHORIZONTAL,
								XmNrowColumnType,			XmWORK_AREA,
								NULL);
	NamesRowCol = XtVaCreateManagedWidget ("UIAttribViewNamesRowColumn",xmRowColumnWidgetClass,rowCol,
								XmNorientation,			XmVERTICAL,
								XmNrowColumnType,			XmWORK_AREA,
								XmNpacking,					XmPACK_COLUMN,
								XmNspacing,					0,
								NULL);
	FieldsRowCol = XtVaCreateManagedWidget ("UIAttribViewNamesRowColumn",xmRowColumnWidgetClass,rowCol,
								XmNorientation,			XmVERTICAL,
								XmNrowColumnType,			XmWORK_AREA,
								XmNspacing,					0,
								NULL);
	
	
	switch (DataPTR->Type ())
		{
		case DBTypeVectorPoint:
		case DBTypeVectorLine:
		case DBTypeVectorPolygon:
		case DBTypeGridDiscrete:
			fields = ItemTable->Fields ();
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field) == true)	NewField (field->Name (),field->Format ());
			break;
		case DBTypeGridContinuous:
			{
			DBObjRecord *record;
			DBGridIO *gridIO = new DBGridIO (DataPTR);
			ItemTable = DataPTR->Table (DBrNLayers);
			for (record = ItemTable->First ();record != (DBObjRecord *) NULL;record = ItemTable->Next ())
				NewField (record->Name (),gridIO->ValueFormat ());
			delete gridIO;
			} break;
		case DBTypeNetwork:	
			{
			DBObjTable *cellTable = DataPTR->Table (DBrNCells);

			NewField ("Basin Name","%s");
			fields = ItemTable->Fields ();
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field) == true)	NewField (field->Name (),field->Format ());

			fields =  cellTable->Fields ();
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field) == true)	NewField (field->Name (),field->Format ());
			} break;
		default:	break;
		}
	XtManageChild (mainForm);
	UILoop ();
	}

void UIAttribView::DrawField (char *fieldName,char *textSTR)

	{
	Widget label;
	label = XtNameToWidget (FieldsRowCol,fieldName);
	if (label != (Widget) NULL)
		{
		label =  XtNameToWidget (label,"UIAttribViewTextField");
		XmTextFieldSetString (label,textSTR);
		}
	}

void UIAttribView::Draw (DBObjRecord *record)

	{
	DBObjTableField *field;
	DBObjectLIST<DBObjTableField> *fields;
	UIXYGraphShell *graphCLS;

	if (record == (DBObjRecord *) NULL)
		{
		Cardinal i, numChildren;
		WidgetList rowCols;

		UIAuxSetLabelString (ItemNameWGT,"");
		XtVaGetValues (FieldsRowCol,XmNchildren,	&rowCols, XmNnumChildren,	&numChildren,NULL);
		for (i = 0;i < numChildren; ++i)
			XmTextFieldSetString (XtNameToWidget (rowCols [i],"UIAttribViewTextField"),"");
		return;
		}
	
	UIAuxSetLabelString (ItemNameWGT,record->Name ());
	switch (DataPTR->Type ())
		{
		case DBTypeVectorPoint:
		case DBTypeVectorLine:
		case DBTypeVectorPolygon:
		case DBTypeGridDiscrete:
			fields = ItemTable->Fields ();
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field) == true)	DrawField (field->Name (),field->String (record));
			break;
		case DBTypeGridContinuous:
			{
			DBCoordinate coord = *((DBCoordinate *) (record->Data ()));
			DBGridIO *gridIO = new DBGridIO (DataPTR);
			for (record = ItemTable->First ();record != (DBObjRecord *) NULL;record = ItemTable->Next ())
				DrawField (record->Name (),gridIO->ValueString (record,coord));
			delete gridIO;
			} break;
		case DBTypeNetwork:
			{
			DBObjTable *cellTable = DataPTR->Table (DBrNCells);
			DBObjTableField *basinFLD = cellTable->Field (DBrNBasin);
			DBObjRecord *basinRec;
			
			if ((basinRec =  ItemTable->Item (basinFLD->Int (record) - 1)) == (DBObjRecord *) NULL)
				{ fprintf (stderr,"BasinID Error in: UIAttribView::Draw (DBObjRecord *)\n"); return; }
			DrawField ("Basin Name",basinRec->Name ());
			fields = ItemTable->Fields ();
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field) == true)	DrawField (field->Name (),field->String (basinRec));

			fields = cellTable->Fields ();	
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field) == true)	DrawField (field->Name (),field->String (record));
			} break;
		default:	break;
		}
	if ((graphCLS = (UIXYGraphShell *) DataPTR->Display (UIXYGraphShellStr)) != (UIXYGraphShell *) NULL)
		graphCLS->Configure (record);
	}

void UIAttribView::Raise () const

	{
	XtManageChild (DShellWGT);
	}
