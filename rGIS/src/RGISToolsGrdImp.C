/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISToolsGrdImp.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <string.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleBG.h>
#include <Xm/PushBG.h>
#include <rgis.H>


static int _RGISToolsImportWidgetMaxWidth (Widget widget,int maxWidth)

	{
	Dimension width;

	XtVaGetValues (widget,XmNwidth, &width, NULL);
	return (maxWidth > width ? maxWidth : width);
	}

static void _RGISToolsImportSetButtonWidth (Widget menu,Dimension width)

	{
	Cardinal childrenNum, child;
	WidgetList children;

	XtVaGetValues (menu,XmNsubMenuId, &menu, NULL);
	XtVaGetValues (menu,XmNchildren, &children,XmNnumChildren, &childrenNum, NULL);

	for (child = 0;child < childrenNum;++child)
		XtVaSetValues (children [child],XmNwidth, width, XmNrecomputeSize, False, NULL);
	}

static void _RGISToolsImpGridMapMenuCBK (Widget widget,Widget mapWidget, XmAnyCallbackStruct *callData)

	{
	DBInt *val;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, 	&val, NULL);
	if ((*val & 0x01) == 0x01)
			XtUnmapWidget (mapWidget);
	else	XtMapWidget (mapWidget);
	}

static void _RGISToolsImpGridSetMapCallback (Widget menu,Widget mapWidget)

	{
	Cardinal childrenNum, child;
	WidgetList children;

	XtVaGetValues (menu,XmNsubMenuId, &menu, NULL);
	XtVaGetValues (menu,XmNchildren, &children,XmNnumChildren, &childrenNum, NULL);
	for (child = 0;child < childrenNum;++child)
		XtAddCallback (children [child],XmNactivateCallback, (XtCallbackProc) _RGISToolsImpGridMapMenuCBK,(void *) mapWidget);
	}

#define RGISLaoutByRow			0
#define RGISLaoutByCol			1

#define RGISItemOrderTopDown	0
#define RGISItemOrderBottomUp	1

#define RGISReferenceCenter	0
#define RGISReferenceLLCorner	1

#define RGISGridBinary			0
#define RGISGridASCII			1

#define RGISBinTypeByte			0
#define RGISBinTypeShort		1
#define RGISBinTypeLong			2
#define RGISBinTypeSingle		3
#define RGISBinTypeDouble		4

void RGISToolsImportGridCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	int confOk, colNum, rowNum;
	char *text, listFileName [DBDataFileNameLen], buffer [256];
	double cellWidth, cellHeight, llXCoord, llYCoord;
	static int proc;
	static int layout = 0, rowOrder = 0, fileType = 0, binaryType = 0, byteOrder = 0;
	static int llCellCol, llCellRow = 0, skipHeader = 0, skipPad = 0, listFile = false;
	static float missingVal = DBDefaultMissingFloatVal;
	static Widget dShell = NULL, mainForm;
	static Widget rowNumFLD, colNumFLD, cellWidthFLD, llCellRowFLD, llCellColFLD, cellHeightFLD, llXCoordFLD, llYCoordFLD;
	static Widget missingValFLD, skipHeaderFLD, skipPadFLD, listFileNameFLD, listFileTGL;

	widget = widget; callData = callData;

	if (dShell == (Widget) NULL)
		{
		Dimension buttonWidth = 0, optionWidth = 0, widgetHeight;
		Widget rowColWGT, colWGT, label, button;
		Widget layoutMenu, rowOrderMenu, fileTypeMenu;
		Widget binaryTypeMenu, byteOrderMenu;
		XmString string;

		dShell = UIDialogForm ((char *) "Grid Import",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		rowColWGT = XtVaCreateManagedWidget ("RGISToolsGridImportSubform",xmRowColumnWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNshadowThickness,		0,
								XmNpacking,					XmPACK_COLUMN,
								XmNspacing,					4,
								XmNnumColumns,				2,
								XmNorientation,			XmHORIZONTAL,
								NULL);

		string = XmStringCreate ((char *) " ",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportColNumLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Columns",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportColNumLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Rows",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportRowNumLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Dimensions:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISToolsGridImportDimensionLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		colNumFLD = XtVaCreateManagedWidget ("RGISToolsGridImportColNumFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x02,
								XmNcolumns,					DBStringLength >> 0x02,
								NULL);
		rowNumFLD = XtVaCreateManagedWidget ("RGISToolsGridImportRowNumFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x02,
								XmNcolumns,					DBStringLength >> 0x02,
								NULL);

		colWGT = XtVaCreateManagedWidget ("RGISToolsGridImportSubform",xmRowColumnWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				rowColWGT,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				30,
								XmNpacking,					XmPACK_COLUMN,
								XmNspacing,					0,
								XmNorientation,			XmVERTICAL,
								NULL);
		layoutMenu = XmCreatePulldownMenu (colWGT,(char *) "RGISToolsGridImportLayoutMenu",NULL,0);
		string = XmStringCreate ((char *) "by Row",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportLayoutButton",xmPushButtonGadgetClass,layoutMenu,
								XmNuserData,				&layout,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISLaoutByRow);
		string = XmStringCreate ((char *) "by Column",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportFLayoutButton",xmPushButtonGadgetClass,layoutMenu,
								XmNuserData,				&layout,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISLaoutByCol);
		string = XmStringCreate ((char *) "Grid Layout",UICharSetBold);
		layoutMenu = XtVaCreateManagedWidget ("RGISToolsGridImportLayoutMenu",xmRowColumnWidgetClass,colWGT,
								XmNsubMenuId,				layoutMenu,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);
		optionWidth = _RGISToolsImportWidgetMaxWidth (XmOptionLabelGadget (layoutMenu),optionWidth);
		rowOrderMenu = XmCreatePulldownMenu (colWGT,(char *) "RGISToolsGridImportLayoutMenu",NULL,0);
		string = XmStringCreate ((char *) "Top-Down",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportLayoutButton",xmPushButtonGadgetClass,rowOrderMenu,
								XmNuserData,				&rowOrder,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISItemOrderTopDown);
		string = XmStringCreate ((char *) "Bottom-Up",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportFLayoutButton",xmPushButtonGadgetClass,rowOrderMenu,
								XmNuserData,				&rowOrder,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISItemOrderBottomUp);
		string = XmStringCreate ((char *) "Row Order",UICharSetBold);
		rowOrderMenu = XtVaCreateManagedWidget ("RGISToolsGridImportLayoutMenu",xmRowColumnWidgetClass,colWGT,
								XmNsubMenuId,				rowOrderMenu,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);
		optionWidth = _RGISToolsImportWidgetMaxWidth (XmOptionLabelGadget (rowOrderMenu),optionWidth);
		fileTypeMenu = XmCreatePulldownMenu (colWGT,(char *) "RGISToolsGridImportFileTypeMenu",NULL,0);
		string = XmStringCreate ((char *) "Binary",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportFileTypeButton",xmPushButtonGadgetClass,fileTypeMenu,
								XmNuserData,				&fileType,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISGridBinary);
		string = XmStringCreate ((char *) "ASCII",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportFileTypeButton",xmPushButtonGadgetClass,fileTypeMenu,
								XmNuserData,				&fileType,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISGridASCII);
		string = XmStringCreate ((char *) "File Type",UICharSetBold);
		fileTypeMenu = XtVaCreateManagedWidget ("RGISToolsGridImportFileTypeMenu",xmRowColumnWidgetClass,colWGT,
								XmNsubMenuId,				fileTypeMenu,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);
		optionWidth = _RGISToolsImportWidgetMaxWidth (XmOptionLabelGadget (fileTypeMenu),optionWidth);
		binaryTypeMenu = XmCreatePulldownMenu (colWGT,(char *) "RGISToolsGridImportBinaryTypeMenu",NULL,0);
		string = XmStringCreate ((char *) "Byte",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportBinaryTypeButton",xmPushButtonGadgetClass,binaryTypeMenu,
								XmNuserData,				&binaryType,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISBinTypeByte);
		string = XmStringCreate ((char *) "Short Integer",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportBinaryTypeButton",xmPushButtonGadgetClass,binaryTypeMenu,
								XmNuserData,				&binaryType,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISBinTypeShort);
		string = XmStringCreate ((char *) "Long Integer",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportBinaryTypeButton",xmPushButtonGadgetClass,binaryTypeMenu,
								XmNuserData,				&binaryType,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISBinTypeLong);
		string = XmStringCreate ((char *) "Single Float",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportBinaryTypeButton",xmPushButtonGadgetClass,binaryTypeMenu,
								XmNuserData,				&binaryType,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISBinTypeSingle);
		string = XmStringCreate ((char *) "Double Float",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportBinaryTypeButton",xmPushButtonGadgetClass,binaryTypeMenu,
								XmNuserData,				&binaryType,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) RGISBinTypeDouble);
		string = XmStringCreate ((char *) "Binary Type",UICharSetBold);
		binaryTypeMenu = XtVaCreateManagedWidget ("RGISToolsGridImportBinaryTypeMenu",xmRowColumnWidgetClass,colWGT,
								XmNsubMenuId,				binaryTypeMenu,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);
		optionWidth = _RGISToolsImportWidgetMaxWidth (XmOptionLabelGadget (binaryTypeMenu),optionWidth);
		byteOrderMenu = XmCreatePulldownMenu (colWGT,(char *) "RGISToolsGridImportByteOrderMenu",NULL,0);
		string = XmStringCreate ((char *) "Big Endian",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportFileTypeButton",xmPushButtonGadgetClass,byteOrderMenu,
								XmNuserData,				&byteOrder,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) DBByteOrderBIG);
		string = XmStringCreate ((char *) "Little Endian",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportFileTypeButton",xmPushButtonGadgetClass,byteOrderMenu,
								XmNuserData,				&byteOrder,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		buttonWidth = _RGISToolsImportWidgetMaxWidth (button,buttonWidth);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetIntegerCBK,(void *) DBByteOrderLITTLE);
		string = XmStringCreate ((char *) "ByteOrder",UICharSetBold);
		byteOrderMenu = XtVaCreateManagedWidget ("RGISToolsGridImportByteOrderMenu",xmRowColumnWidgetClass,colWGT,
								XmNsubMenuId,				byteOrderMenu,
								XmNlabelString,			string,
								XmNrowColumnType,			XmMENU_OPTION,
								XmNtraversalOn,			False,
								NULL);
		XmStringFree (string);
		optionWidth = _RGISToolsImportWidgetMaxWidth (XmOptionLabelGadget (byteOrderMenu),optionWidth);

		_RGISToolsImpGridSetMapCallback (fileTypeMenu,byteOrderMenu);
		_RGISToolsImportSetButtonWidth (layoutMenu,buttonWidth);
		_RGISToolsImportSetButtonWidth (rowOrderMenu,buttonWidth);
		_RGISToolsImportSetButtonWidth (fileTypeMenu,buttonWidth);
		_RGISToolsImportSetButtonWidth (binaryTypeMenu,buttonWidth);
		_RGISToolsImportSetButtonWidth (byteOrderMenu,buttonWidth);
		XtVaSetValues (XmOptionLabelGadget (layoutMenu),	XmNwidth,	optionWidth,	XmNalignment, XmALIGNMENT_BEGINNING, XmNrecomputeSize, False, NULL);
		XtVaSetValues (XmOptionLabelGadget (rowOrderMenu),	XmNwidth,	optionWidth,	XmNalignment, XmALIGNMENT_BEGINNING, XmNrecomputeSize, False, NULL);
		XtVaSetValues (XmOptionLabelGadget (fileTypeMenu),	XmNwidth,	optionWidth,	XmNalignment, XmALIGNMENT_BEGINNING, XmNrecomputeSize, False, NULL);
		XtVaSetValues (XmOptionLabelGadget (binaryTypeMenu),XmNwidth,	optionWidth,	XmNalignment, XmALIGNMENT_BEGINNING, XmNrecomputeSize, False, NULL);
		XtVaSetValues (XmOptionLabelGadget (byteOrderMenu),XmNwidth,	optionWidth,	XmNalignment, XmALIGNMENT_BEGINNING, XmNrecomputeSize, False, NULL);

		rowColWGT = XtVaCreateManagedWidget ("RGISToolsGridImportSubform",xmRowColumnWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_FORM,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				rowColWGT,
								XmNleftOffset,				5,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNshadowThickness,		0,
								XmNpacking,					XmPACK_COLUMN,
								XmNspacing,					4,
								XmNmarginWidth,			0,
								XmNnumColumns,				6,
								XmNorientation,			XmHORIZONTAL,
								NULL);
		string = XmStringCreate ((char *) " ",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportColNumLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Column",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportLLCellColLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Row",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportLLCellRowLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "LL Cell:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISToolsGridImportLLCellLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		llCellColFLD = XtVaCreateManagedWidget ("RGISToolsGridImportLLCellColFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x02,
								XmNcolumns,					DBStringLength >> 0x02,
								XmNvalue,					"0",
								NULL);
		llCellRowFLD = XtVaCreateManagedWidget ("RGISToolsGridImportLLCellRowFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x02,
								XmNcolumns,					DBStringLength >> 0x02,
								XmNvalue,					"0",
								NULL);

		string = XmStringCreate ((char *) " ",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportColNumLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Width",UICharSetNormal);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportCellWidthLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Height",UICharSetNormal);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportCellHeightLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Cell Size:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISToolsGridImportCellSizeLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		cellWidthFLD = XtVaCreateManagedWidget ("RGISToolsGridImportCellWidthFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x01,
								XmNcolumns,					DBStringLength >> 0x02,
								NULL);
		cellHeightFLD = XtVaCreateManagedWidget ("RGISToolsGridImportCellHeightFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x01,
								XmNcolumns,					DBStringLength >> 0x02,
								NULL);


		string = XmStringCreate ((char *) " ",UICharSetNormal);
		XtVaCreateManagedWidget ("RGISToolsGridImportColNumLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Horizontal",UICharSetNormal);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportllXCoordLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Vertical",UICharSetNormal);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportllYCoordLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "LL Corner:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISToolsGridImportllCornerLabel",xmLabelWidgetClass,rowColWGT,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		llXCoordFLD = XtVaCreateManagedWidget ("RGISToolsGridImportllYCoordFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x01,
								XmNcolumns,					DBStringLength >> 0x02,
								NULL);
		llYCoordFLD = XtVaCreateManagedWidget ("RGISToolsGridImportllXCoordFLD",xmTextFieldWidgetClass,rowColWGT,
								XmNmaxLength,				DBStringLength >> 0x01,
								XmNcolumns,					DBStringLength >> 0x02,
								NULL);
		XtVaGetValues (llYCoordFLD,XmNheight,	&widgetHeight, NULL);

		string = XmStringCreate ((char *) "Missing Value:",UICharSetBold);
		missingValFLD = XtVaCreateManagedWidget ("RGISToolsGridImportMissingValFLD",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				rowColWGT,
								XmNtopOffset,				5,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNmaxLength,				DBStringLength >> 0x01,
								XmNcolumns,					DBStringLength >> 0x02,
								XmNvalue,					"0",
								NULL);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportSkipPadLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				missingValFLD,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			missingValFLD,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			missingValFLD,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISToolsGridImportPathSelectButton",xmPushButtonGadgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				missingValFLD,
								XmNtopOffset,				10,
								XmNrightAttachment,		XmATTACH_FORM,
								XmNrightOffset,			10,
								XmNheight,					widgetHeight,
								XmNrecomputeSize,			False,
								XmNmarginWidth,			5,
								XmNlabelString,			string,
								XmNtraversalOn,			False,
								XmNuserData,				true,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "File Name:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportSkipPadLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		listFileNameFLD = XtVaCreateManagedWidget ("RGISToolsGridImportPathFLD",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				button,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				label,
								XmNleftOffset,				5,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			button,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			button,
								XmNmaxLength,				DBDataFileNameLen,
								XmNuserData,				"*",
								NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxFileSelectCBK,(void *) listFileNameFLD);
		skipHeaderFLD = XtVaCreateManagedWidget ("RGISToolsGridImportHeaderFLD",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_WIDGET,
								XmNtopWidget,				listFileNameFLD,
								XmNtopOffset,				5,
								XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNleftWidget,				listFileNameFLD,
								XmNbottomAttachment,		XmATTACH_FORM,
								XmNbottomOffset,			10,
								XmNmaxLength,				6,
								XmNcolumns,					6,
								XmNvalue,					"0",
								NULL);
		string = XmStringCreate ((char *) "Header:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportSkipPadLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				skipHeaderFLD,
								XmNleftAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNleftWidget,				label,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			skipHeaderFLD,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Record Padding:",UICharSetBold);
		label = XtVaCreateManagedWidget ("RGISToolsGridImportSkipPadLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				skipHeaderFLD,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				skipHeaderFLD,
								XmNleftOffset,				10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			skipHeaderFLD,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		skipPadFLD = XtVaCreateManagedWidget ("RGISToolsGridImportPadFLD",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				skipHeaderFLD,
								XmNleftAttachment,		XmATTACH_WIDGET,
								XmNleftWidget,				label,
								XmNleftOffset,				5,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			skipHeaderFLD,
								XmNmaxLength,				4,
								XmNcolumns,					4,
								XmNvalue,					"0",
								NULL);
		_RGISToolsImpGridSetMapCallback (fileTypeMenu,label);
		_RGISToolsImpGridSetMapCallback (fileTypeMenu,skipPadFLD);
		string = XmStringCreate ((char *) "List File",UICharSetBold);
		listFileTGL = XtVaCreateManagedWidget ("RGISToolsGridImportListFileToggle",xmToggleButtonGadgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				skipHeaderFLD,
								XmNrightAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNrightWidget,			listFileNameFLD,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			skipHeaderFLD,
								XmNshadowThickness,		0,
								XmNtraversalOn,			False,
								XmNlabelString,			string,
								NULL);
		XtAddCallback (listFileTGL,XmNvalueChangedCallback,(XtCallbackProc) UIAuxSetToggleCBK,&listFile);

		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&proc);
		}
	sprintf (buffer,"%d",llCellCol);
	XmTextFieldSetString (llCellColFLD,buffer);
	sprintf (buffer,"%d",llCellRow);
	XmTextFieldSetString (llCellRowFLD,buffer);
	sprintf (buffer,"%d",skipHeader);
	XmTextFieldSetString (skipHeaderFLD,buffer);
	sprintf (buffer,"%d",skipPad);
	XmTextFieldSetString (skipPadFLD,buffer);
	sprintf (buffer,"%3.1f",missingVal);
	XmTextFieldSetString (missingValFLD,buffer);

	proc = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{
		confOk = True;
		text =  XmTextFieldGetString (rowNumFLD);
		if (sscanf (text,"%d",&rowNum) != 1) confOk = False;
		XtFree (text);
		if (confOk)
			{
			text =  XmTextFieldGetString (colNumFLD);
			if (sscanf (text,"%d",&colNum) != 1) confOk = False;
			XtFree (text);
			}
		if (confOk)
			{
			text =  XmTextFieldGetString (cellWidthFLD);
			if (sscanf (text,"%lf",&cellWidth) != 1) confOk = False;
			XtFree (text);
			}
		if (confOk)
			{
			text =  XmTextFieldGetString (cellHeightFLD);
			if (sscanf (text,"%lf",&cellHeight) != 1) confOk = False;
			XtFree (text);
			}
		if (confOk)
			{
			text =  XmTextFieldGetString (llXCoordFLD);
			if (sscanf (text,"%lf",&llXCoord) != 1) confOk = False;
			XtFree (text);
			}
		if (confOk)
			{
			text =  XmTextFieldGetString (llYCoordFLD);
			if (sscanf (text,"%lf",&llYCoord) != 1) confOk = False;
			XtFree (text);
			}
		if (confOk)
			{
			text =  XmTextFieldGetString (listFileNameFLD);
			if (sscanf (text,"%s",listFileName) != 1) confOk = False;
			XtFree (text);
			}
		XtSetSensitive (UIDialogFormGetOkButton (dShell),confOk);
		}
	UIDialogFormPopdown (dShell);
	text =  XmTextFieldGetString (llCellColFLD);
	if (sscanf (text,"%d",&llCellCol) != 1) llCellCol = 0;
	XtFree (text);
	if (llCellCol >= colNum) llCellCol = colNum - 1;
	text =  XmTextFieldGetString (llCellRowFLD);
	if (sscanf (text,"%d",&llCellRow) != 1) llCellRow = 0;
	XtFree (text);
	if (llCellRow >= rowNum) llCellRow = rowNum - 1;
	text =  XmTextFieldGetString (skipHeaderFLD);
	if (sscanf (text,"%d",&skipHeader) != 1) skipHeader = 0;
	XtFree (text);
	text =  XmTextFieldGetString (skipPadFLD);
	if (sscanf (text,"%d",&skipPad) != 1) skipPad = 0;
	XtFree (text);
	text =  XmTextFieldGetString (missingValFLD);
	if (sscanf (text,"%f",&missingVal) != 1) missingVal = DBDefaultMissingFloatVal;
	XtFree (text);

	if (proc)
		{
		DBObjData *grdData = new DBObjData ("",binaryType == RGISBinTypeShort ?
				DBTypeGrid : (binaryType < RGISBinTypeShort ? DBTypeGridDiscrete : DBTypeGridContinuous));

		if (UIDataHeaderForm (grdData))
			{
			FILE *inFILE, *lstFILE = (FILE *) NULL;
			char fileName [DBDataFileNameLen], recordName [DBStringLength];
			DBInt pathLen, itemSize, chunk, i, j, row, col, recordLen;
			DBCoordinate coord;
			DBRegion extent;
			DBObjTable *layerTable = grdData->Table (DBrNLayers);
			DBObjTable *itemTable  = grdData->Table (DBrNItems);
			DBObjTableField *missingValueFLD	= grdData->Type () == DBTypeGridContinuous ?
														  itemTable->Field (DBrNMissingValue) : (DBObjTableField *) NULL;
			DBObjTableField *rowNumFLD		= layerTable->Field (DBrNRowNum);
			DBObjTableField *colNumFLD 	= layerTable->Field (DBrNColNum);
			DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
			DBObjTableField *cellHeightFLD= layerTable->Field (DBrNCellHeight);
			DBObjTableField *valueTypeFLD = layerTable->Field (DBrNValueType);
			DBObjTableField *valueSizeFLD = layerTable->Field (DBrNValueSize);
			DBObjTableField *layerFLD 		= layerTable->Field (DBrNLayer);
			DBObjRecord *layerRec, *itemRec, *dataRec;
			DBGridIO *gridIO;

			coord.X = llXCoord;
			coord.Y = llYCoord;
			extent.Expand (coord);
			coord.X += colNum * cellWidth;
			coord.Y += rowNum * cellHeight;
			extent.Expand (coord);
			grdData->Extent (extent);
			grdData->Projection (DBMathGuessProjection (extent));
			grdData->Precision  (DBMathGuessPrecision (extent));

			switch (binaryType)
				{
				case RGISBinTypeByte:	itemSize = sizeof (DBByte);	break;
				case RGISBinTypeShort:	itemSize = sizeof (DBShort);	break;
				case RGISBinTypeLong:	itemSize = sizeof (DBInt);		break;
				case RGISBinTypeSingle:	itemSize = sizeof (DBFloat4);	break;
				case RGISBinTypeDouble:	itemSize = sizeof (DBFloat);	break;
				default:
					fprintf (stderr,"Invalid Data Type in: DBNetworkToGrid ()\n");
					delete grdData;
					return;
				}

			for (pathLen = strlen (listFileName) - 1;(pathLen > 0) && (listFileName [pathLen] != '/');--pathLen);
			if (listFileName [pathLen] == '/') ++pathLen;
			if (listFile)
				{
				if ((lstFILE = fopen (listFileName,"r")) == (FILE *) NULL)
					{
					fprintf (stderr,"List File Openining Error in: RGISToolsImportGridCBK ()\n");
					delete grdData;
					return;
					}
				listFileName [pathLen] = '\0';
				}

			while (true)
				{
				if (listFile)
					{
					if (fgets (buffer,sizeof (buffer) - 2,lstFILE) != buffer)	break;
					if (buffer [strlen (buffer) - 1] == '\n') buffer [strlen (buffer) - 1] = '\0';
					if (buffer [0] == '/')	strncpy (fileName,buffer,sizeof (fileName) - 1);
					else
						sprintf (fileName,"%s%s",listFileName,buffer);
					for (i = strlen (fileName) - 1;(i > 0) && fileName [i] != '/';--i);
					if (fileName [i] == '/') ++i;
					strncpy (recordName,fileName + i,sizeof (recordName) - 1);
					}
				else
					{
					if (layerTable->ItemNum () > 0) break;
					strncpy (fileName,listFileName,sizeof (fileName) - 1);
					strncpy (recordName,listFileName + pathLen,sizeof (recordName) - 1);
					}

				if ((inFILE = fopen (fileName,"r")) == (FILE *) NULL)
					{ perror ("File Openining Error in: RGISToolsImportGridCBK ()"); delete grdData; return; }
				if (fileType == RGISGridBinary)
					while (skipHeader > 0)
						{
						chunk = skipHeader < (int) sizeof (buffer) ? skipHeader : (int) sizeof (buffer);
						if (fread (buffer,chunk,1,inFILE) != 1)
							{
							perror ("File Reading Error in: RGISToolsImportGridCBK ()");
							fclose (inFILE);
							delete grdData;
							}
						skipHeader -= chunk;
						}
				else
					for (chunk = 0;chunk < skipHeader;++chunk)
						do	fgets (buffer,sizeof (buffer) - 2,inFILE);
							while (buffer [strlen (buffer) - 1] != '\n');

				for (i = 0;i < (int) strlen (recordName);++i) if (recordName [i] == '.') recordName [i] = ' ';
				layerTable->Add (recordName);
				if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL)
					{ fclose (inFILE); delete grdData; return; }
				rowNumFLD->Int (layerRec,rowNum);
				colNumFLD->Int (layerRec,colNum);
				cellWidthFLD->Float  (layerRec,cellWidth);
				cellHeightFLD->Float (layerRec,cellHeight);
				switch (binaryType)
					{
					case RGISBinTypeByte:
					case RGISBinTypeShort:
					case RGISBinTypeLong:
						valueTypeFLD->Int (layerRec,DBTableFieldInt);	break;
					case RGISBinTypeSingle:
					case RGISBinTypeDouble:
						valueTypeFLD->Int (layerRec,DBTableFieldFloat);	break;
					default:
						fprintf (stderr,"Invalid Data Type in: DBNetworkToGrid ()\n");
						fclose (inFILE); delete grdData;
						return;
					}
				valueSizeFLD->Int (layerRec,itemSize);
				if ((dataRec = new DBObjRecord (layerRec->Name (),colNum * rowNum * valueSizeFLD->Int (layerRec),valueSizeFLD->Int (layerRec))) == (DBObjRecord *) NULL)
					{ fclose (inFILE); delete grdData; return; }
				(grdData->Arrays ())->Add (dataRec);
				layerFLD->Record (layerRec,dataRec);
				if (grdData->Type () == DBTypeGridContinuous)
					{
					itemTable->Add (layerRec->Name ());
					if ((itemRec = itemTable->Item ()) == (DBObjRecord *) NULL)
						{ fclose (inFILE); delete grdData; return; }
					missingValueFLD->Float (itemRec,(DBFloat) missingVal);
					}
				if (fileType == RGISGridBinary)
					{
					recordLen = (layout == RGISLaoutByRow ? colNum : rowNum) * itemSize + skipPad;
					for (j = 0;(chunk = fread (buffer,1,sizeof (buffer),inFILE)) > 0;++j)
						for (i = 0;i < chunk;i += itemSize)
							{
							if (byteOrder != DBByteOrder ())
								switch (itemSize)
									{
									default:	break;
									case 2:	DBByteOrderSwapHalfWord	(buffer + i);	break;
									case 4:	DBByteOrderSwapWord 		(buffer + i);	break;
									case 8:	DBByteOrderSwapLongWord	(buffer + i);	break;
									}
							if (layout == RGISLaoutByRow)
								{
								if ((row =  (j * sizeof (buffer) + i) / recordLen) >= rowNum) continue;
								if ((col = ((j * sizeof (buffer) + i) % recordLen) / itemSize) >= colNum) continue;
								}
							else
								{
								if ((col =  (j * sizeof (buffer) + i) / recordLen) >= colNum) continue;
								if ((row = ((j * sizeof (buffer) + i) % recordLen) / itemSize) >= rowNum) continue;
								}
							if (rowOrder == RGISItemOrderBottomUp)
								row = rowNum - row - 1;
							col -= llCellCol;
							col = col < 0 ? colNum + col : col;
							row -= llCellRow;
							row = row < 0 ? rowNum + row : row;
							memcpy (((char *) dataRec->Data ()) + (row * colNum + col) * itemSize,buffer + i,itemSize);
							}
					}
				else
					{
					DBFloat val;
					i = j = 0;
					while (fscanf (inFILE,"%lf",&val) == 1)
						{
						if (j >= rowNum) continue;
						if (i >= colNum) continue;
						switch (binaryType)
							{
							case RGISBinTypeByte:	((DBByte *) 	buffer) [0] = (DBByte) 	val;	break;
							case RGISBinTypeShort:	((DBShort *)	buffer) [0] = (DBShort) val;	break;
							case RGISBinTypeLong:	((DBInt *)		buffer) [0] = (DBInt) 	val;	break;
							case RGISBinTypeSingle:	((DBFloat4 *)	buffer) [0] = (DBFloat4)val;	break;
							case RGISBinTypeDouble:	((DBFloat *)	buffer) [0] = (DBFloat)	val;	break;
							}
						col = i; row = j;
						if (rowOrder == RGISItemOrderBottomUp)
							row = rowNum - row - 1;
						col -= llCellCol;
						col = col < 0 ? colNum + col : col;
						row -= llCellRow;
						row = row < 0 ? rowNum + row : row;
						memcpy (((char *) dataRec->Data ()) + (row * colNum + col) * itemSize,buffer,itemSize);
						if (layout == RGISLaoutByRow)
							{
							i += 1;
							if (i >= colNum) { i = 0; j += 1; }
							}
						else
							{
							j += 1;
							if (j >= rowNum) { j = 0; i += 1; }
							}
						}
					}
				fclose (inFILE);
				}
			if (listFile) fclose (lstFILE);
			gridIO = new DBGridIO (grdData);
			if (grdData->Type () == DBTypeGridContinuous)
				{
				DBObjTableField *missingValueFLD	= itemTable->Field (DBrNMissingValue);
				missingValueFLD->Float (itemTable->Item (layerRec->Name ()),(DBFloat) missingVal);
				gridIO->RecalcStats ();
				}
			else
				{
				DBInt intVal;
				DBObjRecord *symRec = (grdData->Table (DBrNSymbols))->Add ("Default Symbol");
				DBObjTableField *gridValueFLD  = itemTable->Field (DBrNGridValue);
				DBObjTableField *gridSymbolFLD = itemTable->Field (DBrNSymbol);
				DBObjTableField *symbolIDFLD	 = (grdData->Table (DBrNSymbols))->Field (DBrNSymbolID);
				DBObjTableField *foregroundFLD = (grdData->Table (DBrNSymbols))->Field (DBrNForeground);
				DBObjTableField *backgroundFLD = (grdData->Table (DBrNSymbols))->Field (DBrNBackground);
				DBObjTableField *styleFLD = (grdData->Table (DBrNSymbols))->Field (DBrNStyle);

				symbolIDFLD->Int (symRec,0);
				foregroundFLD->Int (symRec,1);
				backgroundFLD->Int (symRec,0);
				styleFLD->Int (symRec,0);
				for (dataRec = (grdData->Arrays ())->First ();dataRec != (DBObjRecord *) NULL;dataRec = (grdData->Arrays ())->Next ())
					{
					for (i = 0;i < colNum * rowNum;++i)
						{
						if (itemSize == (int) sizeof (DBByte))
								intVal = (DBInt) (*((DBByte *)  ((char *) dataRec->Data () + i * itemSize)));
						else	intVal = (DBInt) (*((DBShort *) ((char *) dataRec->Data () + i * itemSize)));
						sprintf (buffer,"Category%d",intVal);
						if ((itemRec = itemTable->Item (buffer)) == (DBObjRecord *) NULL)
							{
							if ((itemRec = itemTable->Add (buffer)) == (DBObjRecord *) NULL)
								{
								fprintf (stderr,"Item Object Creation Error in: RGISToolsImportGridCBK ()\n");
								delete gridIO; delete grdData;
								return;
								}
							gridValueFLD->Int (itemRec,intVal);
							gridSymbolFLD->Record (itemRec,symRec);
							}
						intVal = itemRec->RowID ();
						if (itemSize == (int) sizeof (DBByte))
								*((DBByte *)  ((char *) dataRec->Data () + i * itemSize)) = intVal;
						else	*((DBShort *) ((char *) dataRec->Data () + i * itemSize)) = intVal;
						}
					}
				itemTable->ListSort (gridValueFLD);
				for (dataRec = (grdData->Arrays ())->First ();dataRec != (DBObjRecord *) NULL;dataRec = (grdData->Arrays ())->Next ())
					{
					for (i = 0;i < colNum * rowNum;++i)
						{
						if (itemSize == (int) sizeof (DBByte))
								intVal = (DBInt) (*((DBByte *)  ((char *) dataRec->Data () + i * itemSize)));
						else	intVal = (DBInt) (*((DBShort *) ((char *) dataRec->Data () + i * itemSize)));
						itemRec = itemTable->Item (intVal);
						intVal = itemRec->ListPos ();
						if (itemSize == (int) sizeof (DBByte))
								*((DBByte *)  ((char *) dataRec->Data () + i * itemSize)) = intVal;
						else	*((DBShort *) ((char *) dataRec->Data () + i * itemSize)) = intVal;
						}
					}
				itemTable->ItemSort ();
				gridIO->DiscreteStats ();
				}
			delete gridIO;
			workspace->CurrentData (grdData);
			}
		else delete grdData;
		}
	}

void RGISToolsImportGridDMCBK (Widget widget,RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBObjData *data = new DBObjData ("",DBTypeGrid);
	static Widget fileSelect = NULL;

	widget = widget; callData = callData;
	if (fileSelect == NULL) fileSelect = UIFileSelectionCreate ((char *) "Data Manager Import",NULL,(char *) "M:*",XmFILE_REGULAR);
	if (UIDataHeaderForm (data))
		{
		char *fileName;

		if ((fileName = UIFileSelection (fileSelect,true)) == NULL) { delete data; return; }
		if (DBImportDMGrid (data,fileName) == DBFault)	delete data;
		else workspace->CurrentData (data);
		}
	else delete data;
	}
