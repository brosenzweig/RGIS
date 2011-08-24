#include <string.h>
#include <cm.h>
#include <NCdsHandle.h>
#include <NCmath.h>

NCstate NCdsHandleGDiscDefine (NCdsHandleGDisc_t *gDisc, int *ncids, size_t n)
{
	int status, varid;

	if (n < 1) return (NCfailed);
	if (NCdsHandleGridDefine ((NCdsHandleGrid_t *) gDisc, ncids, n) == NCfailed) return (NCfailed);

	if (gDisc->DataType != NCtypeGDisc)
	{
		CMmsgPrint (CMmsgAppError, "Invalid grid data in: %s %d",__FILE__,__LINE__);
		NCdsHandleGridClear ((NCdsHandleGrid_t *) gDisc);
		return (NCfailed);
	}
	return (NCsucceeded);
}

void NCdsHandleGDiscClear (NCdsHandleGDisc_t *gDisc)
{
	NCdsHandleGridClear ((NCdsHandleGrid_t *) gDisc);
}
