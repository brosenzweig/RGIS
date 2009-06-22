/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISAnalyse.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <rgis.H>

extern void RGISAnalyseFieldCalcAddCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISAnalyseFieldCalcSubtractCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISAnalyseFieldCalcMultiplyCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISAnalyseFieldCalcDivideCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISAnalyseFieldSetValueCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISAnalyseFieldsCompareCBK (Widget,void *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseFieldCalcMenu [] = {
	UIMenuItem ((char *) "Add",						RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcAddCBK,				(char *) "RGIS22MenuSystem.html#AnalyzeAll_Fields_Add"),
	UIMenuItem ((char *) "Subtract",				RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcSubtractCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeAll_Fields_Subtract"),
	UIMenuItem ((char *) "Multiply",					RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcMultiplyCBK,	(char *) "RGIS22MenuSystem.html#AnalyzeAll_Fields_Multiply"),
	UIMenuItem ((char *) "Divide",					RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcDivideCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeAll_Fields_Divide"),
	UIMenuItem ((char *) "Set Value",				RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldSetValueCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeAll_Fields_SetValue"),
	UIMenuItem (RGISDataGroup,			UIMENU_NORULE),
	UIMenuItem ((char *) "Compare",					RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldsCompareCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeAll_Fields_Compare"),
	UIMenuItem ()};

extern void RGISAnalyseFieldTopoAccumCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISAnalyseFieldTopoSubtractCBK (Widget,void *,XmAnyCallbackStruct *);


extern void RGISAnalyseSingleSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalyseMultiSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalysePointGridSampleMenu [] = {
	UIMenuItem ((char *) "Single Layer",			RGISPointGroup,	RGISLinkedGridGroup,		(UIMenuCBK) RGISAnalyseSingleSampleGridCBK,	(char *) "RGIS22MenuSystem.html#"),
	UIMenuItem ((char *) "Multi Layer",				RGISPointGroup,	RGISLinkedGridGroup,		(UIMenuCBK) RGISAnalyseMultiSampleGridCBK,	(char *) "RGIS22MenuSystem.html#"),
	UIMenuItem ()};

extern void RGISAnalyseLineSSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalyseLineMSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseLineGridSampleMenu [] = {
	UIMenuItem ((char *) "Single Layer",			RGISLineGroup,		RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnalyseLineSSampleGridCBK,	(char *) "RGIS22MenuSystem.html#AnalyzePoint_GridSampling"),
	UIMenuItem ((char *) "Multi Layer",				RGISLineGroup,		RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnalyseLineMSampleGridCBK,	(char *) "RGIS22MenuSystem.html#AnalyzePoint_GridSampling"),
	UIMenuItem ()};

extern void RGISAnNetworkCellSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseNetGridSampleMenu [] = {
	UIMenuItem ((char *) "Single Layer",RGISNetworkGroup,		RGISLinkedGridGroup,				(UIMenuCBK) RGISAnalyseSingleSampleGridCBK,	(char *) "RGIS22MenuSystem.html#AnalyzeNet_GridSampling"),
	UIMenuItem ((char *) "Multi Layer",	RGISNetworkGroup,		RGISLinkedGridGroup,				(UIMenuCBK) RGISAnalyseMultiSampleGridCBK,	(char *) "RGIS22MenuSystem.html#AnalyzeNet_GridSampling"),
	UIMenuItem ((char *) "Cell",		RGISNetworkGroup,		RGISLinkedGridGroup,				(UIMenuCBK) RGISAnNetworkCellSampleGridCBK,	(char *) "RGIS22MenuSystem.html#AnalyzeNet_GridSampling"),
	UIMenuItem ()};


extern void RGISAnNetworkBasinStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkHeadStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkDivideStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkHistogramCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkBasinDistribCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseNetStatMenu [] = {
	UIMenuItem ((char *) "Basin",			RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkBasinStatsCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ((char *) "Headwater",	RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkHeadStatsCBK ,			(char *) "RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ((char *) "Divide",		RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkDivideStatsCBK ,		(char *) "RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ((char *) "Histogram",	RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkHistogramCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ((char *) "Category Distribution",RGISNetworkGroup,RGISLinkedDiscreteGroup, (UIMenuCBK) RGISAnNetworkBasinDistribCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ()};

extern void RGISAnGNUXYPlotCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISAnalysePointSTNCharCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalysePointSTNPointsCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalysePointSubbasinCenterCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalysePointSubbasinCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalysePointInterStationTSCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalysePointSubbasinStatsCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalysePointSubbasinHistCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISAnGridResampleCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISAnGDiscUniformRunoffCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGDiscReclassDiscreteCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGDiscReclassContinuousCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGDiscZoneHistCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGDiscZoneStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISAnGContPitsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContPourCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContCreateNetworkCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContMergeCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContAbsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContNoNegCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContMakeDiscreteCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContGirdCellStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

extern void RGISAnGContAddCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContSubtractCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContMultiplyCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnGContDivideCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseGContOperMenu [] = {
	UIMenuItem ((char *) "Add",			RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContAddCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Add"),
	UIMenuItem ((char *) "Subtract",	RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContSubtractCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Subtract"),
	UIMenuItem ((char *) "Multiply",	RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContMultiplyCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Multiply"),
	UIMenuItem ((char *) "Divide",		RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContDivideCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Divide"),
	UIMenuItem ()};

extern void RGISAnNetworkBasinGridCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkToGridCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkStationsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkAccumulateCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkUpStreamAvgCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkCellSlopesCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkBasinProfCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkStreamLinesCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

UIMenuItem RGISAnalyseMenu [] = {
	UIMenuItem ((char *) "GNU XY-Plot",				RGISDataGroup,		UIMENU_NORULE, 				(UIMenuCBK) RGISAnGNUXYPlotCBK,						(char *) "RGIS22MenuSystem.html#AnalyzeAll_GNUXY-Plot"),
	UIMenuItem ((char *) "Fields",					RGISDataGroup,		UIMENU_NORULE,					_RGISAnalyseFieldCalcMenu),
	UIMenuItem ((char *) "Topo Accumulation",		RGISDataGroup,		UIMENU_NORULE,					(UIMenuCBK) RGISAnalyseFieldTopoAccumCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeAll_TopoAccumulation"),
	UIMenuItem ((char *) "Topo Subtraction",		RGISDataGroup,		UIMENU_NORULE,					(UIMenuCBK) RGISAnalyseFieldTopoSubtractCBK,	(char *) "RGIS22MenuSystem.html#AnalyzeAll_TopoSubtraction"),
	UIMenuItem (RGISPointGroup,			UIMENU_NORULE),
	UIMenuItem ((char *) "STN Characteristics",	RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSTNCharCBK,			(char *) "RGIS22MenuSystem.html#AnalyzePoint_STNcharacteristics"),
	UIMenuItem ((char *) "STN Points",				RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSTNPointsCBK,			(char *) "RGIS22MenuSystem.html#AnalyzePoint_STNPoints"),
	UIMenuItem ((char *) "Subbasin Grid",			RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSubbasinCBK,			(char *) "RGIS22MenuSystem.html#AnalyzePoint_SubbasinGrid"),
	UIMenuItem ((char *) "Subbasin Center",		RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSubbasinCenterCBK,	(char *) "RGIS22MenuSystem.html#AnalyzePoint_SubbasinCenter"),
	UIMenuItem ((char *) "Inter-station TS",		RGISPointGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnalysePointInterStationTSCBK,	(char *) "RGIS22MenuSystem.html#AnalyzePoint_InterstatTS"),
	UIMenuItem ((char *) "Grid Sampling",			RGISPointGroup,	RGISLinkedGridGroup,			_RGISAnalysePointGridSampleMenu),
	UIMenuItem ((char *) "Subbasin Statistics",	RGISPointGroup,	RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnalysePointSubbasinStatsCBK,	(char *) "RGIS22MenuSystem.html#AnalyzePoint_SubbasinStats"),
	UIMenuItem ((char *) "Subbasin Histogram",	RGISPointGroup,	RGISLinkedDiscreteGroup,	(UIMenuCBK) RGISAnalysePointSubbasinHistCBK,		(char *) "RGIS22MenuSystem.html#AnalyzePoint_SubbasinHist"),
	UIMenuItem (RGISLineGroup,				UIMENU_NORULE),
	UIMenuItem ((char *) "Grid Sampling",			RGISLineGroup,		RGISLinkedContinuousGroup,	_RGISAnalyseLineGridSampleMenu),
	UIMenuItem (RGISGridGroup,				UIMENU_NORULE),
	UIMenuItem ((char *) "Resample",					RGISGridGroup,		RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnGridResampleCBK,					(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_Resample"),
	UIMenuItem (RGISDiscreteGroup,		UIMENU_NORULE),
	UIMenuItem ((char *) "Uniform Runoff Fields",RGISDiscreteGroup,UIMENU_NORULE,					(UIMenuCBK) RGISAnGDiscUniformRunoffCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeDGrid_UniformRunoffFields"),
	UIMenuItem ((char *) "Reclass Discrete",		RGISDiscreteGroup,UIMENU_NORULE,					(UIMenuCBK) RGISAnGDiscReclassDiscreteCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeDGrid_ReclassDGrid"),
	UIMenuItem ((char *) "Reclass Continuous",	RGISDiscreteGroup,UIMENU_NORULE,					(UIMenuCBK) RGISAnGDiscReclassContinuousCBK,		(char *) "RGIS22MenuSystem.html#AnalyzeDGrid_ReclassCGrid"),
	UIMenuItem ((char *) "Zone Histogram",			RGISDiscreteGroup,RGISLinkedDiscreteGroup,	(UIMenuCBK) RGISAnGDiscZoneHistCBK,					(char *) "RGIS22MenuSystem.html#AnalyzeDGrid_ZoneHist"),
	UIMenuItem ((char *) "Zone Staistics",			RGISDiscreteGroup,RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnGDiscZoneStatsCBK,				(char *) "RGIS22MenuSystem.html#AnalyzeDGrid_ZoneStats"),
	UIMenuItem (RGISContinuousGroup,		UIMENU_NORULE),
	UIMenuItem ((char *) "Find Pits",				RGISContinuousGroup,	RGISLinkedNetworkGroup,	(UIMenuCBK) RGISAnGContPitsCBK,						(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_FindPits"),
	UIMenuItem ((char *) "Find Pour Points",		RGISContinuousGroup, RGISLinkedNetworkGroup,	(UIMenuCBK) RGISAnGContPourCBK,						(char *) "RGIS22MenuSystem.html#"),
	UIMenuItem ((char *) "Create Network",			RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContCreateNetworkCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_CreateNetwork"),
	UIMenuItem ((char *) "Merge",						RGISContinuousGroup,	RGISLinkedContinuousGroup,(UIMenuCBK) RGISAnGContMergeCBK,					(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_Merge"),
	UIMenuItem ((char *) "Absolute",					RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContAbsCBK,						(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_Abs"),
	UIMenuItem ((char *) "No Negative",				RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContNoNegCBK,						(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_NoNeg"),
	UIMenuItem ((char *) "Operation",				RGISContinuousGroup,	RGISLinkedContinuousGroup,_RGISAnalyseGContOperMenu ),
	UIMenuItem ((char *) "Make Discrete",			RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContMakeDiscreteCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_MakeDiscrete"),
	UIMenuItem ((char *) "Cell Statistics",		RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContGirdCellStatsCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeCGrid_CellStats"),
	UIMenuItem (RGISNetworkGroup,			UIMENU_NORULE),
	UIMenuItem ((char *) "Create Basingrid",		RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkBasinGridCBK,				(char *) "RGIS22MenuSystem.html#AnalyzeNet_Convert2BasinGrid"),
	UIMenuItem ((char *) "Convert to Grid",		RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkToGridCBK,					(char *) "RGIS22MenuSystem.html#AnalyzeNet_Convert2Grid"),
	UIMenuItem ((char *) "Station Points",			RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkStationsCBK,				(char *) "RGIS22MenuSystem.html#AnalyzeNet_StationPts"),
	UIMenuItem ((char *) "Accumulate",				RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkAccumulateCBK ,			(char *) "RGIS22MenuSystem.html#AnalyzeNet_Accumulate"),
	UIMenuItem ((char *) "Upstream Average",		RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkUpStreamAvgCBK ,			(char *) "RGIS22MenuSystem.html#AnalyzeNet_UpstreamAvg"),
	UIMenuItem ((char *) "Cell Slopes",				RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkCellSlopesCBK ,			(char *) "RGIS22MenuSystem.html#AnalyzeNet_CellSlopes"),
	UIMenuItem ((char *) "Basin Profile",			RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkBasinProfCBK ,			(char *) "RGIS22MenuSystem.html#AnalyzeNet_BasinProfile"),
	UIMenuItem ((char *) "Grid Sampling",			RGISNetworkGroup,	RGISLinkedGridGroup,			_RGISAnalyseNetGridSampleMenu),
	UIMenuItem ((char *) "Statistics",				RGISNetworkGroup,	UIMENU_NORULE,					_RGISAnalyseNetStatMenu),
	UIMenuItem ((char *) "Stream Lines",			RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkStreamLinesCBK,			(char *) "RGIS22MenuSystem.html#AnalyzeNet_StreamLines"),
	UIMenuItem ()};
