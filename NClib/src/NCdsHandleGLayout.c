#include<NCdsHandle.h>

NCGstate NCGdsHandleGLayoutDefine (NCGdsHandleGLayout_t *glt, int *ncids, size_t n)
{
	int status;
	int i, xdimid, xvarid, ydimid, yvarid;
	nc_type gtype;
	size_t xdim, ydim;

	if (n < 1) return (NCGfailed);
	if (NCGdsHandleGeoDefine ((NCGdsHandleGeo_t *) glt, ncids, n) == NCGfailed) return (NCGfailed);
	glt->XCoords = glt->YCoords = (double *) NULL;
	glt->GVarIds = (int *) NULL;

	if ((glt->DataType != NCGtypeGCont) && (glt->DataType != NCGtypeGDisc) && (glt->DataType != NCGtypeNetwork))
	{ fprintf (stderr,"Invalid data in: NCGdsHandleGLayoutDefine ()\n"); goto ABORT; }

	if ((glt->GVarIds = (int *) calloc (n, sizeof (int))) == (int *) NULL)
	{ perror ("Memory allocation Error in: NCGdsHandleGLayoutDefine ()"); goto ABORT; }

	for (i = 0;i < n;++i)
		{
			if ((glt->GVarIds [i] = NCGdataGetCoreVarId (ncids [i])) == NCGfailed)
			{ NCGdsHandleGeoClear ((NCGdsHandleGeo_t *) glt);      goto ABORT; }

			if ((status = nc_inq_vartype (ncids [i],glt->GVarIds [i], &gtype)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGdsHandleGLayoutDefine"); goto ABORT; }

			if ((((status = nc_inq_dimid  (ncids [i], NCGnameDNXCoord,    &xdimid)) != NC_NOERR) ||
			     ((status = nc_inq_dimlen (ncids [i], xdimid, &xdim))               != NC_NOERR) ||
				  ((status = nc_inq_varid  (ncids [i], NCGnameDNXCoord,    &xvarid)) != NC_NOERR)) &&
		       (((status = nc_inq_dimid  (ncids [i], NCGnameDNLon,       &xdimid)) != NC_NOERR) ||
				  ((status = nc_inq_dimlen (ncids [i], xdimid, &xdim))               != NC_NOERR) ||
				  ((status = nc_inq_varid  (ncids [i], NCGnameDNLon,       &xvarid)) != NC_NOERR)) &&
   		    (((status = nc_inq_dimid  (ncids [i], NCGnameDNLongitude, &xdimid)) != NC_NOERR) ||
			     ((status = nc_inq_dimlen (ncids [i], xdimid, &xdim))               != NC_NOERR) ||
			     ((status = nc_inq_varid  (ncids [i], NCGnameDNLongitude, &xvarid)) != NC_NOERR)))
			{ NCGprintNCError (status,"NCGdsHandleGLayoutDefine"); goto ABORT; }

			if ((((status = nc_inq_dimid  (ncids [i], NCGnameDNYCoord,    &ydimid)) != NC_NOERR) ||
				  ((status = nc_inq_dimlen (ncids [i], ydimid, &ydim))               != NC_NOERR) ||
				  ((status = nc_inq_varid  (ncids [i], NCGnameDNYCoord,    &yvarid)) != NC_NOERR)) &&
			    (((status = nc_inq_dimid  (ncids [i], NCGnameDNLat,       &ydimid)) != NC_NOERR) ||
				  ((status = nc_inq_dimlen (ncids [i], ydimid, &ydim))               != NC_NOERR) ||
			     ((status = nc_inq_varid  (ncids [i], NCGnameDNLat,       &yvarid)) != NC_NOERR)) &&
			    (((status = nc_inq_dimid  (ncids [i], NCGnameDNLatitude,  &ydimid)) != NC_NOERR) ||
			     ((status = nc_inq_dimlen (ncids [i], ydimid, &ydim))               != NC_NOERR) ||
			     ((status = nc_inq_varid  (ncids [i], NCGnameDNLatitude,  &yvarid)) != NC_NOERR)))
			{ NCGprintNCError (status,"NCGdsHandleGLayoutDefine"); goto ABORT; }

			if (i == 0)
			{
				if ((glt->XCoords = (double *) calloc (xdim + 1, sizeof (double))) == (double *) NULL)
				{ perror ("Memory allocation error in: NCGdsHandleGLayoutDefine ()\n"); goto ABORT; }

				if ((status = nc_get_var_double (ncids [i],xvarid,glt->XCoords)) != NC_NOERR) 
				{ NCGprintNCError (status,"NCGdsHandleGLayoutDefine"); goto ABORT; }

				glt->XCoords [xdim] = xdim > 1 ? 2 * glt->XCoords [xdim - 1] - glt->XCoords [xdim - 2] : glt->XCoords [0];
				if (glt->Extent.LowerLeft.X  > glt->XCoords [0])    glt->Extent.LowerLeft.X  = glt->XCoords [0];
				if (glt->Extent.LowerLeft.X  > glt->XCoords [xdim]) glt->Extent.LowerLeft.X  = glt->XCoords [xdim];
				if (glt->Extent.UpperRight.X < glt->XCoords [0])    glt->Extent.UpperRight.X = glt->XCoords [0];
				if (glt->Extent.UpperRight.X < glt->XCoords [xdim]) glt->Extent.UpperRight.X = glt->XCoords [xdim];

				if ((glt->YCoords = (double *) calloc (ydim + 1, sizeof (double))) == (double *) NULL)
				{ perror ("Memory allocation error in: NCGdsHandleGLayoutDefine ()\n"); goto ABORT; }

				if ((status = nc_get_var_double (ncids [i],yvarid,glt->YCoords)) != NC_NOERR) 
				{ NCGprintNCError (status,"NCGdsHandleGLayoutDefine"); goto ABORT; }

				glt->YCoords [ydim] = ydim > 1 ? 2 * glt->YCoords [ydim - 1] - glt->YCoords [ydim - 2] : glt->YCoords [0];
				if (glt->Extent.LowerLeft.Y  > glt->YCoords [0])    glt->Extent.LowerLeft.Y  = glt->YCoords [0];
				if (glt->Extent.LowerLeft.Y  > glt->YCoords [ydim]) glt->Extent.LowerLeft.Y  = glt->YCoords [ydim];
				if (glt->Extent.UpperRight.Y < glt->YCoords [0])    glt->Extent.UpperRight.Y = glt->YCoords [0];
				if (glt->Extent.UpperRight.Y < glt->YCoords [ydim]) glt->Extent.UpperRight.Y = glt->YCoords [ydim];

				glt->GType = gtype;
				glt->ColNum = xdim;
				glt->RowNum = ydim;
			}
			else if ((glt->GType != gtype) || (glt->ColNum != xdim) || (glt->RowNum != ydim))
			{ fprintf (stderr,"Inconsistent NetCDF bundle in: NCGdsHandleGLayoutDefine ()\n"); goto ABORT; }
		}
	return (NCGsucceeded);

ABORT:
	NCGdsHandleGeoClear ((NCGdsHandleGeo_t *) glt);
	return (NCGfailed);
}

void NCGdsHandleGLayoutClear (NCGdsHandleGLayout_t *glt)
{
	NCGdsHandleGeoClear ((NCGdsHandleGeo_t *) glt);
	if (glt->GVarIds != (int *)    NULL) free (glt->GVarIds);
	if (glt->XCoords != (double *) NULL) free (glt->XCoords);
	if (glt->YCoords != (double *) NULL) free (glt->YCoords);
	glt->XCoords = glt->YCoords = (double *) NULL;
	glt->GVarIds = (int *) NULL;
	glt->ColNum  = glt->RowNum  = (size_t)   0;
}

int NCGdsHandleGLayoutColByCoord (NCGdsHandleGLayout_t *glt, double x)
{
	int i;
	if (glt->XCoords [0] < glt->XCoords [glt->ColNum - 1])
	{ for (i = 0;i <= glt->ColNum;++i) if (glt->XCoords [i] > x) break; }
	else
	{ for (i = 0;i <= glt->ColNum;++i) if (glt->XCoords [i] < x) break; }
	return ((i > 0) && (i <= glt->ColNum) ? i - 1 : NCGfailed);
}

int NCGdsHandleGLayoutRowByCoord (NCGdsHandleGLayout_t *glt, double y)
{
	int i;
	if (glt->YCoords [0] < glt->YCoords [glt->RowNum - 1])
	{ for (i = 0;i <= glt->RowNum;++i) if (glt->YCoords [i] > y) break; }
	else
	{ for (i = 0;i <= glt->RowNum;++i) if (glt->YCoords [i] < y) break; }
	return ((i > 0) && (i <= glt->RowNum) ? i - 1 : NCGfailed);
}
