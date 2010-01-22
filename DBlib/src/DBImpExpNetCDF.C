/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

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
			{ (char *) "CellXCoord",     (char *) "cell_x-coord"},
			{ (char *) "CellYCoord",     (char *) "cell_y-coord"},
			{ (char *) "Point",          (char *) "point"},
			{ (char *) "Line",           (char *) "line"},
			{ (char *) "Polygon",        (char *) "polygon"},
			{ (char *) "Continuous",     (char *) "continuous"},
			{ (char *) "Discrete",       (char *) "discrete"},
			{ (char *) "Network",        (char *) "network"},
			{ GHAASSubjRunoff,           (char *) "runoff"},
			{ GHAASSubjDischarge,        (char *) "discharge"},
			{ GHAASSubjFlowHeight,       (char *) "flow_height"},
			{ GHAASSubjFlowWidth,        (char *) "flow_width"},
			{ GHAASSubjFlowVelocity,     (char *) "flow_velocity"},
			{ GHAASSubjFlowSurfSlope,    (char *) "slope"},
			{ GHAASSubjRiverStorage,     (char *) "river_storage"},
			{ GHAASSubjFloodStorage,     (char *) "flood_storage"},
			{ GHAASSubjFloodArea,        (char *) "flood_area"},
			{ GHAASSubjOverlandStorage,  (char *) "overland_storage"},
			{ GHAASSubjOverlandVelocity, (char *) "overland_velocity"},
			{ GHAASSubjPrecip,           (char *) "precipitation"},
			{ GHAASSubjAirTemp,          (char *) "air_temperature"},
			{ GHAASSubjHumidity,         (char *) "humidity"},
			{ GHAASSubjVaporPres,        (char *) "vapor_pressure"},
			{ GHAASSubjWindSpeed,        (char *) "wind_speed"},
			{ GHAASSubjSolarRad,         (char *) "solar_radiation"},
			{ GHAASSubjStations,         (char *) "stations"},
			{ GHAASSubjReservoirs,       (char *) "reservoirs"},
			{ GHAASSubjElevation,        (char *) "elevation"},
			{ GHAASSubjPits,             (char *) "pits"},
			{ DBrNCoord,                 (char *) "coord"},
			{ DBrNVertexes,              (char *) "vertexes"},
			{ DBrNVertexNum,             (char *) "vertex_num"},
			{ DBrNRegion,                (char *) "region"},
			{ DBrNSymbol,                (char *) "symbol"},
			{ DBrNSymbolID,              (char *) "symbol_id"},
			{ DBrNForeground,            (char *) "foreground"},
			{ DBrNBackground,            (char *) "background"},
			{ DBrNStyle,                 (char *) "style"},
			{ DBrNLinkNum,               (char *) "link_num"},
			{ DBrNLeftPoly,              (char *) "left_polygon"},
			{ DBrNRightPoly,             (char *) "right_polygon"},
			{ DBrNNextLine,              (char *) "next_line"},
			{ DBrNPrevLine,              (char *) "prev_line"},
			{ DBrNFromNode,              (char *) "from_node"},
			{ DBrNToNode,                (char *) "to_node"},
			{ DBrNPerimeter,             (char *) "perimeter"},
			{ DBrNFirstLine,             (char *) "first_line"},
			{ DBrNLineNum,               (char *) "line_num"},
			{ DBrNArea,                  (char *) "area"},
			{ DBrNMouthPos,              (char *) "mouth"},
			{ DBrNColor,                 (char *) "color"},
			{ DBrNBasinOrder,            (char *) "basin_order"},
			{ DBrNBasinArea,             (char *) "basin_area"},
			{ DBrNBasinLength,           (char *) "basin_length"},
			{ DBrNLookupGrid,            (char *) "lookup_grid"},
			{ DBrNPosition,              (char *) "position"},
			{ DBrNToCell,                (char *) "to_cell"},
			{ DBrNFromCell,              (char *) "from_cell"},
			{ DBrNOrder,                 (char *) "order"},
			{ DBrNMagnitude,             (char *) "magnitude"},
			{ DBrNBasin,                 (char *) "basin_id"},
			{ DBrNBasinCells,            (char *) "basin_cells"},
			{ DBrNTravel,                (char *) "travel"},
			{ DBrNUpCellPos,             (char *) "most_upcell_pos"},
			{ DBrNCellArea,              (char *) "cell_area"},
			{ DBrNCellLength,            (char *) "cell_length"},
			{ DBrNSubbasinArea,          (char *) "subbasin_area"},
			{ DBrNSubbasinLength,        (char *) "subbasin_length"},
			{ DBrNDistToMouth,           (char *) "dist_to_mouth"},
			{ DBrNDistToOcean,           (char *) "dist_to_outlet"},
			{ DBrNRowNum,                (char *) "row_num"},
			{ DBrNColNum,                (char *) "col_num"},
			{ DBrNCellWidth,             (char *) "cell_width"},
			{ DBrNCellHeight,            (char *) "cell_height"},
			{ DBrNValueType,             (char *) "value_type"},
			{ DBrNValueSize,             (char *) "value_size"},
			{ DBrNLayer,                 (char *) "layer"},
			{ DBrNMinimum,               (char *) "minimum"},
			{ DBrNMaximum,               (char *) "maximum"},
			{ DBrNAverage,               (char *) "average"},
			{ DBrNStdDev,                (char *) "stddev"},
			{ DBrNMissingValue,          (char *) "missing_value"},
			{ DBrNGridValue,             (char *) "grid_value"},
			{ DBrNGridArea,              (char *) "arid_area"},
			{ DBrNGridPercent,           (char *) "grid_percent"},
			{ DBrNItems,                 (char *) "items"},
			{ DBrNSelection,             (char *) "selection"},
			{ DBrNNodes,                 (char *) "nodes"},
			{ DBrNLineList,              (char *) "line_list"},
			{ DBrNContours,              (char *) "contours"},
			{ DBrNContourList,           (char *) "contour_list"},
			{ DBrNSymbols,               (char *) "symbols"},
			{ DBrNCells,                 (char *) "cells"},
			{ DBrNLayers,                (char *) "layers"},
			{ DBrNGroups,                (char *) "groups"},
			{ DBrNRelateData,            (char *) "relate_data"},
			{ DBrNRelateField,           (char *) "relate_field"},
			{ DBrNRelateJoinField,       (char *) "join_field"},
			{ DBrNRelations,             (char *) "relations"},
			{ DBrNCategoryID,            (char *) "category_id"},
			{ DBrNCategory,              (char *) "Category"},
			{ DBrNPercent,               (char *) "percent"},
			{ DBDocSubject,              (char *) "subject"},
			{ DBDocGeoDomain,            (char *) "geo_domain"},
			{ DBDocVersion,              (char *) "version"},
			{ DBDocCitationRef,          (char *) "reference"},
			{ DBDocCitationInst,         (char *) "institute"},
			{ DBDocSourceInst,           (char *) "source_institue"},
			{ DBDocSourcePerson,         (char *) "source_person"},
			{ DBDocOwnerPerson,          (char *) "contact_person"},
			{ DBDocComment,              (char *) "comment"}};
	size_t i;

	for (i = 0;i < sizeof (names) / sizeof (names [0]);++i)
		if (strcmp (names [i].OldName,name) == 0) return (names [i].NewName);
	return (name);
	}

enum { DIMTime, DIMLat, DIMLon, };

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
	int status, latid, lonid, latbndid, lonbndid, bdimids[2];
	size_t start [2], count [2];
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
	if ((status = nc_def_dim (ncid,"latitude", rowNum,dimids + DIMLat))   != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	if ((status = nc_def_dim (ncid,"longitude",colNum,dimids + DIMLon))   != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	if ((status = nc_def_dim (ncid,"bnds",     2,    bdimids + 1))        != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	/* End Defining Dimensions */

	/* Begin Defining Latitude Variable */
	if ((status = nc_def_var (ncid,"latitude",  NC_DOUBLE, (int) 1,dimids + DIMLat, &latid))  != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "Latitude";
	if ((status = nc_put_att_text (ncid,latid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "latitude_north";
	if ((status = nc_put_att_text (ncid,latid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "degrees_north";
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
	str = (char *) "latitude_bnds";
	if ((status = nc_put_att_text (ncid,latid,"bounds",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "y";
	if ((status = nc_put_att_text (ncid,latid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	/* End Defining Latitude Variable */

	/* Begin Defining Longitude Variable */
	if ((status = nc_def_var (ncid,"longitude",  NC_DOUBLE, (int) 1,dimids + DIMLon, &lonid))  != NC_NOERR)
	 	{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "Longitude";
	if ((status = nc_put_att_text (ncid,lonid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "longitude_east";
	if ((status = nc_put_att_text (ncid,lonid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "degrees_east";
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
	str = (char *) "longitude_bnds";
	if ((status = nc_put_att_text (ncid,latid,"bounds",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	str = (char *) "x";
	if ((status = nc_put_att_text (ncid,lonid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	/* End Defining Longitude Variable */

	/* Begin Defining Latitude_bounds Variable */
	bdimids [0] = dimids[DIMLat];
	if ((status = nc_def_var (ncid,"latitude_bnds",  NC_DOUBLE, (int) 2,bdimids, &latbndid))  != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	/* End Defining Latitude_bounds Variable */

	/* Begin Defining Longitude_bounds Variable */
	bdimids [0] = dimids[DIMLon];
	if ((status = nc_def_var (ncid,"longitude_bnds",  NC_DOUBLE, (int) 2,bdimids, &lonbndid))  != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
	/* End Defining Longitude_bounds Variable */

	if ((status = nc_enddef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

	if ((record = (double *) calloc ((rowNum > colNum ? rowNum : colNum) * 2,sizeof (double))) == (double *) NULL)
		{ fprintf (stderr,"Memory allocation error in: DBExportNetCDF ()\n"); return (DBFault); }

	start[0] = start[1] =0;
	count[0] = rowNum;
	count[1] = 2;
	for (i = 0;i < rowNum;i++) record [i] = (dbData->Extent ()).LowerLeft.Y + i * cellHeight + cellHeight / 2.0;
	if ((status = nc_put_vara_double (ncid,latid,start,count,record)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); free (record); return (DBFault); }
	for (i = 0;i < rowNum;i++) {
		record [i * 2]     = (dbData->Extent ()).LowerLeft.Y + i       * cellHeight;
		record [i * 2 + 1] = (dbData->Extent ()).LowerLeft.Y + (i + 1) * cellHeight;
	}
	if ((status = nc_put_vara_double (ncid,latbndid,start,count,record)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); free (record); return (DBFault); }

	count[0] = colNum;
	for (i = 0;i < colNum;i++) record [i] = (dbData->Extent ()).LowerLeft.X + i * cellWidth + cellWidth / 2.0;
	if ((status = nc_put_vara_double (ncid,lonid,start,count,record)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); free (record); return (DBFault); }
	for (i = 0;i < colNum;i++) {
		record [i * 2]     = (dbData->Extent ()).LowerLeft.X + i       * cellWidth;
		record [i * 2 + 1] = (dbData->Extent ()).LowerLeft.X + (i + 1) * cellWidth;
	}
	if ((status = nc_put_vara_double (ncid,lonbndid,start,count,record)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); free (record); return (DBFault); }
	free (record);
	return (DBSuccess);
	}

static DBInt _DBExportNetCDFTimeDefine (DBObjData *dbData,int ncid,int dimids [])
	{
	char *str, timeStr [DBStringLength], unitStr [NC_MAX_NAME];
	int status, timeid, i;
	int year, month, day, hour, minute;
	utUnit unit;
	double val;
	size_t start, count;
	int *record, extent [2];
	DBInt layerID;
	DBGridIO	*gridIO = new DBGridIO (dbData);

	/* Begin Defining Dimensions */
	if ((status = nc_redef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	if ((status = nc_def_dim (ncid,"time",NC_UNLIMITED,    dimids + DIMTime)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	/* End Defining Dimensions */

	/* Begin Defining Time Variable */
	if ((status = nc_def_var (ncid,"time", NC_INT,(int) 1,dimids + DIMTime,&timeid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	str = (char *) "Time";
	if ((status = nc_put_att_text (ncid,timeid,"long_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	str = (char *) "time";
	if ((status = nc_put_att_text (ncid,timeid,"standard_name",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	strcpy (timeStr,(gridIO->Layer (gridIO->LayerNum () - 1))->Name ());
	if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
	month  = 6;
	day    = 15;
	hour   = 12;
	minute = 30;
	switch (strlen (timeStr))
		{
		case  4:
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d",&year);
			sprintf (unitStr,"years since %s-01-01 00:00", timeStr);
			break;
		case  7:
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d",&year,&month);
			sprintf (unitStr,"months since %s-01 00:00", timeStr);
			break;
		case 10:
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d-%2d",&year,&month,&day);
			sprintf (unitStr,"days since %s 00:00",   timeStr);
			break;
		case 13:
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d-%2d %2d",&year,&month,&day,&hour);
			sprintf (unitStr,"hours since %s:00",  timeStr);
			break;
		case 16:
			strcpy (timeStr,(gridIO->Layer (0))->Name ());
			if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
			sscanf (timeStr,"%4d-%2d-%2d %2d:%2d",&year,&month,&day,&hour,&minute);
			sprintf (unitStr,"minutes since %s",timeStr);
			break;
		}
	if (utScan (unitStr,&unit) != 0)
		{ fprintf (stderr,"Invalid time Unit [%s] in: DBImportNetCDF ()",unitStr); delete gridIO; return (DBFault); }

	if ((status = nc_put_att_text (ncid,timeid,"units",strlen (unitStr),unitStr)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	str = (char *) "t";
	if ((status = nc_put_att_text (ncid,timeid,"axis",strlen (str),str)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	/* End Defining Time Variable */

	if ((status = nc_enddef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "%s\n", nc_strerror(status)); delete gridIO; return (DBFault); }

	if ((record = (int *) calloc (gridIO->LayerNum () * 2,sizeof (int))) == (int *) NULL)
		{
		fprintf (stderr,"Memory allocation error in: DBExportNetCDF ()\n");
		delete gridIO;
		return (DBFault);
		}
	start = 0;
	count = gridIO->LayerNum ();;
	month  = 6;
	day    = 15;
	hour   = 12;
	minute = 30;
	for (layerID = 0;layerID < gridIO->LayerNum ();layerID++)
		{
		strcpy (timeStr,(gridIO->Layer (layerID))->Name ());
		if (strncmp (timeStr,"XXXX",4) == 0) for (i = 0;i < 4;i++) timeStr [i] = '0';
		switch (strlen (timeStr))
			{
			case  4: sscanf (timeStr,"%4d",                &year);                           break;
			case  7: sscanf (timeStr,"%4d-%2d",            &year,&month);                    break;
			case 10: sscanf (timeStr,"%4d-%2d-%2d",        &year,&month,&day);               break;
			case 13: sscanf (timeStr,"%4d-%2d-%2d %2d",    &year,&month,&day,&hour);         break;
			case 16: sscanf (timeStr,"%4d-%2d-%2d %2d:%2d",&year,&month,&day,&hour,&minute); break;
			}
		if (utInvCalendar (year,month,day,hour,minute,(double) 0.0,&unit, &val) != 0)
			{
			fprintf (stderr,"Invalid time [%s] in: DBImportNetCDF ()\n",timeStr);
			fprintf (stderr,"Year: %d, Month: %d, Day: %d, Hours: %d, Minutes: %d\n",year, month, day, hour, minute);
			delete gridIO;
			free (record);
			return (DBFault);
			}
		else record [layerID] = (int) val;
		}
	if ((status = nc_put_vara_int (ncid,timeid,&start,&count,record)) != NC_NOERR)
		{
		fprintf(stderr, "%s\n", nc_strerror(status));
		delete gridIO;
		free (record);
		return (DBFault);
		}

	if ((status = nc_redef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	extent[0] = record[0];
	extent[1] = record[layerID - 1];
	if ((status = nc_put_att_int (ncid,timeid,"actual_range",NC_INT,2,extent)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); delete gridIO; return (DBFault); }
	if ((status = nc_enddef (ncid)) != NC_NOERR)
		{ fprintf(stderr, "%s\n", nc_strerror(status)); delete gridIO; return (DBFault); }

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

				if      (fieldRec->Length () <= 8)           { dimname = (char *) "short_string"; dimlen = 8; }
				else if (fieldRec->Length () <= 64)          { dimname = (char *) "string";       dimlen = 64; }
				else if (fieldRec->Length () <= NC_MAX_NAME) { dimname = (char *) "long_string";  dimlen = NC_MAX_NAME; }
				else                                         { dimname = (char *) "text";         dimlen = 256; }

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
				DBPosition pos;

				sprintf (rowName,"%s_row",fieldName);
				sprintf (colName,"%s_col",fieldName);
				if (((status = nc_def_var (ncid,rowName,NC_INT,(int) 1,dimids,&rvarid))  == NC_NOERR) &&
				    ((status = nc_def_var (ncid,colName,NC_INT,(int) 1,dimids,&cvarid))  == NC_NOERR) &&
					 ((status = nc_enddef (ncid)) == NC_NOERR))
					{

					for (itemID = 0;itemID < table->ItemNum ();itemID++)
						{
						itemRec = table->Item (itemID);
						pos = fieldRec->Position (itemRec);
						index [0] = itemID;
						if (((status = nc_put_var1_int (ncid,cvarid,index, &pos.Col)) != NC_NOERR) ||
						    ((status = nc_put_var1_int (ncid,rvarid,index, &pos.Row)) != NC_NOERR))
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
	int ncid, status, dimids [3], varid;
	size_t start [3], count [3];

	switch (utInit (""))
		{
		case 0: break;
		case UT_ENOFILE: fprintf (stderr,"Invalid UDUNITS_PATH in: DBImportNetCDF ()\n"); return (DBFault);
		case UT_ESYNTAX: fprintf (stderr,"Corrupt udunit file in: DBImportNetCDF ()\n");   return (DBFault);
		default: fprintf (stderr,"UDUNIT Initialization error in: DBImportNetCDF ()\n");   return (DBFault);
		}

	if ((status = nc_create (fileName,NC_CLOBBER,&ncid)) != NC_NOERR)
		{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); return (DBFault); }
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"Conventions",strlen ("CF-1.2"),"CF-1.2")) != NC_NOERR)
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
			if ((status = nc_def_var (ncid,_DBExportNetCDFRename (dbData->Document (DBDocSubject)),NC_FLOAT,(int) 3,dimids,&varid))  != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			str = dbData->Name ();
			if ((status = nc_put_att_text (ncid,varid,"long_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			str = dbData->Document (DBDocSubject);
			if ((status = nc_put_att_text (ncid,varid,"standard_name",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
			if ((status = nc_put_att_text (ncid,varid,"var_desc",strlen (str),str)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }

			gridIO = new DBGridIO (dbData);

			fillVal     = gridIO->MissingValue ();
			scaleFactor = 1.0;
			dataOffset  = 0.0;
			extent [0]  = gridIO->Minimum ();
			extent [1]  = gridIO->Maximum ();
			if ((status = nc_put_att_float (ncid,varid,"missing_value",NC_FLOAT,1,&fillVal)) != NC_NOERR)
				{ fprintf(stderr, "NC Error: %s\n", nc_strerror(status)); nc_close (ncid); return (DBFault); }
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
	size_t attlen;
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
	int rowNum = 0, colNum = 0, layerNum = 1, layerID, colID, rowID;
	double *vector, *latitudes, *longitudes;
	double *timeSteps;
	double missingValue, fillValue;
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
		case UT_ENOFILE: fprintf (stderr,"Invalid UDUNITS_PATH in: DBImportNetCDF ()\n"); return (DBFault);
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
		if       (strcmp (name,"level")          == 0)  continue;
		else if  (strcmp (name,"time_bnds")      == 0)  continue;
		else if ((strcmp (name,"lon_bnds")       == 0) ||
			     (strcmp (name,"longitude_bnds") == 0)) continue;
		else if ((strcmp (name,"lat_bnds")       == 0) ||
		         (strcmp (name,"latitude_bnds")  == 0)) continue;
		else if ((strcmp (name,"lon")            == 0) ||
		         (strcmp (name,"longitude")      == 0))
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
				if (CMmathEqualValues (cellSize.X,fabs (longitudes [i] - longitudes [i - 1])) != true)
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
		else if ((strcmp (name,"lat")      == 0) ||
			     (strcmp (name,"latitude") == 0))
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
				if (CMmathEqualValues (cellSize.Y,fabs (latitudes [i - 1] - latitudes [i])) != true)
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
		else if (strcmp (name,"time") == 0)
			{
			if (((status = nc_inq_attlen   (ncid, id, "units", &attlen))    != NC_NOERR) ||
				((status = nc_get_att_text (ncid, id, "units", timeString)) != NC_NOERR))
				{
				fprintf(stderr, "NC Error: %s\n", nc_strerror(status));
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			else timeString [attlen] = '\0';
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
			if ((ndims < 2) || (ndims > 4)) continue;
			if (((status = nc_inq_attlen   (ncid,id,"long_name", &attlen)) != NC_NOERR) ||
			    ((status = nc_get_att_text (ncid,id,"long_name",longName)) != NC_NOERR)) strcpy (longName,"Noname");
			else longName [attlen] = '\0';
			if (((status = nc_inq_attlen   (ncid,id,"units", &attlen)) != NC_NOERR) ||
			    ((status = nc_get_att_text (ncid,id,"units", varUnit)) != NC_NOERR))
				{
				fprintf(stderr, "NC Error [%s,units]: %s\n", nc_strerror(status),varname);
				free (vector);
				free (longitudes);
				free (latitudes);
				free (timeSteps);
				nc_close (ncid);
				return (DBFault);
				}
			else varUnit [attlen] = '\0';
			if ((status = nc_get_att_double (ncid, id, "_FillValue",    &fillValue))    != NC_NOERR) fillValue    = -9999.0;
			if (((status = nc_get_att_double (ncid,id, "missing_value", &missingValue)) != NC_NOERR) ||
			    CMmathEqualValues (fillValue, missingValue)) missingValue = -9999.0; // TODO I am not sure if it is a good idea.
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
	doTimeUnit = (timedim != -1) && (utScan (timeString,&timeUnit) == 0) ? true : false;
	for (layerID = 0;layerID < layerNum;layerID++)
		{
		if (timedim != -1)
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
			}
		else sprintf (layerName,"LayerName:%04d",layerID);

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
			for (colID = 0;colID < colNum;colID++)
				vector [colID] = CMmathEqualValues (vector [colID], fillValue) ? missingValue : scaleFactor * vector [colID] + dataOffset;
			if (longitudes [0] < longitudes [1])
				for (colID = 0;colID < colNum;colID++) ((float *) (dataRec->Data ())) [colNum * rowID + colID] = vector [colID];
			else
				for (colID = 0;colID < colNum;colID++) ((float *) (dataRec->Data ())) [colNum * rowID + colID] = vector [colNum - colID - 1];
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
