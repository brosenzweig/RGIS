/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISAnPoint.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <rgis.H>

void RGISAnalysePointSTNCharCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBObjTable *pointTable = dbData->Table (DBrNItems);
	DBObjTableField *cellIDFLD			= pointTable->Field (RGlibCellID);
	DBObjTableField *basinFLD			= pointTable->Field (DBrNBasin);
	DBObjTableField *basinNameFLD		= pointTable->Field (RGlibBasinName);
	DBObjTableField *orderFLD			= pointTable->Field (DBrNOrder);
	DBObjTableField *colorFLD			= pointTable->Field (RGlibColor);
	DBObjTableField *basinCellsFLD	= pointTable->Field (RGlibCellNum);
	DBObjTableField *basinLengthFLD	= pointTable->Field (RGlibLength);
	DBObjTableField *basinAreaFLD		= pointTable->Field (RGlibArea);
	DBObjTableField *interAreaFLD		= pointTable->Field (RGlibInterStation);
	DBObjTableField *nextStationFLD	= pointTable->Field (RGlibNextStation);
	UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,pointTable));

	widget = widget; workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "STN Characteristics");
	if ((RGlibPointSTNCharacteristics (dbData) == DBSuccess) && (tableCLS != (UITable *) NULL))
		{
		if (cellIDFLD      == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibCellID));
		if (basinFLD       == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (DBrNBasin));
		if (basinNameFLD   == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibBasinName));
		if (orderFLD       == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (DBrNOrder));
		if (colorFLD       == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibColor));
		if (basinCellsFLD  == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibCellNum));
		if (basinLengthFLD == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibLength));
		if (basinAreaFLD   == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibArea));
		if (interAreaFLD   == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibInterStation));
		if (nextStationFLD == (DBObjTableField *) NULL) tableCLS->AddField (pointTable->Field (RGlibNextStation));
		tableCLS->Draw ();
		}
	UIPauseDialogClose ();
	}

void RGISAnalysePointSTNPointsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *srcText;
	static DBInt cont, diffMethod;
	DBDataset *dataset;
	DBObjData *dbData;
	DBObjTable *table;
	DBObjTableField *field = (DBObjTableField *) NULL;
	static Widget dShell = NULL, mainForm, button;
	static Widget srcTextF, dstTextF, cmpTextF, errTextF;
	static Widget menu, mButton;
	widget = widget; callData = callData;

	if (dShell == (Widget) NULL)
		{
		XmString string;

		dShell = UIDialogForm ((char *) "Area Field",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsSrcButton",xmPushButtonWidgetClass,mainForm,
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
		srcTextF = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsSrcTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,srcTextF);
		string = XmStringCreate ((char *) "Area Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsSrcLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			srcTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsSrcButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				srcTextF,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		dstTextF = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsDstTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,dstTextF);
		string = XmStringCreate ((char *) "Target Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsDstLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			dstTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsErrorButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				dstTextF,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		cmpTextF = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsCompTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,cmpTextF);

		string = XmStringCreate ((char *) "Compare Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsDstLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			cmpTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsErrorButton",xmPushButtonWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				cmpTextF,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmarginHeight,			5,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								XmNuserData,				DBTableFieldIsNumeric,
								NULL);
		XmStringFree (string);
		errTextF = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsErrorTextF",xmTextFieldWidgetClass,mainForm,
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
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxObjectSelectCBK,errTextF);
		string = XmStringCreate ((char *) "Error Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsDstLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			errTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		menu = XmCreatePulldownMenu (mainForm,(char *) "RGISAnalysePointSTNPointsMenu",NULL,0);
		string = XmStringCreate ((char *) "Plain",UICharSetNormal);
		mButton = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsMenuButton",xmPushButtonWidgetClass,menu,
								XmNlabelString,			string,
								XmNuserData,				&diffMethod,
								NULL);
		XmStringFree (string);
		XtAddCallback (mButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(XtPointer) 0);
		string = XmStringCreate ((char *) "Simple %",UICharSetNormal);
		mButton = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsMenuButton",xmPushButtonWidgetClass,menu,
								XmNlabelString,			string,
								XmNuserData,				&diffMethod,
								NULL);
		XmStringFree (string);
		XtAddCallback (mButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(XtPointer) 1);
		string = XmStringCreate ((char *) "Symmetric %",UICharSetNormal);
		mButton = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsMenuButton",xmPushButtonWidgetClass,menu,
								XmNlabelString,			string,
								XmNuserData,				&diffMethod,
								NULL);
		XmStringFree (string);
		XtAddCallback (mButton,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(XtPointer) 2);
		string = XmStringCreate ((char *) "Difference Method:",UICharSetBold);
		menu = XtVaCreateManagedWidget ("RGISAnalysePointSTNPointsMenu",xmRowColumnWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				button,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNlabelString,			string,
								XmNsubMenuId,				menu,
								NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&cont);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),true);
		}

	dataset = UIDataset ();
	dbData = dataset->Data ();
	table = dbData->Table (DBrNItems);
	XtVaSetValues (srcTextF,XmNuserData,table->Fields (),NULL);
	XtVaSetValues (dstTextF,XmNuserData,table->Fields (),NULL);
	XtVaSetValues (cmpTextF,XmNuserData,table->Fields (),NULL);
	XtVaSetValues (errTextF,XmNuserData,table->Fields (),NULL);
	UIDialogFormPopup (dShell);
	cont = false;
	while (UILoop ())
		{
		if ((strlen (srcText = XmTextFieldGetString (srcTextF)) > 0) &&
			 ((field = table->Field (srcText)) != (DBObjTableField *) NULL))
				XtSetSensitive (UIDialogFormGetOkButton (dShell),True);
		else	XtSetSensitive (UIDialogFormGetOkButton (dShell),False);
		XtFree (srcText);
		if ((strlen (srcText = XmTextFieldGetString (cmpTextF)) > 0) &&
			 ((field = table->Field (srcText)) != (DBObjTableField *) NULL))
			{
			XtSetSensitive (button,True);
			XtSetSensitive (errTextF,True);
			XtSetSensitive (menu,True);
			}
		else
			{
			XmTextFieldSetString (errTextF,(char *) "");
			XtSetSensitive (button,False);
			XtSetSensitive (errTextF,False);
			XtSetSensitive (menu,False);
			}
		XtFree (srcText);
		}
	UIDialogFormPopdown (dShell);
	if (cont)
		{
		char dataName [DBDataNameLen + 5];
		DBObjData *netData = dbData->LinkedData ();

		sprintf (dataName,"%s STN",dbData->Name ());
		dbData = new DBObjData (*dbData);
		dbData->Name (dataName);
		if (UIDataHeaderForm (dbData))
			{
			char *dstText;
			DBObjTable *groups;
			DBObjRecord *pointRec;

			srcText = XmTextFieldGetString (srcTextF);
			dbData->LinkedData (netData);
			table = dbData->Table (DBrNItems);
			groups = dbData->Table (DBrNGroups);
			for (pointRec = table->First ();pointRec != (DBObjRecord *) NULL;pointRec = table->Next ())
				if ((pointRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
					{
					if (groups != (DBObjTable *) NULL)
						groups->Delete (groups->Item (pointRec->RowID ()));
					table->Delete (pointRec);
					pointRec = table->Next (DBBackward);
					}

			field = table->Field (srcText);
			XtFree (srcText);
			RGlibPointSTNCoordinates (dbData,field);
			UIPauseDialogOpen ((char *) "Moving Points");
			RGlibPointSTNCharacteristics (dbData);
			UIPauseDialogClose ();
			srcText = XmTextFieldGetString (srcTextF);
			if (strlen (dstText = XmTextFieldGetString (dstTextF)) > 0)
				{
				srcText = XmTextFieldGetString (srcTextF);
				RGlibGenFuncTopoSubtract (table,RGlibNextStation,srcText,dstText);
				XtFree (dstText);
				}
			if (strlen (dstText = XmTextFieldGetString (errTextF)) > 0)
				{
				char *cmpText;
				srcText = XmTextFieldGetString (srcTextF);
				cmpText = XmTextFieldGetString (cmpTextF);
				RGlibGenFuncFieldCompare (table,srcText,cmpText,dstText,diffMethod);
				XtFree (dstText);
				XtFree (cmpText);
				}
			XtFree (srcText);
			UI2DViewRedrawAll ();
			workspace->CurrentData (dbData);
			}
		else delete dbData;
		}
	}

void RGISAnalysePointSubbasinCenterCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *pntData =dataset->Data ();
	DBObjTable *pointTable = pntData->Table (DBrNItems);
	DBObjTableField *massCoordXFLD = pointTable->Field (RGlibMassCoordX);
	DBObjTableField *massCoordYFLD = pointTable->Field (RGlibMassCoordY);
	DBObjData *netData = pntData->LinkedData ();
	UITable *tableCLS = (UITable *) pntData->Display (UITableName (pntData,pointTable));

	widget = widget;	workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Subbasin Centers");
	if ((RGlibPointSubbasinCenter (pntData, netData) == DBSuccess) && (tableCLS != (UITable *) NULL))
		{
		if (massCoordXFLD == NULL) tableCLS->AddField (pointTable->Field (RGlibMassCoordX));
		if (massCoordYFLD == NULL) tableCLS->AddField (pointTable->Field (RGlibMassCoordY));
		tableCLS->Draw ();
		}
	UIPauseDialogClose ();
	}

void RGISAnalysePointInterStationTSCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset = UIDataset ();
	DBObjMetaEntry *metaEntry;
	DBObjData *pntData = dataset->Data (), *tsData;
	DBObjTable *relateTBL = pntData->Table (DBrNRelations);
	DBObjRecord *relateRec;
	DBObjTableField *relDataFLD;
	DBObjTableField *relateFLD;
	DBObjTableField *joinFLD;
	static Widget selectWidget = (Widget) NULL;

	widget = widget; workspace = workspace; callData = callData;

	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ((char *) "Select Time Series");

	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) relateTBL)) == (char *) NULL) return;
	if ((relateRec = relateTBL->Item (selection)) == (DBObjRecord *) NULL)
		{ fprintf (stderr,"Relate Record Error in: RGISAnalysePointInterfluvialTSCBK ()\n"); return; }

	relDataFLD = relateTBL->Field (DBrNRelateData);
	relateFLD = relateTBL->Field (DBrNRelateField);
	joinFLD	= relateTBL->Field (DBrNRelateJoinField);

	if ((tsData = dataset->Data (relDataFLD->String (relateRec))) == (DBObjData *) NULL)
		{
		if ((metaEntry = dataset->Meta (relDataFLD->String (relateRec))) == (DBObjMetaEntry *) NULL)
			{ fprintf (stderr,"Meta Enrty Finding Error in: RGISAnalysePointInterfluvialTSCBK ()\n"); return; }
		tsData = new DBObjData ();
		if (tsData->Read (metaEntry->FileName ()) != DBSuccess) return;
		}
	workspace->CurrentData (tsData);

	UIPauseDialogOpen ((char *) "Calculating");
	RGlibPointInterStationTS (pntData,tsData,relateFLD->String (relateRec),joinFLD->String (relateRec));
	UIPauseDialogClose ();
	}

void RGISAnalysePointSubbasinCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *pntData = dataset->Data ();
	DBObjData *netData = pntData->LinkedData ();
	DBObjData *grdData = DBNetworkToGrid (netData,DBTypeGridDiscrete);

	widget = widget; callData = callData;

	grdData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	grdData->Document (DBDocSubject,"Subbasins");
	if (UIDataHeaderForm (grdData))
		{
		UIPauseDialogOpen ((char *) "Creating Subbasin Grid");
		if (DBPointToGrid (pntData,netData,grdData) == DBFault)	delete grdData;
		else workspace->CurrentData (grdData);
		UIPauseDialogClose ();
		}
	else delete grdData;
	}

static DBInt _RGISAnalyseTestNetwork (const DBObject *object)

	{ return (((DBObjData *) object)->Type () == DBTypeNetwork); }

void RGISAnalysePointSubbasinStatsCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset  = UIDataset ();
	DBObjData *pntData  = dataset->Data ();
	DBObjData *grdData  = pntData->LinkedData ();
	DBObjData *netData;
	DBObjData *tblData;
	static Widget selectWidget = (Widget) NULL;

	widget = widget; callData = callData;

	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ((char *) "Select Network");
	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) dataset->DataList (),_RGISAnalyseTestNetwork)) == (char *) NULL) return;
	if ((netData = dataset->Data (selection)) == (DBObjData *) NULL) return;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,pntData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,(char *) "Subbasin Statistics");
	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Statistics");
		if (RGlibPointSubbasinStats (pntData,netData,grdData,tblData) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else delete tblData;
	return;
	}

void RGISAnalysePointSubbasinHistCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBDataset *dataset  = UIDataset ();
	DBObjData *pntData  = dataset->Data ();
	DBObjData *grdData  = pntData->LinkedData ();
	DBObjData *netData;
	DBObjData *tblData;
	static Widget selectWidget = (Widget) NULL;

	widget = widget; callData = callData;

	if (selectWidget == (Widget) NULL) selectWidget = UISelectionCreate ((char *) "Select Network");
	if ((selection = UISelectObject (selectWidget,(DBObjectLIST<DBObject> *) dataset->DataList (),_RGISAnalyseTestNetwork)) == (char *) NULL) return;
	if ((netData = dataset->Data (selection)) == (DBObjData *) NULL) {  return; }

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,pntData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Subbasin Histogram");
	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Histogram");
		if (RGlibPointSubbasinHist (pntData,netData,grdData,tblData) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else	delete tblData;
	}
