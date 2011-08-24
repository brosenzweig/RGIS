#include <string.h>
#include <cm.h>
#include <NC.h>
#include <NCnames.h>

NCdataType NCdataGetType (int ncid)
{
	int status;
	char dTypeStr [NC_MAX_NAME];

	if ((status = nc_get_att_text (ncid,NC_GLOBAL,NCnameGADataType,dTypeStr)) == NC_NOERR)
	{
		if      (strncmp (dTypeStr,NCnameTypeGCont,  strlen (NCnameTypeGCont))   == 0)  return (NCtypeGCont);
		else if (strncmp (dTypeStr,NCnameTypeGDisc,  strlen (NCnameTypeGDisc))   == 0)  return (NCtypeGDisc);
		else if (strncmp (dTypeStr,NCnameTypePoint,  strlen (NCnameTypePoint))   == 0)  return (NCtypePoint);
		else if (strncmp (dTypeStr,NCnameTypeLine,   strlen (NCnameTypeLine))    == 0)  return (NCtypeLine);
		else if (strncmp (dTypeStr,NCnameTypePolygon,strlen (NCnameTypePolygon)) == 0)  return (NCtypePolygon);
		else if (strncmp (dTypeStr,NCnameTypeNetwork,strlen (NCnameTypeNetwork)) == 0)  return (NCtypeNetwork);
		CMmsgPrint (CMmsgAppError, "Invalid data type in: %s %d",__FILE__,__LINE__);
		return (NCundefined);
	}
	return (NCtypeGCont);
}

char *NCdataGetTextAttribute (int ncid, int varid, const char *attName)
{
	int status;
	char *att;
	size_t attlen;

	if ((status = nc_inq_attlen (ncid, varid, attName, &attlen)) != NC_NOERR) return ("undefined");

	if ((att = (char *) malloc (attlen + 1)) == (char *) NULL)
	{
		CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__);
		return ((char *) NULL);
	}
	if ((status = nc_get_att_text (ncid, varid, attName, att)) != NC_NOERR)
	{
		NCprintNCError (status,"NCdataGetTextAttribute");
		free (att);
		return ((char *) NULL);
	}
	att [attlen] = '\0';
	return (att);
}

NCstate NCdataSetTextAttribute (int ncid, int varid, const char *attName, const char *text)
{
	int status;
	bool redef;


	redef = nc_redef (ncid) == NC_NOERR ? true : false;

	status = nc_put_att_text (ncid,varid,attName,strlen (text) + 1,text);
	if (redef) nc_enddef (ncid);
	if (status != NC_NOERR)
	{
		NCprintNCError (status,"NCdataSetTextAttribute");
		return (NCfailed);
	}
	return (NCsucceeded);
}

NCstate NCdataCopyAttributes (int inNCid, int inVarid, int outNCid, int outVarid, bool overwrite)
{
	bool redef;
	int status, att = 0, attId;
	char attName [NC_MAX_NAME];

	redef = nc_redef (outNCid) == NC_NOERR;
	while ((status = nc_inq_attname (inNCid, inVarid, att++,  attName)) == NC_NOERR)
	{
		if (overwrite || (nc_inq_attid (outNCid,outVarid,attName,&attId) != NC_NOERR))
		{
			if ((status = nc_copy_att (inNCid,  inVarid, attName, outNCid, outVarid)) != NC_NOERR)
			{
				NCprintNCError (status,"_NCdataCopyAttributes");
				return (NCfailed);
			}
		}
	}
	if (redef) nc_enddef (outNCid);
	return (NCsucceeded);
}

NCstate NCdataCopyAllAttibutes (int inNCid, int outNCid, bool overwrite)
{
	int status, inVarid, outVarid, varnum;
	char varName [NC_MAX_NAME];
	if (NCdataCopyAttributes (inNCid, NC_GLOBAL, outNCid, NC_GLOBAL, overwrite) == NCfailed) return (NCfailed);

	if ((status = nc_inq_nvars (inNCid,&varnum)) != NC_NOERR)	
	{
		NCprintNCError (status,"NCdataCopyAttibutes");
		return (NCfailed);
	}
	for (inVarid = 0; inVarid < varnum;++inVarid)
	{
		if ((status = nc_inq_varname (inNCid,inVarid,varName)) != NC_NOERR)
		{
			NCprintNCError (status,"NCdataCopyAttibutes");
			return (NCfailed);
		}
		if ((status = nc_inq_varid (outNCid,varName,&outVarid)) != NC_NOERR) continue;
		if (NCdataCopyAttributes (inNCid, inVarid, outNCid, outVarid, overwrite) == NCfailed) return (NCfailed);
	}
	return (NCsucceeded);
}

NCprojection NCdataGetProjection (int ncid)
{
	int dimid;
	char projName [NC_MAX_NAME];

	if (nc_get_att_text (ncid,NC_GLOBAL,NCnameGAProjection,projName) == NC_NOERR)
	{
		if      (strcmp (projName, NCnameGAProjCartesian)) return (NCprojCartesian);
		else if (strcmp (projName, NCnameGAProjSpherical)) return (NCprojSpherical);
		CMmsgPrint (CMmsgAppError, "Invalid data type in: %s %d",__FILE__,__LINE__);
	}
	else
	{
		if      (nc_inq_dimid (ncid, NCnameDNXCoord,    &dimid) == NC_NOERR) return (NCprojCartesian);
		else if (nc_inq_dimid (ncid, NCnameDNLon,       &dimid) == NC_NOERR) return (NCprojSpherical);
		else if (nc_inq_dimid (ncid, NCnameDNLongitude, &dimid) == NC_NOERR) return (NCprojSpherical);
		CMmsgPrint (CMmsgAppError, "Nongeographical data in: %s %d",__FILE__,__LINE__);
	}
	return (NCundefined);
}

int NCdataGetXDimId (int ncid)
{
	int dimid;

	if ((nc_inq_dimid  (ncid, NCnameDNCoord,     &dimid) == NC_NOERR) ||
	    (nc_inq_dimid  (ncid, NCnameDNXCoord,    &dimid) == NC_NOERR) ||
		(nc_inq_dimid  (ncid, NCnameDNLon,       &dimid) == NC_NOERR) ||
		(nc_inq_dimid  (ncid, NCnameDNLongitude, &dimid) == NC_NOERR)) return (dimid);
	return (NCfailed);
}

int NCdataGetXVarId (int ncid)
{
	int varid;

	if ((nc_inq_varid  (ncid, NCnameDNXCoord,    &varid) == NC_NOERR) ||
		(nc_inq_varid  (ncid, NCnameDNLon,       &varid) == NC_NOERR) ||
		(nc_inq_varid  (ncid, NCnameDNLongitude, &varid) == NC_NOERR)) return (varid);
	return (NCfailed);
}

int NCdataGetYDimId (int ncid)
{
	int dimid;

	if ((nc_inq_dimid  (ncid, NCnameDNCoord,     &dimid) == NC_NOERR) ||
	    (nc_inq_dimid  (ncid, NCnameDNYCoord,    &dimid) == NC_NOERR) ||
		(nc_inq_dimid  (ncid, NCnameDNLat,       &dimid) == NC_NOERR) ||
		(nc_inq_dimid  (ncid, NCnameDNLatitude,  &dimid) == NC_NOERR)) return (dimid);
	return (NCfailed);
}

int NCdataGetYVarId (int ncid)
{
	int varid;

	if ((nc_inq_varid  (ncid, NCnameDNYCoord,    &varid) == NC_NOERR) ||
		(nc_inq_varid  (ncid, NCnameDNLat,       &varid) == NC_NOERR) ||
		(nc_inq_varid  (ncid, NCnameDNLatitude,  &varid) == NC_NOERR)) return (varid);
	return (NCfailed);
}

int NCdataGetLVarId (int ncid)
{
	int varid;

	if (nc_inq_varid  (ncid, NCnameDNLevel,  &varid) == NC_NOERR) return (varid);
	return (NCfailed);
}

int NCdataGetTVarId (int ncid)
{
	int varid;

	if (nc_inq_varid  (ncid, NCnameDNTime,  &varid) == NC_NOERR) return (varid);
	return (NCfailed);
}

int NCdataGetCDimId (int ncid)
{
	int dimid;

	if (nc_inq_dimid  (ncid, NCnameDNCoord, &dimid) == NC_NOERR) return (dimid);
	return (NCfailed);
}

double *NCdataGetVector (int ncid, int dimid, int varid, size_t *len)
{
	int status;
	size_t start;
	double *coords;

	if ((dimid == NCfailed) || (varid == NCfailed)) return ((double *) NULL);
	if ((status = nc_inq_dimlen (ncid, dimid, len)) != NC_NOERR)
	{
		NCprintNCError (status,"NCdataGetVector");
		return ((double *) NULL);
	}
	if ((coords = (double  *) calloc (*len, sizeof (double))) == (double *) NULL)
	{
		CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__);
		return ((double *) NULL);
	}
	start = 0;
	if ((status = nc_get_vara_double (ncid, varid, &start, len, coords)) != NC_NOERR)
	{
		NCprintNCError (status,"NCdataGetVector");
		free (coords);
		return ((double *) NULL);
	}
	return (coords);
}

int NCdataGetCoreVarId (int ncid)
{
	int status;
	int varid, nvars, ndims;
	int dimid [4], xdim = NCundefined, ydim = NCundefined, i, j;

	if ((status = nc_inq_nvars (ncid, &nvars)) != NC_NOERR)
	{
		NCprintNCError (status,"NCdataGetGridVarId");
		return (NCfailed);
	}

	if (((xdim = NCdataGetCDimId (ncid)) == NCfailed) &&
	    (((xdim = NCdataGetXDimId (ncid)) == NCfailed) || ((ydim = NCdataGetYDimId (ncid)) == NCfailed))) return (NCfailed);

	for (varid = 0;varid < nvars; ++varid)
	{
		if ((status = nc_inq_varndims (ncid, varid, &ndims)) != NC_NOERR)
		{
			NCprintNCError (status,"NCdataGetGridVarId");
			return (NCfailed);
		}
		if ((ndims < 1) || (ndims > 4)) continue;
		if ((status = nc_inq_vardimid (ncid, varid, dimid)) != NC_NOERR)
		{
			NCprintNCError (status,"NCdataGetGridVarId");
			return (NCfailed);
		}
		for (i = 0;i < ndims; ++i)
			if (xdim == dimid [i])
			{
				if (ydim == NCundefined) return (varid);
				for (j = 0;j < ndims; ++j) if (ydim == dimid [j]) return (varid);
			}
	}
	return (NCfailed);
}
