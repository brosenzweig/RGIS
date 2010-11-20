#include<NCdsHandle.h>
#include<NCmath.h>

NCGstate NCGdsHandleVPointDefine (NCGdsHandleVPoint_t *point, int ncid)
{
	if (NCGdataGetType (ncid) != NCGtypePoint) 
	{
		fprintf (stderr,"Invalid point data in: NCGdsHandleVPointCreate ()\n");
		return (NCGfailed);
	}
	if (NCGdsHandleVectorDefine ((NCGdsHandleVector_t *) point, ncid) == NCGfailed)
	{
		NCGdsHandleVectorClear ((NCGdsHandleVector_t *) point);
		return (NCGfailed);
	}

	point->XCoords = point->YCoords = (double *) NULL;
	if ((point->XCoords = (double *) calloc (point->ItemNum,sizeof (double))) == (double *) NULL)
	{
		perror ("Memory allocation error in: NCGdsHandleVPointCreate ()");
		NCGdsHandleVectorClear ((NCGdsHandleVector_t *) point);
		return (NCGfailed);
	}

	if ((point->YCoords = (double *) calloc (point->ItemNum,sizeof (double))) == (double *) NULL)
	{
		perror ("Memory allocation error in: NCGdsHandleVPointCreate ()");
		free (point->XCoords);
		NCGdsHandleVectorClear ((NCGdsHandleVector_t *) point);
		return (NCGfailed);
	}
	return (NCGsucceeded);
}

void NCGdsHandleVPointClear (NCGdsHandleVPoint_t *point)
{
	if (point->XCoords != (double *) NULL) free (point->XCoords);
	if (point->YCoords != (double *) NULL) free (point->YCoords);
}

#define NCGVPointNeighborNum 6

int NCGdsHandleVPointReference (const NCGdsHandleVPoint_t *point, const NCGcoordinate_t *coord, NCGreference_t *ref)
{
	int idx [NCGVPointNeighborNum], i, k, l, num;
	double minDist [NCGVPointNeighborNum], dist;
	NCGcoordinate_t pCoord;

	num = 0;
	for (i = 0;i < point->ItemNum;i++)
	{
		pCoord.X = point->XCoords [i];
		pCoord.Y = point->YCoords [i];
		if (NCGmathEqualValues (pCoord.X,coord->X) && NCGmathEqualValues (pCoord.Y,coord->Y))
		{
			num = 1;
			idx [0] = i;
			break;
		}
		dist = NCGmathCoordinateDistance (point->Projection, &pCoord, coord);
		if (num < NCGVPointNeighborNum) num++;
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
		perror ("Memory allocation error in: NCGVPointReference ()");
		return (NCGfailed);
	}
	for (k = 0;k < num;k++) { ref->Idx [k] = idx [k]; }
	if (num > 1)
	{
		if ((ref->Weight = (double *) calloc (num,sizeof (double))) == (double *) NULL)
		{
			perror ("Memory allocation error in: NCGVPointReference ()");
			free (ref->Idx);
			NCGreferenceInitialize (ref);
			return (NCGfailed);
		}
		for (k = 0;k < num;k++) ref->Weight [k] = minDist [k];
	}
	ref->Num = num;
	return (NCGsucceeded);
}
