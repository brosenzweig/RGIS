#include<NC.h>
#include<NCnames.h>

NCGdataType NCGdataGetType (int ncid)
{
	int status;
	char dTypeStr [NC_MAX_NAME];

	if ((status = nc_get_att_text (ncid,NC_GLOBAL,NCGnameGADataType,dTypeStr)) == NC_NOERR)
	{
		if      (strncmp (dTypeStr,NCGnameTypeGCont,  strlen (NCGnameTypeGCont))   == 0)  return (NCGtypeGCont);
		else if (strncmp (dTypeStr,NCGnameTypeGDisc,  strlen (NCGnameTypeGDisc))   == 0)  return (NCGtypeGDisc);
		else if (strncmp (dTypeStr,NCGnameTypePoint,  strlen (NCGnameTypePoint))   == 0)  return (NCGtypePoint);
		else if (strncmp (dTypeStr,NCGnameTypeLine,   strlen (NCGnameTypeLine))    == 0)  return (NCGtypeLine);
		else if (strncmp (dTypeStr,NCGnameTypePolygon,strlen (NCGnameTypePolygon)) == 0)  return (NCGtypePolygon);
		else if (strncmp (dTypeStr,NCGnameTypeNetwork,strlen (NCGnameTypeNetwork)) == 0)  return (NCGtypeNetwork);
		fprintf (stderr,"Invalid data type in: NCGdataGetType ()\n");
		return (NCGundefined);
	}
	return (NCGtypeGCont);
}

char *NCGdataGetTextAttribute (int ncid, int varid, const char *attName)
{
	int status;
	char *att;
	size_t attlen;

	if ((status = nc_inq_attlen (ncid, varid, attName, &attlen)) != NC_NOERR) return ("undefined");

	if ((att = (char *) malloc (attlen + 1)) == (char *) NULL)
	{
		fprintf (stderr,"Memory allocation error in: NCGdataGetTextAttribute ()\n");
		return ((char *) NULL);
	}
	if ((status = nc_get_att_text (ncid, varid, attName, att)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGdataGetTextAttribute");
		free (att);
		return ((char *) NULL);
	}
	att [attlen] = '\0';
	return (att);
}

NCGstate NCGdataSetTextAttribute (int ncid, int varid, const char *attName, const char *text)
{
	int status;
	bool redef;

	redef = status = nc_redef (ncid) == NC_NOERR ? true : false;

	status = nc_put_att_text (ncid,varid,attName,strlen (text) + 1,text);
	if (redef) nc_enddef (ncid);
	if (status != NC_NOERR)
	{
		NCGprintNCError (status,"NCGdataSetTextAttribute");
		return (NCGfailed);
	}
	return (NCGsucceeded);
}

NCGstate NCGdataCopyAttributes (int inNCid, int inVarid, int outNCid, int outVarid, bool overwrite)
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
				NCGprintNCError (status,"_NCGdataCopyAttributes");
				return (NCGfailed);
			}
		}
	}
	if (redef) nc_enddef (outNCid);
	return (NCGsucceeded);
}

NCGstate NCGdataCopyAllAttibutes (int inNCid, int outNCid, bool overwrite)
{
	int status, inVarid, outVarid, varnum;
	char varName [NC_MAX_NAME];
	if (NCGdataCopyAttributes (inNCid, NC_GLOBAL, outNCid, NC_GLOBAL, overwrite) == NCGfailed) return (NCGfailed);

	if ((status = nc_inq_nvars (inNCid,&varnum)) != NC_NOERR)	
	{
		NCGprintNCError (status,"NCGdataCopyAttibutes");
		return (NCGfailed);
	}
	for (inVarid = 0; inVarid < varnum;++inVarid)
	{
		if ((status = nc_inq_varname (inNCid,inVarid,varName)) != NC_NOERR)
		{
			NCGprintNCError (status,"NCGdataCopyAttibutes");
			return (NCGfailed);
		}
		if ((status = nc_inq_varid (outNCid,varName,&outVarid)) != NC_NOERR) continue;
		if (NCGdataCopyAttributes (inNCid, inVarid, outNCid, outVarid, overwrite) == NCGfailed) return (NCGfailed);
	}
	return (NCGsucceeded);
}

NCGprojection NCGdataGetProjection (int ncid)
{
	int dimid;
	char projName [NC_MAX_NAME];

	if (nc_get_att_text (ncid,NC_GLOBAL,NCGnameGAProjection,projName) == NC_NOERR)
	{
		if      (strcmp (projName, NCGnameGAProjCartesian)) return (NCGprojCartesian);
		else if (strcmp (projName, NCGnameGAProjSpherical)) return (NCGprojSpherical);
		fprintf (stderr,"Invalid data type in: NCGdataGetProjection ()\n");
	}
	else
	{
		if      (nc_inq_dimid (ncid, NCGnameDNXCoord,    &dimid) == NC_NOERR) return (NCGprojCartesian);
		else if (nc_inq_dimid (ncid, NCGnameDNLon,       &dimid) == NC_NOERR) return (NCGprojSpherical);
		else if (nc_inq_dimid (ncid, NCGnameDNLongitude, &dimid) == NC_NOERR) return (NCGprojSpherical);
		fprintf (stderr,"Nongeographical data in: NCGdataGetProjection ()\n");
	}
	return (NCGundefined);
}

int NCGdataGetXDimId (int ncid)
{
	int dimid;

	if ((nc_inq_dimid  (ncid, NCGnameDNCoord,     &dimid) == NC_NOERR) ||
	    (nc_inq_dimid  (ncid, NCGnameDNXCoord,    &dimid) == NC_NOERR) ||
		 (nc_inq_dimid  (ncid, NCGnameDNLon,       &dimid) == NC_NOERR) ||
		 (nc_inq_dimid  (ncid, NCGnameDNLongitude, &dimid) == NC_NOERR)) return (dimid);
	return (NCGfailed);
}

int NCGdataGetXVarId (int ncid)
{
	int varid;

	if ((nc_inq_varid  (ncid, NCGnameDNXCoord,    &varid) == NC_NOERR) ||
		 (nc_inq_varid  (ncid, NCGnameDNLon,       &varid) == NC_NOERR) ||
		 (nc_inq_varid  (ncid, NCGnameDNLongitude, &varid) == NC_NOERR)) return (varid);
	return (NCGfailed);
}

int NCGdataGetYDimId (int ncid)
{
	int dimid;

	if ((nc_inq_dimid  (ncid, NCGnameDNCoord,     &dimid) == NC_NOERR) ||
	    (nc_inq_dimid  (ncid, NCGnameDNYCoord,    &dimid) == NC_NOERR) ||
		 (nc_inq_dimid  (ncid, NCGnameDNLat,       &dimid) == NC_NOERR) ||
		 (nc_inq_dimid  (ncid, NCGnameDNLatitude,  &dimid) == NC_NOERR)) return (dimid);
	return (NCGfailed);
}

int NCGdataGetYVarId (int ncid)
{
	int varid;

	if ((nc_inq_varid  (ncid, NCGnameDNYCoord,    &varid) == NC_NOERR) ||
		 (nc_inq_varid  (ncid, NCGnameDNLat,       &varid) == NC_NOERR) ||
		 (nc_inq_varid  (ncid, NCGnameDNLatitude,  &varid) == NC_NOERR)) return (varid);
	return (NCGfailed);
}

int NCGdataGetLVarId (int ncid)
{
	int varid;

	if (nc_inq_varid  (ncid, NCGnameDNLevel,  &varid) == NC_NOERR) return (varid);
	return (NCGfailed);
}

int NCGdataGetTVarId (int ncid)
{
	int varid;

	if (nc_inq_varid  (ncid, NCGnameDNTime,  &varid) == NC_NOERR) return (varid);
	return (NCGfailed);
}

int NCGdataGetCDimId (int ncid)
{
	int dimid;

	if (nc_inq_dimid  (ncid, NCGnameDNCoord, &dimid) == NC_NOERR) return (dimid);
	return (NCGfailed);
}

double *NCGdataGetVector (int ncid, int dimid, int varid, size_t *len)
{
	int status;
	size_t start;
	double *coords;

	if ((dimid == NCGfailed) || (varid == NCGfailed)) return ((double *) NULL);
	if ((status = nc_inq_dimlen (ncid, dimid, len)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGdataGetVector");
		return ((double *) NULL);
	}
	if ((coords = (double  *) calloc (*len, sizeof (double))) == (double *) NULL)
	{
		fprintf (stderr,"Memory allocation error in: NCGdataGetVector ()\n");
		return ((double *) NULL);
	}
	start = 0;
	if ((status = nc_get_vara_double (ncid, varid, &start, len, coords)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGdataGetVector");
		free (coords);
		return ((double *) NULL);
	}
	return (coords);
}

int NCGdataGetCoreVarId (int ncid)
{
	int status;
	int varid;
	int dimid [4], xdim = NCGundefined, ydim = NCGundefined, i, j;
	size_t nvars, ndims;

	if ((status = nc_inq_nvars (ncid, &nvars)) != NC_NOERR)
	{
		NCGprintNCError (status,"NCGdataGetGridVarId");
		return (NCGfailed);
	}

	if (((xdim = NCGdataGetCDimId (ncid)) == NCGfailed) &&
	    (((xdim = NCGdataGetXDimId (ncid)) == NCGfailed) || ((ydim = NCGdataGetYDimId (ncid)) == NCGfailed))) return (NCGfailed);

	for (varid = 0;varid < nvars; ++varid)
	{
		if ((status = nc_inq_varndims (ncid, varid, &ndims)) != NC_NOERR)
		{
			NCGprintNCError (status,"NCGdataGetGridVarId");
			return (NCGfailed);
		}
		if ((ndims < 1) || (ndims > 4)) continue;
		if ((status = nc_inq_vardimid (ncid, varid, dimid)) != NC_NOERR)
		{
			NCGprintNCError (status,"NCGdataGetGridVarId");
			return (NCGfailed);
		}
		for (i = 0;i < ndims; ++i)
			if (xdim == dimid [i])
			{
				if (ydim == NCGundefined) return (varid);
				for (j = 0;j < ndims; ++j) if (ydim == dimid [j]) return (varid);
			}
	}
	return (NCGfailed);
}
