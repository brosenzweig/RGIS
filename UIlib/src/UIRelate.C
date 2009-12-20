/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

UIRelate.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/TextF.h>
#include <UI.H>

#define UIRelateNameTextF 			"*UIRelateNameTextField"
#define UIRelateDataTextF 			"*UIRelateDataTextField"
#define UIRelateFieldTextF			"*UIRelateFieldTextField"
#define UIRelateJoinFieldTextF	"*UIRelateJoinFieldTextField"
#define UIRelateJoinFieldButton	"*UIRelateJoinFieldButton"
#define UIRelateField 		0x00
#define UIRelateJoinField	0x01

class UIRelate

	{
	private:
		DBObjData  *PDataPTR;
		DBObjData  *RDataPTR;
		DBObjTable *RelateTBL;
		DBObjTableField *RelDataFLD;
		DBObjTableField *RelateFLD;
		DBObjTableField *JoinFLD;
	public:
		char RelateNameSTR	[DBStringLength];
		char DataNameSTR		[DBDataNameLen];
		char RelateFieldSTR	[DBStringLength];
		char JoinFieldSTR		[DBStringLength];
		void Initialize (DBObjData *data,DBObjRecord *relRec)
			{
			PDataPTR = data;
			RelateTBL	= data->Table (DBrNRelations);
			RelDataFLD	= RelateTBL->Field (DBrNRelateData);
			RelateFLD	= RelateTBL->Field (DBrNRelateField);
			JoinFLD		= RelateTBL->Field (DBrNRelateJoinField);
			if (relRec == (DBObjRecord *) NULL)
				{
				strcpy (RelateNameSTR,"");
				strcpy (DataNameSTR,"");
				strcpy (RelateFieldSTR,"");
				strcpy (JoinFieldSTR,"");
				}
			else
				{
				strncpy (RelateNameSTR,	relRec->Name (),					strlen (RelateNameSTR) - 1);
				strncpy (DataNameSTR,	RelDataFLD->String (relRec),	strlen (DataNameSTR) - 1);
				strncpy (RelateFieldSTR,RelateFLD->String (relRec),	strlen (RelateFieldSTR) - 1);
				strncpy (JoinFieldSTR,	JoinFLD->String (relRec),		strlen (JoinFieldSTR) - 1);
				}
			}
		DBObjData *PData () { return (PDataPTR); }
		void PData (DBObjData *data) { PDataPTR = data; }
		DBObjData *RData () { return (RDataPTR); }
		void RData (DBObjData *data) { RDataPTR = data; }
		DBInt IsSet () const
			{
			if (strlen (RelateNameSTR) < 1)	return (false);
			if (strlen (DataNameSTR) < 1)		return (false);
			if (strlen (RelateFieldSTR) < 1)	return (false);
			if (strlen (JoinFieldSTR) < 1)	return (false);
			return (true);
			}
		DBInt IsChanged (DBObjRecord *relRec) const
			{
			if (relRec == (DBObjRecord *) NULL) return (true);
			if (strcmp (RelateNameSTR,	relRec->Name ()) != 0) return (true);
			if (strcmp (DataNameSTR,	RelDataFLD->String (relRec))	!= 0) return (true);
			if (strcmp (RelateFieldSTR,RelateFLD->String (relRec)) 	!= 0) return (true);
			if (strcmp (JoinFieldSTR,	JoinFLD->String (relRec))		!= 0) return (true);
			return (false);
			}
		DBInt Save (DBObjRecord *relRec)
			{
			if (relRec == (DBObjRecord *) NULL)	relRec = RelateTBL->Add ();
			if (relRec == (DBObjRecord *) NULL) return (DBFault);
			relRec->Name (RelateNameSTR);
			RelDataFLD->String (relRec,DataNameSTR);
			RelateFLD->String (relRec,RelateFieldSTR);
			JoinFLD->String (relRec,JoinFieldSTR);
			return (DBSuccess);
			}
	};

static void _UIRelateNameChangedCBK (Widget widget,char *nameSTR,XmAnyCallbackStruct *callData)

	{
	char *text = XmTextFieldGetString (widget);
	int length;
	callData = callData;

	XtVaGetValues (widget, XmNuserData, 	&length, NULL);
	strncpy (nameSTR,text,length - 1);
	XtFree (text);
	}

static void _UIRelateDataSelectButtonCBK (Widget widget,Widget dataField,XmAnyCallbackStruct *callData)

	{
	UIRelate *relateCLS;
	DBObjData *relData;

	callData = callData;
	XtVaGetValues (widget, XmNuserData, 	&relateCLS, NULL);

	relData = UIDatasetSelectData (relateCLS->PData ());
	if (relData != (DBObjData *) NULL)
		{
		XmTextFieldSetString (dataField,relData->Name ());
		relateCLS->RData (relData);
		XtSetSensitive (XtNameToWidget (XtParent (widget),UIRelateJoinFieldButton),true);
		}
	else
		{
		XmTextFieldSetString (dataField,(char *) "");
		relateCLS->RData ((DBObjData *) NULL);
		XtSetSensitive (XtNameToWidget (XtParent (widget),UIRelateJoinFieldButton),false);
		}
	}

static void _UIRelateFieldSelectButtonCBK (Widget widget,DBInt fieldCode,XmAnyCallbackStruct *callData)

	{
	char *selection, *fieldSTR;
	DBInt fieldType;
	DBObjData *data;
	DBObjTable *itemTBL;
	DBObjTableField *field;
	UIRelate *relateCLS;
	static Widget selectField = (Widget) NULL;
	Widget fieldWidget;

	widget = widget; callData = callData;
	XtVaGetValues (widget, XmNuserData, 	&relateCLS, NULL);

	if (selectField == (Widget) NULL) selectField = UISelectionCreate ((char *) "Select Field");
	switch (fieldCode)
		{
		case UIRelateField:
			if (strlen (relateCLS->JoinFieldSTR) > 0)
				{
				data = relateCLS->RData ();
				itemTBL = data->Table (DBrNItems);
				field = itemTBL->Field (relateCLS->JoinFieldSTR);
				fieldType = field->Type ();
				}
			else	fieldType = DBFault;
			data = relateCLS->PData ();
			itemTBL = data->Table (DBrNItems);
			fieldSTR = relateCLS->RelateFieldSTR;
			fieldWidget = XtNameToWidget (XtParent (widget),UIRelateFieldTextF);
			break;
		case UIRelateJoinField:
			if (strlen (relateCLS->RelateFieldSTR) > 0)
				{
				data = relateCLS->PData ();
				itemTBL = data->Table (DBrNItems);
				field = itemTBL->Field (relateCLS->RelateFieldSTR);
				fieldType = field->Type ();
				}
			else	fieldType = DBFault;
			data = relateCLS->RData ();
			itemTBL = data->Table (DBrNItems);
			fieldSTR = relateCLS->JoinFieldSTR;
			fieldWidget = XtNameToWidget (XtParent (widget),UIRelateJoinFieldTextF);
			break;
		}
	switch (fieldType)
		{
		case DBTableFieldString:
			selection = UISelectObject (selectField,(DBObjectLIST<DBObject> *) itemTBL->Fields (),DBTableFieldIsString);	break;
		case DBTableFieldInt:
			selection = UISelectObject (selectField,(DBObjectLIST<DBObject> *) itemTBL->Fields (),DBTableFieldIsInteger);	break;
		default:
			selection = UISelectObject (selectField,(DBObjectLIST<DBObject> *) itemTBL->Fields (),DBTableFieldIsCategory);	break;
		}
	if (selection != (char *) NULL)
			strncpy (fieldSTR,selection,DBStringLength);
	else	strcpy (fieldSTR,"");
	XmTextFieldSetString (fieldWidget,fieldSTR);
	}

DBInt UIRelateData (DBObjData *data, DBObjRecord *relRec)

	{
	static int save;
	static Widget dShell, mainForm = (Widget) NULL;
	static UIRelate relateCLS;

	if (data == (DBObjData *) NULL) return (DBFault);

	if (dShell == (Widget) NULL)
		{
		Widget fieldTextF, dataField, button;
		XmString string;

		dShell = UIDialogForm ((char *) "Relates");
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&save);
		mainForm = UIDialogFormGetMainForm (dShell);

		fieldTextF = XtVaCreateManagedWidget (UIRelateNameTextF + 1,xmTextFieldWidgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_FORM,
														XmNtopOffset,				10,
														XmNleftAttachment,		XmATTACH_POSITION,
														XmNleftPosition,			20,
														XmNmaxLength,				DBStringLength - 1,
														XmNcolumns,					DBStringLength - 1,
														XmNuserData,				DBStringLength,
														NULL);
		XtAddCallback (fieldTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIRelateNameChangedCBK,(XtPointer) relateCLS.RelateNameSTR);
		string = XmStringCreate ((char *) "Relate Name:",UICharSetBold);
		XtVaCreateManagedWidget ("UIRelateNameLabel",xmLabelGadgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				fieldTextF,
														XmNrightAttachment,		XmATTACH_WIDGET,
														XmNrightWidget,			fieldTextF,
														XmNrightOffset,			5,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			fieldTextF,
														XmNlabelString,			string,
														NULL);
		XmStringFree (string);


		dataField = XtVaCreateManagedWidget (UIRelateDataTextF + 1,xmTextFieldWidgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_WIDGET,
														XmNtopWidget,				fieldTextF,
														XmNtopOffset,				5,
														XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNleftWidget,				fieldTextF,
														XmNmaxLength,				DBDataNameLen - 1,
														XmNcolumns,					DBDataNameLen - 1,
														XmNuserData,				DBDataNameLen,
														NULL);
		XtAddCallback (dataField,XmNvalueChangedCallback,(XtCallbackProc) _UIRelateNameChangedCBK,(XtPointer) relateCLS.DataNameSTR);
		string = XmStringCreate ((char *) "Relate Data:",UICharSetBold);
		XtVaCreateManagedWidget ("UIRelateNameLabel",xmLabelGadgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				dataField,
														XmNrightAttachment,		XmATTACH_WIDGET,
														XmNrightWidget,			fieldTextF,
														XmNrightOffset,			5,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			dataField,
														XmNlabelString,			string,
														NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("UIRelateJoinFieldSelectButton",xmPushButtonGadgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				dataField,
														XmNleftAttachment,		XmATTACH_WIDGET,
														XmNleftWidget,				dataField,
														XmNleftOffset,				10,
														XmNrightAttachment,		XmATTACH_FORM,
														XmNrightOffset,			15,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			dataField,
														XmNtraversalOn,			false,
														XmNlabelString,			string,
														XmNuserData,				&relateCLS,
														NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIRelateDataSelectButtonCBK,(XtPointer) dataField);

		fieldTextF = XtVaCreateManagedWidget (UIRelateFieldTextF + 1,xmTextFieldWidgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_WIDGET,
														XmNtopWidget,				dataField,
														XmNtopOffset,				5,
														XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNleftWidget,				dataField,
														XmNbottomAttachment,		XmATTACH_FORM,
														XmNbottomOffset,			10,
														XmNmaxLength,				DBStringLength - 1,
														XmNcolumns,					DBStringLength * 2 / 3,
														XmNuserData,				DBStringLength,
														NULL);
		XtAddCallback (fieldTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIRelateNameChangedCBK,(XtPointer) relateCLS.RelateFieldSTR);
		string = XmStringCreate ((char *) "Relate Field:",UICharSetBold);
		XtVaCreateManagedWidget ("UIRelateFieldLabel",xmLabelGadgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				fieldTextF,
														XmNrightAttachment,		XmATTACH_WIDGET,
														XmNrightWidget,			fieldTextF,
														XmNrightOffset,			5,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			fieldTextF,
														XmNlabelString,			string,
														NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("UIRelateFieldSelectButton",xmPushButtonGadgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				fieldTextF,
														XmNleftAttachment,		XmATTACH_WIDGET,
														XmNleftWidget,				fieldTextF,
														XmNleftOffset,				5,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			fieldTextF,
														XmNtraversalOn,			false,
														XmNlabelString,			string,
														XmNuserData,				&relateCLS,
														NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIRelateFieldSelectButtonCBK,(XtPointer) UIRelateField);
		fieldTextF = XtVaCreateManagedWidget (UIRelateJoinFieldTextF + 1,xmTextFieldWidgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				fieldTextF,
														XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNrightWidget,			dataField,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			fieldTextF,
														XmNmaxLength,				DBStringLength - 1,
														XmNcolumns,					DBStringLength * 2 / 3,
														XmNuserData,				DBStringLength,
														NULL);
		XtAddCallback (fieldTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIRelateNameChangedCBK,(XtPointer) relateCLS.JoinFieldSTR);

		string = XmStringCreate ((char *) "Join Field:",UICharSetBold);
		XtVaCreateManagedWidget ("UIRelateJoinFieldLabel",xmLabelGadgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				fieldTextF,
														XmNrightAttachment,		XmATTACH_WIDGET,
														XmNrightWidget,			fieldTextF,
														XmNrightOffset,			5,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			fieldTextF,
														XmNlabelString,			string,
														NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget (UIRelateJoinFieldButton + 1,xmPushButtonGadgetClass,mainForm,
														XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
														XmNtopWidget,				fieldTextF,
														XmNleftAttachment,		XmATTACH_WIDGET,
														XmNleftWidget,				fieldTextF,
														XmNleftOffset,				10,
														XmNrightAttachment,		XmATTACH_FORM,
														XmNrightOffset,			15,
														XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
														XmNbottomWidget,			fieldTextF,
														XmNtraversalOn,			false,
														XmNlabelString,			string,
														XmNuserData,				&relateCLS,
														NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIRelateFieldSelectButtonCBK,(XtPointer) UIRelateJoinField);
		XtSetSensitive (button,false);
		}

	relateCLS.Initialize (data,relRec);
	XmTextFieldSetString (XtNameToWidget (mainForm,UIRelateNameTextF),relateCLS.RelateNameSTR);
	XmTextFieldSetString (XtNameToWidget (mainForm,UIRelateDataTextF),relateCLS.DataNameSTR);
	XmTextFieldSetString (XtNameToWidget (mainForm,UIRelateFieldTextF),relateCLS.RelateFieldSTR);
	XmTextFieldSetString (XtNameToWidget (mainForm,UIRelateJoinFieldTextF),relateCLS.JoinFieldSTR);

	UIDialogFormPopup (dShell);
	while (UILoop ())
		XtSetSensitive (UIDialogFormGetOkButton (dShell),relateCLS.IsSet () && relateCLS.IsChanged (relRec));
	UIDialogFormPopdown (dShell);
	if (save) return (relateCLS.Save (relRec));
	return (DBSuccess);
	}
