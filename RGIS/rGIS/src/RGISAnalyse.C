/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

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
	UIMenuItem ("Add",						RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcAddCBK,				"RGIS22MenuSystem.html#AnalyzeAll_Fields_Add"),
	UIMenuItem ("Subtract",				RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcSubtractCBK,		"RGIS22MenuSystem.html#AnalyzeAll_Fields_Subtract"),
	UIMenuItem ("Multiply",					RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcMultiplyCBK,			"RGIS22MenuSystem.html#AnalyzeAll_Fields_Multiply"),
	UIMenuItem ("Divide",					RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldCalcDivideCBK,			"RGIS22MenuSystem.html#AnalyzeAll_Fields_Divide"),
	UIMenuItem ("Set Value",				RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldSetValueCBK,				"RGIS22MenuSystem.html#AnalyzeAll_Fields_SetValue"),
	UIMenuItem (RGISDataGroup,			UIMENU_NORULE),
	UIMenuItem ("Compare",					RGISDataGroup,		UIMENU_NORULE,		(UIMenuCBK) RGISAnalyseFieldsCompareCBK,				"RGIS22MenuSystem.html#AnalyzeAll_Fields_Compare"),
	UIMenuItem ()};

extern void RGISAnalyseFieldTopoAccumCBK (Widget,void *,XmAnyCallbackStruct *);
extern void RGISAnalyseFieldTopoSubtractCBK (Widget,void *,XmAnyCallbackStruct *);


extern void RGISAnalyseSingleSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalyseMultiSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalysePointGridSampleMenu [] = {
	UIMenuItem ("Single Layer",			RGISPointGroup,	RGISLinkedGridGroup,		(UIMenuCBK) RGISAnalyseSingleSampleGridCBK,	"RGIS22MenuSystem.html#"),
	UIMenuItem ("Multi Layer",				RGISPointGroup,	RGISLinkedGridGroup,		(UIMenuCBK) RGISAnalyseMultiSampleGridCBK,	"RGIS22MenuSystem.html#"),
	UIMenuItem ()};

extern void RGISAnalyseLineSSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnalyseLineMSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseLineGridSampleMenu [] = {
	UIMenuItem ("Single Layer",			RGISLineGroup,		RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnalyseLineSSampleGridCBK,	"RGIS22MenuSystem.html#AnalyzePoint_GridSampling"),
	UIMenuItem ("Multi Layer",				RGISLineGroup,		RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnalyseLineMSampleGridCBK,	"RGIS22MenuSystem.html#AnalyzePoint_GridSampling"),
	UIMenuItem ()};

extern void RGISAnNetworkCellSampleGridCBK (Widget, RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseNetGridSampleMenu [] = {
	UIMenuItem ("Single Layer",RGISNetworkGroup,		RGISLinkedGridGroup,				(UIMenuCBK) RGISAnalyseSingleSampleGridCBK,	"RGIS22MenuSystem.html#AnalyzeNet_GridSampling"),
	UIMenuItem ("Multi Layer",	RGISNetworkGroup,		RGISLinkedGridGroup,				(UIMenuCBK) RGISAnalyseMultiSampleGridCBK,	"RGIS22MenuSystem.html#AnalyzeNet_GridSampling"),
	UIMenuItem ("Cell",			RGISNetworkGroup,		RGISLinkedGridGroup,				(UIMenuCBK) RGISAnNetworkCellSampleGridCBK,	"RGIS22MenuSystem.html#AnalyzeNet_GridSampling"),
	UIMenuItem ()};


extern void RGISAnNetworkBasinStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkHeadStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkDivideStatsCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkHistogramCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);
extern void RGISAnNetworkBasinDistribCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

static UIMenuItem _RGISAnalyseNetStatMenu [] = {
	UIMenuItem ("Basin",			RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkBasinStatsCBK,		"RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ("Headwater",	RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkHeadStatsCBK ,		"RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ("Divide",		RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkDivideStatsCBK ,		"RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ("Histogram",	RGISNetworkGroup,		RGISLinkedContinuousGroup, 	(UIMenuCBK) RGISAnNetworkHistogramCBK,			"RGIS22MenuSystem.html#AnalyzeNet_Stats"),
	UIMenuItem ("Category Distribution",RGISNetworkGroup,RGISLinkedDiscreteGroup, (UIMenuCBK) RGISAnNetworkBasinDistribCBK,		"RGIS22MenuSystem.html#AnalyzeNet_Stats"),
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
	UIMenuItem ("Add",			RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContAddCBK,			"RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Add"),
	UIMenuItem ("Subtract",		RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContSubtractCBK,		"RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Subtract"),
	UIMenuItem ("Multiply",		RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContMultiplyCBK,		"RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Multiply"),
	UIMenuItem ("Divide",		RGISContinuousGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnGContDivideCBK,		"RGIS22MenuSystem.html#AnalyzeCGrid_Operations_Divide"),
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
	UIMenuItem ("GNU XY-Plot",				RGISDataGroup,		UIMENU_NORULE, 				(UIMenuCBK) RGISAnGNUXYPlotCBK,						"RGIS22MenuSystem.html#AnalyzeAll_GNUXY-Plot"),
	UIMenuItem ("Fields",					RGISDataGroup,		UIMENU_NORULE,					_RGISAnalyseFieldCalcMenu),
	UIMenuItem ("Topo Accumulation",		RGISDataGroup,		UIMENU_NORULE,					(UIMenuCBK) RGISAnalyseFieldTopoAccumCBK,			"RGIS22MenuSystem.html#AnalyzeAll_TopoAccumulation"),
	UIMenuItem ("Topo Subtraction",		RGISDataGroup,		UIMENU_NORULE,					(UIMenuCBK) RGISAnalyseFieldTopoSubtractCBK,	"RGIS22MenuSystem.html#AnalyzeAll_TopoSubtraction"),
	UIMenuItem (RGISPointGroup,			UIMENU_NORULE),
	UIMenuItem ("STN Characteristics",	RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSTNCharCBK,			"RGIS22MenuSystem.html#AnalyzePoint_STNcharacteristics"),
	UIMenuItem ("STN Points",				RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSTNPointsCBK,			"RGIS22MenuSystem.html#AnalyzePoint_STNPoints"),
	UIMenuItem ("Subbasin Grid",			RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSubbasinCBK,			"RGIS22MenuSystem.html#AnalyzePoint_SubbasinGrid"),
	UIMenuItem ("Subbasin Center",		RGISPointGroup,	RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnalysePointSubbasinCenterCBK,	"RGIS22MenuSystem.html#AnalyzePoint_SubbasinCenter"),
	UIMenuItem ("Inter-station TS",		RGISPointGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnalysePointInterStationTSCBK,	"RGIS22MenuSystem.html#AnalyzePoint_InterstatTS"),
	UIMenuItem ("Grid Sampling",			RGISPointGroup,	RGISLinkedGridGroup,			_RGISAnalysePointGridSampleMenu),
	UIMenuItem ("Subbasin Statistics",	RGISPointGroup,	RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnalysePointSubbasinStatsCBK,	"RGIS22MenuSystem.html#AnalyzePoint_SubbasinStats"),
	UIMenuItem ("Subbasin Histogram",	RGISPointGroup,	RGISLinkedDiscreteGroup,	(UIMenuCBK) RGISAnalysePointSubbasinHistCBK,		"RGIS22MenuSystem.html#AnalyzePoint_SubbasinHist"),
	UIMenuItem (RGISLineGroup,				UIMENU_NORULE),
	UIMenuItem ("Grid Sampling",			RGISLineGroup,		RGISLinkedContinuousGroup,	_RGISAnalyseLineGridSampleMenu),
	UIMenuItem (RGISGridGroup,				UIMENU_NORULE),
	UIMenuItem ("Resample",					RGISGridGroup,		RGISLinkedNetworkGroup,		(UIMenuCBK) RGISAnGridResampleCBK,					"RGIS22MenuSystem.html#AnalyzeCGrid_Resample"),
	UIMenuItem (RGISDiscreteGroup,		UIMENU_NORULE),
	UIMenuItem ("Uniform Runoff Fields",RGISDiscreteGroup,UIMENU_NORULE,					(UIMenuCBK) RGISAnGDiscUniformRunoffCBK,			"RGIS22MenuSystem.html#AnalyzeDGrid_UniformRunoffFields"),
	UIMenuItem ("Reclass Discrete",		RGISDiscreteGroup,UIMENU_NORULE,					(UIMenuCBK) RGISAnGDiscReclassDiscreteCBK,		"RGIS22MenuSystem.html#AnalyzeDGrid_ReclassDGrid"),
	UIMenuItem ("Reclass Continuous",	RGISDiscreteGroup,UIMENU_NORULE,					(UIMenuCBK) RGISAnGDiscReclassContinuousCBK,		"RGIS22MenuSystem.html#AnalyzeDGrid_ReclassCGrid"),
	UIMenuItem ("Zone Histogram",			RGISDiscreteGroup,RGISLinkedDiscreteGroup,	(UIMenuCBK) RGISAnGDiscZoneHistCBK,					"RGIS22MenuSystem.html#AnalyzeDGrid_ZoneHist"),
	UIMenuItem ("Zone Staistics",			RGISDiscreteGroup,RGISLinkedContinuousGroup,	(UIMenuCBK) RGISAnGDiscZoneStatsCBK,				"RGIS22MenuSystem.html#AnalyzeDGrid_ZoneStats"),
	UIMenuItem (RGISContinuousGroup,		UIMENU_NORULE),
	UIMenuItem ("Find Pits",				RGISContinuousGroup,	RGISLinkedNetworkGroup,	(UIMenuCBK) RGISAnGContPitsCBK,						"RGIS22MenuSystem.html#AnalyzeCGrid_FindPits"),
	UIMenuItem ("Find Pour Points",		RGISContinuousGroup, RGISLinkedNetworkGroup,	(UIMenuCBK) RGISAnGContPourCBK,						"RGIS22MenuSystem.html#"),
	UIMenuItem ("Create Network",			RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContCreateNetworkCBK,			"RGIS22MenuSystem.html#AnalyzeCGrid_CreateNetwork"),
	UIMenuItem ("Merge",						RGISContinuousGroup,	RGISLinkedContinuousGroup,(UIMenuCBK) RGISAnGContMergeCBK,					"RGIS22MenuSystem.html#AnalyzeCGrid_Merge"),
	UIMenuItem ("Absolute",					RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContAbsCBK,						"RGIS22MenuSystem.html#AnalyzeCGrid_Abs"),
	UIMenuItem ("No Negative",				RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContNoNegCBK,						"RGIS22MenuSystem.html#AnalyzeCGrid_NoNeg"),
	UIMenuItem ("Operation",				RGISContinuousGroup,	RGISLinkedContinuousGroup,_RGISAnalyseGContOperMenu ),
	UIMenuItem ("Make Discrete",			RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContMakeDiscreteCBK,			"RGIS22MenuSystem.html#AnalyzeCGrid_MakeDiscrete"),
	UIMenuItem ("Cell Statistics",		RGISContinuousGroup,	UIMENU_NORULE,				(UIMenuCBK) RGISAnGContGirdCellStatsCBK,			"RGIS22MenuSystem.html#AnalyzeCGrid_CellStats"),
	UIMenuItem (RGISNetworkGroup,			UIMENU_NORULE),
	UIMenuItem ("Create Basingrid",		RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkBasinGridCBK,				"RGIS22MenuSystem.html#AnalyzeNet_Convert2BasinGrid"),
	UIMenuItem ("Convert to Grid",		RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkToGridCBK,					"RGIS22MenuSystem.html#AnalyzeNet_Convert2Grid"),
	UIMenuItem ("Station Points",			RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkStationsCBK,				"RGIS22MenuSystem.html#AnalyzeNet_StationPts"),
	UIMenuItem ("Accumulate",				RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkAccumulateCBK ,			"RGIS22MenuSystem.html#AnalyzeNet_Accumulate"),
	UIMenuItem ("Upstream Average",		RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkUpStreamAvgCBK ,			"RGIS22MenuSystem.html#AnalyzeNet_UpstreamAvg"),
	UIMenuItem ("Cell Slopes",				RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkCellSlopesCBK ,			"RGIS22MenuSystem.html#AnalyzeNet_CellSlopes"),
	UIMenuItem ("Basin Profile",			RGISNetworkGroup,	RGISLinkedContinuousGroup, (UIMenuCBK) RGISAnNetworkBasinProfCBK ,			"RGIS22MenuSystem.html#AnalyzeNet_BasinProfile"),
	UIMenuItem ("Grid Sampling",			RGISNetworkGroup,	RGISLinkedGridGroup,			_RGISAnalyseNetGridSampleMenu),
	UIMenuItem ("Statistics",				RGISNetworkGroup,	UIMENU_NORULE,					_RGISAnalyseNetStatMenu),
	UIMenuItem ("Stream Lines",			RGISNetworkGroup,	UIMENU_NORULE,					(UIMenuCBK) RGISAnNetworkStreamLinesCBK,			"RGIS22MenuSystem.html#AnalyzeNet_StreamLines"),
	UIMenuItem ()};
