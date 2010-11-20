#include<NCdsHandle.h>
#include<NCmath.h>

NCGstate NCGdsHandleGDiscDefine (NCGdsHandleGDisc_t *gDisc, int *ncids, size_t n)
{
	int status, varid;

	if (n < 1) return (NCGfailed);
	if (NCGdsHandleGridDefine ((NCGdsHandleGrid_t *) gDisc, ncids, n) == NCGfailed) return (NCGfailed);

	if (gDisc->DataType != NCGtypeGDisc)
	{
		fprintf (stderr,"Invalid grid data in: NCGdsHandleGDiscDefine ()\n");
		NCGdsHandleGridClear ((NCGdsHandleGrid_t *) gDisc);
		return (NCGfailed);
	}
	return (NCGsucceeded);
}

void NCGdsHandleGDiscClear (NCGdsHandleGDisc_t *gDisc)
{
	NCGdsHandleGridClear ((NCGdsHandleGrid_t *) gDisc);
}
