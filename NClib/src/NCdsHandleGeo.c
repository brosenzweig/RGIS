#include <cm.h>
#include <NCdsHandle.h>

NCstate NCdsHandleGeoDefine (NCdsHandleGeo_t *dsHandleGeo, int *ncids, size_t n)
{
	NCprojection proj;

	if (n < 1) return (NCfailed);
	if (NCdsHandleDefine ((NCdsHandle_t *) dsHandleGeo, ncids, n) == NCfailed) return (NCfailed);
	if ((proj = NCdataGetProjection (ncids [0])) == NCundefined)
	{ NCdsHandleClear ((NCdsHandle_t *) dsHandleGeo); return (NCfailed); }

	dsHandleGeo->Projection = proj;
	dsHandleGeo->Extent.LowerLeft.X  = dsHandleGeo->Extent.LowerLeft.Y  =  HUGE_VAL;
	dsHandleGeo->Extent.UpperRight.X = dsHandleGeo->Extent.UpperRight.Y = -HUGE_VAL;
	return (NCsucceeded);
}

void NCdsHandleGeoClear (NCdsHandleGeo_t *dsHandleGeo)
{
	NCdsHandleClear ((NCdsHandle_t *) dsHandleGeo);
	dsHandleGeo->Projection = NCundefined;
	dsHandleGeo->Extent.LowerLeft.X  = dsHandleGeo->Extent.LowerLeft.Y  =  HUGE_VAL;
	dsHandleGeo->Extent.UpperRight.X = dsHandleGeo->Extent.UpperRight.Y = -HUGE_VAL;
}
