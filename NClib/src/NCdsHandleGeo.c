#include<NCdsHandle.h>

NCGstate NCGdsHandleGeoDefine (NCGdsHandleGeo_t *dsHandleGeo, int *ncids, size_t n)
{
	NCGprojection proj;

	if (n < 1) return (NCGfailed);
	if (NCGdsHandleDefine ((NCGdsHandle_t *) dsHandleGeo, ncids, n) == NCGfailed) return (NCGfailed);
	if ((proj = NCGdataGetProjection (ncids [0])) == NCGundefined)
	{ NCGdsHandleClear ((NCGdsHandle_t *) dsHandleGeo); return (NCGfailed); }

	dsHandleGeo->Projection = proj;
	dsHandleGeo->Extent.LowerLeft.X  = dsHandleGeo->Extent.LowerLeft.Y  =  HUGE_VAL;
	dsHandleGeo->Extent.UpperRight.X = dsHandleGeo->Extent.UpperRight.Y = -HUGE_VAL;
	return (NCGsucceeded);
}

void NCGdsHandleGeoClear (NCGdsHandleGeo_t *dsHandleGeo)
{
	NCGdsHandleClear ((NCGdsHandle_t *) dsHandleGeo);
	dsHandleGeo->Projection = NCGundefined;
	dsHandleGeo->Extent.LowerLeft.X  = dsHandleGeo->Extent.LowerLeft.Y  =  HUGE_VAL;
	dsHandleGeo->Extent.UpperRight.X = dsHandleGeo->Extent.UpperRight.Y = -HUGE_VAL;
}
