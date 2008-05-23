/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UITableFields.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/TextF.h>
#include <Xm/Scale.h>
#include <Xm/Frame.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <UI.H>

class UITableFieldSubForm : public DBObject

	{
	private:
		Widget SubFormWGT;
		Widget NameWGT;
		Widget TypeMenuWGT, TypeButtonWGTs [4];
		Widget SizeMenuWGT, SizeButtonWGTs [3];
		Widget IdleWGT;
		Widget WidthSWGT, WidthFWGT, WidthLWGT;
		Widget DecimalsLWGT, DecimalsSWGT;
		Widget NodataLWGT, NodataFWGT;
		DBObjTableField *OldFieldPTR;
		DBObjTableField *NewFieldPTR;
	public:
		UITableFieldSubForm ()	{ OldFieldPTR = NewFieldPTR = (DBObjTableField *) NULL; }
		UITableFieldSubForm (DBObjTableField *);
		UITableFieldSubForm (Widget,DBObjTableField *);
		~UITableFieldSubForm ()
			{
			delete NewFieldPTR;
			if (SubFormWGT != (Widget) NULL) XtDestroyWidget (SubFormWGT);
			}
		void Display ();
		void  Type (DBInt type)
			{
			if (NewFieldPTR->Type () == type) return; 
			NewFieldPTR->Type (type);
			if (OldFieldPTR->Type () == type) { NewFieldPTR->Format (OldFieldPTR->Format ()); }
			}
		DBInt Type () { return (NewFieldPTR->Type ()); }
		void Size (DBInt size)
			{
			if (NewFieldPTR->Type () == DBTableFieldFloat)	NewFieldPTR->Length (size << 0x01);
			if (NewFieldPTR->Type () == DBTableFieldInt) 	NewFieldPTR->Length (size);
			}
		void FormatWidth (DBInt width)
			{
			char numberString [DBStringLength];
			switch (NewFieldPTR->Type ())
				{
				case DBTableFieldString:
					width = 0x01 << width; NewFieldPTR->Length (width);	break;
				case DBTableFieldInt:		NewFieldPTR->FormatWidth (width);		break;
				case DBTableFieldFloat:
					NewFieldPTR->FormatWidth (width);
					XmScaleSetValue (DecimalsSWGT,NewFieldPTR->FormatDecimals ());
					XtVaSetValues (DecimalsSWGT, XmNmaximum, width - 3, NULL);
					break;
				}
			sprintf (numberString,"%d",width);
			UIAuxSetLabelString (WidthLWGT,numberString);
			}
		void Nodata (char *string)
			{
			char numberString [DBStringLength];
			switch (NewFieldPTR->Type ())
				{
				case DBTableFieldInt:
					{
					DBInt noData;
					sprintf (numberString,"%d",NewFieldPTR->IntNoData ());
					if (strcmp (numberString,string) == 0)return;
					if (sscanf (string,"%d",&noData) == 1) NewFieldPTR->IntNoData (noData);
					else	XmTextFieldSetString (NodataFWGT,numberString);
					} break;
				case DBTableFieldFloat:
					{
					DBFloat noData;
					sprintf (numberString,"%f",NewFieldPTR->FloatNoData ());
					if (strcmp (numberString,string) == 0)return;
					if (sscanf (string,"%lf",&noData) == 1)	NewFieldPTR->FloatNoData ((DBFloat) noData);
					else	XmTextFieldSetString (NodataFWGT,numberString);
					} break;
				}
			}
		void Update ()
			{
			OldFieldPTR->Name	  (NewFieldPTR->Name ());
			OldFieldPTR->Format (NewFieldPTR->Format ());
			OldFieldPTR->Flags  (NewFieldPTR->Flags ());
			switch (NewFieldPTR->Type ())
				{
				case DBTableFieldInt:	OldFieldPTR->IntNoData		(NewFieldPTR->IntNoData ());		break;
				case DBTableFieldFloat:	OldFieldPTR->FloatNoData	(NewFieldPTR->FloatNoData ());	break;
				}
			}
		DBObjTableField *NewField () { return (NewFieldPTR); }
		DBObjTableField *OldField () { return (OldFieldPTR); }
		void OrigField (DBObjectLIST<DBObjTableField> *fields)
			{ OldFieldPTR = fields->Item (OldFieldPTR->Name ()); }
		DBInt StructureIsChanged ()
			{
			if (SubFormWGT == (Widget) NULL) return (false);
			if (OldFieldPTR->Type () 	!= NewFieldPTR->Type ())	return (true);
			if (OldFieldPTR->Length ()	!= NewFieldPTR->Length ())	return (true);
			return (false);
			}
		DBInt FormatIsChanged ()
			{
			char nodataString0 [DBStringLength], nodataString1 [DBStringLength];

			if (SubFormWGT == (Widget) NULL) return (false);
			if (strcmp (OldFieldPTR->Name (),	NewFieldPTR->Name ()))		return (true);
			if (strcmp (OldFieldPTR->Format (),	NewFieldPTR->Format ()))	return (true);
			if (OldFieldPTR->Flags () != NewFieldPTR->Flags ())				return (true);
			switch (NewFieldPTR->Type ())
				{
				case DBTableFieldInt:
					sprintf (nodataString0,"%d",OldFieldPTR->IntNoData ());
					sprintf (nodataString1,"%d",NewFieldPTR->IntNoData ());
					break;
				case DBTableFieldFloat:
					sprintf (nodataString0,"%f",OldFieldPTR->FloatNoData ());
					sprintf (nodataString1,"%f",NewFieldPTR->FloatNoData ());
					break;
				default:		return (false);
				}
			if (strcmp (nodataString0,nodataString1)) return (true);
			return (false);
			}
		DBInt IsChanged ()
			{
			if (StructureIsChanged ())	return (true);
			if (FormatIsChanged ())		return (true);
			return (false);
			}
	};

static void _UITableFieldSubFormNameCBK (Widget widget,DBObjTableField *field,XmTextVerifyCallbackStruct *callData)

	{
	char *string;

	if (callData->reason != XmCR_VALUE_CHANGED) return;
	if ((string = XmTextFieldGetString (widget)) != (char *) NULL)
		{ field->Name (string); XtFree (string); }
	}

static void _UITableFieldSubFormTypeButtonCBK (Widget widget,DBInt type, XmAnyCallbackStruct *callData)

	{
	UITableFieldSubForm *subForm;

	callData = callData;
	XtVaGetValues (widget, XmNuserData, &subForm, NULL);
	subForm->Type (type);
	subForm->Display ();
	}

static void _UITableFieldSubFormSizeButtonCBK (Widget widget,DBInt size, XmAnyCallbackStruct *callData)

	{
	UITableFieldSubForm *subForm;
	
	callData = callData;
	XtVaGetValues (widget, XmNuserData, &subForm, NULL);
	subForm->Size (size);
	}

static void UITableFieldSubFormIdleToggleCBK (Widget widget,DBObjTableField *field,XmToggleButtonCallbackStruct *callData)

	{
	widget = widget;
	field->Flags (DBObjectFlagIdle,callData->set ? DBSet : DBClear);
	}

static void _UITableFieldSubFormWidthScaleCBK (Widget widget,UITableFieldSubForm *subForm,XmScaleCallbackStruct *callData)

	{
	widget = widget; callData = callData;
	subForm->FormatWidth (callData->value);
	}

static void _UITableFieldSubFormDecimalScaleCBK (Widget widget,DBObjTableField *field,XmScaleCallbackStruct *callData)

	{
	widget = widget; callData = callData;
	if (field->Type () != DBTableFieldFloat) return;
	field->FormatDecimals (callData->value);
	}

static void _UITableFieldSubFormNodataCBK (Widget widget,UITableFieldSubForm *subForm,XmTextVerifyCallbackStruct *callData)

	{
	char *string;

	if (callData->reason != XmCR_LOSING_FOCUS) return;
	if ((string = XmTextFieldGetString (widget)) != (char *) NULL)
		{ subForm->Nodata (string); XtFree (string); }
	}

UITableFieldSubForm::UITableFieldSubForm (DBObjTableField *field)

	{
	OldFieldPTR = NewFieldPTR = field;
	NewFieldPTR = new DBObjTableField (*field);
	SubFormWGT = (Widget) NULL;
	}

UITableFieldSubForm::UITableFieldSubForm (Widget parent,DBObjTableField *field)
	
	{
	int i;
	XmString string;
			
	OldFieldPTR = NewFieldPTR = field;
	NewFieldPTR = new DBObjTableField (*field);
	SubFormWGT = XtVaCreateManagedWidget ("UITableFieldSubForm",xmFormWidgetClass,parent,
								XmNshadowThickness,		1,
								NULL);
	SizeMenuWGT = XmCreatePulldownMenu (SubFormWGT,"UITableFieldSubFormSizeMenu",NULL,0);
	string = XmStringCreate ("Byte",UICharSetNormal);
	SizeButtonWGTs [0] = XtVaCreateManagedWidget ("UITableFieldSubFormSizeByteButton",xmPushButtonGadgetClass,SizeMenuWGT,
								XmNlabelString,			string,
								XmNuserData,				this,
								NULL);
	XmStringFree (string);
	XtAddCallback (SizeButtonWGTs [0],XmNactivateCallback,(XtCallbackProc) _UITableFieldSubFormSizeButtonCBK,(XtPointer) sizeof (DBByte));
	string = XmStringCreate ("Short",UICharSetNormal);
	SizeButtonWGTs [1] = XtVaCreateManagedWidget ("UITableFieldSubFormSizeShortButton",xmPushButtonGadgetClass,SizeMenuWGT,
								XmNlabelString,			string,
								XmNuserData,				this,
								NULL);
	XmStringFree (string);
	XtAddCallback (SizeButtonWGTs [1],XmNactivateCallback,(XtCallbackProc) _UITableFieldSubFormSizeButtonCBK,(XtPointer) sizeof (DBShort));
	string = XmStringCreate ("Long",UICharSetNormal);
	SizeButtonWGTs [2] = XtVaCreateManagedWidget ("UITableFieldSubFormSizeLongButton",xmPushButtonGadgetClass,SizeMenuWGT,
								XmNlabelString,			string,
								XmNuserData,				this,
								NULL);
	XmStringFree (string);
	XtAddCallback (SizeButtonWGTs [2],XmNactivateCallback,(XtCallbackProc) _UITableFieldSubFormSizeButtonCBK,(XtPointer) sizeof (DBInt));
	string = XmStringCreate ("Size:",UICharSetBold);
	SizeMenuWGT = XtVaCreateManagedWidget ("UITableFieldSubFormSizeOptionMenu",xmRowColumnWidgetClass,SubFormWGT,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				15,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNsubMenuId,				SizeMenuWGT,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			false,
								NULL);
	XmStringFree (string);
	TypeMenuWGT = XmCreatePulldownMenu (SubFormWGT,"UITableFieldSubFormTypeMenu",NULL,0);
	string = XmStringCreate ("Character",UICharSetNormal);
	TypeButtonWGTs [0] = XtVaCreateManagedWidget ("UITableFieldSubFormCharButton",xmPushButtonGadgetClass,TypeMenuWGT,
								XmNlabelString,			string,
								XmNuserData,				this,
								NULL);
	XmStringFree (string);
	XtAddCallback (TypeButtonWGTs [0],XmNactivateCallback,(XtCallbackProc) _UITableFieldSubFormTypeButtonCBK,(XtPointer) DBTableFieldString);
	string = XmStringCreate ("Integer",UICharSetNormal);
	TypeButtonWGTs [1] = XtVaCreateManagedWidget ("UITableFieldSubFormIntButton",xmPushButtonGadgetClass,TypeMenuWGT,
								XmNlabelString,			string,
								XmNuserData,				this,
								NULL);
	XmStringFree (string);
	XtAddCallback (TypeButtonWGTs [1],XmNactivateCallback,(XtCallbackProc) _UITableFieldSubFormTypeButtonCBK,(XtPointer) DBTableFieldInt);
	string = XmStringCreate ("Float",UICharSetNormal);
	TypeButtonWGTs [2] = XtVaCreateManagedWidget ("UITableFieldSubFormFloatButton",xmPushButtonGadgetClass,TypeMenuWGT,
								XmNlabelString,			string,
								XmNuserData,				this,
								NULL);
	XmStringFree (string);
	XtAddCallback (TypeButtonWGTs [2],XmNactivateCallback,(XtCallbackProc) _UITableFieldSubFormTypeButtonCBK,(XtPointer) DBTableFieldFloat);
	string = XmStringCreate ("Date",UICharSetNormal);
	TypeButtonWGTs [3] = XtVaCreateManagedWidget ("UITableFieldSubFormDateButton",xmPushButtonGadgetClass,TypeMenuWGT,
								XmNlabelString,			string,
								XmNuserData,				this,
								NULL);
	XmStringFree (string);
	XtAddCallback (TypeButtonWGTs [3],XmNactivateCallback,(XtCallbackProc) _UITableFieldSubFormTypeButtonCBK,(XtPointer) DBTableFieldDate);
	string = XmStringCreate ("Type:",UICharSetBold);
	TypeMenuWGT = XtVaCreateManagedWidget ("UITableFieldEditTypeOptionMenu",xmRowColumnWidgetClass,SubFormWGT,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				SizeMenuWGT,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			SizeMenuWGT,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			SizeMenuWGT,
								XmNsubMenuId,				TypeMenuWGT,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			false,
								NULL);
	XmStringFree (string);
	NameWGT = XtVaCreateManagedWidget ("UITableFieldSubFormNameField",xmTextFieldWidgetClass,SubFormWGT,
								XmNtopAttachment, 		XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				SizeMenuWGT,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			TypeMenuWGT,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			SizeMenuWGT,
								XmNmaxLength,				DBStringLength,
								XmNcolumns,					DBStringLength,
								XmNtraversalOn,			true,
								NULL);
	string = XmStringCreate ("Name:",UICharSetBold);
	XtAddCallback (NameWGT,XmNvalueChangedCallback,(XtCallbackProc) _UITableFieldSubFormNameCBK,(XtPointer) NewFieldPTR);
	XtVaCreateManagedWidget ("UITableFieldSubFormNameLabel",xmLabelGadgetClass,SubFormWGT,
								XmNtopAttachment, 		XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				SizeMenuWGT,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			NameWGT,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			SizeMenuWGT,
								XmNlabelString,			string,
								NULL);
	XmStringFree (string);

	string = XmStringCreate ("Width:",UICharSetBold);
	XtVaCreateManagedWidget ("UITableFieldSubFormWidthLabel",xmLabelGadgetClass,SubFormWGT,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				NameWGT,
								XmNtopOffset,				20,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNlabelString,			string,
								NULL);
	XmStringFree (string);
	WidthSWGT = XtVaCreateManagedWidget ("UITableFieldSubFormWidthScale",xmScaleWidgetClass,SubFormWGT,
								XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNleftWidget,				NameWGT,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNorientation,			XmHORIZONTAL,
								XmNminimum,					0,
								XmNmaximum,					9,
								XmNvalue,					6,
								XmNscaleWidth,				60,
								XmNtraversalOn,			false,
								NULL);
	XtAddCallback (WidthSWGT,XmNdragCallback,(XtCallbackProc) _UITableFieldSubFormWidthScaleCBK,(XtPointer) this);
	WidthFWGT = XtVaCreateManagedWidget ("UITableFieldSubFormWidthFrame",xmFrameWidgetClass,SubFormWGT,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				WidthSWGT,
								XmNleftOffset,				5,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								NULL);
	string = XmStringCreate ("data",UICharSetNormal);
	WidthLWGT = XtVaCreateManagedWidget ("UITableFieldSubFormWidthLabelF",xmLabelWidgetClass,WidthFWGT,
								XmNmarginWidth,			5,
								XmNalignment,				XmALIGNMENT_END,
								XmNlabelString,			string,
								XmNrecomputeSize,			false,
								NULL);
	XmStringFree (string);
	string = XmStringCreate ("Decimals:",UICharSetBold);
	DecimalsLWGT = XtVaCreateManagedWidget ("UITableFieldSubFormDecimalsLabel",xmLabelWidgetClass,SubFormWGT,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				WidthFWGT,
								XmNleftOffset,				10,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNlabelString,			string,
								NULL);
	XmStringFree (string);
	DecimalsSWGT = XtVaCreateManagedWidget ("UITableFieldSubFormDecimalsScale",xmScaleWidgetClass,SubFormWGT,
								XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNrightWidget,			NameWGT,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNorientation,			XmHORIZONTAL,
								XmNminimum,					0,
								XmNmaximum,					9,
								XmNvalue,					3,
								XmNshowValue,				true,
								XmNscaleWidth,				60,
								XmNtraversalOn,			false,
								NULL);
	XtAddCallback (DecimalsSWGT,XmNdragCallback,(XtCallbackProc) _UITableFieldSubFormDecimalScaleCBK,(XtPointer) NewFieldPTR);
	string = XmStringCreate ("Idle",UICharSetBold);
	IdleWGT = XtVaCreateManagedWidget ("UITableRedefineFieldNameFieldToggle",xmToggleButtonGadgetClass,SubFormWGT,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				NameWGT,
								XmNleftOffset,				30,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNlabelString,			string,
								XmNset,						(NewFieldPTR->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle,
								XmNshadowThickness,		0,
								XmNmarginHeight,			0,
								XmNtraversalOn,			false,
								NULL);
	XtAddCallback (IdleWGT,XmNvalueChangedCallback,(XtCallbackProc) UITableFieldSubFormIdleToggleCBK,(XtPointer) NewFieldPTR);
	XmStringFree (string);
	NodataFWGT = XtVaCreateWidget ("UITableFieldSubFormNodataField",xmTextFieldWidgetClass,SubFormWGT,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNmaxLength,				10,
								XmNcolumns,					10,
								XmNtraversalOn,			true,
								NULL);
	XtAddCallback (NodataFWGT,XmNlosingFocusCallback,(XtCallbackProc) _UITableFieldSubFormNodataCBK,(XtPointer) this);
	string = XmStringCreate ("Nodata:",UICharSetBold);
	NodataLWGT = XtVaCreateWidget ("UITableFieldSubFormWidthLabel",xmLabelWidgetClass,SubFormWGT,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				NodataFWGT,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			NodataFWGT,
								XmNrightOffset,			5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			NodataFWGT,
								XmNlabelString,			string,
								NULL);
	XmStringFree (string);

	XmTextFieldSetEditable (NameWGT,NewFieldPTR->Required () ? false : true);
	XtVaSetValues (NameWGT, XmNtraversalOn, NewFieldPTR->Required () ? false : true, NULL);
	for (i = 0;i < (int) (sizeof (TypeButtonWGTs) / sizeof (Widget));++i)
		XtSetSensitive (TypeButtonWGTs [i],NewFieldPTR->Required () ? false : true);
	for (i = 0;i < (int) (sizeof (SizeButtonWGTs) / sizeof (Widget));++i)
		XtSetSensitive (SizeButtonWGTs [i],NewFieldPTR->Required () ? false : true);
	switch (NewFieldPTR->Type ())
		{
		case DBTableFieldString:
			XtSetSensitive (TypeButtonWGTs [0],true);
			XtSetSensitive (SizeButtonWGTs [0],true);
			break;
		case DBTableFieldInt:
			XtSetSensitive (TypeButtonWGTs [1],true);
			switch (NewFieldPTR->Length ())
				{
				case sizeof (DBByte):	XtSetSensitive (SizeButtonWGTs [0],true);	break;
				case sizeof (DBShort):	XtSetSensitive (SizeButtonWGTs [1],true);	break;
				case sizeof (DBInt):		XtSetSensitive (SizeButtonWGTs [2],true);	break;
				}
			break;
		case DBTableFieldFloat:
			XtSetSensitive (TypeButtonWGTs [2],true);
			switch (NewFieldPTR->Length ())
				{
				case sizeof (DBFloat4):	XtSetSensitive (SizeButtonWGTs [1],true);	break;
				case sizeof (DBFloat):	XtSetSensitive (SizeButtonWGTs [2],true);	break;
				}
			break;
		}
	}

void UITableFieldSubForm::Display ()
	
	{
	int minimum, currentMin, maximum, currentMax, value;
	char numberString [DBStringLength];
	switch (NewFieldPTR->Type ())
		{
		case DBTableFieldString:
			XtVaSetValues (TypeMenuWGT,	XmNmenuHistory,	TypeButtonWGTs [0],NULL);
			minimum = 0;	maximum = 10;
			for (value = 0;((0x01 << value) != NewFieldPTR->FormatWidth ()) && (value < maximum);++value);
			XtUnmanageChild (DecimalsSWGT);
			XtUnmanageChild (DecimalsLWGT);
			XtUnmanageChild (NodataLWGT);
			XtUnmanageChild (NodataFWGT);
			XtManageChild (SizeButtonWGTs [0]);
			XtUnmanageChild (SizeButtonWGTs [1]);
			XtUnmanageChild (SizeButtonWGTs [2]);
			XtVaSetValues (SizeMenuWGT,	XmNmenuHistory,	SizeButtonWGTs [0],NULL);
			sprintf (numberString,"%d",NewFieldPTR->FormatWidth ());
			break;
		case DBTableFieldInt:
			XtVaSetValues (TypeMenuWGT,	XmNmenuHistory,	TypeButtonWGTs [1],NULL);
			minimum = 1;	maximum = 16;
			value = NewFieldPTR->FormatWidth ();
			XtUnmanageChild (DecimalsLWGT);
			XtUnmanageChild (DecimalsSWGT);
			if (NewFieldPTR->Required ())
				{
				XtUnmanageChild (NodataLWGT);
				XtUnmanageChild (NodataFWGT);
				}
			else
				{
				XtManageChild (NodataFWGT);
				XtManageChild (NodataLWGT);
				}
			XtManageChild (SizeButtonWGTs [0]);
			XtManageChild (SizeButtonWGTs [1]);
			XtManageChild (SizeButtonWGTs [2]);
			switch (NewFieldPTR->Length ())
				{
				case sizeof (DBByte):
					XtVaSetValues (SizeMenuWGT,	XmNmenuHistory,	SizeButtonWGTs [0],NULL);	break;
				case sizeof (DBShort):
					XtVaSetValues (SizeMenuWGT,	XmNmenuHistory,	SizeButtonWGTs [1],NULL);	break;
				case sizeof (DBInt):
					XtVaSetValues (SizeMenuWGT,	XmNmenuHistory,	SizeButtonWGTs [2],NULL);	break;
				}
			sprintf (numberString,"%d",NewFieldPTR->IntNoData ());
			XmTextFieldSetString (NodataFWGT,numberString);
			sprintf (numberString,"%d",value);
			break;
		case DBTableFieldFloat:
			XtVaSetValues (TypeMenuWGT,	XmNmenuHistory,	TypeButtonWGTs [2],NULL);
			minimum = 4;	maximum = 20;
			value = NewFieldPTR->FormatWidth ();
			XtManageChild (DecimalsLWGT);
			XtManageChild (DecimalsSWGT);
			if (NewFieldPTR->Required ())
				{
				XtUnmanageChild (NodataLWGT);
				XtUnmanageChild (NodataFWGT);
				}
			else
				{
				XtManageChild (NodataFWGT);
				XtManageChild (NodataLWGT);
				}
			XtUnmanageChild (SizeButtonWGTs [0]);
			XtManageChild (SizeButtonWGTs [1]);
			XtManageChild (SizeButtonWGTs [2]);
			XmScaleSetValue (DecimalsSWGT,NewFieldPTR->FormatDecimals ());
			switch (NewFieldPTR->Length ())
				{
				case sizeof (DBFloat4):
					XtVaSetValues (SizeMenuWGT,	XmNmenuHistory,	SizeButtonWGTs [1],NULL);
					break;
				case sizeof (DBFloat):
					XtVaSetValues (SizeMenuWGT,	XmNmenuHistory,	SizeButtonWGTs [2],NULL);
					break;
				}
			sprintf (numberString,"%f",NewFieldPTR->FloatNoData ());
			XmTextFieldSetString (NodataFWGT,numberString);
			sprintf (numberString,"%d",value);
			break;
		case DBTableFieldDate:
			XtVaSetValues (TypeMenuWGT,	XmNmenuHistory,	TypeButtonWGTs [3],NULL);
			minimum = 8;	maximum = 12;
			value = NewFieldPTR->FormatWidth ();
			XtVaSetValues (SizeMenuWGT,	XmNmenuHistory,	SizeButtonWGTs [0],NULL);
			XtUnmanageChild (DecimalsSWGT);
			XtUnmanageChild (DecimalsLWGT);
			XtUnmanageChild (NodataLWGT);
			XtUnmanageChild (NodataFWGT);
			XtManageChild (SizeButtonWGTs [0]);
			XtUnmanageChild (SizeButtonWGTs [1]);
			XtUnmanageChild (SizeButtonWGTs [2]);
			sprintf (numberString,"%d",value);
			break;
		}
	XmTextFieldSetString (NameWGT,NewFieldPTR->Name ());
	XtVaGetValues (WidthSWGT,	XmNminimum,	&currentMin,	XmNmaximum,	&currentMax,	NULL);
	XtVaSetValues (WidthSWGT,	XmNminimum,	currentMin < minimum ? currentMin : minimum,
												XmNmaximum,	currentMax > maximum ? currentMax : maximum,	NULL);
	UIAuxSetLabelString (WidthLWGT,numberString);
	XmScaleSetValue (WidthSWGT,value);
	XtVaSetValues (WidthSWGT,	XmNminimum,	minimum, XmNmaximum,	maximum,	NULL);
	}

DBInt UITableRedefineFields (DBObjTable *table)

	{
	DBInt save, changed, visibleCount = 0, recordID;
	DBObjTableField *field;
	DBObjectLIST<DBObjTableField> *fields = table->Fields ();
	UITableFieldSubForm *fieldSubForm;
	DBObjectLIST<UITableFieldSubForm> *fieldSubFormList = new DBObjectLIST<UITableFieldSubForm> ("FieldubFormList");
	XmString string; 
	Widget dShell, mainForm, label, scrolledW, scrollBar, rowCol;

	for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
		if (DBTableFieldIsVisible (field)) visibleCount++;

	dShell = UIDialogForm ("Redefine Table");
	mainForm = UIDialogFormGetMainForm (dShell);

	string = XmStringCreate ("Fields:",UICharSetBold);
	label = XtVaCreateManagedWidget ("UITableRedefineFieldsLabel",xmLabelGadgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				10,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNlabelString,			string,
								NULL);
	XmStringFree (string);
	scrolledW = XtVaCreateManagedWidget ("UITableRedefineScrolledWindow",xmScrolledWindowWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				label,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				5,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			5,
								XmNwidth,					612,
								XmNheight,					12 + 105 * (visibleCount < 5? visibleCount : 5),
								XmNvisualPolicy,			XmCONSTANT,
								XmNscrollingPolicy,		XmAUTOMATIC,
								XmNspacing,					2,
								XmNtraversalOn,			true,
								NULL);
	XtVaGetValues (scrolledW,XmNverticalScrollBar, &scrollBar, NULL);
	XtVaSetValues (scrollBar,XmNtraversalOn,	false, NULL);
	rowCol = XtVaCreateManagedWidget ("UITableRedefineRowCol",xmRowColumnWidgetClass,scrolledW,
								XmNorientation,			XmVERTICAL,
								XmNrowColumnType,			XmWORK_AREA,
								XmNspacing,					2,
								NULL);
	XmScrolledWindowSetAreas(scrolledW,(Widget) NULL,(Widget) NULL,rowCol);
	for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
		{
		if (DBTableFieldIsVisible (field))
			{
			fieldSubForm = new UITableFieldSubForm (rowCol, field);
			fieldSubForm->Display ();
			}
		else	fieldSubForm = new UITableFieldSubForm (field);
		fieldSubFormList->Add (fieldSubForm);
		}
	XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&save);
	XtSetSensitive (UIDialogFormGetOkButton (dShell),true);

	save = changed = false;
	UIDialogFormPopup (dShell);
	while (UILoop ());

	for (fieldSubForm = fieldSubFormList->First ();fieldSubForm != (UITableFieldSubForm *) NULL;fieldSubForm = fieldSubFormList->Next ())
		if ((changed = fieldSubForm->IsChanged ()) == true) break;
	UIDialogFormPopdown (dShell);
	if (save && changed)
		{
		changed = false;
		for (fieldSubForm = fieldSubFormList->First ();fieldSubForm != (UITableFieldSubForm *) NULL;fieldSubForm = fieldSubFormList->Next ())
			if ((changed = fieldSubForm->StructureIsChanged ()) == true) break;
		if (changed)
			{
			DBInt intValue;
			DBFloat floatValue;
			DBDate dateValue;
			DBObjTable *origTable = new DBObjTable (*table);
			DBObjTableField *oldField, *newField;
			DBObjRecord *srcRecord, *dstRecord;

			fields = origTable->Fields ();
			for (fieldSubForm = fieldSubFormList->First ();fieldSubForm != (UITableFieldSubForm *) NULL;fieldSubForm = fieldSubFormList->Next ())
				fieldSubForm->OrigField (fields);
			table->DeleteAllFields ();
			for (fieldSubForm = fieldSubFormList->First ();fieldSubForm != (UITableFieldSubForm *) NULL;fieldSubForm = fieldSubFormList->Next ())
				{
				field = fieldSubForm->NewField ();
				oldField = fieldSubForm->OldField ();
				newField = new DBObjTableField (*field);
				table->AddField (newField);
				for (recordID = 0;recordID < table->ItemNum ();recordID++)
					{
					dstRecord = table->Item (recordID);
					srcRecord = origTable->Item (recordID);
					switch (newField->Type ())
						{
						case DBTableFieldString:
							newField->String (dstRecord,oldField->String (srcRecord));
							break;
						case DBTableFieldInt:
							switch (oldField->Type ())
								{
								case DBTableFieldString:
									if (sscanf (oldField->String (srcRecord),"%d",&intValue) == 1)
										newField->Int (dstRecord,intValue);
									else	newField->Int (dstRecord,newField->IntNoData ());
									break;
								case DBTableFieldInt:
									newField->Int (dstRecord,oldField->Int (srcRecord));
									break;
								case DBTableFieldFloat:
									floatValue = oldField->Float (srcRecord);
									newField->Int (dstRecord,(DBInt) floatValue);
									break;
								}
							break;
						case DBTableFieldFloat:
							switch (oldField->Type ())
								{
								case DBTableFieldString:
									if (sscanf (oldField->String (srcRecord),"%lf",&floatValue) == 1)
										newField->Float (dstRecord,floatValue);
									else	newField->Float (dstRecord,newField->FloatNoData ());
									break;
								case DBTableFieldInt:
									floatValue = (DBFloat) oldField->Int (srcRecord);
									newField->Float (dstRecord,floatValue);
									break;
								case DBTableFieldFloat:
									newField->Float (dstRecord,oldField->Float (srcRecord));
									break;
								}
							break;
						case DBTableFieldDate:
							switch (oldField->Type ())
								{
								case DBTableFieldString:
								case DBTableFieldInt:
									dateValue.Set (oldField->String (srcRecord));
									newField->Date (dstRecord,dateValue);
									break;
								case DBTableFieldDate:
									newField->Date (dstRecord,oldField->Date (srcRecord));
									break;
								}
							break;
						case DBTableFieldCoord:
							newField->Coordinate (dstRecord,oldField->Coordinate (srcRecord));	break;
						case DBTableFieldRegion:
							newField->Region (dstRecord,oldField->Region (srcRecord));		break;
						case DBTableFieldPosition:
							newField->Position (dstRecord,oldField->Position (srcRecord));	break;
						case DBTableFieldTableRec:
						case DBTableFieldDataRec:
							newField->Record (dstRecord,oldField->Record (srcRecord));		break;
						}
					}
				}
			delete origTable;
			}
		else
			for (fieldSubForm = fieldSubFormList->First ();fieldSubForm != (UITableFieldSubForm *) NULL;fieldSubForm = fieldSubFormList->Next ())
				fieldSubForm->Update ();
		}
	fieldSubFormList->DeleteAll ();
	delete fieldSubFormList;
	return (save);
	}
