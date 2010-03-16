/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGISAnNetwork.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <rgis.H>

void RGISAnNetworkBasinGridCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *grdData;

	grdData = DBNetworkToGrid (netData,DBTypeGridDiscrete);
	grdData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	grdData->Document (DBDocSubject,netData->Document (DBDocSubject));
	if (UIDataHeaderForm (grdData))
		{
		UIPauseDialogOpen ((char *) "Converting Network to Basin Grid");
		if (RGlibNetworkBasinGrid (netData,grdData) == DBSuccess)
			workspace->CurrentData (grdData);
		else delete grdData;
		UIPauseDialogClose ();
		}
	else	delete grdData;
	}

void RGISAnNetworkToGridCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *selection;
	DBInt dataType;
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjTable *cellTable = netData->Table (DBrNCells);
	DBObjTableField *field;
	DBObjData *grdData;
	static Widget select = NULL;

	if (select == NULL) select = UISelectionCreate ((char *) "Field Selection");
	selection = UISelectObject (select,(DBObjectLIST<DBObject> *) (cellTable->Fields ()),DBTableFieldIsSimple);
	if (selection == (char *) NULL) return;
	if ((field = cellTable->Field (selection)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Corrupt Network Dataset in: RGISAnNetworkToGridCBK ()\n"); return; }

	if (DBTableFieldIsCategory (field) && DBTableFieldIsNumeric (field))
		{
		DBObjData *tmpData;
		tmpData = new DBObjData ("",DBTypeGrid);
		tmpData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
		tmpData->Document (DBDocSubject,field->Name ());
		if (UIDataHeaderForm (tmpData))
			{
			grdData = DBNetworkToGrid (netData,tmpData->Type ());
			grdData->Name (tmpData->Name ());
			grdData->Document (DBDocGeoDomain,tmpData->Document (DBDocGeoDomain));
			grdData->Document (DBDocSubject,tmpData->Document (DBDocSubject));
			grdData->Document (DBDocVersion,tmpData->Document (DBDocVersion));
			grdData->Document (DBDocCitationRef,tmpData->Document (DBDocCitationRef));
			grdData->Document (DBDocCitationInst,tmpData->Document (DBDocCitationInst));
			grdData->Document (DBDocSourceInst,tmpData->Document (DBDocSourceInst));
			grdData->Document (DBDocSourcePerson,tmpData->Document (DBDocSourcePerson));
			grdData->Document (DBDocOwnerPerson,tmpData->Document (DBDocOwnerPerson));
			grdData->Document (DBDocComment,tmpData->Document (DBDocComment));
			delete tmpData;
			}
		else { return; }
		}
	else
		{
		if (DBTableFieldIsCategory (field))		dataType = DBTypeGridDiscrete;
		else if (DBTableFieldIsNumeric (field))	dataType = DBTypeGridContinuous;
		else
			{
			fprintf (stderr,"Invalid field type in: RGISAnNetworkToGridCBK ()\n");
			return;
			}
		grdData = DBNetworkToGrid (netData,dataType);
		if (UIDataHeaderForm (grdData) == false) { delete grdData; return; }
		}

	UIPauseDialogOpen ((char *) "Converting Network to Grid");
	if (RGlibNetworkToGrid (netData,field,grdData) == DBSuccess) workspace->CurrentData (grdData);
	else delete grdData;
	UIPauseDialogClose ();
	}

void RGISAnNetworkStationsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *areaText, *toleranceText;
	DBFloat area, tolerance;
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	static int proc;
	static Widget dShell = (Widget) NULL, areaField, toleranceField;

	if (dShell == (Widget) NULL)
		{
		Widget mainForm, label;
		XmString string;

		dShell = UIDialogForm ((char *) "Network Stations");
		mainForm = UIDialogFormGetMainForm (dShell);
		areaField = XtVaCreateManagedWidget ("RGISAnNetworkStationsAreaTextF",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_FORM,
											XmNtopOffset,				10,
											XmNrightAttachment,		XmATTACH_FORM,
											XmNrightOffset,			10,
											XmNmaxLength,				DBStringLength / 2,
											XmNcolumns,					DBStringLength / 2,
											NULL);
		string = XmStringCreate ((char *) "Subbasin Area [km2]:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISAnNetworkStationsAreaLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				areaField,
											XmNleftAttachment,		XmATTACH_FORM,
											XmNleftOffset,				10,
											XmNrightAttachment,		XmATTACH_WIDGET,
											XmNrightWidget,			areaField,
											XmNrightOffset,			10,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			areaField,
											XmNlabelString,			string,
											XmNmarginWidth,			5,
											NULL);
		XmStringFree (string);
		toleranceField = XtVaCreateManagedWidget ("RGISAnNetworkStationsToleranceTextF",xmTextFieldWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_WIDGET,
											XmNtopWidget,				areaField,
											XmNtopOffset,				5,
											XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNleftWidget,				areaField,
											XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNrightWidget,			areaField,
											XmNbottomAttachment,		XmATTACH_FORM,
											XmNbottomOffset,			10,
											XmNmaxLength,				DBStringLength / 2,
											XmNcolumns,					DBStringLength / 2,
											NULL);
		string = XmStringCreate ((char *) "Tolerance [%]:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISAnNetworkStationsToleranceLabel",xmLabelWidgetClass,mainForm,
											XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
											XmNtopWidget,				toleranceField,
											XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNleftWidget,				label,
											XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
											XmNbottomWidget,			toleranceField,
											XmNlabelString,			string,
											XmNmarginWidth,			5,
											NULL);
		XmStringFree (string);

		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&proc);
		XtAddCallback (UIDialogFormGetCancelButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanFalseCBK,&proc);
		}
	proc = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		areaText = XmTextFieldGetString (areaField);
		toleranceText = XmTextFieldGetString (toleranceField);
		if ((strlen (areaText) > 0) 		&& (sscanf (areaText,"%lf",&area) == 1) &&
			 (strlen (toleranceText) > 0) && (sscanf (toleranceText,"%lf",&tolerance) == 1))
				XtSetSensitive (UIDialogFormGetOkButton (dShell),true);
		else	XtSetSensitive (UIDialogFormGetOkButton (dShell),false);
		XtFree (areaText);
		XtFree (toleranceText);
		}
	UIDialogFormPopdown (dShell);
	if (proc)
		{
		DBObjData *pntData;
		pntData = new DBObjData ("",DBTypeVectorPoint);
		pntData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
		pntData->Document (DBDocSubject,GHAASSubjStations);
		if (UIDataHeaderForm (pntData))
			{
			UIPauseDialogOpen ((char *) "Creating Stations");
			if (RGlibNetworkStations (netData,area,tolerance,pntData) == DBSuccess)
				{
				workspace->CurrentData  (pntData);
				UI2DViewRedrawAll ();
				}
			else delete pntData;
			UIPauseDialogClose ();
			}
		else	delete pntData;
		}
	}

void RGISAnNetworkAccumulateCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *inGridData, *outGridData;

	widget = widget; callData = callData;

	if ((inGridData = netData->LinkedData ()) == (DBObjData *) NULL)
		{ fprintf (stderr,"Null Linked Data in: _RGISAnnNetworkAccumulate ()\n"); return; }

	outGridData = DBNetworkToGrid (netData,DBTypeGridContinuous);
	outGridData->Document (DBDocSubject,inGridData->Document (DBDocSubject));
	if (UIDataHeaderForm (outGridData))
		{
		UIPauseDialogOpen ((char *) "Accumulating Grid");
		if (RGlibNetworkAccumulate (netData,inGridData,outGridData) == DBSuccess)
			workspace->CurrentData  (outGridData);
		else	delete outGridData;
		UIPauseDialogClose ();
	 	}
	else	delete outGridData;
	}

void RGISAnNetworkUpStreamAvgCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *inGridData, *outGridData;

	widget = widget; callData = callData;

	if ((inGridData = netData->LinkedData ()) == (DBObjData *) NULL)
		{ fprintf (stderr,"Null Linked Data in: RGISAnNetworkUpStreamAvgCBK ()\n"); return; }

	outGridData = DBNetworkToGrid (netData,DBTypeGridContinuous);
	outGridData->Document (DBDocSubject,inGridData->Document (DBDocSubject));
	if (UIDataHeaderForm (outGridData))
		{
		UIPauseDialogOpen ((char *) "Upstream Average");
		if (RGlibNetworkUpStreamAvg (netData,inGridData,outGridData) == DBSuccess)
			workspace->CurrentData  (outGridData);
		else	delete outGridData;
		UIPauseDialogClose ();
		}
	else delete outGridData;
	}

void RGISAnNetworkCellSlopesCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *inGridData, *outGridData;

	widget = widget; callData = callData;

	if ((inGridData = netData->LinkedData ()) == (DBObjData *) NULL)
		{ fprintf (stderr,"Null Linked Data in: RGISAnNetworkCellSlopesCBK ()\n"); return; }

	outGridData = DBNetworkToGrid (netData,DBTypeGridContinuous);
	outGridData->Document (DBDocGeoDomain,inGridData->Document (DBDocGeoDomain));
	outGridData->Document (DBDocSubject,"Slope");
	if (UIDataHeaderForm (outGridData))
		{
		UIPauseDialogOpen ((char *) "Calculating Slopes");
		if (RGlibNetworkCellSlopes (netData,inGridData,outGridData) == DBSuccess)
			workspace->CurrentData  (outGridData);
		else	delete outGridData;
		UIPauseDialogClose ();
	 	}
	else	delete outGridData;
	}

void RGISAnNetworkBasinProfCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *gridData, *tblData;

	widget = widget; callData = callData;

	if ((gridData = netData->LinkedData ()) == (DBObjData *) NULL)
		{ fprintf (stderr,"Null Linked Data in: RGISAnNetworkBasinProfCBK ()\n"); return; }

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Basin Profiles");

	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Creating Basin Profiles");
		if (RGlibNetworkBasinProf (netData,gridData,tblData) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else delete tblData;
	}

void RGISAnNetworkBasinStatsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset  = UIDataset ();
	DBObjData *netData  = dataset->Data ();
	DBObjData *grdData = netData->LinkedData ();
	DBObjData *tblData;

	widget = widget; callData = callData;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Basin Statistics");

	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Statistics");
		if (RGlibNetworkBasinStats (netData,grdData,tblData) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else	delete tblData;
	}

void RGISAnNetworkHeadStatsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *grdData = netData->LinkedData ();
	DBObjData *tblData;

	widget = widget; callData = callData;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Head Statistics");

	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Statistics");
		if (RGlibNetworkHeadStats (netData,grdData,tblData,false) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else delete tblData;
	}

void RGISAnNetworkDivideStatsCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData = dataset->Data ();
	DBObjData *grdData = netData->LinkedData ();
	DBObjData *tblData;

	widget = widget; callData = callData;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Head Statistics");

	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Statistics");
		if (RGlibNetworkHeadStats (netData,grdData,tblData,true) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else delete tblData;
	}

void RGISAnNetworkHistogramCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset  = UIDataset ();
	DBObjData *netData  = dataset->Data ();
	DBObjData *grdData = netData->LinkedData ();
	DBObjData *tblData = new DBObjData ("",DBTypeTable);

	tblData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Elevation Distribution");
	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Histogram");
		if (RGlibNetworkHistogram (netData,grdData,tblData) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else	delete tblData;
	}

void RGISAnNetworkBasinDistribCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset  = UIDataset ();
	DBObjData *netData  = dataset->Data ();
	DBObjData *grdData = netData->LinkedData ();
	DBObjData *tblData;

	widget = widget; callData = callData;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,netData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,"Category Distribution");

	if (UIDataHeaderForm (tblData))
		{
		UIPauseDialogOpen ((char *) "Calculating Statistics");
		if (RGlibNetworkBasinDistrib (netData,grdData,tblData) == DBSuccess)
			workspace->CurrentData  (tblData);
		else	delete tblData;
		UIPauseDialogClose ();
		}
	else delete  tblData;
	}

void RGISAnNetworkCellSampleGridCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *netData  = dataset->Data (), *grdData = netData->LinkedData ();

	widget = widget;	workspace = workspace; callData = callData;
	UIPauseDialogOpen ((char *) "Cell Sampling");
	RGlibGridSampling (netData,grdData);
	UIPauseDialogClose ();
	}
