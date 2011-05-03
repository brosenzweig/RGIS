#include <cm.h>
#include <NCdsHandle.h>

NCstate NCdsHandleVectorDefine (NCdsHandleVector_t *vector, int ncid)
{
	NCdataType dtype;
//	char *xNames [] = { NCnameDNXCoord, NCnameDNLon, NCnameDNLongitude };
//	char *yNames [] = { NCnameDNYCoord, NCnameDNLat, NCnameDNLatitude };

	vector->ItemNum = 0;
	dtype = NCdataGetType (ncid);
	if ((dtype != NCtypePoint) && (dtype != NCtypeLine) && (dtype != NCtypePolygon))
	{ CMmsgPrint (CMmsgAppError, "Invalid point data in: %s %d",__FILE__,__LINE__);      return (NCfailed); }
	if (NCdsHandleGeoDefine ((NCdsHandleGeo_t *) vector, &ncid, 1) == NCfailed) return (NCfailed);
	return (NCsucceeded);
}

void NCdsHandleVectorClear (NCdsHandleVector_t *vector)
{
	NCdsHandleGeoClear ((NCdsHandleGeo_t *) vector);
}
