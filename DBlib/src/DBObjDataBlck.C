/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

DBObjDataBlck.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

DBObjTable *_DBCreateDataBlockSymbols ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNSymbolID,		DBTableFieldInt,		"%2d",	sizeof (DBShort)),
			DBTableFieldDefinition (DBrNForeground,	DBTableFieldInt,		"%2d",	sizeof (DBShort)),
			DBTableFieldDefinition (DBrNBackground,	DBTableFieldInt,		"%2d",	sizeof (DBShort)),
			DBTableFieldDefinition (DBrNStyle,			DBTableFieldInt,		"%2d",	sizeof (DBShort)),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNSymbols,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockPoints ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNCoord,			DBTableFieldCoord,	DBHiddenField,	sizeof (DBCoordinate),true),
			DBTableFieldDefinition (DBrNSymbol,			DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNItems,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockNodes ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNCoord,			DBTableFieldCoord,	DBHiddenField,	sizeof (DBCoordinate),true),
			DBTableFieldDefinition (DBrNLinkNum,		DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNNodes,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockLines ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNFromNode,		DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNToNode,			DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNPerimeter,		DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNRegion,			DBTableFieldRegion,	DBHiddenField,	sizeof (DBRegion),true),
			DBTableFieldDefinition (DBrNSymbol,			DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNVertexes,		DBTableFieldDataRec,	DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNVertexNum,		DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNItems,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockContours ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNFromNode,		DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNToNode,			DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNLeftPoly,		DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNRightPoly,		DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNNextLine,		DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNPrevLine,		DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNPerimeter,		DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNRegion,			DBTableFieldRegion,	DBHiddenField,	sizeof (DBRegion),true),
			DBTableFieldDefinition (DBrNVertexes,		DBTableFieldDataRec,	DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNVertexNum,		DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNContours,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockPolygons ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNArea,			DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNPerimeter,		DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNRegion,			DBTableFieldRegion,	DBHiddenField,	sizeof (DBRegion),true),
			DBTableFieldDefinition (DBrNSymbol,			DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNFirstLine,		DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNLineNum,		DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition (DBrNVertexNum,		DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNItems,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockNetBasins ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNMouthPos,		DBTableFieldPosition,DBHiddenField,	sizeof (DBPosition),true),
			DBTableFieldDefinition (DBrNBasinOrder,	DBTableFieldInt,		"%2d",			sizeof (DBByte),true),
			DBTableFieldDefinition (DBrNColor,			DBTableFieldInt,		"%2d",			sizeof (DBInt),true),
			DBTableFieldDefinition (DBrNSymbol,			DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition (DBrNBasinLength,	DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat4),true),
			DBTableFieldDefinition (DBrNBasinArea,		DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat4),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNItems,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockNetCells ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNPosition,		DBTableFieldPosition,DBHiddenField,	sizeof (DBPosition),true),
			DBTableFieldDefinition (DBrNToCell,			DBTableFieldInt,		"%2X",			sizeof (DBByte),true),
			DBTableFieldDefinition (DBrNFromCell,		DBTableFieldInt,		"%2X",			sizeof (DBByte),true),
			DBTableFieldDefinition (DBrNOrder,			DBTableFieldInt,		"%2d",			sizeof (DBByte),true),
			DBTableFieldDefinition (DBrNBasin,			DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition (DBrNBasinCells,	DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition (DBrNTravel,			DBTableFieldInt,		"%8d",			sizeof (DBInt),true),
			DBTableFieldDefinition (DBrNUpCellPos,		DBTableFieldPosition,DBHiddenField,	sizeof (DBPosition),true),
			DBTableFieldDefinition (DBrNCellArea,		DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat4),true),
			DBTableFieldDefinition (DBrNCellLength,	DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat4),true),
			DBTableFieldDefinition (DBrNSubbasinArea,	DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat4),true),
			DBTableFieldDefinition (DBrNSubbasinLength,DBTableFieldFloat,	"%10.1f",		sizeof (DBFloat4),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNCells,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockGridLayers ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNRowNum,			DBTableFieldInt,		"%6d",			sizeof (DBShort),true),
			DBTableFieldDefinition (DBrNColNum,			DBTableFieldInt,		"%6d",			sizeof (DBShort),true),
			DBTableFieldDefinition (DBrNValueType,		DBTableFieldInt,		"%1d",			sizeof (DBByte),true),
			DBTableFieldDefinition (DBrNValueSize,		DBTableFieldInt,		"%1d",			sizeof (DBByte),true),
			DBTableFieldDefinition (DBrNCellWidth,		DBTableFieldFloat,	"%8.3f",			sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNCellHeight,	DBTableFieldFloat,	"%8.3f",			sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNLayer,			DBTableFieldDataRec,	DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNLayers,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockGridVariableStats ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNMinimum,		DBTableFieldFloat,	"%10.3f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNMaximum,		DBTableFieldFloat,	"%10.3f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNAverage,		DBTableFieldFloat,	"%10.3f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNStdDev,			DBTableFieldFloat,	"%10.3f",		sizeof (DBFloat),true),
			DBTableFieldDefinition (DBrNMissingValue,	DBTableFieldFloat,	DBHiddenField,	sizeof (DBFloat),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNItems,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockGridCategoryTable ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNGridValue,		DBTableFieldInt,		"%3d",			sizeof (DBInt),true),
			DBTableFieldDefinition (DBrNSymbol,			DBTableFieldTableRec,DBHiddenField,	sizeof (DBAddress),true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNItems,fieldDefs));
	}

DBObjTable *_DBCreateDataBlockRelateTable ()

	{
	DBTableFieldDefinition fieldDefs [] =
		{	DBTableFieldDefinition (DBrNRelateData,		DBTableFieldString,"%s",			DBDataNameLen,true),
			DBTableFieldDefinition (DBrNRelateField,		DBTableFieldString,"%s",			DBStringLength,true),
			DBTableFieldDefinition (DBrNRelateJoinField,	DBTableFieldString,"%s",			DBStringLength,true),
			DBTableFieldDefinition () };
	return (new DBObjTable (DBrNRelations,fieldDefs));
	}

