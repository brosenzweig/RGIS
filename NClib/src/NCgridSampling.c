#include<NCdsHandle.h>

int NCGGridGetPointVector (NCGdsHandleGCont_t *gCont, NCGreference_t *ref,double *vector)
{
	return (NCGsucceeded);
}

/*
NCGObjTable_t *NCGGridContPointSampling (NCGObjData_t *objGrid, NCGObjData_t *objPoint, NCGtableLayout layout)
{
	int i;
	NCGdsHandleGCont_t    *gCont;
	NCGdsHandleVPoint_t *point;
	NCGObjTable_t *result = (NCGObjTable_t *) NULL;
	NCGReference_t *refs;
	NCGCoordinate_t coord;

	if ((gCont = NCGdsHandleGContCreate    (objGrid))  == (NCGdsHandleGCont_t *)    NULL) return (NCGObjTable_t *) NULL;
	if ((point = NCGdsHandleVPointCreate (objPoint)) == (NCGdsHandleVPoint_t *) NULL) return (NCGObjTable_t *) NULL;

	refs = (NCGReference_t *) calloc (point->Vector.ItemNum,sizeof (NCGReference_t *));
	if (refs == (NCGReference_t *) NULL)
	{
		perror ("Memory allocation error in: NCGGridPointSampling ()");
		return (NCGObjTable_t *) NULL;
	}
	for (i = 0;i < point->Vector.ItemNum;i++) NCGReferenceInitialize (refs + i);
	for (i = 0;i < point->Vector.ItemNum;i++)
	{
		coord.X = point->XCoords [i];
		coord.Y = point->YCoords [i];
		if (NCGdsHandleGContReference (gCont,&coord,refs + i) == NCGfailed)
		{
			for (i = i - 1;i >= 0;i--) NCGReferenceClear (refs + i);
			return (NCGObjTable_t *) NULL;
		}
	}
	NCGdsHandleGContFree    (gCont);
	NCGdsHandleVPointFree (point);
	return (result);
}
*/

NCGstate NCGGridContSampling (int inNC, int outNC)
{
	size_t tStep, level, row, col, refNum = 0, ref;
	double val;
	NCGcoordinate_t coord;
	NCGdsHandleGCont_t *inGrid = (NCGdsHandleGCont_t *) NULL, *outGrid = (NCGdsHandleGCont_t *) NULL;
	NCGreference_t *gRefs = (NCGreference_t *) NULL;

	if (((inGrid  = (NCGdsHandleGCont_t *) NCGdsHandleOpenById (inNC))  == (NCGdsHandleGCont_t *) NULL) ||
	    ((outGrid = (NCGdsHandleGCont_t *) NCGdsHandleOpenById (outNC)) == (NCGdsHandleGCont_t *) NULL)) goto ABORT;

	gRefs = (NCGreference_t *) calloc (outGrid->RowNum * outGrid->ColNum, sizeof (NCGreference_t));
	if (gRefs == (NCGreference_t *) NULL)
	{ perror ("Memory allocation error in: NCGGridSampling ()"); goto ABORT; }

	for (row = 0;row < outGrid->RowNum;row++)
	{
		coord.Y = outGrid->YCoords [row];
		for (col = 0;col < outGrid->ColNum;col++)
		{
			coord.X = outGrid->XCoords [col];
			refNum = row * outGrid->ColNum + col;
			if (NCGdsHandleGContReference (inGrid,&coord, gRefs + refNum) == NCGfailed) goto ABORT;
		}
	}
	if ((outGrid->TNum < inGrid->TNum) && ((outGrid->Times = (double *) realloc (outGrid->Times,inGrid->TNum * sizeof (double))) == (double *) NULL))
	{ perror ("Memory allocation error in: NCGGridSampling ()"); goto ABORT; }

	outGrid->TNum = inGrid->TNum;
	if (((outGrid->NCindex  = (size_t *) realloc (outGrid->NCindex,  outGrid->TNum * sizeof (size_t))) == (size_t *) NULL) ||
	    ((outGrid->NCoffset = (size_t *) realloc (outGrid->NCoffset, outGrid->TNum * sizeof (size_t))) == (size_t *) NULL))
	{ perror ("Memory allocation error in: NCGGridSampling ()"); goto ABORT; }
	for (tStep = 0;tStep < inGrid->TNum; tStep++)
		outGrid->NCindex [tStep] = outGrid->NCoffset [tStep] = 0;

	for (tStep = 0;tStep < inGrid->TNum; tStep++)
	{
		if (outGrid->Times != (double *) NULL) outGrid->Times [tStep] = inGrid->Times [tStep];
		for (level = 0;level < inGrid->LNum;level++)
		{
			if (outGrid->Levels != (double *) NULL) outGrid->Levels [level] = inGrid->Levels [level];
			if (NCGdsHandleGContLoadCache (inGrid, tStep, 1, level, 1) == NCGfailed) goto ABORT;

			for (row = 0;row < outGrid->RowNum;row++)
				for (col = 0;col < outGrid->ColNum;col++)
				{
				ref = row * outGrid->ColNum + col;
				if (NCGdsHandleGContGetFloat  (inGrid,  gRefs + ref, &val))
					  NCGdsHandleGContSetFloat (outGrid, row, col, val);
				else NCGdsHandleGContSetFill  (outGrid, row, col);
				}
			if (NCGdsHandleGContSaveCache (outGrid, tStep, level) == NCGfailed) goto ABORT;
		}
	}
	for (ref = 0;ref < refNum;ref++) NCGreferenceClear (gRefs + ref);
	free (gRefs);
	NCGdsHandleClose ((NCGdsHandle_t *) inGrid);
	NCGdsHandleClose ((NCGdsHandle_t *) outGrid);
	return (NCGsucceeded);
ABORT:
	for (ref = 0;ref < refNum;ref++) NCGreferenceClear (gRefs + ref);
	free (gRefs);
	NCGdsHandleClose ((NCGdsHandle_t *) inGrid);
	NCGdsHandleClose ((NCGdsHandle_t *) outGrid);
	return (NCGsucceeded);
}
