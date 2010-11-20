#include<NCdsHandle.h>

int NCGridGetPointVector (NCdsHandleGCont_t *gCont, NCreference_t *ref,double *vector)
{
	return (NCsucceeded);
}

/*
NCObjTable_t *NCGridContPointSampling (NCObjData_t *objGrid, NCObjData_t *objPoint, NCtableLayout layout)
{
	int i;
	NCdsHandleGCont_t    *gCont;
	NCdsHandleVPoint_t *point;
	NCObjTable_t *result = (NCObjTable_t *) NULL;
	NCReference_t *refs;
	NCCoordinate_t coord;

	if ((gCont = NCdsHandleGContCreate    (objGrid))  == (NCdsHandleGCont_t *)    NULL) return (NCObjTable_t *) NULL;
	if ((point = NCdsHandleVPointCreate (objPoint)) == (NCdsHandleVPoint_t *) NULL) return (NCObjTable_t *) NULL;

	refs = (NCReference_t *) calloc (point->Vector.ItemNum,sizeof (NCReference_t *));
	if (refs == (NCReference_t *) NULL)
	{
		perror ("Memory allocation error in: NCGridPointSampling ()");
		return (NCObjTable_t *) NULL;
	}
	for (i = 0;i < point->Vector.ItemNum;i++) NCReferenceInitialize (refs + i);
	for (i = 0;i < point->Vector.ItemNum;i++)
	{
		coord.X = point->XCoords [i];
		coord.Y = point->YCoords [i];
		if (NCdsHandleGContReference (gCont,&coord,refs + i) == NCfailed)
		{
			for (i = i - 1;i >= 0;i--) NCReferenceClear (refs + i);
			return (NCObjTable_t *) NULL;
		}
	}
	NCdsHandleGContFree    (gCont);
	NCdsHandleVPointFree (point);
	return (result);
}
*/

NCstate NCGridContSampling (int inNC, int outNC)
{
	size_t tStep, level, row, col, refNum = 0, ref;
	double val;
	NCcoordinate_t coord;
	NCdsHandleGCont_t *inGrid = (NCdsHandleGCont_t *) NULL, *outGrid = (NCdsHandleGCont_t *) NULL;
	NCreference_t *gRefs = (NCreference_t *) NULL;

	if (((inGrid  = (NCdsHandleGCont_t *) NCdsHandleOpenById (inNC))  == (NCdsHandleGCont_t *) NULL) ||
	    ((outGrid = (NCdsHandleGCont_t *) NCdsHandleOpenById (outNC)) == (NCdsHandleGCont_t *) NULL)) goto ABORT;

	gRefs = (NCreference_t *) calloc (outGrid->RowNum * outGrid->ColNum, sizeof (NCreference_t));
	if (gRefs == (NCreference_t *) NULL)
	{ perror ("Memory allocation error in: NCGridSampling ()"); goto ABORT; }

	for (row = 0;row < outGrid->RowNum;row++)
	{
		coord.Y = outGrid->YCoords [row];
		for (col = 0;col < outGrid->ColNum;col++)
		{
			coord.X = outGrid->XCoords [col];
			refNum = row * outGrid->ColNum + col;
			if (NCdsHandleGContReference (inGrid,&coord, gRefs + refNum) == NCfailed) goto ABORT;
		}
	}
	if ((outGrid->TNum < inGrid->TNum) && ((outGrid->Times = (double *) realloc (outGrid->Times,inGrid->TNum * sizeof (double))) == (double *) NULL))
	{ perror ("Memory allocation error in: NCGridSampling ()"); goto ABORT; }

	outGrid->TNum = inGrid->TNum;
	if (((outGrid->NCindex  = (size_t *) realloc (outGrid->NCindex,  outGrid->TNum * sizeof (size_t))) == (size_t *) NULL) ||
	    ((outGrid->NCoffset = (size_t *) realloc (outGrid->NCoffset, outGrid->TNum * sizeof (size_t))) == (size_t *) NULL))
	{ perror ("Memory allocation error in: NCGridSampling ()"); goto ABORT; }
	for (tStep = 0;tStep < inGrid->TNum; tStep++)
		outGrid->NCindex [tStep] = outGrid->NCoffset [tStep] = 0;

	for (tStep = 0;tStep < inGrid->TNum; tStep++)
	{
		if (outGrid->Times != (double *) NULL) outGrid->Times [tStep] = inGrid->Times [tStep];
		for (level = 0;level < inGrid->LNum;level++)
		{
			if (outGrid->Levels != (double *) NULL) outGrid->Levels [level] = inGrid->Levels [level];
			if (NCdsHandleGContLoadCache (inGrid, tStep, 1, level, 1) == NCfailed) goto ABORT;

			for (row = 0;row < outGrid->RowNum;row++)
				for (col = 0;col < outGrid->ColNum;col++)
				{
				ref = row * outGrid->ColNum + col;
				if (NCdsHandleGContGetFloat  (inGrid,  gRefs + ref, &val))
					  NCdsHandleGContSetFloat (outGrid, row, col, val);
				else NCdsHandleGContSetFill  (outGrid, row, col);
				}
			if (NCdsHandleGContSaveCache (outGrid, tStep, level) == NCfailed) goto ABORT;
		}
	}
	for (ref = 0;ref < refNum;ref++) NCreferenceClear (gRefs + ref);
	free (gRefs);
	NCdsHandleClose ((NCdsHandle_t *) inGrid);
	NCdsHandleClose ((NCdsHandle_t *) outGrid);
	return (NCsucceeded);
ABORT:
	for (ref = 0;ref < refNum;ref++) NCreferenceClear (gRefs + ref);
	free (gRefs);
	NCdsHandleClose ((NCdsHandle_t *) inGrid);
	NCdsHandleClose ((NCdsHandle_t *) outGrid);
	return (NCsucceeded);
}
