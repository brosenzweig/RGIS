/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGISAnTable.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <rgis.H>

extern void RGISAnGNUXYPlotCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

UIMenuItem RGISAnalyseTableMenu [] = {
	UIMenuItem ("GNU XY-Plot",				RGISTableGroup,	UIMENU_NORULE, 				(UIMenuCBK) RGISAnGNUXYPlotCBK,						(char *) NULL),
	UIMenuItem ()};
