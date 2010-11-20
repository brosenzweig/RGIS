#include<NC.h>
#include<unistd.h>

int NCGfileCreate (char *fileName, int tplNCId)
{
	int status, ncid, latdim, latid, londim, lonid;
	size_t latNum,   lonNum, start;
	char  *latName, *lonName, *projStr;
	double *xcoords, *ycoords;
	NCGprojection proj;

	if ((projStr  = NCGnameProjString (proj = NCGdataGetProjection (tplNCId))) == (char *) NULL) return (NCGfailed);

	if ((status = nc_create (fileName,NC_CLOBBER, &ncid)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		return (NCGfailed);
	} 
	if ((status = nc_put_att_text (ncid, NC_GLOBAL, NCGnameGAProjection, strlen (projStr), projStr)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	if ((status = nc_put_att_text (ncid,NC_GLOBAL,"Conventions",strlen ("COARDS-NCghaas"),"COARDS-NCghaas")) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	switch (proj)
	{
		default:
		case NCGprojSpherical: lonName = NCGnameDNLongitude; latName = NCGnameDNLatitude; break;
		case NCGprojCartesian: lonName = NCGnameDNXCoord;    latName = NCGnameDNYCoord;   break;
	}
	if ((xcoords = NCGdataGetXCoords (tplNCId, &lonNum)) == (double *) NULL)
	{
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	if ((ycoords = NCGdataGetYCoords (tplNCId, &latNum)) == (double *) NULL)
	{
		free (xcoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	if ((status = nc_def_dim (ncid, lonName, lonNum, &londim)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	if ((status = nc_def_dim (ncid, latName, latNum, &latdim)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	if ((status = nc_def_var (ncid, lonName, NC_DOUBLE, 1, &londim, &lonid)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	if ((status = nc_def_var (ncid, latName, NC_DOUBLE, 1, &latdim, &latid)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	nc_enddef (ncid);
	start = 0;
	if ((status = nc_put_vara_double (ncid, lonid, &start, &lonNum, xcoords)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	if ((status = nc_put_vara_double (ncid, latid, &start, &latNum, ycoords)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGfileCreate");
		free (xcoords); free (ycoords);
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	free (xcoords); free (ycoords);
	if ((NCGdataCopyAttributes (tplNCId, NCGdataGetXVarId (tplNCId), ncid, lonid, true) == NCGfailed) ||
	    (NCGdataCopyAttributes (tplNCId, NCGdataGetYVarId (tplNCId), ncid, latid, true) == NCGfailed))
	{
		nc_close (ncid);
		unlink (fileName);
		return (NCGfailed);
	}
	return (ncid);
}

int NCGfileVarAdd (int ncid, const char *varname, nc_type ltype, nc_type ttype, nc_type vtype)
{
	int status, varid, dimids [4], ndims = 1, xdim, ydim = NCGundefined;
	char *typeString;
	bool redef;

	if (( (xdim = NCGdataGetCDimId (ncid)) == NCGfailed) && 
		 (((xdim = NCGdataGetXDimId (ncid)) == NCGfailed) || ((ydim = NCGdataGetYDimId (ncid)) == NCGfailed))) return (NCGfailed);

	redef = nc_redef (ncid) == NC_NOERR;
	if ((status = nc_def_dim (ncid, NCGnameDNTime,  NC_UNLIMITED, dimids)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }
	if (ltype != NC_NAT)
	{
		if ((status = nc_def_dim (ncid, NCGnameDNLevel, 1, dimids + 1)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }
		if ((status = nc_def_var (ncid, NCGnameDNLevel,   ltype, 1, dimids + 1, &varid)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }
		ndims += 1;
	}
	if ((status = nc_def_var (ncid, NCGnameDNTime,    ttype, 1, dimids, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }
	if ((status = nc_def_var (ncid, NCGnameVAMaximum, vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }
	if ((status = nc_def_var (ncid, NCGnameVAMinimum, vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }
	if ((status = nc_def_var (ncid, NCGnameVAAverage, vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }
	if ((status = nc_def_var (ncid, NCGnameVAStdDev,  vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }

	if (ydim != NCGundefined)
	{
		dimids [ndims++] = ydim;
		typeString = NCGnameTypeGCont;
	}
	else typeString = NCGnameTypePoint;
	dimids [ndims++] = xdim;

	if ((status = nc_def_var (ncid, varname,          vtype, ndims, dimids, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }

	if ((status = nc_put_att_text (ncid, NC_GLOBAL, NCGnameGADataType, strlen (typeString), typeString)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarAdd"); return (NCGfailed); }

	if (redef) nc_enddef (ncid);
	return (varid);
}

int NCGfileVarClone (int sncid, int dncid)
{
	bool redef;
	int status, varid, lvarid, tvarid, svarid, dvarid;
	size_t ndims;
	nc_type ltype, ttype, vtype;
	char varname [NC_MAX_NAME];

	if ((status = nc_inq_varid (sncid, NCGnameDNLevel, &lvarid)) != NC_NOERR)
	{
		lvarid = NCGundefined;
		ltype = NC_NAT;
	}
	else
	{
		if ((status = nc_inq_varndims (sncid, lvarid, &ndims)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
		if (ndims > 1) { fprintf (stderr,"Level variable has too many dimensions in: NCGfileVarClone ()\n"); return (NCGfailed); }
		if ((status = nc_inq_vartype  (sncid, lvarid, &ltype)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	}

	if ((status = nc_inq_varid (sncid, NCGnameDNTime, &tvarid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	if ((status = nc_inq_varndims (sncid, tvarid, &ndims)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	if (ndims > 1) { fprintf (stderr,"Time variable has too many dimensions in: NCGfileVarClone ()\n"); return (NCGfailed); }
	if ((status = nc_inq_vartype  (sncid, tvarid, &ttype)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }

	if ((svarid = NCGdataGetCoreVarId (sncid)) == NCGfailed) return (NCGfailed);
	if ((status = nc_inq_varname (sncid, svarid, varname)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	if ((status = nc_inq_vartype (sncid, svarid, &vtype)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }

	if ((dvarid = NCGfileVarAdd (dncid, varname, ltype, ttype, vtype)) == NCGfailed) return (NCGfailed);
	if (NCGdataCopyAttributes (sncid, svarid, dncid, dvarid, true) == NCGfailed) return (NCGfailed);

	if (lvarid != NCGundefined)
	{
		if ((status = nc_inq_varid (dncid, NCGnameDNLevel, &varid)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
		if (NCGdataCopyAttributes (sncid, lvarid, dncid,  varid, true) == NCGfailed) return (NCGfailed);
	}
	if ((status = nc_inq_varid (dncid, NCGnameDNTime,  &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	if (NCGdataCopyAttributes (sncid, tvarid, dncid,  varid, true) == NCGfailed) return (NCGfailed);

	redef = status = nc_redef (dncid) == NC_NOERR ? true : false;

	if ((status = nc_inq_varid (dncid, NCGnameVAMaximum, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	nc_copy_att (sncid,svarid,NCGnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAAddOffset,  dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAActualRange,dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAValidRange, dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAFillValue,  dncid,varid);
	if ((status = nc_inq_varid (dncid, NCGnameVAMinimum, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	nc_copy_att (sncid,svarid,NCGnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAAddOffset,  dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAActualRange,dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAValidRange, dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAFillValue,  dncid,varid);
	if ((status = nc_inq_varid (dncid, NCGnameVAAverage, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	nc_copy_att (sncid,svarid,NCGnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAAddOffset,  dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAActualRange,dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAValidRange, dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAFillValue,  dncid,varid);
	if ((status = nc_inq_varid (dncid, NCGnameVAStdDev, &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarClone"); return (NCGfailed); }
	nc_copy_att (sncid,svarid,NCGnameVAUnits,      dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAScaleFactor,dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAMissingVal, dncid,varid);
	nc_copy_att (sncid,svarid,NCGnameVAFillValue,  dncid,varid);
	if (redef) nc_enddef (dncid);
	return (dvarid);
}

NCGstate NCGfileSetLevelUnit (int ncid, const char *unitStr)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((status = nc_inq_varid (ncid, NCGnameDNLevel,  &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarSetUnits"); goto ABORT; }
	if ((status = nc_put_att_text (ncid, varid, NCGnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarSetUnits"); goto ABORT; }

	if (redef) nc_enddef (ncid);
	return (NCGsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCGfailed);
}

NCGstate NCGfileSetTimeUnit (int ncid, const char *unitStr)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((status = nc_inq_varid (ncid, NCGnameDNTime,  &varid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarSetUnits"); goto ABORT; }
	if ((status = nc_put_att_text (ncid, varid, NCGnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileVarSetUnits"); goto ABORT; }

	if (redef) nc_enddef (ncid);
	return (NCGsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCGfailed);
}

NCGstate NCGfileSetVarUnit (int ncid, const char *unitStr)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((varid = NCGdataGetCoreVarId (ncid)) == NCGfailed)
	{ fprintf (stderr,"Missing core variable in: NCGfileSetVarUnit ()\n"); goto ABORT; }
	else if ((status = nc_put_att_text (ncid, varid, NCGnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGfileSetVarUnit"); goto ABORT; }

	if (nc_inq_varid (ncid, NCGnameVAAverage, &varid) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCGnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileSetVarUnit"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMaximum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCGnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileSetVarUnit"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMinimum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCGnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileSetVarUnit"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAStdDev, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_text (ncid, varid, NCGnameVAUnits, strlen (unitStr), unitStr)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileSetVarUnit"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCGsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCGfailed);
}

NCGstate NCGfileSetVarTransform (int ncid, double scale, double offset)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((varid = NCGdataGetCoreVarId (ncid)) == NCGfailed)
	{ fprintf (stderr,"Missing core variable in: NCGfileSetVarTransform ()\n"); goto ABORT; }
	else
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAScaleFactor,NC_FLOAT, 1, &scale))  != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAAddOffset,  NC_FLOAT, 1, &offset)) != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileSetVarTransform"); goto ABORT; }
	}

	if ((status = nc_inq_varid (ncid, NCGnameVAAverage, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAScaleFactor,NC_FLOAT, 1, &scale))  != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAAddOffset,  NC_FLOAT, 1, &offset)) != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileSetVarTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMaximum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAScaleFactor,NC_FLOAT, 1, &scale))   != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAAddOffset,  NC_FLOAT, 1, &offset))  != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileSetVarTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMinimum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAScaleFactor,NC_FLOAT, 1, &scale))   != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAAddOffset,  NC_FLOAT, 1, &offset))  != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileSetVarTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAStdDev, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAScaleFactor,NC_FLOAT, 1, &scale))   != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAAddOffset,  NC_FLOAT, 1, &offset))  != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileSetVarTransform"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCGsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCGfailed);
}

NCGstate NCGfileSetMissingVal (int ncid,  double missingVal)
{
	int status, varid;
	bool redef;

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	if ((varid = NCGdataGetCoreVarId (ncid)) == NCGfailed)
	{ fprintf (stderr,"Missing core variable in: NCGfileSetMissingVal ()\n"); goto ABORT; }
	else
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
			 ((status = nc_put_att_double (ncid, varid, NCGnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileVarSetMissingVal"); goto ABORT; }
	}

	if ((status = nc_inq_varid (ncid, NCGnameVAAverage, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileVarSetMissingVal"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMaximum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileVarSetMissingVal"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMinimum, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAFillValue,   NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAMissingVal,  NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileVarSetMissingVal"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAStdDev, &varid)) == NC_NOERR)
	{
		if (((status = nc_put_att_double (ncid, varid, NCGnameVAFillValue,  NC_FLOAT, 1, &missingVal)) != NC_NOERR) ||
		    ((status = nc_put_att_double (ncid, varid, NCGnameVAMissingVal, NC_FLOAT, 1, &missingVal)) != NC_NOERR))
		{ NCGprintNCError (status,"NCGfileVarSetMissingVal"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCGsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCGfailed);
}

NCGstate NCGfileSetValidRange (int ncid, double min, double max)
{
	int status, varid;
	bool redef;
	double range [2];

	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	range [0] = min;
	range [1] = max;
	if ((varid = NCGdataGetCoreVarId (ncid)) == NCGfailed)
	{ fprintf (stderr,"Missing core variable in: NCGfileSetValidRange ()\n"); goto ABORT; }
	else
	{
		if ((status = nc_put_att_double (ncid, varid, NCGnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarSetTransform"); goto ABORT; }
	}

	if ((status = nc_inq_varid (ncid, NCGnameVAAverage, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCGnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarSetTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMaximum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCGnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarSetTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAMinimum, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCGnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarSetTransform"); goto ABORT; }
	}
	if ((status = nc_inq_varid (ncid, NCGnameVAStdDev, &varid)) == NC_NOERR)
	{
		if ((status = nc_put_att_double (ncid, varid, NCGnameVAValidRange,  NC_FLOAT, 2, range))       != NC_NOERR)
		{ NCGprintNCError (status,"NCGfileVarSetTransform"); goto ABORT; }
	}
	if (redef) nc_enddef (ncid);
	return (NCGsucceeded);
ABORT:
	if (redef) nc_enddef (ncid);
	return (NCGfailed);
}
