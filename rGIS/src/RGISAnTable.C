/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISAnTable.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

extern void RGISAnGNUXYPlotCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

UIMenuItem RGISAnalyseTableMenu [] = {
	UIMenuItem ((char *) "GNU XY-Plot",				RGISTableGroup,	UIMENU_NORULE, 				(UIMenuCBK) RGISAnGNUXYPlotCBK,						(char *) NULL),
	UIMenuItem ()};
