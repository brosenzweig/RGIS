/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UINumberInput.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <UI.H>

#define UINumberInputLabel		"*UINumberInputLabel"
#define UINumberInputScale		"*UINumberInputScale"
#define UINumberInputField		"*UINumberInputField"

class UINumberProp

	{
	private:
		DBFloat MinVAR;
		DBFloat MaxVAR;
		DBInt	  ModeVAR, MinScaleVAR, MaxScaleVAR;
		char FormatSTR [DBStringLength];
		char NumberSTR [DBStringLength];
	public:
		UINumberProp (DBFloat minVar,DBFloat maxVar,DBInt mode,char *format)
			{
			MinVAR = minVar;
			MaxVAR = maxVar;
			ModeVAR = mode;
			MinScaleVAR = 0;
			MaxScaleVAR = 100;
			strncpy (FormatSTR,format,sizeof (FormatSTR) - 1);
			}
		DBInt MinScale ()		{ return (MinScaleVAR); }
		DBInt MaxScale ()		{ return (MaxScaleVAR); } 
		DBInt ScaleValue (DBFloat value)
			{
			if (ModeVAR == UINumberInputModeLogarithmic)
				return ((DBInt) ((log10 (value) - log10 (MinVAR)) * (MaxScaleVAR - MinScaleVAR) / (log10 (MaxVAR) - log10 (MinVAR))));
			else
				return ((DBInt) ((value - MinVAR) * (MaxScaleVAR - MinScaleVAR) / (MaxVAR - MinVAR)));
			}
		DBFloat Value (DBInt scale)
			{
			if (ModeVAR == UINumberInputModeLogarithmic)
				return (pow (10.0,log10 (MinVAR) + (log10 (MaxVAR) - log10 (MinVAR)) * (DBFloat) scale / (MaxScaleVAR - MinScaleVAR)));
			else
				return (MinVAR + (MaxVAR - MinVAR) * (DBFloat) scale / (MaxScaleVAR - MinScaleVAR));
			}
		char *ValueString (DBFloat value)
			{ sprintf (NumberSTR,FormatSTR,value); return (NumberSTR);  }
		char *ValueString (DBInt scaleValue)
			{ return (ValueString (Value (scaleValue))); }
	};
		
static void _UINumberInputScaleCBK (Widget widget,Widget form,XmScaleCallbackStruct *callData)
	
	{
	UINumberProp *numberProp;

	widget = widget;

	XtVaGetValues (form,XmNuserData, &numberProp, NULL);
	UIAuxSetLabelString (XtNameToWidget (form,UINumberInputField),numberProp->ValueString (callData->value));
	}

Widget UINumberInputCreate (Widget parent,char *labelText,Arg *wargs,DBInt argNum,DBFloat min,DBFloat max,DBFloat value,DBInt mode,char *format)

	{
	Widget form, frame, scale;
	XmString string;
	UINumberProp *numberProp = new UINumberProp (min,max,mode,format);

	form = XmCreateForm (parent,"UINumberInputScaleForm",wargs,argNum);
	XtVaSetValues (form, XmNshadowThickness,	0, XmNuserData,	numberProp, NULL);
	XtManageChild (form);

	frame = XtVaCreateManagedWidget ("UINumberInputScaleFrame",xmFrameWidgetClass,form,
												XmNtopAttachment,				XmATTACH_FORM,
												XmNrightAttachment,			XmATTACH_FORM,
												XmNbottomAttachment,			XmATTACH_FORM,
												NULL);
	string = XmStringCreate ("NotSet",UICharSetNormal);
	XtVaCreateManagedWidget (UINumberInputField + 1,xmLabelWidgetClass,frame,
												XmNmarginWidth,				5,
												XmNalignment,					XmALIGNMENT_END,
												XmNlabelString,				string,
												XmNrecomputeSize,				false,
												NULL);
	XmStringFree (string);
	scale = XtVaCreateManagedWidget (UINumberInputScale + 1,xmScaleWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					frame,
												XmNtopOffset,					3,
												XmNrightAttachment,			XmATTACH_WIDGET,
												XmNrightWidget,				frame,
												XmNrightOffset,				5,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				frame,
												XmNbottomOffset,				3,
												XmNorientation,				XmHORIZONTAL,
												XmNtraversalOn,				false,
												XmNminimum,						numberProp->MinScale (),
												XmNmaximum,						numberProp->MaxScale (),
												XmNvalue,						numberProp->ScaleValue (value),
												NULL);
	XtAddCallback (scale,XmNdragCallback,			(XtCallbackProc) _UINumberInputScaleCBK,(XtPointer) form);
	XtAddCallback (scale,XmNvalueChangedCallback,(XtCallbackProc) _UINumberInputScaleCBK,(XtPointer) form);
	string = XmStringCreate (labelText,UICharSetBold);
	XtVaCreateManagedWidget (UINumberInputLabel + 1,xmLabelWidgetClass,form,
												XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
												XmNtopWidget,					frame,
												XmNleftAttachment,			XmATTACH_FORM,
												XmNrightAttachment,			XmATTACH_WIDGET,
												XmNrightWidget,				scale,
												XmNrightOffset,				5,
												XmNlabelString,				string,
												XmNbottomAttachment,			XmATTACH_OPPOSITE_WIDGET,
												XmNbottomWidget,				frame,
												XmNalignment,					XmALIGNMENT_END,
												XmNrecomputeSize,				false,
												NULL);
	XmStringFree (string);
	UINumberInputSetValue (form,value);
	return (form);
	}

void UINumberInputResize (Widget form,Dimension labelWidth,Dimension scaleWidth, Dimension fieldWidth)

	{
	if (labelWidth > 0)
		XtVaSetValues (XtNameToWidget (form,UINumberInputLabel),XmNwidth,	labelWidth, XmNrecomputeSize,	false,	NULL);
	if (scaleWidth > 0)
		XtVaSetValues (XtNameToWidget (form,UINumberInputScale),XmNwidth,	scaleWidth, XmNrecomputeSize,	false,	NULL);
	if (fieldWidth > 0)
		XtVaSetValues (XtNameToWidget (form,UINumberInputField),XmNwidth,	fieldWidth, XmNrecomputeSize,	false,	NULL);
	}

Widget UINumberInputGetScale (Widget form)

	{ return (XtNameToWidget (form,UINumberInputScale)); }

void UINumberInputSetValue (Widget form,DBFloat value)

	{
	UINumberProp *numberProp;
	
	XtVaGetValues (form,XmNuserData, &numberProp, NULL);
	UIAuxSetLabelString (XtNameToWidget (form,UINumberInputField),numberProp->ValueString (value));
	XmScaleSetValue (UINumberInputGetScale (form),numberProp->ScaleValue (value));
	}

DBFloat UINumberInputGetValue (Widget form)

	{
	int scaleValue;
	UINumberProp *numberProp;

	XtVaGetValues (form,XmNuserData, &numberProp, NULL);
	XmScaleGetValue (UINumberInputGetScale (form),&scaleValue);
	
	return (numberProp->Value (scaleValue));
	}
