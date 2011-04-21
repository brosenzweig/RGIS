#include <cm.h>
#include <NCdsHandle.h>
#include <NCmath.h>

NCstate NCdsHandleVPointDefine (NCdsHandleVPoint_t *point, int ncid)
{
	if (NCdataGetType (ncid) != NCtypePoint) 
	{
		CMmsgPrint (CMmsgAppError, "Invalid point data in: %s %d",__FILE__,__LINE__);
		return (NCfailed);
	}
	if (NCdsHandleVectorDefine ((NCdsHandleVector_t *) point, ncid) == NCfailed)
	{
		NCdsHandleVectorClear ((NCdsHandleVector_t *) point);
		return (NCfailed);
	}

	point->XCoords = point->YCoords = (double *) NULL;
	if ((point->XCoords = (double *) calloc (point->ItemNum,sizeof (double))) == (double *) NULL)
	{
		CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__);
		NCdsHandleVectorClear ((NCdsHandleVector_t *) point);
		return (NCfailed);
	}

	if ((point->YCoords = (double *) calloc (point->ItemNum,sizeof (double))) == (double *) NULL)
	{
		CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__);
		free (point->XCoords);
		NCdsHandleVectorClear ((NCdsHandleVector_t *) point);
		return (NCfailed);
	}
	return (NCsucceeded);
}

void NCdsHandleVPointClear (NCdsHandleVPoint_t *point)
{
	if (point->XCoords != (double *) NULL) free (point->XCoords);
	if (point->YCoords != (double *) NULL) free (point->YCoords);
}

#define NCVPointNeighborNum 6

int NCdsHandleVPointReference (const NCdsHandleVPoint_t *point, const NCcoordinate_t *coord, NCreference_t *ref)
{
	int idx [NCVPointNeighborNum], i, k, l, num;
	double minDist [NCVPointNeighborNum], dist;
	NCcoordinate_t pCoord;

	num = 0;
	for (i = 0;i < point->ItemNum;i++)
	{
		pCoord.X = point->XCoords [i];
		pCoord.Y = point->YCoords [i];
		if (NCmathEqualValues (pCoord.X,coord->X) && NCmathEqualValues (pCoord.Y,coord->Y))
		{
			num = 1;
			idx [0] = i;
			break;
		}
		dist = NCmathCoordinateDistance (point->Projection, &pCoord, coord);
		if (num < NCVPointNeighborNum) num++;
		for (k = 0;k < num;k++)
			if (minDist [k] > dist)
			{ 
				for (l = num - 1;l > k;l--)
				{
					minDist  [l] = minDist [l - 1];
					idx [l] = idx [l - 1];
				}
				minDist  [k] = dist;
				idx [k] = i;
				break;
			}
	}
	if ((ref->Idx = (int *) calloc (num,sizeof (int))) == (int *) NULL)
	{
		CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__);
		return (NCfailed);
	}
	for (k = 0;k < num;k++) { ref->Idx [k] = idx [k]; }
	if (num > 1)
	{
		if ((ref->Weight = (double *) calloc (num,sizeof (double))) == (double *) NULL)
		{
			CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__);
			free (ref->Idx);
			NCreferenceInitialize (ref);
			return (NCfailed);
		}
		for (k = 0;k < num;k++) ref->Weight [k] = minDist [k];
	}
	ref->Num = num;
	return (NCsucceeded);
}
