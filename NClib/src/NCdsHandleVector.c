#include<NCdsHandle.h>

NCGstate NCGdsHandleVectorDefine (NCGdsHandleVector_t *vector, int ncid)
{
	NCGdataType dtype;
	char *xNames [] = { NCGnameDNXCoord, NCGnameDNLon, NCGnameDNLongitude };
	char *yNames [] = { NCGnameDNYCoord, NCGnameDNLat, NCGnameDNLatitude };

	vector->ItemNum = 0;
	dtype = NCGdataGetType (ncid);
	if ((dtype != NCGtypePoint) && (dtype != NCGtypeLine) && (dtype != NCGtypePolygon))
	{ fprintf (stderr,"Invalid point data in: NCGdsHandleVectorDefine ()\n");      return (NCGfailed); }
	if (NCGdsHandleGeoDefine ((NCGdsHandleGeo_t *) vector, &ncid, 1) == NCGfailed) return (NCGfailed);
	return (NCGsucceeded);
}

void NCGdsHandleVectorClear (NCGdsHandleVector_t *vector)
{
	NCGdsHandleGeoClear ((NCGdsHandleGeo_t *) vector);
}
