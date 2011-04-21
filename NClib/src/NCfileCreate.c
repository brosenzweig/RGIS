#include <cm.h>
#include <unistd.h>
#include <NC.h>

int NCfileCreate (char *fileName, int tplNCId)
{
	int status, ncid, latdim, latid, londim, lonid;
	size_t latNum,   lonNum, start;
	char  *latName, *lonName, *projStr;
	double *xcoords, *ycoords;
	NCprojection proj;

	if ((projStr  = NCnameProjString (proj = NCdataGetProjection (tplNCId))) == (char *) NULL) return (NCfailed);

	if ((status = nc_create (fileName,NC_CLOBBER, &ncid)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		return (NCfailed);
	} 
	if ((status = nc_put_att_text (ncid, NC_GLOBAL, NCnameGAProjection, strlen (projStr), projStr)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"Conventions",strlen ("COARDS-NCghaas"),"COARDS-NCghaas")) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	switch (proj)
	{
		default:
		case NCprojSpherical: lonName = NCnameDNLongitude; latName = NCnameDNLatitude; break;
		case NCprojCartesian: lonName = NCnameDNXCoord;    latName = NCnameDNYCoord;   break;
	}
	if ((xcoords = NCdataGetXCoords (tplNCId, &lonNum)) == (double *) NULL)
	{
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	if ((ycoords = NCdataGetYCoords (tplNCId, &latNum)) == (double *) NULL)
	{
		free (xcoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	if ((status = nc_def_dim (ncid, lonName, lonNum, &londim)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	if ((status = nc_def_dim (ncid, latName, latNum, &latdim)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	if ((status = nc_def_var (ncid, lonName, NC_DOUBLE, 1, &londim, &lonid)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	if ((status = nc_def_var (ncid, latName, NC_DOUBLE, 1, &latdim, &latid)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	nc_enddef (ncid);
	start = 0;
	if ((status = nc_put_vara_double (ncid, lonid, &start, &lonNum, xcoords)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	if ((status = nc_put_vara_double (ncid, latid, &start, &latNum, ycoords)) != NC_NOERR)
	{
		NCprintNCError (status,"NCfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	free (xcoords); free (ycoords);
	if ((NCdataCopyAttributes (tplNCId, NCdataGetXVarId (tplNCId), ncid, lonid, true) == NCfailed) ||
	    (NCdataCopyAttributes (tplNCId, NCdataGetYVarId (tplNCId), ncid, latid, true) == NCfailed))
	{
		nc_close (ncid);
		unlink (fileName);
		return (NCfailed);
	}
	return (ncid);
}

int NCfileVarAdd (int ncid, const char *varname, nc_type ltype, nc_type ttype, nc_type vtype)
{
	int status, varid, dimids [4], ndims = 1, xdim, ydim = NCundefined;
	char *typeString;
	bool redef;

	if (( (xdim = NCdataGetCDimId (ncid)) == NCfailed) && 
		 (((xdim = NCdataGetXDimId (ncid)) == NCfailed) || ((ydim = NCdataGetYDimId (ncid)) == NCfailed))) return (NCfailed);

	redef = nc_redef (ncid) == NC_NOERR;
	if ((status = nc_def_dim (ncid, NCnameDNTime,  NC_UNLIMITED, dimids)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }
	if (ltype != NC_NAT)
	{
		if ((status = nc_def_dim (ncid, NCnameDNLevel, 1, dimids + 1)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }
		if ((status = nc_def_var (ncid, NCnameDNLevel,   ltype, 1, dimids + 1, &varid)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }
		ndims += 1;
	}
	if ((status = nc_def_var (ncid, NCnameDNTime,    ttype, 1, dimids, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }
	if ((status = nc_def_var (ncid, NCnameVAMaximum, vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }
	if ((status = nc_def_var (ncid, NCnameVAMinimum, vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }
	if ((status = nc_def_var (ncid, NCnameVAAverage, vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }
	if ((status = nc_def_var (ncid, NCnameVAStdDev,  vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }

	if (ydim != NCundefined)
	{
		dimids [ndims++] = ydim;
		typeString = NCnameTypeGCont;
	}
	else typeString = NCnameTypePoint;
	dimids [ndims++] = xdim;

	if ((status = nc_def_var (ncid, varname,          vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }

	if ((status = nc_put_att_text (ncid, NC_GLOBAL, NCnameGADataType, strlen (typeString), typeString)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarAdd"); return (NCfailed); }

	if (redef) nc_enddef (ncid);
	return (varid);
}

int NCfileVarClone (int sncid, int dncid)
{
	bool redef;
	int status, varid, lvarid, tvarid, svarid, dvarid, ndims;
	nc_type ltype, ttype, vtype;
	char varname [NC_MAX_NAME];

	if ((status = nc_inq_varid (sncid, NCnameDNLevel, &lvarid)) != NC_NOERR)
	{
		lvarid = NCundefined;
		ltype = NC_NAT;
	}
	else
	{
		if ((status = nc_inq_varndims (sncid, lvarid, &ndims)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
		if (ndims > 1) { CMmsgPrint (CMmsgAppError, "Level variable has too many dimensions in: NCfileVarClone ()\n"); return (NCfailed); }
		if ((status = nc_inq_vartype  (sncid, lvarid, &ltype)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	}

	if ((status = nc_inq_varid (sncid, NCnameDNTime, &tvarid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	if ((status = nc_inq_varndims (sncid, tvarid, &ndims)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	if (ndims > 1) { CMmsgPrint (CMmsgAppError, "Time variable has too many dimensions in: NCfileVarClone ()\n"); return (NCfailed); }
	if ((status = nc_inq_vartype  (sncid, tvarid, &ttype)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }

	if ((svarid = NCdataGetCoreVarId (sncid)) == NCfailed) return (NCfailed);
	if ((status = nc_inq_varname (sncid, svarid, varname)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	if ((status = nc_inq_vartype (sncid, svarid, &vtype)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }

	if ((dvarid = NCfileVarAdd (dncid, varname, ltype, ttype, vtype)) == NCfailed) return (NCfailed);
	if (NCdataCopyAttributes (sncid, svarid, dncid, dvarid, true) == NCfailed) return (NCfailed);

	if (lvarid != NCundefined)
	{
		if ((status = nc_inq_varid (dncid, NCnameDNLevel, &varid)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
		if (NCdataCopyAttributes (sncid, lvarid, dncid,  varid, true) == NCfailed) return (NCfailed);
	}
	if ((status = nc_inq_varid (dncid, NCnameDNTime,  &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	if (NCdataCopyAttributes (sncid, tvarid, dncid,  varid, true) == NCfailed) return (NCfailed);

	status = nc_redef (dncid) == NC_NOERR ? true : false;
	redef = status;
	if ((status = nc_inq_varid (dncid, NCnameVAMaximum, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	nc_copy_att (sncid,svarid,NCnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAAddOffset,  dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAActualRange,dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAValidRange, dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAFillValue,  dncid,varid);
	if ((status = nc_inq_varid (dncid, NCnameVAMinimum, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	nc_copy_att (sncid,svarid,NCnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAAddOffset,  dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAActualRange,dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAValidRange, dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAFillValue,  dncid,varid);
	if ((status = nc_inq_varid (dncid, NCnameVAAverage, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	nc_copy_att (sncid,svarid,NCnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAAddOffset,  dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAActualRange,dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAValidRange, dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAFillValue,  dncid,varid);
	if ((status = nc_inq_varid (dncid, NCnameVAStdDev, &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarClone"); return (NCfailed); }
	nc_copy_att (sncid,svarid,NCnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCnameVAFillValue,  dncid,varid);
	if (redef) nc_enddef (dncid);
	return (dvarid);
}

NCstate NCfileSetLevelUnit (int ncid, const char *unitStr)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((status = nc_inq_varid (ncid, NCnameDNLevel,  &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarSetUnits"); goto ABORT; }
	if ((status = nc_put_att_text (ncid, varid, NCnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarSetUnits"); goto ABORT; }

	if (redef) nc_enddef (ncid);
	return (NCsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCfailed);
}

NCstate NCfileSetTimeUnit (int ncid, const char *unitStr)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((status = nc_inq_varid (ncid, NCnameDNTime,  &varid)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarSetUnits"); goto ABORT; }
	if ((status = nc_put_att_text (ncid, varid, NCnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileVarSetUnits"); goto ABORT; }

	if (redef) nc_enddef (ncid);
	return (NCsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCfailed);
}

NCstate NCfileSetVarUnit (int ncid, const char *unitStr)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((varid = NCdataGetCoreVarId (ncid)) == NCfailed)
	{ CMmsgPrint (CMmsgAppError, "Missing core variable in: %s %d",__FILE__,__LINE__); goto ABORT; }
	else if ((status = nc_put_att_text (ncid, varid, NCnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
	{ NCprintNCError (status,"NCfileSetVarUnit"); goto ABORT; }

	if (nc_inq_varid (ncid, NCnameVAAverage, &varid) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileSetVarUnit"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMaximum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileSetVarUnit"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMinimum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileSetVarUnit"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAStdDev, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCprintNCError (status,"NCfileSetVarUnit"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCfailed);
}

NCstate NCfileSetVarTransform (int ncid, double scale, double offset)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((varid = NCdataGetCoreVarId (ncid)) == NCfailed)
	{ CMmsgPrint (CMmsgAppError, "Missing core variable in: %s %d",__FILE__,__LINE__); goto ABORT; }
	else
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAScaleFactor,NC_FLOAT, 1, &scale))  != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAAddOffset,  NC_FLOAT, 1, &offset)) != NC_NOERR))
		{ NCprintNCError (status,"NCfileSetVarTransform"); goto ABORT; }
	}

	if ((status = nc_inq_varid (ncid, NCnameVAAverage, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAScaleFactor,NC_FLOAT, 1, &scale))  != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAAddOffset,  NC_FLOAT, 1, &offset)) != NC_NOERR))
		{ NCprintNCError (status,"NCfileSetVarTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMaximum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAScaleFactor,NC_FLOAT, 1, &scale))   != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAAddOffset,  NC_FLOAT, 1, &offset))  != NC_NOERR))
		{ NCprintNCError (status,"NCfileSetVarTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMinimum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAScaleFactor,NC_FLOAT, 1, &scale))   != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAAddOffset,  NC_FLOAT, 1, &offset))  != NC_NOERR))
		{ NCprintNCError (status,"NCfileSetVarTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAStdDev, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAScaleFactor,NC_FLOAT, 1, &scale))   != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAAddOffset,  NC_FLOAT, 1, &offset))  != NC_NOERR))
		{ NCprintNCError (status,"NCfileSetVarTransform"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCfailed);
}

NCstate NCfileSetMissingVal (int ncid,  double missingVal)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((varid = NCdataGetCoreVarId (ncid)) == NCfailed)
	{ CMmsgPrint (CMmsgAppError, "Missing core variable in: %s %d",__FILE__,__LINE__); goto ABORT; }
	else
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
			 ((status = nc_put_att_double (ncid, varid, NCnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCprintNCError (status,"NCfileVarSetMissingVal"); goto ABORT; }
	}

	if ((status = nc_inq_varid (ncid, NCnameVAAverage, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCprintNCError (status,"NCfileVarSetMissingVal"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMaximum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCprintNCError (status,"NCfileVarSetMissingVal"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMinimum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCprintNCError (status,"NCfileVarSetMissingVal"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAStdDev, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCnameVAFillValue,  NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCnameVAMissingVal, NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCprintNCError (status,"NCfileVarSetMissingVal"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCfailed);
}

NCstate NCfileSetValidRange (int ncid, double min, double max)
{
	int status, varid;
	bool redef;
	double range [2];

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	range [0] = min;
	range [1] = max;
	if ((varid = NCdataGetCoreVarId (ncid)) == NCfailed)
	{ CMmsgPrint (CMmsgAppError, "Missing core variable in: %s %d",__FILE__,__LINE__); goto ABORT; }
	else
	{
		if ((status = nc_put_att_double (ncid, varid, NCnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarSetTransform"); goto ABORT; }
	}

	if ((status = nc_inq_varid (ncid, NCnameVAAverage, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarSetTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMaximum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarSetTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAMinimum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarSetTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCnameVAStdDev, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCprintNCError (status,"NCfileVarSetTransform"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCfailed);
}
