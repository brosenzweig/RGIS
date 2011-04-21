#include <cm.h>
#include <NCdsHandle.h>

NCstate NCdsHandleGLayoutDefine (NCdsHandleGLayout_t *glt, int *ncids, size_t n)
{
	int status;
	int i, xdimid, xvarid, ydimid, yvarid;
	nc_type gtype;
	size_t xdim, ydim;

	if (n < 1) return (NCfailed);
	if (NCdsHandleGeoDefine ((NCdsHandleGeo_t *) glt, ncids, n) == NCfailed) return (NCfailed);
	glt->XCoords = glt->YCoords = (double *) NULL;
	glt->GVarIds = (int *) NULL;

	if ((glt->DataType != NCtypeGCont) && (glt->DataType != NCtypeGDisc) && (glt->DataType != NCtypeNetwork))
	{ CMmsgPrint (CMmsgAppError, "Invalid data in: %s %d",__FILE__,__LINE__); goto ABORT; }

	if ((glt->GVarIds = (int *) calloc (n, sizeof (int))) == (int *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation Error in: %s %d",__FILE__,__LINE__); goto ABORT; }

	for (i = 0;i < n;++i)
		{
			if ((glt->GVarIds [i] = NCdataGetCoreVarId (ncids [i])) == NCfailed)
			{ NCdsHandleGeoClear ((NCdsHandleGeo_t *) glt);      goto ABORT; }

			if ((status = nc_inq_vartype (ncids [i],glt->GVarIds [i], &gtype)) != NC_NOERR)
			{ NCprintNCError (status,"NCdsHandleGLayoutDefine"); goto ABORT; }

			if ((((status = nc_inq_dimid  (ncids [i], NCnameDNXCoord,    &xdimid)) != NC_NOERR) ||
			     ((status = nc_inq_dimlen (ncids [i], xdimid, &xdim))               != NC_NOERR) ||
				  ((status = nc_inq_varid  (ncids [i], NCnameDNXCoord,    &xvarid)) != NC_NOERR)) &&
		       (((status = nc_inq_dimid  (ncids [i], NCnameDNLon,       &xdimid)) != NC_NOERR) ||
				  ((status = nc_inq_dimlen (ncids [i], xdimid, &xdim))               != NC_NOERR) ||
				  ((status = nc_inq_varid  (ncids [i], NCnameDNLon,       &xvarid)) != NC_NOERR)) &&
   		    (((status = nc_inq_dimid  (ncids [i], NCnameDNLongitude, &xdimid)) != NC_NOERR) ||
			     ((status = nc_inq_dimlen (ncids [i], xdimid, &xdim))               != NC_NOERR) ||
			     ((status = nc_inq_varid  (ncids [i], NCnameDNLongitude, &xvarid)) != NC_NOERR)))
			{ NCprintNCError (status,"NCdsHandleGLayoutDefine"); goto ABORT; }

			if ((((status = nc_inq_dimid  (ncids [i], NCnameDNYCoord,    &ydimid)) != NC_NOERR) ||
				  ((status = nc_inq_dimlen (ncids [i], ydimid, &ydim))               != NC_NOERR) ||
				  ((status = nc_inq_varid  (ncids [i], NCnameDNYCoord,    &yvarid)) != NC_NOERR)) &&
			    (((status = nc_inq_dimid  (ncids [i], NCnameDNLat,       &ydimid)) != NC_NOERR) ||
				  ((status = nc_inq_dimlen (ncids [i], ydimid, &ydim))               != NC_NOERR) ||
			     ((status = nc_inq_varid  (ncids [i], NCnameDNLat,       &yvarid)) != NC_NOERR)) &&
			    (((status = nc_inq_dimid  (ncids [i], NCnameDNLatitude,  &ydimid)) != NC_NOERR) ||
			     ((status = nc_inq_dimlen (ncids [i], ydimid, &ydim))               != NC_NOERR) ||
			     ((status = nc_inq_varid  (ncids [i], NCnameDNLatitude,  &yvarid)) != NC_NOERR)))
			{ NCprintNCError (status,"NCdsHandleGLayoutDefine"); goto ABORT; }

			if (i == 0)
			{
				if ((glt->XCoords = (double *) calloc (xdim + 1, sizeof (double))) == (double *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }

				if ((status = nc_get_var_double (ncids [i],xvarid,glt->XCoords)) != NC_NOERR) 
				{ NCprintNCError (status,"NCdsHandleGLayoutDefine"); goto ABORT; }

				glt->XCoords [xdim] = xdim > 1 ? 2 * glt->XCoords [xdim - 1] - glt->XCoords [xdim - 2] : glt->XCoords [0];
				if (glt->Extent.LowerLeft.X  > glt->XCoords [0])    glt->Extent.LowerLeft.X  = glt->XCoords [0];
				if (glt->Extent.LowerLeft.X  > glt->XCoords [xdim]) glt->Extent.LowerLeft.X  = glt->XCoords [xdim];
				if (glt->Extent.UpperRight.X < glt->XCoords [0])    glt->Extent.UpperRight.X = glt->XCoords [0];
				if (glt->Extent.UpperRight.X < glt->XCoords [xdim]) glt->Extent.UpperRight.X = glt->XCoords [xdim];

				if ((glt->YCoords = (double *) calloc (ydim + 1, sizeof (double))) == (double *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }

				if ((status = nc_get_var_double (ncids [i],yvarid,glt->YCoords)) != NC_NOERR) 
				{ NCprintNCError (status,"NCdsHandleGLayoutDefine"); goto ABORT; }

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
			{ CMmsgPrint (CMmsgAppError, "Inconsistent NetCDF bundle in: %s %d",__FILE__,__LINE__); goto ABORT; }
		}
	return (NCsucceeded);

ABORT:
	NCdsHandleGeoClear ((NCdsHandleGeo_t *) glt);
	return (NCfailed);
}

void NCdsHandleGLayoutClear (NCdsHandleGLayout_t *glt)
{
	NCdsHandleGeoClear ((NCdsHandleGeo_t *) glt);
	if (glt->GVarIds != (int *)    NULL) free (glt->GVarIds);
	if (glt->XCoords != (double *) NULL) free (glt->XCoords);
	if (glt->YCoords != (double *) NULL) free (glt->YCoords);
	glt->XCoords = glt->YCoords = (double *) NULL;
	glt->GVarIds = (int *) NULL;
	glt->ColNum  = glt->RowNum  = (size_t)   0;
}

int NCdsHandleGLayoutColByCoord (NCdsHandleGLayout_t *glt, double x)
{
	int i;
	if (glt->XCoords [0] < glt->XCoords [glt->ColNum - 1])
	{ for (i = 0;i <= glt->ColNum;++i) if (glt->XCoords [i] > x) break; }
	else
	{ for (i = 0;i <= glt->ColNum;++i) if (glt->XCoords [i] < x) break; }
	return ((i > 0) && (i <= glt->ColNum) ? i - 1 : NCfailed);
}

int NCdsHandleGLayoutRowByCoord (NCdsHandleGLayout_t *glt, double y)
{
	int i;
	if (glt->YCoords [0] < glt->YCoords [glt->RowNum - 1])
	{ for (i = 0;i <= glt->RowNum;++i) if (glt->YCoords [i] > y) break; }
	else
	{ for (i = 0;i <= glt->RowNum;++i) if (glt->YCoords [i] < y) break; }
	return ((i > 0) && (i <= glt->RowNum) ? i - 1 : NCfailed);
}
