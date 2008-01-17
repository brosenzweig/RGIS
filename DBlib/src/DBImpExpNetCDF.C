/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

DBImpExpNetCDF.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>
#include <netcdf.h>

extern "C" {
#include <udunits.h>
}

static const char *_DBExportNetCDFRename (const char *name)
	{
	class {
		public:
			char *OldName;
			char *NewName;
		} names [] = { 
			{ "CellXCoord",              "cell_x-coord"},
			{ "CellYCoord",              "cell_y-coord"},
			{ "Point",                   "point"},
			{ "Line",                    "line"},
			{ "Polygon",                 "polygon"},
			{ "Continuous",              "continuous"},
			{ "Discrete",                "discrete"},
			{ "Network",                 "network"},
			{ GHAASSubjRunoff,           "runoff"},
			{ GHAASSubjDischarge,        "discharge"},
			{ GHAASSubjFlowHeight,       "flow_height"},
			{ GHAASSubjFlowWidth,        "flow_width"},
			{ GHAASSubjFlowVelocity,     "flow_velocity"},
			{ GHAASSubjFlowSurfSlope,    "slope"},
			{ GHAASSubjRiverStorage,     "river_storage"},
			{ GHAASSubjFloodStorage,     "flood_storage"},
			{ GHAASSubjFloodArea,        "flood_area"},
			{ GHAASSubjOverlandStorage,  "overland_storage"},
			{ GHAASSubjOverlandVelocity, "overland_velocity"},
			{ GHAASSubjPrecip,           "precipitation"},
			{ GHAASSubjAirTemp,          "air_temperature"},
			{ GHAASSubjHumidity,         "humidity"},
			{ GHAASSubjVaporPres,        "vapor_pressure"},
			{ GHAASSubjWindSpeed,        "wind_speed"},
			{ GHAASSubjSolarRad,         "solar_radiation"},
			{ GHAASSubjStations,         "stations"},
			{ GHAASSubjReservoirs,       "reservoirs"},
			{ GHAASSubjElevation,        "elevation"},
			{ GHAASSubjPits,             "pits"},
			{ DBrNCoord,                 "coord"},
			{ DBrNVertexes,              "vertexes"},
			{ DBrNVertexNum,             "vertex_num"},
			{ DBrNRegion,                "region"},
			{ DBrNSymbol,                "symbol"},
			{ DBrNSymbolID,              "symbol_id"},
			{ DBrNForeground,            "foreground"},
			{ DBrNBackground,            "background"},
			{ DBrNStyle,                 "style"},
			{ DBrNLinkNum,               "link_num"},
			{ DBrNLeftPoly,              "left_polygon"},
			{ DBrNRightPoly,             "right_polygon"},
			{ DBrNNextLine,              "next_line"},
			{ DBrNPrevLine,              "prev_line"},
			{ DBrNFromNode,              "from_node"},
			{ DBrNToNode,                "to_node"},
			{ DBrNPerimeter,             "perimeter"},
			{ DBrNFirstLine,             "first_line"},
			{ DBrNLineNum,               "line_num"},
			{ DBrNArea,                  "area"},
			{ DBrNMouthPos,              "mouth"},
			{ DBrNColor,                 "color"},
			{ DBrNBasinOrder,            "basin_order"},
			{ DBrNBasinArea,             "basin_area"},
			{ DBrNBasinLength,           "basin_length"},
			{ DBrNLookupGrid,            "lookup_grid"},
			{ DBrNPosition,              "position"},
			{ DBrNToCell,                "to_cell"},
			{ DBrNFromCell,              "from_cell"},
			{ DBrNOrder,                 "order"},
			{ DBrNMagnitude,             "magnitude"},
			{ DBrNBasin,                 "basin_id"},
			{ DBrNBasinCells,            "basin_cells"},
			{ DBrNTravel,                "travel"},
			{ DBrNUpCellPos,             "most_upcell_pos"},
			{ DBrNCellArea,              "cell_area"},
			{ DBrNCellLength,            "cell_length"},
			{ DBrNSubbasinArea,          "subbasin_area"},
			{ DBrNSubbasinLength,        "subbasin_length"},
			{ DBrNDistToMouth,           "dist_to_mouth"},
			{ DBrNDistToOcean,           "dist_to_outlet"},
			{ DBrNRowNum,                "row_num"},
			{ DBrNColNum,                "col_num"},
			{ DBrNCellWidth,             "cell_width"},
			{ DBrNCellHeight,            "cell_height"},
			{ DBrNValueType,             "value_type"},
			{ DBrNValueSize,             "value_size"},
			{ DBrNLayer,                 "layer"},
			{ DBrNMinimum,               "minimum"},
			{ DBrNMaximum,               "maximum"},
			{ DBrNAverage,               "average"},
			{ DBrNStdDev,                "stddev"},
			{ DBrNMissingValue,          "missing_value"},
			{ DBrNGridValue,             "grid_value"},
			{ DBrNGridArea,              "arid_area"},
			{ DBrNGridPercent,           "grid_percent"},
			{ DBrNItems,                 "items"},
			{ DBrNSelection,             "selection"},
			{ DBrNNodes,                 "nodes"},
			{ DBrNLineList,              "line_list"},
			{ DBrNContours,              "contours"},
			{ DBrNContourList,           "contour_list"},
			{ DBrNSymbols,               "symbols"},
			{ DBrNCells,                 "cells"},
			{ DBrNLayers,                "layers"},
			{ DBrNGroups,                "groups"},
			{ DBrNRelateData,            "relate_data"},
			{ DBrNRelateField,           "relate_field"},
			{ DBrNRelateJoinField,       "join_field"},
			{ DBrNRelations,             "relations"},
			{ DBrNCategoryID,            "category_id"},
			{ DBrNCategory,              "Category"},
			{ DBrNPercent,               "percent"},
			{ DBDocSubject,              "subject"},
			{ DBDocGeoDomain,            "geo_domain"},
			{ DBDocVersion,              "version"},
			{ DBDocCitationRef,          "reference"},
			{ DBDocCitationInst,         "institue"},
			{ DBDocSourceInst,           "source_institue"},
			{ DBDocSourcePerson,         "source_person"},
			{ DBDocOwnerPerson,          "contact_person"},
			{ DBDocComment,              "comment"}};
	size_t i;

	for (i = 0;i < sizeof (names) / sizeof (names [0]);++i)
		if (strcmp (names [i].OldName,name) == 0) return (names [i].NewName);
	return (name);
	}

enum { DIMTime, DIMLevel, DIMLat, DIMLon, };

static DBInt _DBExportNetCDFPoint (DBObjData *dbData, int ncid)
	{
	const char *str, *varname;
	int status, latid, lonid, dimid;
	size_t start, count;
	double extent [2];
	DBInt pntID;
	DBCoordinate coord;
	DBObjTable *table = dbData->Table (DBrNItems);
	DBVPointIO *pointIO;

	if ((status = nc_redef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	if ((status = nc_def_dim (ncid,_DBExportNetCDFRename (DBrNItems),table->ItemNum (),&dimid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }

	/* Begin Defining Latitude Variable */
	varname = dbData->Projection () ==  DBProjectionSpherical ? "latitude" : "y-coord";
	if ((status = nc_def_var (ncid,varname,  NC_DOUBLE, (int) 1,&dimid, &latid))  != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	str = "Latitude";
	if ((status = nc_put_att_text (ncid,latid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	str = "latitude_north";
	if ((status = nc_put_att_text (ncid,latid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	str = "degrees_north";
	if ((status = nc_put_att_text (ncid,latid,"units",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	extent [0] = -90.0;
	extent [1] =  90.0;
	if ((status = nc_put_att_double (ncid,latid,"valid_range", NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	extent [0] = (dbData->Extent ()).LowerLeft.Y;
	extent [1] = (dbData->Extent ()).UpperRight.Y;
	if ((status = nc_put_att_double (ncid,latid,"actual_range",NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	str = "y";
	if ((status = nc_put_att_text (ncid,latid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	/* End Defining Latitude Variable */

	/* Begin Defining Longitude Variable */
	varname = dbData->Projection () ==  DBProjectionSpherical ? "longitude" : "x-coord";
	if ((status = nc_def_var (ncid,"longitude",  NC_DOUBLE, (int) 1,&dimid, &lonid))  != NC_NOERR)
	 	{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	str = "Longitude";
	if ((status = nc_put_att_text (ncid,lonid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
		str = "longitude_east";
	if ((status = nc_put_att_text (ncid,lonid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	str = "degrees_east";
	if ((status = nc_put_att_text (ncid,lonid,"units",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	extent [0] = -180.0;
	extent [1] =  180.0;
	if ((status = nc_put_att_double (ncid,lonid,"valid_range", NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	extent [0] = (dbData->Extent ()).LowerLeft.X;
	extent [1] = (dbData->Extent ()).UpperRight.X;
	if ((status = nc_put_att_double (ncid,lonid,"actual_range",NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	str = "x";
	if ((status = nc_put_att_text (ncid,lonid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	/* End Defining Longitude Variable */

	if ((status = nc_enddef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }

	pointIO = new DBVPointIO (dbData);
	for (pntID = 0;pntID < pointIO->ItemNum ();pntID++)
	{
		coord = pointIO->Coordinate (pointIO->Item (pntID));
		start = pntID;
		count = 1;
	if ((status = nc_put_vara_double (ncid,latid,&start,&count,&coord.X)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete pointIO; return (DBFault); }
	if ((status = nc_put_vara_double (ncid,lonid,&start,&count,&coord.Y)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete pointIO; return (DBFault); }
	}
	
	delete pointIO;
	return (DBSuccess);
	}

static DBInt _DBExportNetCDFGridDefine (DBObjData *dbData,int ncid, int dimids [])

	{
	char *str;
	int rowNum, colNum, i;
	int status, levelid, latid,  lonid;
	size_t start, count;
	double extent [2], cellWidth, cellHeight, *record;

	switch (dbData->Type ())
		{
		case DBTypeGridDiscrete:
		case DBTypeGridContinuous:
			{
			DBGridIO *gridIO = new DBGridIO (dbData);

			rowNum = gridIO->RowNum ();
			colNum = gridIO->ColNum ();
			cellWidth  = gridIO->CellWidth ();
			cellHeight = gridIO->CellHeight ();
			delete gridIO;
			} break;
		case DBTypeNetwork:
			{
			DBNetworkIO *netIO = new DBNetworkIO (dbData);
			rowNum = netIO->RowNum ();
			colNum = netIO->ColNum ();
			cellWidth  = netIO->CellWidth ();
			cellHeight = netIO->CellHeight ();
			delete netIO;
			} break;
		default:
			fprintf (stderr,"Invalid data type in: _DBExportNetCDFGridDefine ()\n");
			return (DBFault);
		}

	if ((status = nc_redef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	/* Begin Defining Dimensions */
	if ((status = nc_def_dim (ncid,"level",    1,     dimids + DIMLevel)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	if ((status = nc_def_dim (ncid,"latitude", rowNum,dimids + DIMLat))   != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	if ((status = nc_def_dim (ncid,"longitude",colNum,dimids + DIMLon))   != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	/* End Defining Dimensions */

	/* Begin Defining Latitude Variable */
	if ((status = nc_def_var (ncid,"latitude",  NC_DOUBLE, (int) 1,dimids + DIMLat, &latid))  != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "Latitude";
	if ((status = nc_put_att_text (ncid,latid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "latitude_north";
	if ((status = nc_put_att_text (ncid,latid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "degrees_north";
	if ((status = nc_put_att_text (ncid,latid,"units",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	extent [0] = -90.0;
	extent [1] =  90.0;
	if ((status = nc_put_att_double (ncid,latid,"valid_range", NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	extent [0] = (dbData->Extent ()).LowerLeft.Y;
	extent [1] = (dbData->Extent ()).UpperRight.Y;
	if ((status = nc_put_att_double (ncid,latid,"actual_range",NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "y";
	if ((status = nc_put_att_text (ncid,latid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	/* End Defining Latitude Variable */

	/* Begin Defining Longitude Variable */
	if ((status = nc_def_var (ncid,"longitude",  NC_DOUBLE, (int) 1,dimids + DIMLon, &lonid))  != NC_NOERR)
	 	{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "Longitude";
	if ((status = nc_put_att_text (ncid,lonid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "longitude_east";
	if ((status = nc_put_att_text (ncid,lonid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "degrees_east";
	if ((status = nc_put_att_text (ncid,lonid,"units",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	extent [0] = -180.0;
	extent [1] =  180.0;
	if ((status = nc_put_att_double (ncid,lonid,"valid_range", NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	extent [0] = (dbData->Extent ()).LowerLeft.X;
	extent [1] = (dbData->Extent ()).UpperRight.X;
	if ((status = nc_put_att_double (ncid,lonid,"actual_range",NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "x";
	if ((status = nc_put_att_text (ncid,lonid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	/* End Defining Longitude Variable */

	/* Begin Defining Level Variable */
	if ((status = nc_def_var (ncid,"level",  NC_DOUBLE, (int) 1,dimids + DIMLevel, &levelid))  != NC_NOERR)
	 	{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "Level";
	if ((status = nc_put_att_text (ncid,levelid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = "level";
	if ((status = nc_put_att_text (ncid,levelid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	/* End Defining Level Variable */

	if ((status = nc_enddef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

	if ((record = (double *) calloc (rowNum > colNum ? rowNum : colNum,sizeof (double))) == (double *) NULL)
		{ fprintf (stderr,"Memory allocation error in: DBExportNetCDF ()\n"); return (DBFault); }
	for (i = 0;i < rowNum;i++) record [i] = (dbData->Extent ()).LowerLeft.Y + i * cellHeight;
	start = 0;
	count = rowNum;
	if ((status = nc_put_vara_double (ncid,latid,&start,&count,record)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); free (record); return (DBFault); }
	for (i = 0;i < colNum;i++) record [i] = (dbData->Extent ()).LowerLeft.X + i * cellWidth;
	count = colNum;
	if ((status = nc_put_vara_double (ncid,lonid,&start,&count,record)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); free (record); return (DBFault); }
	free (record);
	return (DBSuccess);
	}

static DBInt _DBExportNetCDFTimeDefine (DBObjData *dbData,int ncid,int dimids [])
	{
	char *str, timeStr [DBStringLength], unitStr [NC_MAX_NAME];
	int status, timeid, i;
	int bYear, bMonth, bDay, bHour, bMinute;
	int eYear, eMonth, eDay, eHour, eMinute;
	utUnit unit;
	double extent [2];
	size_t start, count;
	double *record;
	DBInt layerID;
	DBGridIO	*gridIO = new DBGridIO (dbData);

	/* Begin Defining Dimensions */
	if ((status = nc_redef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	if ((status = nc_def_dim (ncid,"time",NC_UNLIMITED,    dimids + DIMTime)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	/* End Defining Dimensions */

	/* Begin Defining Time Variable */
	if ((status = nc_def_var (ncid,"time", NC_DOUBLE,(int) 1,dimids + DIMTime,&timeid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	str = "Time";
	if ((status = nc_put_att_text (ncid,timeid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	str = "time";
	if ((status = nc_put_att_text (ncid,timeid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	strcpy (timeStr,(gridIO->Layer (gridIO->LayerNum () - 1))->Name ());
	if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
	switch (strlen (timeStr))
		{
		case  4:
			strcat (timeStr,"-01-01 00:00:00");
			sscanf (timeStr,"%4d",&eYear);
			eMonth = eDay = eHour = eMinute = 0;
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d",&bYear);
			bMonth = bDay = bHour = bMinute = 0;
			sprintf (unitStr,"years since %s", timeStr);
			str = "0001-00-00 00:00:00";
			break;
		case  7:
			strcat (timeStr,"-01 00:00:00");
			sscanf (timeStr,"%4d-%2d",&eYear,&eMonth);
			eDay = eHour = eMinute = 0;
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d",&bYear,&bMonth);
			bDay = bHour = bMinute = 0;
			sprintf (unitStr,"months since %s", timeStr);
			str = "0000-01-00 00:00:00";
			break;
		case 10:
			strcat (timeStr," 00:00:00");
			sscanf (timeStr,"%4d-%2d-%2d",&eYear,&eMonth,&eDay);
			eHour = eMinute = 0;
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d-%2d",&bYear,&bMonth,&bDay);
			bHour = bMinute = 0;
			sprintf (unitStr,"days since %s",   timeStr);
			str = "0000-00-01 00:00:00";
			break;
		case 13:
			strcat (timeStr,":00:00");
			sscanf (timeStr,"%4d-%2d-%2d %2d",&eYear,&eMonth,&eDay,&eHour);
			eMinute = 0;
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d-%2d %2d",&bYear,&bMonth,&bDay,&bHour);
			bMinute = 0;
			sprintf (unitStr,"hours since %s",  timeStr);
			str = "0000-00-00 01:00:00";
			break;
		case 16:
			strcat (timeStr,":00");
			sscanf (timeStr,"%4d-%2d-%2d %2d:%2d",&eYear,&eMonth,&eDay,&eHour,&eMinute);
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d-%2d %2d:%2d",&bYear,&bMonth,&bDay,&bHour,&bMinute);
			sprintf (unitStr,"minutes since %s",timeStr);
			str = "0000-00-00 00:01:00";
			break;
		}
	if (utScan (unitStr,&unit) != 0)
		{ fprintf (stderr,"Invalid time Unit [%s] in: DBImportNetCDF ()",unitStr); delete gridIO; return (DBFault); }
	if (utInvCalendar (bYear,bMonth,bDay,bHour,bMinute,(double) 0.0,&unit,extent) != 0)	
		{ fprintf (stderr,"Invalid time [%s] in: DBImportNetCDF ()",timeStr); delete gridIO; return (DBFault); }
	if (utInvCalendar (eYear,eMonth,eDay,eHour,eMinute,(double) 0.0,&unit,extent + 1) != 0)	
		{ fprintf (stderr,"Invalid time [%s] in: DBImportNetCDF ()",timeStr); delete gridIO; return (DBFault); }
	if ((status = nc_put_att_double (ncid,timeid,"actual_range",NC_DOUBLE,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }

	if ((status = nc_put_att_text (ncid,timeid,"units",strlen (unitStr),unitStr)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	if ((status = nc_put_att_text (ncid,timeid,"delta_t",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	if ((status = nc_put_att_text (ncid,timeid,"avg_period",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	str = "t";
	if ((status = nc_put_att_text (ncid,timeid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	/* End Defining Time Variable */

	if ((status = nc_enddef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "%s\n", nc_strerror(status)); delete gridIO; return (DBFault); }

	if ((record = (double *) calloc (gridIO->LayerNum (),sizeof (double))) == (double *) NULL)
		{
		fprintf (stderr,"Memory allocation error in: DBExportNetCDF ()\n");
		delete gridIO;
		return (DBFault);
		}
	for (layerID = 0;layerID < gridIO->LayerNum ();layerID++)
		{
		strcpy (timeStr,(gridIO->Layer (layerID))->Name ());
		if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
		switch (strlen (timeStr))
			{
			case  4: sscanf (timeStr,"%4d",&bYear); break;
			case  7: sscanf (timeStr,"%4d-%2d",&bYear,&bMonth); break;
			case 10: sscanf (timeStr,"%4d-%2d-%2d",&bYear,&bMonth,&bDay); break;
			case 13: sscanf (timeStr,"%4d-%2d-%2d %2d",&bYear,&bMonth,&bDay,&bHour); break;
			case 16: sscanf (timeStr,"%4d-%2d-%2d %2d:%2d",&bYear,&bMonth,&bDay,&bHour,&bMinute); break;
			}
		if (utInvCalendar (bYear,bMonth,bDay,bHour,bMinute,(double) 0.0,&unit,record + layerID) != 0)	
			{
			fprintf (stderr,"Invalid time [%s] in: DBImportNetCDF ()",timeStr);
			delete gridIO;
			free (record);
			return (DBFault);
			}
		}
	start = 0;
	count = gridIO->LayerNum ();
	if ((status = nc_put_vara_double (ncid,timeid,&start,&count,record)) != NC_NOERR)
		{
		fprintf(stderr, "%s\n", nc_strerror(status));
		delete gridIO;
		free (record);
		return (DBFault);
		}
	delete gridIO;
	free (record);
	return (DBSuccess);
	}

static DBInt _DBExportNetCDFTable (DBObjTable *table,int ncid)
	{
	int status, dimids [2];
	size_t index [2], count [2];
	const char *tableName, *fieldName;
	nc_type vtype;
	DBInt fieldID, itemID;
	DBObjTableField *fieldRec;
	DBObjRecord *itemRec;

	if ((status = nc_redef (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }

	tableName = _DBExportNetCDFRename (table->Name ());
	if (!(((nc_inq_dimid (ncid,"time",dimids) == NC_NOERR) ||
	       (nc_inq_dimid (ncid,tableName,dimids) == NC_NOERR)) &&
	      (nc_inq_dimlen (ncid,dimids [0],index) == NC_NOERR)  &&
		   (index [0] == (size_t) table->ItemNum ())))
		{
		if ((status = nc_def_dim (ncid,tableName,table->ItemNum (),dimids)) != NC_NOERR)
			{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
		}
	if (strcmp (tableName,"items") == 0)
		{
		int varid;
		char *str;
		if (((nc_inq_dimid (ncid,"string",dimids + 1) == NC_NOERR) || ((nc_def_dim (ncid,"string",64,dimids + 1)) == NC_NOERR)) &&
			 ((status = nc_def_var (ncid,tableName,NC_CHAR,(int) 2,dimids,&varid))  == NC_NOERR) &&
			 ((status = nc_enddef (ncid)) == NC_NOERR))
			{
			index [1] = 0;
			count [0] = 1;
			for (itemID = 0;itemID < table->ItemNum ();itemID++)
				{
				itemRec = table->Item (itemID);
				str = itemRec->Name ();
				index [0] = itemID;
				count [1] = strlen (str) + 1;
				if ((status = nc_put_vara_text (ncid,varid,index,count,str)) != NC_NOERR)
					{ fprintf(stderr, "NC Error: %s [%s]\n", nc_strerror(status), fieldRec->Name ()); return (DBFault); }
				}
			if ((status = nc_redef  (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
			}
		else { fprintf(stderr, "Skipping Items\n"); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
		}

	for (fieldID = 0;fieldID < table->FieldNum ();++fieldID)
		{
		fieldRec = table->Field (fieldID);
		if (!DBTableFieldIsVisible (fieldRec) && (strcmp (fieldRec->Name (),"MissingValue") == 0)) continue;
		fieldName = _DBExportNetCDFRename (fieldRec->Name ());
		switch (fieldRec->Type ())
			{
			case DBTableFieldString:
				{
				int varid;
				size_t dimlen;
				char *str, *dimname;

				if      (fieldRec->Length () <= 8)           { dimname = "short_string"; dimlen = 8; }
				else if (fieldRec->Length () <= 64)          { dimname = "string";       dimlen = 64; }
				else if (fieldRec->Length () <= NC_MAX_NAME) { dimname = "long_string";  dimlen = NC_MAX_NAME; }
				else { dimname = "text";  dimlen = 256; }

				if (((nc_inq_dimid (ncid,dimname,dimids + 1) == NC_NOERR) || ((nc_def_dim (ncid,dimname,dimlen,dimids + 1)) == NC_NOERR)) &&
					 ((status = nc_def_var (ncid,fieldName,NC_CHAR,(int) 2,dimids,&varid))  == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{
					index [1] = 0;
					count [0] = 1;
					for (itemID = 0;itemID < table->ItemNum ();itemID++)
						{
						itemRec = table->Item (itemID);
						str = fieldRec->String (itemRec);
						index [0] = itemID;
						count [1] = strlen (str) + 1;
						if ((status = nc_put_vara_text (ncid,varid,index,count,str)) != NC_NOERR)
							{ fprintf(stderr, "NC Error: %s [%s]\n", nc_strerror(status), fieldRec->Name ()); return (DBFault); }
						}
					if ((status = nc_redef  (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
					}
				else { fprintf(stderr, "Skipping variable :%s\n", fieldName); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
				} break;
			case DBTableFieldInt:
				{
				int varid;
				switch (fieldRec->Length ())
					{
					case sizeof (char):
					case sizeof (short): vtype = NC_SHORT; break;
					case sizeof (int):   vtype = NC_INT;   break;
					default: { fprintf (stderr,"Invalide field size in: _DBExportNetCDF ()\n"); return (DBFault); }
					}
				if (((status = nc_def_var (ncid,fieldName,vtype,(int) 1,dimids,&varid))  == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{
					int var;

					for (itemID = 0;itemID < table->ItemNum ();itemID++)
						{
						itemRec = table->Item (itemID);
						var = fieldRec->Int (itemRec);
						index [0] = itemID;
						if ((status = nc_put_var1_int (ncid,varid,index,&var)) != NC_NOERR)
							{ fprintf(stderr, "NC Error; %s [%s]\n", nc_strerror(status),fieldRec->Name ()); return (DBFault); }
						}
					if ((status = nc_redef  (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
					}
				else { fprintf(stderr, "Skipping variable :%s\n", fieldName); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
				} break;
			case DBTableFieldFloat:
				{
				int varid;
				switch (fieldRec->Length ())
					{
					case sizeof (DBFloat4): vtype = NC_FLOAT;  break;
					case sizeof (DBFloat):  vtype = NC_DOUBLE; break;
					default: { fprintf (stderr,"Invalide field size in: _DBExportNetCDF ()\n"); return (DBFault); }
					}
				if (((status = nc_def_var (ncid,fieldName,vtype,(int) 1,dimids,&varid)) == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{
					double var;

					for (itemID = 0;itemID < table->ItemNum ();itemID++)
						{
						itemRec = table->Item (itemID);
						var = fieldRec->Float (itemRec);
						index [0] = itemID;
						if ((status = nc_put_var1_double (ncid,varid,index,&var)) != NC_NOERR)
							{ fprintf(stderr, "NC Error: %s [%s]\n", nc_strerror(status),fieldRec->Name ()); return (DBFault); }
						}
					if ((status = nc_redef  (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
					}
				else { fprintf(stderr, "Skipping variable :%s\n", fieldName); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
				} break;
			case DBTableFieldDate:
				{
				int varid;
				if (((status = nc_def_var (ncid,fieldName,NC_DOUBLE,(int) 1,dimids,&varid)) == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{
					}
				else { fprintf(stderr, "Skipping variable :%s\n", fieldName); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
				} break;
			case DBTableFieldCoord:  break;
			case DBTableFieldRegion:
				{
				int varid;
				double box [4];
				if (((nc_inq_dimid (ncid,"box",dimids + 1) == NC_NOERR) || ((nc_def_dim (ncid,"box",4,dimids + 1)) == NC_NOERR)) &&
				    ((status = nc_def_var (ncid,fieldName,NC_DOUBLE,(int) 2,dimids, &varid))  == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{
					DBRegion region;

					index [1] = 0;
					count [0] = 1;
					count [1] = 4;
					for (itemID = 0;itemID < table->ItemNum ();itemID++)
						{
						itemRec = table->Item (itemID);
						region = fieldRec->Region (itemRec);
						index [0] = itemID;
						box [0] = region.LowerLeft.X;
						box [1] = region.LowerLeft.Y;
						box [2] = region.UpperRight.X;
						box [3] = region.UpperRight.Y;
						if ((status = nc_put_vara_double (ncid,varid,index,count, box))  != NC_NOERR)
							{ fprintf(stderr, "NC Error: %s [%s]\n", nc_strerror(status),fieldRec->Name ()); return (DBFault); }
						}
					if ((status = nc_redef  (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
					}
				else { fprintf(stderr, "Skipping variable :%s\n", fieldName); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
				} break;
			case DBTableFieldPosition:
				{
				char rowName [NC_MAX_NAME], colName [NC_MAX_NAME];
				int rvarid, cvarid;
				
				sprintf (rowName,"%s_row",fieldName);
				sprintf (colName,"%s_col",fieldName);
				if (((status = nc_def_var (ncid,rowName,NC_SHORT,(int) 1,dimids,&rvarid))  == NC_NOERR) &&
				    ((status = nc_def_var (ncid,colName,NC_SHORT,(int) 1,dimids,&cvarid))  == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{
					DBPosition pos;

					for (itemID = 0;itemID < table->ItemNum ();itemID++)
						{
						itemRec = table->Item (itemID);
						pos = fieldRec->Position (itemRec);
						index [0] = itemID;
						if (((status = nc_put_var1_short (ncid,cvarid,index, &(pos.Col))) != NC_NOERR) ||
						    ((status = nc_put_var1_short (ncid,rvarid,index, &(pos.Row))) != NC_NOERR))
							{ fprintf(stderr, "NC Error: %s [%s]\n", nc_strerror(status),fieldRec->Name ()); return (DBFault); }
						}
					if ((status = nc_redef  (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
					}
				else { fprintf(stderr, "Skipping variable :%s\n", fieldName); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
				} break;
			case DBTableFieldTableRec:
			case DBTableFieldDataRec:
				{
				int varid;
				if (((status = nc_def_var (ncid,fieldName,NC_INT,(int) 1,dimids,&varid)) == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{
					int var;

					for (itemID = 0;itemID < table->ItemNum ();itemID++)
						{
						itemRec = table->Item (itemID);
						var = (fieldRec->Record (itemRec))->RowID ();
						index [0] = itemID;
						if ((status = nc_put_var1_int (ncid,varid,index,&var)) != NC_NOERR)
							{ fprintf(stderr, "NC Error; %s [%s]\n", nc_strerror(status),fieldRec->Name ()); return (DBFault); }
						}
					if ((status = nc_redef  (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
					}
				else { fprintf(stderr, "Skipping variable :%s\n", fieldName); fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); }
				} break;
			default: { fprintf (stderr,"Invalid field type in DBExportNetCDFTable ()\n"); return (DBFault); }
			}
		}
	if ((status = nc_enddef (ncid)) != NC_NOERR) { fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	return (DBSuccess);
	}

DBInt DBExportNetCDF (DBObjData *dbData,const char *fileName)

	{
	const char *str;
	int ncid, status, dimids [4], varid;
	size_t start [4], count [4];

	switch (utInit (""))
		{
		case 0: break;
		case UT_ENOFILE: fprintf (stderr,"Invalide UDUNITS_PATH in: DBImportNetCDF ()\n"); return (DBFault);
		case UT_ESYNTAX: fprintf (stderr,"Corrupt udunit file in: DBImportNetCDF ()\n");   return (DBFault);
		default: fprintf (stderr,"UDUNIT Initialization error in: DBImportNetCDF ()\n");   return (DBFault);
		}

	if ((status = nc_create (fileName,NC_CLOBBER,&ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"Conventions",strlen ("COARDS-NCghaas"),"COARDS-NCghaas")) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"title",strlen (dbData->Name ()),dbData->Name ())) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = _DBExportNetCDFRename (DBDataTypeString (dbData->Type ()));
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"data_type",  strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = dbData->Document (DBDocGeoDomain);
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"domain",     strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = _DBExportNetCDFRename (dbData->Document (DBDocSubject));
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"subject",    strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = dbData->Document (DBDocCitationRef);
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"references", strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = dbData->Document (DBDocCitationInst);
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"institution",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = dbData->Document (DBDocSourceInst);
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"source",     strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = dbData->Document (DBDocComment);
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"comments",   strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

	if ((status = nc_enddef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

	switch (dbData->Type ())
		{
		case DBTypeVectorPoint:
			{
			if (_DBExportNetCDFPoint (dbData, ncid) == DBFault)                     { nc_close (ncid); return (DBFault); }
			if (_DBExportNetCDFTable (dbData->Table (DBrNItems),  ncid) == DBFault) { nc_close (ncid); return (DBFault); }
			if (_DBExportNetCDFTable (dbData->Table (DBrNSymbols),ncid) == DBFault) { nc_close (ncid); return (DBFault); }
			} break;
		case DBTypeGridDiscrete:
			{
			short *record, fillVal = DBFault;
			DBInt intVal;
			DBInt layerID;
			DBPosition pos;
			DBObjRecord *layerRec;
			DBGridIO *gridIO;

			if (_DBExportNetCDFGridDefine (dbData,ncid,dimids)          == DBFault) { nc_close (ncid); return (DBFault); }
			if (_DBExportNetCDFTimeDefine (dbData,ncid,dimids)          == DBFault) { nc_close (ncid); return (DBFault); }
			if (_DBExportNetCDFTable (dbData->Table (DBrNItems),  ncid) == DBFault) { nc_close (ncid); return (DBFault); }
			if (_DBExportNetCDFTable (dbData->Table (DBrNSymbols),ncid) == DBFault) { nc_close (ncid); return (DBFault); }

			if ((status = nc_redef (ncid)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			if ((status = nc_def_var (ncid,_DBExportNetCDFRename (dbData->Document (DBDocSubject)),NC_SHORT,(int) 3,dimids,&varid))  != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			str = dbData->Name ();
			if ((status = nc_put_att_text (ncid,varid,"long_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			str = dbData->Document (DBDocSubject);
			if ((status = nc_put_att_text (ncid,varid,"standard_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_text (ncid,varid,"var_desc",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_short (ncid,varid,"_FillValue",   NC_SHORT,1,&fillVal)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_short (ncid,varid,"missing_value",NC_SHORT,1,&fillVal)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			if ((status = nc_enddef (ncid)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			gridIO = new DBGridIO (dbData);
			if ((record = (short *) calloc (gridIO->ColNum (),sizeof (short))) == (short *) NULL)
				{
				fprintf (stderr,"Memory allocation error in: DBExportNetCDF ()\n");
				delete gridIO;
				nc_close (ncid);
				return (DBFault);
				}
			for (layerID = 0;layerID < gridIO->LayerNum ();layerID++)
				{
				layerRec = gridIO->Layer (layerID);
				start [DIMTime]  = layerID; count [DIMTime]  = 1;
				start [DIMLevel] = 0;       count [DIMLevel] = 1;
				for (pos.Row = 0;pos.Row < gridIO->RowNum ();pos.Row++)
					{
					start [DIMLat] = pos.Row; count [DIMLat] = 1;
					start [DIMLon] = 0; count [DIMLon] = gridIO->ColNum ();
					for (pos.Col = 0;pos.Col < gridIO->ColNum ();pos.Col++)
						record [pos.Col] = gridIO->Value (layerRec,pos,&intVal) ? intVal : fillVal;
					if ((status = nc_put_vara_short (ncid,varid,start,count,record)) != NC_NOERR)
						{
						fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
						free (record);
						delete gridIO;
						nc_close (ncid);
						return (DBFault);
						}
					}
				}
			free (record);
			delete gridIO;
			} break;
		case DBTypeGridContinuous:
			{
			float *record, fillVal;
			float extent [2], dataOffset, scaleFactor;
			DBFloat gridVal;
			DBInt layerID;
			DBPosition pos;
			DBObjRecord *layerRec;
			DBGridIO *gridIO;
			
			if (_DBExportNetCDFGridDefine (dbData,ncid,dimids)          == DBFault) { nc_close (ncid); return (DBFault); } 
			if (_DBExportNetCDFTimeDefine (dbData,ncid,dimids)          == DBFault) { nc_close (ncid); return (DBFault); } 
			if (_DBExportNetCDFTable (dbData->Table (DBrNItems),  ncid) == DBFault) { nc_close (ncid); return (DBFault); }

			/* Begin Defining Core Variable */
			if ((status = nc_redef (ncid)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_def_var (ncid,_DBExportNetCDFRename (dbData->Document (DBDocSubject)),NC_FLOAT,(int) 4,dimids,&varid))  != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			str = dbData->Name ();
			if ((status = nc_put_att_text (ncid,varid,"long_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			str = dbData->Document (DBDocSubject);
			if ((status = nc_put_att_text (ncid,varid,"standard_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_text (ncid,varid,"var_desc",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_float (ncid,varid,"missing_value",NC_FLOAT,1,&fillVal)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			gridIO = new DBGridIO (dbData);

			fillVal     = gridIO->MissingValue ();
			scaleFactor = 1.0;
			dataOffset  = 0.0;
			extent [0]  = gridIO->Minimum ();
			extent [1]  = gridIO->Maximum ();
			if ((status = nc_put_att_float (ncid,varid,"_FillValue",   NC_FLOAT,1,&fillVal)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_float (ncid,varid,"scale_factor",NC_FLOAT,1,&scaleFactor)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_float (ncid,varid,"add_offset",  NC_FLOAT,1,&dataOffset)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_float (ncid,varid,"actual_range",NC_FLOAT,2,extent)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; nc_close (ncid); return (DBFault); }
			if ((status = nc_enddef (ncid)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; nc_close (ncid); return (DBFault); }
			/* End Defining Core Variable */

			if ((record = (float *) calloc (gridIO->ColNum (),sizeof (float))) == (float *) NULL)
				{
				fprintf (stderr,"Memory allocation error in: DBExportNetCDF ()\n");
				delete gridIO;
				nc_close (ncid);
				return (DBFault);
				}
			for (layerID = 0;layerID < gridIO->LayerNum ();layerID++)
				{
				layerRec = gridIO->Layer (layerID);
				start [DIMTime]  = layerID; count [DIMTime]  = 1;
				start [DIMLevel] = 0;       count [DIMLevel] = 1;
				for (pos.Row = 0;pos.Row < gridIO->RowNum ();pos.Row++)
					{
					start [DIMLat] = pos.Row; count [DIMLat] = 1;
					start [DIMLon] = 0; count [DIMLon] = gridIO->ColNum ();
					for (pos.Col = 0;pos.Col < gridIO->ColNum ();pos.Col++)
						record [pos.Col] = gridIO->Value (layerRec,pos,&gridVal) ? gridVal : fillVal;
					if ((status = nc_put_vara_float (ncid,varid,start,count,record)) != NC_NOERR)
						{
						fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
						free (record);
						delete gridIO;
						nc_close (ncid);
						return (DBFault);
						}
					}
				}
			free (record);
			delete gridIO;
			} break;
		case DBTypeNetwork:
			{
			int *record, fillVal = DBFault;
			int extent [2];
			DBPosition pos;
			DBNetworkIO *netIO;
			DBObjRecord *cellRec;

			if (_DBExportNetCDFGridDefine (dbData,ncid,dimids)          == DBFault) { nc_close (ncid); return (DBFault); } 
			if (_DBExportNetCDFTable (dbData->Table (DBrNItems),  ncid) == DBFault) { nc_close (ncid); return (DBFault); }
			if (_DBExportNetCDFTable (dbData->Table (DBrNCells),  ncid) == DBFault) { nc_close (ncid); return (DBFault); }
			if (_DBExportNetCDFTable (dbData->Table (DBrNSymbols),ncid) == DBFault) { nc_close (ncid); return (DBFault); }

			/* Begin Defining Core Variable */
			if ((status = nc_redef (ncid)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_def_var (ncid,_DBExportNetCDFRename (dbData->Document (DBDocSubject)),NC_INT,(int) 3,dimids + 1,&varid))  != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			str = dbData->Name ();
			if ((status = nc_put_att_text (ncid,varid,"long_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			str = dbData->Document (DBDocSubject);
			if ((status = nc_put_att_text (ncid,varid,"standard_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_text (ncid,varid,"var_desc",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_int (ncid,varid,"_FillValue",   NC_INT,1,&fillVal)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_int (ncid,varid,"missing_value",NC_INT,1,&fillVal)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			netIO = new DBNetworkIO (dbData);

			extent [0] = 0;
			extent [1] = netIO->CellNum ();
			if ((status = nc_put_att_int (ncid,varid,"actual_range",NC_INT,2,extent)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete netIO; nc_close (ncid); return (DBFault); }
			if ((status = nc_enddef (ncid)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete netIO; nc_close (ncid); return (DBFault); }

			if ((record = (int *) calloc (netIO->ColNum (),sizeof (int))) == (int *) NULL)
				{
				fprintf (stderr,"Memory allocation error in: DBExportNetCDF ()\n");
				delete netIO;
				nc_close (ncid);
				return (DBFault);
				}
			for (pos.Row = 0;pos.Row < netIO->RowNum ();pos.Row++)
				{
				start [DIMLevel] = 0;     count [DIMLevel] = 1;
				start [DIMLat] = pos.Row; count [DIMLat]   = 1;
				start [DIMLon] = 0;       count [DIMLon]   = netIO->ColNum ();
				for (pos.Col = 0;pos.Col < netIO->ColNum ();pos.Col++)
					{
					cellRec = netIO->Cell (pos);
					record [pos.Col] = cellRec != (DBObjRecord *) NULL ? cellRec->RowID () : fillVal;
					}
				if ((status = nc_put_vara_int (ncid,varid,start + 1,count + 1,record)) != NC_NOERR)
					{
					fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
					free (record);
					delete netIO;
					nc_close (ncid);
					return (DBFault);
					}
				}
			free (record);
			delete netIO;
			} break;
		}
	nc_close (ncid);
	return (DBSuccess);
	}

DBInt DBImportNetCDF (DBObjData *data,const char *filename)

	{
	char name [NC_MAX_NAME], varname [NC_MAX_NAME], timeString [NC_MAX_NAME], varUnit [NC_MAX_NAME], longName [NC_MAX_NAME], layerName [DBStringLength];
	int ncid, status, id, i;
	int ndims, nvars, natts, unlimdim;
	int latdim = -1, londim = -1, levdim = -1, timedim = -1;
	int varid  = -1;
	int latidx = -1, lonidx = -1, levidx = -1, timeidx = -1;
	int dimids [4];
	size_t len, start [4] = { 0, 0, 0, 0}, count [4] = { 1, 1, 1, 1} ;
	int doTimeUnit = false;
	int year, month, day, hour, minute;
	float second;
	utUnit timeUnit;
	int rowNum = 0, colNum = 0, layerNum = 0, layerID, colID, rowID;
	double *vector, *latitudes, *longitudes;
	double *timeSteps;
	double missingValue;
	double scaleFactor, dataOffset;
	DBCoordinate cellSize; 
	DBRegion extent;
	DBObjRecord *layerRec, *dataRec;
	DBObjTable  *itemTable  = data->Table (DBrNItems);
	DBObjTable  *layerTable = data->Table (DBrNLayers);
	DBObjTableField *rowNumFLD     = layerTable->Field (DBrNRowNum);
	DBObjTableField *colNumFLD     = layerTable->Field (DBrNColNum);
	DBObjTableField *cellWidthFLD  = layerTable->Field (DBrNCellWidth);
	DBObjTableField *cellHeightFLD = layerTable->Field (DBrNCellHeight);
	DBObjTableField *valueTypeFLD  = layerTable->Field (DBrNValueType);
	DBObjTableField *valueSizeFLD  = layerTable->Field (DBrNValueSize);
	DBObjTableField *layerFLD      = layerTable->Field (DBrNLayer);
	DBObjTableField *missingValueFLD		= itemTable->Field (DBrNMissingValue);
	DBGridIO *gridIO;

	switch (utInit (""))
		{
		case 0: break;
		case UT_ENOFILE: fprintf (stderr,"Invalide UDUNITS_PATH in: DBImportNetCDF ()\n"); return (DBFault);
		case UT_ESYNTAX: fprintf (stderr,"Corrupt udunit file in: DBImportNetCDF ()\n");   return (DBFault);
		default: fprintf (stderr,"UDUNIT Initialization error in: DBImportNetCDF ()\n");   return (DBFault);
		}

	if ((status = nc_open(filename, NC_NOWRITE, &ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }

	if ((status = nc_inq (ncid, &ndims, &nvars, &natts, &unlimdim)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	
	for (id = 0;id < ndims;id++)
		{
		if ((status = nc_inq_dim (ncid,id,name,&len)) != NC_NOERR)
			{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
		if      (strncmp (name,"lon",   3) == 0) { londim  = id; colNum   = len; }
		else if (strncmp (name,"lat",   3) == 0) { latdim  = id; rowNum   = len; }
		else if (strncmp (name,"time",  4) == 0) { timedim = id; layerNum = len; }
		else if (strncmp (name,"level", 5) == 0) { levdim  = id; }
		}
	if ((colNum < 2) || (rowNum < 2))
		{ fprintf (stderr,"Invalid array dimensions in: DBImportNetCDF ()\n"); nc_close (ncid); return (DBFault); }
	if ((vector = (double *) calloc (colNum,sizeof (double))) == (double *) NULL)
		{
		perror ("Memory Allocation Error in: DBImportNetCDF ()");
		nc_close (ncid);
		return (DBFault);
		}
	if ((longitudes = (double *) calloc (colNum,sizeof (double))) == (double *) NULL)
		{
		perror ("Memory Allocation Error in: DBImportNetCDF ()");
		free (vector);
		nc_close (ncid);
		return (DBFault);
		}
	if ((latitudes  = (double *) calloc (rowNum,sizeof (double))) == (double *) NULL)
		{
		perror ("Memory Allocation Error in: DBImportNetCDF ()");
		free (vector);
		free (longitudes);
		nc_close (ncid);
		return (DBFault);
		}
	if ((timeSteps = (double *) calloc (layerNum,sizeof (double))) == (double *) NULL)
		{
		perror ("Memory Allocation Error in: DBImportNetCDF ()");
		free (vector);
		free (longitudes);
		free (latitudes);
		nc_close (ncid);
		return (DBFault);
		}

	for (id = 0;id < nvars;id++)
		{
		if ((status = nc_inq_varname (ncid,id,name)) != NC_NOERR)
			{
			fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
			free (vector);
			free (longitudes);
			free (latitudes);
			free (timeSteps);
			nc_close (ncid);
			return (DBFault);
			}
		if      (strncmp (name,"level",     strlen ("level"))     == 0) continue;
		else if (strncmp (name,"time_bnds", strlen ("time_bnds")) == 0) continue;
		else if (strncmp (name,"lon_bnds",  strlen ("lon_bnds"))  == 0) continue;
		else if (strncmp (name,"lat_bnds",  strlen ("lat_bnds"))  == 0) continue;
		else if (strncmp (name,"lon",strlen ("lon")) == 0)
			{
			if ((status = nc_inq_varndims (ncid,id,&ndims)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if (ndims != 1)
				{
				fprintf (stderr,"Longitude has more than one dimension in: DBImportNetCDF ()\n");
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if ((status = nc_inq_vardimid (ncid,id,dimids)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if (dimids [0] != londim)
				{
				fprintf (stderr,"Longitude has invalid dimension in: DBImportNetCDF ()\n");
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			start [0] = 0; count [0] = colNum;
			if ((status = nc_get_vara_double (ncid,id,start,count,longitudes)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			cellSize.X = fabs (longitudes  [0] - longitudes [1]);
			extent.LowerLeft.X  = longitudes [0] < longitudes [1] ? longitudes [0] : longitudes [1];
			extent.UpperRight.X = longitudes [0] > longitudes [1] ? longitudes [0] : longitudes [1];
			for (i = 2;i < colNum;i++)
				{
				extent.LowerLeft.X   = extent.LowerLeft.X  < longitudes [i] ? extent.LowerLeft.X  : longitudes [i];
				extent.UpperRight.X  = extent.UpperRight.X > longitudes [i] ? extent.UpperRight.X : longitudes [i];
				if (DBMathEqualValues (cellSize.X,fabs (longitudes [i] - longitudes [i - 1])) != true)
					{
					fprintf (stderr,"Longitude has irregular spacing in: DBImportNetCDF ()\n");
					free (vector);
					free (longitudes);
					free (latitudes);
					free (timeSteps);
					nc_close (ncid);
					return (DBFault);
					}
				}
			extent.LowerLeft.X  = extent.LowerLeft.X  - cellSize.X / 2.0;
			extent.UpperRight.X = extent.UpperRight.X + cellSize.X / 2.0;
			}
		else if (strncmp (name,"lat", strlen ("lat")) == 0)
			{
			if ((status = nc_inq_varndims (ncid,id,&ndims)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if (ndims != 1) { fprintf (stderr,"Latitude has more than one dimension in: DBImportNetCDF ()\n"); return (DBFault); }
			if ((status = nc_inq_vardimid (ncid,id,dimids)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if (dimids [0] != latdim)
				{
				fprintf (stderr,"Latitude has invalid dimension in: DBImportNetCDF ()\n");
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			start [0] = 0; count [0] = rowNum;
			if ((status = nc_get_vara_double (ncid,id,start,count,latitudes)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			cellSize.Y = fabs (latitudes [0] - latitudes [1]);
			extent.LowerLeft.Y  = latitudes [0] < latitudes [1] ? latitudes [0] : latitudes [1];
			extent.UpperRight.Y = latitudes [0] > latitudes [1] ? latitudes [0] : latitudes [1];
			for (i = 2;i < rowNum;i++)
				{
				extent.LowerLeft.Y   = extent.LowerLeft.Y  < latitudes [i] ? extent.LowerLeft.Y  : latitudes [i];
				extent.UpperRight.Y  = extent.UpperRight.Y > latitudes [i] ? extent.UpperRight.Y : latitudes [i];
				if (DBMathEqualValues (cellSize.Y,fabs (latitudes [i - 1] - latitudes [i])) != true)
					{
					fprintf (stderr,"Latitude has irregular spacing in: DBImportNetCDF ()\n");
					free (vector);
					free (longitudes);
					free (latitudes);
					free (timeSteps);
					nc_close (ncid);
					return (DBFault);
					}
				}
			extent.LowerLeft.Y  = extent.LowerLeft.Y  - cellSize.Y / 2.0;
			extent.UpperRight.Y = extent.UpperRight.Y + cellSize.Y / 2.0;
			}
		else if (strncmp (name,"time",     strlen (name)) == 0)
			{
			if ((status = nc_get_att_text (ncid,id,"units",timeString)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if ((status = nc_inq_varndims (ncid,id,&ndims)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if (ndims != 1)
				{
				fprintf (stderr,"Time has more than one dimension in: DBImportNetCDF ()\n");
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if ((status = nc_inq_vardimid (ncid,id,dimids)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if (dimids [0] != timedim)
				{
				fprintf (stderr,"Time has invalid dimension in: DBImportNetCDF ()\n");
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			start [0] = 0; count [0] = layerNum;
			if ((status = nc_get_vara_double (ncid,id,start,count,timeSteps)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			}
		else if (varid == -1)
			{
			strcpy (varname,name);
			if ((status = nc_inq_varndims (ncid,id,&ndims)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if ((ndims < 3) || (ndims > 4)) continue;
			if ((status = nc_get_att_text (ncid,id,"long_name",longName)) != NC_NOERR) strcpy (longName,"Noname");
			if ((status = nc_get_att_text (ncid,id,"units", varUnit)) != NC_NOERR)
				{
				fprintf(stderr, "NC Error [%s,units]: %s\n", nc_strerror(status),varname);
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if ((status = nc_get_att_double (ncid,id, "missing_value", &missingValue)) != NC_NOERR) missingValue = -9999.0;
			if ((status = nc_get_att_double (ncid,id, "scale_factor",  &scaleFactor))  != NC_NOERR) scaleFactor  = 1.0;
			if ((status = nc_get_att_double (ncid,id, "add_offset",    &dataOffset))   != NC_NOERR) dataOffset   = 0.0;
			varid = id;
			}
		}

	if ((rowNum < 1) || (colNum < 1) || (layerNum < 1))
		{
		fprintf (stderr,"Incomplete NetCDF file: DBImportNetCDF ()\n");
		free (vector);
		free (longitudes);
		free (latitudes);
		free (timeSteps);
		nc_close (ncid);
		return (DBFault);
		}
	data->Name (longName);

	if ((status = nc_inq_varndims (ncid,varid,&ndims)) != NC_NOERR)
		{
		fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
		free (vector);
		free (longitudes);
		free (latitudes);
		free (timeSteps);
		nc_close (ncid);
		return (DBFault);
		}
	if ((status = nc_inq_vardimid (ncid,varid,dimids)) != NC_NOERR)
		{
		fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
		free (vector);
		free (longitudes);
		free (latitudes);
		free (timeSteps);
		nc_close (ncid);
		return (DBFault);
		}

	for (id = 0;id < ndims;id++)
		{
		start [id] = 0;
		if      (dimids [id] == londim)  { count [id] = colNum; lonidx  = id; }
		else if (dimids [id] == latdim)  { count [id] = 1;      latidx  = id; }
		else if (dimids [id] == timedim) { count [id] = 1;      timeidx = id; }
		else if (dimids [id] == levdim)  { count [id] = 1;      levidx  = id; }
		}

	data->Extent (extent);
	doTimeUnit = utScan (timeString,&timeUnit) == 0 ? true : false;
	for (layerID = 0;layerID < layerNum;layerID++)
		{
		start [timeidx] = layerID;
		if ((doTimeUnit) && (utCalendar (timeSteps [layerID],&timeUnit,&year,&month,&day,&hour,&minute,&second) == 0))
			{
			if (year != 0) sprintf (layerName,"%04d",year);
			else           sprintf (layerName,"XXXX");
			if (month != 0)
				{
				sprintf (layerName + strlen (layerName),"-%02d",month);
				if (day != 0)
					{
					sprintf (layerName + strlen (layerName),"-%02d",day);
					if (hour != 0)
						{
						sprintf (layerName + strlen (layerName)," %02d",hour);
						if (minute != 0) sprintf (layerName + strlen (layerName),":%02d",minute);
						}
					}
				}
			}
		else	sprintf (layerName,"LayerName:%04d",layerID);

		if ((layerRec = layerTable->Add (layerName)) == (DBObjRecord *) NULL)
			{
			free (vector);
			free (longitudes);
			free (latitudes);
			free (timeSteps);
			nc_close (ncid);
			return (DBFault);
			}
		missingValueFLD->Float (itemTable->Add (layerRec->Name ()),(DBFloat) missingValue);
		rowNumFLD->Int (layerRec,rowNum);
		colNumFLD->Int (layerRec,colNum);
		cellWidthFLD->Float (layerRec,cellSize.X);
		cellHeightFLD->Float (layerRec,cellSize.Y);
		valueTypeFLD->Int (layerRec,DBTableFieldFloat);
		valueSizeFLD->Int (layerRec,sizeof (DBFloat4));
		layerFLD->Record (layerRec,dataRec = new DBObjRecord (layerName,colNum * rowNum * sizeof (DBFloat4),sizeof (DBFloat4)));
		(data->Arrays ())->Add (dataRec);

		for (rowID = 0;rowID < rowNum;rowID++)
			{
			start [latidx] = latitudes [0] < latitudes [1] ? rowNum - rowID - 1 : rowID;
			if ((status = nc_get_vara_double (ncid,varid,start,count,vector)) != NC_NOERR)
				{
				fprintf(stderr,"NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			if (longitudes [0] < longitudes [1])
				for (colID = 0;colID < colNum;colID++) ((float *) (dataRec->Data ())) [colNum * rowID + colID] = scaleFactor * vector [colID]              + dataOffset;
			else
				for (colID = 0;colID < colNum;colID++) ((float *) (dataRec->Data ())) [colNum * rowID + colID] = scaleFactor * vector [colNum - colID - 1] + dataOffset;
			}
		}
	gridIO = new DBGridIO (data);
	data->Document (DBDocSubject,varname);
	gridIO->RecalcStats ();
	free (vector);
	free (longitudes);
	free (latitudes);
	free (timeSteps);
	nc_close (ncid);
	return (DBSuccess);
	}
