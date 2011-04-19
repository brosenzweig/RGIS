/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

UIDataHeaderF.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/Scale.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <UI.H>

class UIButtonList
	{
	public:
		DBInt ID;
		Widget Button;
	};

static UIButtonList _UIDataTypeList [] = {
		{DBTypeVectorPoint,		NULL},
		{DBTypeVectorLine,		NULL},
		{DBTypeVectorPolygon,	NULL},
		{DBTypeGridContinuous,	NULL},
		{DBTypeGridDiscrete,		NULL},
		{DBTypeNetwork,			NULL},
		{DBTypeTable,				NULL}};

static void _UIDataHeaderFormButtonCBK (Widget widget,DBInt *code,XmAnyCallbackStruct *callData)

	{
	callData = callData;
	XtVaGetValues (widget,XmNuserData, code, NULL);
	}

static void _UIDataHeaderButtonCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	char *string;
	char *(*function) (void);

	callData = callData;

	XtVaGetValues (widget,XmNuserData, (int *) (&function), NULL);
	if ((string = (*function) ()) != NULL) XmTextFieldSetString (text,string);
	}

static void _UIDataHeaderTextFieldCBK (Widget widget,int *changed,XmAnyCallbackStruct *callData)

	{
	char *text, *origText;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &text, NULL);
	origText = XmTextFieldGetString (widget);
	*changed = strcmp (origText,text) == 0 ? false : true;
	XtFree (origText);
	}

static void _UIDataHeaderTextCBK (Widget widget,int *changed,XmAnyCallbackStruct *callData)

	{
	char *text, *origText;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &text, NULL);
	origText = XmTextGetString (widget);
	*changed = strcmp (origText,text) == 0 ? false : true;
	XtFree (origText);
	}

DBInt UIDataHeaderForm (DBObjData *data)

	{
	char *text;
	DBInt i, edit;
	DBUnsigned var;
	static DBInt dataType, save, changed [10];
	static Widget dShell = NULL;
	static Widget nameTextF, subjectTextF, geoDomTextF, versionTextF;
	static Widget citRefText, citInstTextF, srcInstTextF, srcPersTextF, commentText;
	static Widget typeMenu;

	if (dShell == NULL)
		{
		XmString string, select;
		Widget mainForm, label, button, separator;

		var = 0;
		dShell = UIDialogForm ((char *) "Data Header Information");
		mainForm = UIDialogFormGetMainForm (dShell);
		select = XmStringCreate ((char *) "Select",UICharSetBold);
		string = XmStringCreate ((char *) "Name:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormNameLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_FORM,
											XmNtopOffset,			10,
											XmNrightAttachment,	XmATTACH_POSITION,
											XmNrightPosition,		25,
											XmNrightOffset,		10,
											XmNalignment,			XmALIGNMENT_END,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		nameTextF = XtVaCreateManagedWidget ("UIDataHeaderFormNameText",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_FORM,
											XmNtopOffset,			10,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNmaxLength,			DBDataNameLen - 1,
											XmNcolumns,				DBDataNameLen - 1,
											NULL);
		XtAddCallback (nameTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextFieldCBK,changed + var++);
		typeMenu = XmCreatePulldownMenu (mainForm,(char *) "UIDataHeaderFormTypeMenu",NULL,0);
		for (i = 0;i < (int) (sizeof (_UIDataTypeList) / sizeof (UIButtonList));++i)
			{
			string = XmStringCreate (DBDataTypeString (_UIDataTypeList [i].ID),UICharSetNormal);
			_UIDataTypeList [i].Button = XtVaCreateManagedWidget ("UIDataHeaderFormTypeButton",xmPushButtonGadgetClass,typeMenu,
													XmNuserData,		_UIDataTypeList [i].ID,
													XmNlabelString,	string,
													NULL);
			XmStringFree (string);
			XtAddCallback (_UIDataTypeList [i].Button,XmNactivateCallback,(XtCallbackProc) _UIDataHeaderFormButtonCBK,&dataType);
			}
		string = XmStringCreate ((char *) "Data Type:",UICharSetBold);
		typeMenu = XtVaCreateManagedWidget ("UIDataHeaderFormTypeMenu",xmRowColumnWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			nameTextF,
											XmNtopOffset,			2,
											XmNrightAttachment,	XmATTACH_FORM,
											XmNrightOffset,		10,
											XmNsubMenuId,			typeMenu,
											XmNlabelString,		string,
											XmNrowColumnType,		XmMENU_OPTION,
											XmNtraversalOn,		false,
											NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Subject:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormSubjectLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			nameTextF,
											XmNtopOffset,			10,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		nameTextF,
											XmNrightOffset,		5,
											XmNalignment,			XmALIGNMENT_END,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		subjectTextF = XtVaCreateManagedWidget ("UIDataHeaderFormSubjectField",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			nameTextF,
											XmNtopOffset,			10,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNmaxLength,			DBDataSubjectLen - 1,
											XmNcolumns,				DBDataSubjectLen - 1,
											NULL);
		XtAddCallback (subjectTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextFieldCBK,changed + var++);
		button = XtVaCreateManagedWidget ("UIDataHeaderFormSubjectSelectButton",xmPushButtonWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			nameTextF,
											XmNtopOffset,			10,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			subjectTextF,
											XmNleftOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		typeMenu,
											XmNrightOffset,		20,
											XmNlabelString,		select,
											XmNtraversalOn,		false,
											XmNuserData,			(XtArgVal) UIDatasetSubject,
											NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIDataHeaderButtonCBK,subjectTextF);


		string = XmStringCreate ((char *) "Geograhic Domain:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormGeoDomainLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			subjectTextF,
											XmNtopOffset,			10,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		nameTextF,
											XmNrightOffset,		5,
											XmNalignment,			XmALIGNMENT_END,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		geoDomTextF = XtVaCreateManagedWidget ("UIDataHeaderFormGeoField",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			subjectTextF,
											XmNtopOffset,			10,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNmaxLength,			DBDataSubjectLen - 1,
											XmNcolumns,				DBDataSubjectLen - 1,
											NULL);
		XtAddCallback (geoDomTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextFieldCBK,changed + var++);
		button = XtVaCreateManagedWidget ("UIDataHeaderFormGeoDomainSelectButton",xmPushButtonWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			subjectTextF,
											XmNtopOffset,			10,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			geoDomTextF,
											XmNleftOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		typeMenu,
											XmNrightOffset,		20,
											XmNlabelString,		select,
											XmNuserData,			(XtArgVal) UIDatasetGeoDomain,
											XmNtraversalOn,		false,
											NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UIDataHeaderButtonCBK,geoDomTextF);


		versionTextF = XtVaCreateManagedWidget ("UIDataHeaderFormVersionText",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			subjectTextF,
											XmNtopOffset,			5,
											XmNrightAttachment,	XmATTACH_FORM,
											XmNrightOffset,		10,
											XmNcolumns,				DBDataVersionLen - 1,
											NULL);
		XtAddCallback (versionTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextFieldCBK,changed + var++);
		string = XmStringCreate ((char *) "Version:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormVersion",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			subjectTextF,
											XmNtopOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		versionTextF,
											XmNrightOffset,		5,
											XmNalignment,			XmALIGNMENT_END,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		separator = XtVaCreateManagedWidget ("UIDataHeaderFormSeparator",xmSeparatorWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			geoDomTextF,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_FORM,
											XmNleftOffset,			20,
											XmNrightAttachment,	XmATTACH_FORM,
											XmNrightOffset,		20,
											NULL);
		string = XmStringCreate ((char *) "Citation Reference:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormCitRefLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			separator,
											XmNtopOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		nameTextF,
											XmNrightOffset,		5,
											XmNalignment,			XmALIGNMENT_END,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		citRefText = XtVaCreateManagedWidget ("UIDataHeaderFormCitRefText",xmTextWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			separator,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNmaxLength,			DBDataCitationRefLen - 1,
											XmNrows,					DBDataCitationRefLen / DBDataNameLen,
											XmNcolumns,				DBDataNameLen - 1,
											XmNeditMode,			XmMULTI_LINE_EDIT,
											XmNwordWrap,			true,
											NULL);
		XtAddCallback (citRefText,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextCBK,changed + var++);
		string = XmStringCreate ((char *) "Citation Institute:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormInstRefLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			citRefText,
											XmNtopOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		nameTextF,
											XmNrightOffset,		5,
											XmNalignment,			XmALIGNMENT_END,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		citInstTextF = XtVaCreateManagedWidget ("UIDataHeaderFormCitInstText",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			citRefText,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNmaxLength,			DBDataCitationInstLen - 1,
											XmNcolumns,				DBDataNameLen - 1,
											NULL);
		XtAddCallback (citInstTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextFieldCBK,changed + var++);
		string = XmStringCreate ((char *) "Source Institue:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormSourceInstLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			citInstTextF,
											XmNtopOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		nameTextF,
											XmNrightOffset,		5,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		srcInstTextF = XtVaCreateManagedWidget ("UIDataHeaderFormSourceInstText",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			citInstTextF,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNmaxLength,			DBDataSourceInstLen - 1,
											XmNcolumns,				DBDataNameLen - 1,
											NULL);
		XtAddCallback (srcInstTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextFieldCBK,changed + var++);
		string = XmStringCreate ((char *) "Source Person:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormSourcePersonLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			srcInstTextF,
											XmNtopOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		nameTextF,
											XmNrightOffset,		5,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		srcPersTextF = XtVaCreateManagedWidget ("UIDataHeaderFormSourcePersText",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			srcInstTextF,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNmaxLength,			DBDataSourcePersonLen - 1,
											XmNcolumns,				DBDataSourcePersonLen - 1,
											NULL);
		XtAddCallback (srcPersTextF,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextFieldCBK,changed + var++);
		string = XmStringCreate ((char *) "Last Modification:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormVersion",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			srcInstTextF,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			srcPersTextF,
											XmNleftOffset,			10,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Not Set",UICharSetNormal);
		XtVaCreateManagedWidget ("UIDataHeaderFormVersion",xmLabelGadgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			srcInstTextF,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNwidth,				100,
											XmNrecomputeSize,		false,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Comment:",UICharSetBold);
		label = XtVaCreateManagedWidget ("UIDataHeaderFormCommentLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,		XmATTACH_WIDGET,
											XmNtopWidget,			srcPersTextF,
											XmNtopOffset,			5,
											XmNrightAttachment,	XmATTACH_WIDGET,
											XmNrightWidget,		nameTextF,
											XmNrightOffset,		5,
											XmNlabelString,		string,
											NULL);
		XmStringFree (string);
		commentText = XtVaCreateManagedWidget ("UIDataHeaderFormCommentText",xmTextWidgetClass,mainForm,
											XmNtopAttachment,	XmATTACH_WIDGET,
											XmNtopWidget,			srcPersTextF,
											XmNtopOffset,			5,
											XmNleftAttachment,	XmATTACH_WIDGET,
											XmNleftWidget,			label,
											XmNleftOffset,			5,
											XmNbottomAttachment,	XmATTACH_FORM,
											XmNtopOffset,			10,
											XmNmaxLength,			DBDataCommentLen - 1,
											XmNrows,					DBDataCommentLen / DBDataNameLen,
											XmNcolumns,				DBDataNameLen - 1,
											XmNeditMode,			XmMULTI_LINE_EDIT,
											XmNwordWrap,			true,
											NULL);
		XtAddCallback (commentText,XmNvalueChangedCallback,(XtCallbackProc) _UIDataHeaderTextCBK,changed + var++);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&save);
		XmStringFree (select);
		}

	edit = true;
	for (i = sizeof (_UIDataTypeList) / sizeof (UIButtonList) - 1;i >= 0; --i)
		if ((_UIDataTypeList [i].ID & data->Type ()) ==  data->Type ())
			{
			XtSetSensitive (_UIDataTypeList	[i].Button,true);
			XtVaSetValues (typeMenu, XmNmenuHistory, _UIDataTypeList	[i].Button,NULL);
			dataType = _UIDataTypeList [i].ID;
			}
		else	XtSetSensitive (_UIDataTypeList	[i].Button,false);

	XtVaSetValues (nameTextF,XmNuserData, data->Name (), NULL);
	XmTextFieldSetString (nameTextF,data->Name ());
	XmTextFieldSetEditable (nameTextF,edit);

	XtVaSetValues (subjectTextF,XmNuserData, data->Document (DBDocSubject), NULL);
	XmTextFieldSetString (subjectTextF,data->Document (DBDocSubject));
	XmTextFieldSetEditable (subjectTextF,edit);

	XtVaSetValues (geoDomTextF,XmNuserData, data->Document (DBDocGeoDomain), NULL);
	XmTextFieldSetString (geoDomTextF,data->Document (DBDocGeoDomain));
	XmTextFieldSetEditable (geoDomTextF,edit);

	XtVaSetValues (versionTextF,XmNuserData, data->Document (DBDocVersion), NULL);
	XmTextFieldSetString (versionTextF, data->Document (DBDocVersion));
	XmTextFieldSetEditable (versionTextF,edit);

	XtVaSetValues (citRefText,XmNuserData, data->Document (DBDocCitationRef), NULL);
	XmTextSetString 	(citRefText,data->Document (DBDocCitationRef) );
	XmTextSetEditable (citRefText,edit);

	XtVaSetValues (citInstTextF,XmNuserData, data->Document (DBDocCitationInst), NULL);
	XmTextFieldSetString (citInstTextF,data->Document (DBDocCitationInst));
	XmTextFieldSetEditable (citInstTextF,edit);

	XtVaSetValues (srcInstTextF,XmNuserData, data->Document (DBDocSourceInst), NULL);
	XmTextFieldSetString (srcInstTextF, data->Document (DBDocSourceInst));
	XmTextFieldSetEditable (srcInstTextF,edit);

	XtVaSetValues (srcPersTextF,XmNuserData, data->Document (DBDocSourcePerson), NULL);
	XmTextFieldSetString (srcPersTextF, data->Document (DBDocSourcePerson));
	XmTextFieldSetEditable (srcPersTextF,edit);

	XtVaSetValues (commentText,XmNuserData, data->Document (DBDocComment), NULL);
	XmTextSetString (commentText, data->Document (DBDocComment));
	XmTextSetEditable (commentText,edit);

	save = false;
	XmProcessTraversal (nameTextF,XmTRAVERSE_CURRENT);
	UIDialogFormPopup (dShell);
	for (i = 0;i < (int) (sizeof (changed) / sizeof (int));++i)  changed [i] = false;
	while (UILoop ())
		{
		var = 0;
		text = XmTextFieldGetString (nameTextF);
		if (strlen (text) == 0)
			XtSetSensitive (UIDialogFormGetOkButton (dShell),false);
		else
			{
			for (i = 0;i < (int) (sizeof (changed) / sizeof (int));++i)
				var = changed [i] ? var + 1 : var;
			XtSetSensitive (UIDialogFormGetOkButton (dShell),var > 0);
			}
		XtFree (text);
		}
	UIDialogFormPopdown (dShell);

	if (save)
		{
		text = XmTextFieldGetString (nameTextF); 		data->Name 		(text);							XtFree (text);
		text = XmTextFieldGetString (subjectTextF);	data->Document (DBDocSubject,text);			XtFree (text);
		text = XmTextFieldGetString (geoDomTextF);	data->Document (DBDocGeoDomain,text);		XtFree (text);
		text = XmTextFieldGetString (versionTextF);	data->Document (DBDocVersion,text);			XtFree (text);
		text = XmTextGetString (citRefText);			data->Document (DBDocCitationRef,text);	XtFree (text);
		text = XmTextFieldGetString (citInstTextF);	data->Document (DBDocCitationInst,text);	XtFree (text);
		text = XmTextFieldGetString (srcInstTextF);	data->Document (DBDocSourceInst,text);		XtFree (text);
		text = XmTextFieldGetString (srcPersTextF);	data->Document (DBDocSourcePerson,text);	XtFree (text);
		text = XmTextGetString (commentText);			data->Document (DBDocComment,text);			XtFree (text);
		if (data->Type () != dataType) data->Type (dataType);
		return (true);
		}
	else	return (false);
	}
