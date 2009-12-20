/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

UIDataPropF.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/Scale.h>
#include <UI.H>

static void _UIDataPropProjectionButtonCBK (Widget widget,void *projection,XmAnyCallbackStruct *callData)

	{
	int *userData;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &userData, NULL);
	*userData = (int) ((char *) projection - (char *) NULL);
	}

class UIDataPropProjection
	{
	private:
		int ProjectionVAR;
		char *ProjectionSTR;
	public:
		UIDataPropProjection (int projection,char *name)
			{
			ProjectionVAR = projection;
			ProjectionSTR = name;
			}
		UIDataPropProjection () { UIDataPropProjection (DBFault,(char *) "NoButton"); }
		void Button (Widget menu,int *userData)
			{
			XmString string;
			Widget button;

			string = XmStringCreate (ProjectionSTR,UICharSetNormal);
			button = XtVaCreateManagedWidget ("UIDataPropProjectionButton",xmPushButtonGadgetClass,menu,
													XmNuserData,		userData,
													XmNlabelString,	string,
													NULL);
			XmStringFree (string);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIDataPropProjectionButtonCBK,(void *) ProjectionVAR);
			}
	};

static UIDataPropProjection _UIDataPropProjections [] = {
	UIDataPropProjection (DBProjectionSpherical,(char *) "Geographic"),
	UIDataPropProjection (DBProjectionCartesian,(char *) "Cartesian")};


class UIDataProperties

	{
	private:
		Widget FormWGT;
		Widget LabelWGT;
	public:
		UIDataProperties ()
			{
			FormWGT = (Widget) NULL;
			LabelWGT = (Widget) NULL;
			}
		UIDataProperties (Widget widget,char *typeString)
			{
			char formLabel [32];
			Pixel foreground, background;
			XmString string;

			XtVaGetValues (widget,XmNforeground, &foreground, XmNbackground, &background, NULL);
			FormWGT = XtVaCreateWidget ("UIDataPropertiesForm",xmFormWidgetClass,widget,
													XmNshadowThickness,			1,
													NULL);
			sprintf (formLabel,"%s Properties",typeString);
			string = XmStringCreate (formLabel,UICharSetBold);
			LabelWGT = XtVaCreateManagedWidget ("UIDataPropertiesLabel",xmLabelWidgetClass,FormWGT,
												XmNtopAttachment,			XmATTACH_FORM,
												XmNleftAttachment,		XmATTACH_FORM,
												XmNrightAttachment,		XmATTACH_FORM,
												XmNforeground, 			background,
												XmNbackground,				foreground,
												XmNlabelString,			string,
												NULL);
			XmStringFree (string);
			}
		Widget Form ()		{ return (FormWGT); }
		Widget Label ()	{ return (LabelWGT); }
	};

static void UIDataPropPrecision (Widget widget,int value)

	{
	char numberString [13], formatString [10];
	Widget label;

	XtVaGetValues (widget,XmNuserData, &label, NULL);
	formatString [0] = '%';
	sprintf (formatString + 1,"%d.%df",8,value > 0 ?  0 : value == 0 ? 1 : 0 - value);
	sprintf (numberString,formatString,pow ((double) 10.0,(double) value));
	UIAuxSetLabelString (label,numberString);
	}

static void UIDataPropPrecisionCBK (Widget widget,void *data,XmScaleCallbackStruct *callData)

	{
	data = data;
	UIDataPropPrecision (widget,callData->value);
	}

static void UIDataPropScale (Widget widget,int value)

	{
	char numberString [13], formatString [15];
	Widget label;

	XtVaGetValues (widget,XmNuserData, &label, NULL);
	sprintf (formatString,"1: %d.0f",value);
	formatString [2] = '%';
	sprintf (numberString,formatString,pow ((double) 10.0,(double) value));
	UIAuxSetLabelString (label,numberString);
	}

static void UIDataPropScaleCBK (Widget widget,void *data,XmScaleCallbackStruct *callData)

	{
	data = data;
	UIDataPropScale (widget,callData->value);
	}

class UIDataPropertiesGeo : public UIDataProperties

 	{
	private:
		Widget ExtentLLHorLabelWGT, ExtentLLVerLabelWGT, ExtentURHorLabelWGT, ExtentURVerLabelWGT;
		Widget ProjectionMenuWGT, PrecisionScaleWGT, MaxScaleScaleWGT, MinScaleScaleWGT;
		int ProjectionVAR;
	public:
 		UIDataPropertiesGeo (Widget widget) : UIDataProperties (widget,(char *) "Geographic")
 			{
 			int i;
 			Widget label, frame;
			XmString string;

			string = XmStringCreate ((char *) "Lower Left",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropExtentLLLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Upper Right",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropExtentURLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) " ",UICharSetNormal);
			ExtentLLHorLabelWGT = XtVaCreateManagedWidget ("UIDataPorpExtentLLHorLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			ExtentURHorLabelWGT = XtVaCreateManagedWidget ("UIDataPropExtentLLVerrLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Horizontal:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropExtentURLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) " ",UICharSetNormal);
			ExtentLLVerLabelWGT = XtVaCreateManagedWidget ("UIDataPropExtentURHorLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			ExtentURVerLabelWGT = XtVaCreateManagedWidget ("UIDataPropExtentURVerLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Vertical:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropExtentURLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			ProjectionMenuWGT = XmCreatePulldownMenu (Form (),(char *) "UIDataPropProjectionMenu",NULL,0);
			for (i = 0;i < (int) (sizeof (_UIDataPropProjections) / sizeof (UIDataPropProjection));++i)
				_UIDataPropProjections [i].Button (ProjectionMenuWGT,&ProjectionVAR);
			string = XmStringCreate ((char *) "Projection:",UICharSetBold);
			ProjectionMenuWGT= XtVaCreateManagedWidget ("UIDataHeaderFormTypeMenu",xmRowColumnWidgetClass,Form (),
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			label,
											XmNtopOffset,			2,
											XmNrightAttachment,	XmATTACH_FORM,
											XmNrightOffset,		5,
											XmNsubMenuId,			ProjectionMenuWGT,
											XmNlabelString,		string,
											XmNrowColumnType,		XmMENU_OPTION,
											XmNtraversalOn,		false,
											NULL);
			XmStringFree (string);


			frame = XtVaCreateManagedWidget ("UIDataPropPrecisionFrame",xmFrameWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				ProjectionMenuWGT,
											XmNtopOffset,				2,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											NULL);
			string = XmStringCreate ((char *) "FieldNotSet",UICharSetNormal);
			label = XtVaCreateManagedWidget ("UIDataPropPrecisonLabel",xmLabelGadgetClass,frame,
											XmNmarginWidth,			5,
											XmNalignment,				XmALIGNMENT_END,
											XmNlabelString,			string,
											XmNrecomputeSize,			false,
											NULL);
			XmStringFree (string);
			PrecisionScaleWGT = XtVaCreateManagedWidget ("UIDataPropPrecison",xmScaleWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				frame,
											XmNtopOffset,				2,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			frame,
											XmNrightOffset,			10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			frame,
											XmNbottomOffset,			2,
											XmNorientation,			XmHORIZONTAL,
											XmNminimum,					-6,
											XmNmaximum,					7,
											XmNvalue,					0,
											XmNscaleWidth,				60,
											XmNtraversalOn,			false,
											XmNuserData,				label,
											NULL);
			XtAddCallback (PrecisionScaleWGT,XmNdragCallback,(XtCallbackProc) UIDataPropPrecisionCBK,(void *) NULL);
			XtAddCallback (PrecisionScaleWGT,XmNvalueChangedCallback,(XtCallbackProc) UIDataPropPrecisionCBK,(void *) NULL);
			string = XmStringCreate ((char *) "Precision:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropPrecisonLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				frame,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			PrecisionScaleWGT,
											XmNrightOffset,			10,
											XmNlabelString,			string,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			frame,
											XmNbottomOffset,			2,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											NULL);
			XmStringFree (string);


			frame = XtVaCreateManagedWidget ("UIDataPropMaxScaleFrame",xmFrameWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				PrecisionScaleWGT,
											XmNtopOffset,				2,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											NULL);
			string = XmStringCreate ((char *) "FieldNotSet",UICharSetNormal);
			label = XtVaCreateManagedWidget ("UIDataPropMaxScaleLabel",xmLabelGadgetClass,frame,
											XmNmarginWidth,			5,
											XmNalignment,				XmALIGNMENT_END,
											XmNlabelString,			string,
											XmNrecomputeSize,			false,
											NULL);
			XmStringFree (string);
			MaxScaleScaleWGT = XtVaCreateManagedWidget ("UIDataPropMaxScale",xmScaleWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				frame,
											XmNtopOffset,				2,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			frame,
											XmNrightOffset,			10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			frame,
											XmNbottomOffset,			2,
											XmNorientation,			XmHORIZONTAL,
											XmNminimum,					0,
											XmNmaximum,					8,
											XmNvalue,					6,
											XmNscaleWidth,				60,
											XmNtraversalOn,			false,
											XmNuserData,				label,
											NULL);
			XtAddCallback (MaxScaleScaleWGT,XmNdragCallback,(XtCallbackProc) UIDataPropScaleCBK,(void *) NULL);
			XtAddCallback (MaxScaleScaleWGT,XmNvalueChangedCallback,(XtCallbackProc) UIDataPropScaleCBK,(void *) NULL);
			string = XmStringCreate ((char *) "Maximum Scale:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropMaxScaleLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				frame,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			MaxScaleScaleWGT,
											XmNrightOffset,			10,
											XmNlabelString,			string,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			frame,
											XmNbottomOffset,			2,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											NULL);
			XmStringFree (string);
			frame = XtVaCreateManagedWidget ("UIDataPropMinScaleFrame",xmFrameWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				MaxScaleScaleWGT,
											XmNtopOffset,				2,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			5,
											NULL);
			string = XmStringCreate ((char *) "FieldNotSet",UICharSetNormal);
			label = XtVaCreateManagedWidget ("UIDataPropMinScaleLabel",xmLabelGadgetClass,frame,
											XmNmarginWidth,			5,
											XmNalignment,				XmALIGNMENT_END,
											XmNlabelString,			string,
											XmNrecomputeSize,			false,
											NULL);
			XmStringFree (string);
			MinScaleScaleWGT = XtVaCreateManagedWidget ("UIDataPropMinScale",xmScaleWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				frame,
											XmNtopOffset,				2,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			frame,
											XmNrightOffset,			10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			frame,
											XmNbottomOffset,			2,
											XmNorientation,			XmHORIZONTAL,
											XmNminimum,					0,
											XmNmaximum,					8,
											XmNvalue,					6,
											XmNscaleWidth,				60,
											XmNtraversalOn,			false,
											XmNuserData,				label,
											NULL);
			XtAddCallback (MinScaleScaleWGT,XmNdragCallback,(XtCallbackProc) UIDataPropScaleCBK,(void *) NULL);
			XtAddCallback (MinScaleScaleWGT,XmNvalueChangedCallback,(XtCallbackProc) UIDataPropScaleCBK,(void *) NULL);
			string = XmStringCreate ((char *) "Minimum Scale:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropMinScaleLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				frame,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			MinScaleScaleWGT,
											XmNrightOffset,			10,
											XmNlabelString,			string,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			frame,
											XmNbottomOffset,			2,
											XmNalignment,				XmALIGNMENT_BEGINNING,
											NULL);
			XmStringFree (string);
			}
		Widget Load (DBObjData *data)
 			{
			char numberString [13];
			DBRegion extent;
			Widget menu, *buttons;

			extent = data->Extent ();
			sprintf (numberString,"%12.3f",extent.LowerLeft.X);
			UIAuxSetLabelString (ExtentLLHorLabelWGT,numberString);
			sprintf (numberString,"%12.3f",extent.LowerLeft.Y);
			UIAuxSetLabelString (ExtentLLVerLabelWGT,numberString);
			sprintf (numberString,"%12.3f",extent.UpperRight.X);
			UIAuxSetLabelString (ExtentURHorLabelWGT,numberString);
			sprintf (numberString,"%12.3f",extent.UpperRight.Y);
			UIAuxSetLabelString (ExtentURVerLabelWGT,numberString);

			XtVaGetValues (ProjectionMenuWGT,XmNsubMenuId, &menu,NULL);
			XtVaGetValues (menu,XmNchildren, &buttons, NULL);
			ProjectionVAR = data->Projection ();
         if (ProjectionVAR > DBProjectionCartesian) ProjectionVAR = DBProjectionCartesian;
			XtVaSetValues (ProjectionMenuWGT, XmNmenuHistory, buttons [ProjectionVAR],NULL);

			XmScaleSetValue (PrecisionScaleWGT,data->Precision ());
			UIDataPropPrecision (PrecisionScaleWGT,data->Precision ());
			XmScaleSetValue (MaxScaleScaleWGT,data->MaxScale ());
			UIDataPropScale (MaxScaleScaleWGT,data->MaxScale ());
			XmScaleSetValue (MinScaleScaleWGT,data->MinScale ());
			UIDataPropScale (MinScaleScaleWGT,data->MinScale ());
			XtManageChild (Form ());
 			return (Form ());
 			}
 		int IsChanged (DBObjData *data)
 			{
 			int value;
 			XmScaleGetValue (PrecisionScaleWGT,&value);	if (data->Precision ()	!= value) return (true);
 			XmScaleGetValue (MaxScaleScaleWGT,&value);	if (data->MaxScale ()	!= value) return (true);
 			XmScaleGetValue (MinScaleScaleWGT,&value);	if (data->MinScale ()	!= value) return (true);
 			if (data->Projection () != ProjectionVAR) return (true);
 			return (false);
 			}
 		void Save (DBObjData *data)
 			{
 			int value;

 			data->Projection (ProjectionVAR);
 			XmScaleGetValue (PrecisionScaleWGT,&value);	data->Precision (value);
 			XmScaleGetValue (MaxScaleScaleWGT,&value);	data->MaxScale (value);
 			XmScaleGetValue (MinScaleScaleWGT,&value);	data->MinScale (value);
 			}
	};

class UIDataPropertiesVector : public UIDataProperties

	{
	private:
		Widget ItemNumLabelWGT, FieldNumLabelWGT;
	public:
		UIDataPropertiesVector (Widget widget) : UIDataProperties (widget, (char *) "Vector")
			{
			XmString string;
			Widget label;

			string = XmStringCreate ((char *) "Number of Items:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropVectorItemNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				30,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);
			string = XmStringCreate ((char *) " ",UICharSetNormal);
			ItemNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropVectorItemNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNlabelString,			string,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			15,
											NULL);
			string = XmStringCreate ((char *) "Number of Fields:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropVectorFieldNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				ItemNumLabelWGT,
											XmNtopOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			ItemNumLabelWGT,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);
			string = XmStringCreate ((char *) " ",UICharSetNormal);
			FieldNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropVectorFieldNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				ItemNumLabelWGT,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			15,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNlabelString,			string,
											NULL);

			}
		Widget Load (DBObjData *data)
			{
			int fieldNum;
			char numberString [10];
			DBObjTable *table = data->Table (DBrNItems);
			DBObjectLIST<DBObjTableField> *fields = table->Fields ();
			DBObjTableField *field;

			sprintf (numberString,"%d",table->ItemNum ());
			UIAuxSetLabelString (ItemNumLabelWGT,numberString);
			fieldNum = table->ItemNum ();

			fieldNum = fields->ItemNum ();
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field) == false) fieldNum--;
			sprintf (numberString,"%d",fieldNum);
			UIAuxSetLabelString (FieldNumLabelWGT,numberString);
			XtManageChild (Form ());
			return (Form ());
			}
	};

static void _UIDataPropContinuousColorCBK (Widget widget,void *Flags,XmAnyCallbackStruct *callData)

	{
	DBUnsigned *userData;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &userData, NULL);
	*userData = (DBUnsigned) ((char *) Flags - (char *) NULL);
	}

class UIDataPropContinuousColor

	{
	private:
		char *NameSTR;
		DBUnsigned FlagVAR;
	public:
		UIDataPropContinuousColor (char *name,DBUnsigned flag) { NameSTR = name; FlagVAR = flag; }
		void MakeButton (Widget widget,DBUnsigned *userData)
			{
			XmString string;
			Widget button;

			string = XmStringCreate (NameSTR,UICharSetNormal);
			button = XtVaCreateManagedWidget ("UIDataPropContinuousColorMenuButton",xmPushButtonGadgetClass,widget,
														XmNlabelString,			string,
														XmNuserData,				userData,
														NULL);
			XmStringFree (string);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIDataPropContinuousColorCBK,(void *) FlagVAR);
			}
		DBInt IsColor (DBUnsigned flag) { return (FlagVAR == flag ? true : false); }
	};

static UIDataPropContinuousColor _UIDataPropContinuousColors [] = {
	UIDataPropContinuousColor ((char *) "Standard", 		DBDataFlagDispModeContStandard),
	UIDataPropContinuousColor ((char *) "Grey Scale",		DBDataFlagDispModeContGreyScale),
	UIDataPropContinuousColor ((char *) "Blue Scale",		DBDataFlagDispModeContBlueScale),
	UIDataPropContinuousColor ((char *) "Blue to Red",		DBDataFlagDispModeContBlueRed),
	UIDataPropContinuousColor ((char *) "Elevation",		DBDataFlagDispModeContElevation)};


class UIDataPropertiesContinuous : public UIDataProperties

	{
	private:
		DBUnsigned ColorVAR;
		Widget RowNumLabelWGT, ColNumLabelWGT;
		Widget CellWidthLabelWGT, CellHeightLabelWGT;
		Widget LayerNumLabelWGT;
		Widget ColorMenuWGT;
	public:
		UIDataPropertiesContinuous (Widget widget) : UIDataProperties (widget, (char *) "Continuous")
			{
			int button;
			XmString string;
			Widget label;

			string = XmStringCreate ((char *) "Horizontal",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropContinuousHorizontalLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Vertical",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropContinuousVerticalLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) " ",UICharSetNormal);
			CellWidthLabelWGT = XtVaCreateManagedWidget ("UIDataPorpContinuousCellWidthLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			CellHeightLabelWGT = XtVaCreateManagedWidget ("UIDataPropContinuousCellHeightLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Cell Size:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropContinuousCellSizeLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				10,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) " ",UICharSetNormal);
			ColNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropContinuousColNumLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			RowNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropContinuousRowNumLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Dimension:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropContinuousDimensionLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				10,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Number of Layers:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropContinuousLayerNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				RowNumLabelWGT,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				30,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);
			string = XmStringCreate ((char *) " ",UICharSetNormal);
			LayerNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropContinuousLayerNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				RowNumLabelWGT,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNlabelString,			string,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			15,
											NULL);
			XmStringFree (string);

			ColorMenuWGT = XmCreatePulldownMenu (Form (),(char *) "UIDataPropContinuousColorMenu",NULL,0);

			for (button = 0;button < (int) (sizeof (_UIDataPropContinuousColors) / sizeof (UIDataPropContinuousColor)); ++button)
				_UIDataPropContinuousColors [button].MakeButton (ColorMenuWGT,&ColorVAR);

			string = XmStringCreate ((char *) "Color Code:",UICharSetBold);
			ColorMenuWGT = XtVaCreateManagedWidget ("UIDataPropContinuousColorMenu",xmRowColumnWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				10,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				30,
											XmNsubMenuId,				ColorMenuWGT,
											XmNlabelString,			string,
											XmNrowColumnType,			XmMENU_OPTION,
											XmNtraversalOn,			false,
											NULL);
			XmStringFree (string);
			}

		Widget Load (DBObjData *data)
			{
			int button;
			char numberString [10];
			DBObjTable *layerTable = data->Table (DBrNLayers);
			DBObjTableField *rowNumFLD = layerTable->Field (DBrNRowNum);
			DBObjTableField *colNumFLD = layerTable->Field (DBrNColNum);
			DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
			DBObjTableField *cellHeightFLD = layerTable->Field (DBrNCellHeight);
			DBObjRecord *record = layerTable->Item ();
			Widget colorMenu, *buttons;

			ColorVAR = (data->Flags () & DBDataFlagDispModeContShadeSets);
			XtVaGetValues (ColorMenuWGT,XmNsubMenuId, &colorMenu, NULL);
			XtVaGetValues (colorMenu,XmNchildren, &buttons, NULL);
			for (button = 0;button < (int) (sizeof (_UIDataPropContinuousColors) / sizeof (UIDataPropContinuousColor)); ++button)
				if (_UIDataPropContinuousColors [button].IsColor(ColorVAR))
					XtVaSetValues (ColorMenuWGT,XmNmenuHistory, buttons [button], NULL);
			sprintf (numberString,"%8d",colNumFLD->Int (record));
			UIAuxSetLabelString (ColNumLabelWGT,numberString);
			sprintf (numberString,"%8d",rowNumFLD->Int (record));
			UIAuxSetLabelString (RowNumLabelWGT,numberString);

			sprintf (numberString,"%8.3f",cellWidthFLD->Float (record));
			UIAuxSetLabelString (CellWidthLabelWGT,numberString);
			sprintf (numberString,"%8.3f",cellHeightFLD->Float (record));
			UIAuxSetLabelString (CellHeightLabelWGT,numberString);

			sprintf (numberString,"%8d",layerTable->ItemNum ());
			UIAuxSetLabelString (LayerNumLabelWGT,numberString);

			XtManageChild (Form ());
			return (Form ());
			}
 		int IsChanged (DBObjData *data)
 			{ return (ColorVAR != (data->Flags () & DBDataFlagDispModeContShadeSets)); }
 		void Save (DBObjData *data)
 			{
 			data->Flags (DBDataFlagDispModeContShadeSets,DBClear);
 			data->Flags (ColorVAR,DBSet);
 			}
	};

class UIDataPropertiesNetwork : public UIDataProperties

	{
	private:
		Widget BasinNumLabelWGT, CellNumLabelWGT, RowNumLabelWGT, ColNumLabelWGT, CellWidthLabelWGT, CellHeightLabelWGT;
	public:
		UIDataPropertiesNetwork (Widget widget) : UIDataProperties (widget, (char *) "Network")
			{
			XmString string;
			Widget label;

			string = XmStringCreate ((char *) "Horizontal",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropNetworkHorizontalLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Vertical",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropNetworkVerticalLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				Label (),
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) " ",UICharSetNormal);
			CellWidthLabelWGT = XtVaCreateManagedWidget ("UIDataPorpNetworkCellWidthLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			CellHeightLabelWGT = XtVaCreateManagedWidget ("UIDataPropNetworkCellHeightLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Cell Size:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropNetworkCellSizeLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				10,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) " ",UICharSetNormal);
			RowNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropNetworkColNumLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			33,
											XmNrightAttachment,		XmATTACH_POSITION,
											XmNrightPosition,			66,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			ColNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropNetworkRowNumLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_POSITION,
											XmNleftPosition,			66,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			5,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Dimension:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropNetworkDimensionLabel",xmLabelGadgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				label,
											XmNtopOffset,				2,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				10,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);

			string = XmStringCreate ((char *) "Number of Basins:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropNetworkBasinNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				RowNumLabelWGT,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				30,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);
			string = XmStringCreate ((char *) " ",UICharSetNormal);
			BasinNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropNetworkBasinNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				RowNumLabelWGT,
											XmNtopOffset,				10,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNlabelString,			string,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			15,
											NULL);
			XmStringFree (string);
			string = XmStringCreate ((char *) "Number of Cells:",UICharSetBold);
			label = XtVaCreateManagedWidget ("UIDataPropNetworkCellNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				BasinNumLabelWGT,
											XmNtopOffset,				5,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			BasinNumLabelWGT,
											XmNrightOffset,			5,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);
			string = XmStringCreate ((char *) " ",UICharSetNormal);
			CellNumLabelWGT = XtVaCreateManagedWidget ("UIDataPropNetworkCellNumLabel",xmLabelWidgetClass,Form (),
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				BasinNumLabelWGT,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_WIDGET,
											XmNleftWidget,				label,
											XmNleftOffset,				5,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			15,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNlabelString,			string,
											NULL);
			XmStringFree (string);
			}
		Widget Load (DBObjData *data)
			{
			char numberString [10];
			DBObjTable *basinTable = data->Table (DBrNItems);
			DBObjTable *cellTable  = data->Table (DBrNCells);
			DBObjTable *layerTable = data->Table (DBrNLayers);

			DBObjTableField *rowNumFLD = layerTable->Field (DBrNRowNum);
			DBObjTableField *colNumFLD = layerTable->Field (DBrNColNum);
			DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
			DBObjTableField *cellHeightFLD = layerTable->Field (DBrNCellHeight);
			DBObjRecord *record = layerTable->Item ();

			sprintf (numberString,"%8d",colNumFLD->Int (record));
			UIAuxSetLabelString (ColNumLabelWGT,numberString);
			sprintf (numberString,"%8d",rowNumFLD->Int (record));
			UIAuxSetLabelString (RowNumLabelWGT,numberString);

			sprintf (numberString,"%8.3f",cellWidthFLD->Float (record));
			UIAuxSetLabelString (CellWidthLabelWGT,numberString);
			sprintf (numberString,"%8.3f",cellHeightFLD->Float (record));
			UIAuxSetLabelString (CellHeightLabelWGT,numberString);

			sprintf (numberString,"%d",basinTable->ItemNum ());
			UIAuxSetLabelString (BasinNumLabelWGT,numberString);
			sprintf (numberString,"%d",cellTable->ItemNum ());
			UIAuxSetLabelString (CellNumLabelWGT,numberString);

			XtManageChild (Form ());
			return (Form ());
			}
	};

DBInt UIDataPropertiesForm (DBObjData *data)

	{
	static int save, changed;
	static Widget dShell = (Widget) NULL, geoForm, currentForm;
	static UIDataPropertiesGeo			*geoProp;
	static UIDataPropertiesVector		*vectorProp;
	static UIDataPropertiesContinuous	*ContGridProp;
	static UIDataPropertiesNetwork	*networkProp;

	data = data;
	if (dShell == NULL)
		{
		Widget rowColumn;

		dShell = UIDialogForm ((char *) "Data Properties");
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&save);

		rowColumn = XtVaCreateManagedWidget ("UIPropRowColumn",xmRowColumnWidgetClass,UIDialogFormGetMainForm (dShell),
											XmNtopAttachment,				XmATTACH_FORM,
											XmNtopOffset,					5,
											XmNleftAttachment,			XmATTACH_FORM,
											XmNleftOffset,					5,
											XmNrightAttachment,			XmATTACH_FORM,
											XmNbottomAttachment,			XmATTACH_FORM,
											XmNbottomOffset,				5,
											XmNorientation,				XmVERTICAL,
											XmNmarginWidth,				0,
											XmNmarginHeight,				0,
											XmNresizeHeight,				true,
											XmNresizeWidth,				true,
											NULL);
		geoProp		= new UIDataPropertiesGeo		(rowColumn);
		vectorProp	= new UIDataPropertiesVector	(rowColumn);
		ContGridProp = new UIDataPropertiesContinuous	(rowColumn);
		networkProp	= new UIDataPropertiesNetwork	(rowColumn);
		}
	else	{ XtUnmanageChild (currentForm);	if (geoForm != (Widget) NULL) XtUnmanageChild (geoForm); }

	geoForm = data->Type () == DBTypeTable ? (Widget) NULL : geoProp->Load (data);
	switch (data->Type ())
		{
		case DBTypeVectorPoint:
		case DBTypeVectorLine:
		case DBTypeVectorPolygon:		currentForm = vectorProp->Load	(data);	break;
		case DBTypeGridContinuous:		currentForm = ContGridProp->Load	(data);	break;
		case DBTypeNetwork:				currentForm = networkProp->Load	(data);	break;
		}

	save = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		changed = false;
		if (geoForm != (Widget) NULL) if (geoProp->IsChanged (data)) changed = true;
		switch (data->Type ())
			{
			case DBTypeGridContinuous: 	if (ContGridProp->IsChanged (data)) changed = true;	break;
			default: break;
			}
		XtSetSensitive (UIDialogFormGetOkButton (dShell),changed);
		}
	UIDialogFormPopdown (dShell);
	if (save)
		{
		if (geoForm != (Widget) NULL) geoProp->Save (data);
		switch (data->Type ())
			{
			case DBTypeGridContinuous: 	ContGridProp->Save (data);	break;
			default:	break;
			}
		}
	return (DBSuccess);
	}
