#include<NCdsHandle.h>
#include<NCmath.h>

NCGstate NCGdsHandleGContDefine (NCGdsHandleGCont_t *gCont, int *ncids, size_t n)
{
	int status, varid, i;
	int missingInt, fillInt;
	double missingFloat, fillFloat, scale, offset;
	size_t unitlen, row, col;
	char   unitstr [NC_MAX_NAME], gunitstr [NC_MAX_NAME];

	gCont->Data       = (double *) NULL;
	gCont->AuxData    = (double *) NULL;
	gCont->ObsNum     = (size_t *) NULL;
	gCont->MeanIds    = gCont->MinIds = gCont->MaxIds = gCont->StdIds = (int *) NULL;

	if (n < 1) return (NCGfailed);
	if (NCGdsHandleGridDefine ((NCGdsHandleGrid_t *) gCont, ncids, n) == NCGfailed) return (NCGfailed);

	if (gCont->DataType != NCGtypeGCont)
	{
		fprintf (stderr,"Invalid grid data in: NCGdsHandleGContDefine ()\n");
		NCGdsHandleGridClear ((NCGdsHandleGrid_t *) gCont);
		return (NCGfailed);
	}
	if (((gCont->MeanIds = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((gCont->MinIds  = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((gCont->MaxIds  = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((gCont->StdIds  = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((gCont->Data    = (double *) calloc (gCont->ColNum * gCont->RowNum,sizeof (double))) == (double *) NULL) ||
	    ((gCont->AuxData = (double *) calloc (gCont->ColNum * gCont->RowNum,sizeof (double))) == (double *) NULL) ||
	    ((gCont->ObsNum  = (size_t *) calloc (gCont->ColNum * gCont->RowNum,sizeof (size_t))) == (size_t *) NULL))
	{
		perror ("Memory allocation error in: NCGdsHandleGContDefine ()");
		if (gCont->MeanIds != (int *) NULL) free (gCont->MeanIds);
		if (gCont->MinIds  != (int *) NULL) free (gCont->MinIds);
		if (gCont->MaxIds  != (int *) NULL) free (gCont->MaxIds);
		if (gCont->StdIds  != (int *) NULL) free (gCont->StdIds);
		NCGdsHandleGridClear ((NCGdsHandleGrid_t *) gCont);
		return (NCGfailed);
	}
	for (i = 0;i < n; ++i) gCont->MeanIds [i] = gCont->MinIds [i] = gCont->MaxIds [i] = gCont->StdIds [i] = NCGundefined;

	for (i = 0;i < n; ++i)
	{
		switch (gCont->GType)
		{
			default: gCont->FillValue.Float = gCont->MissingVal.Float = FLOAT_NOVALUE; break;
			case NC_BYTE:
			case NC_SHORT:
			case NC_INT:
				if ((status = nc_get_att_int (gCont->NCIds [i],gCont->GVarIds [i],NCGnameVAFillValue,  &fillInt))   != NC_NOERR)
				{
					if ((status = nc_get_att_int (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAMissingVal, &missingInt)) != NC_NOERR)
						missingInt = fillInt  = INT_NOVALUE;
					else fillInt = missingInt;
				}
				else if ((status = nc_get_att_int (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAMissingVal, &missingInt))  != NC_NOERR)
					missingInt = fillInt;
				break;
			case NC_FLOAT:
			case NC_DOUBLE:
				if ((status = nc_get_att_double (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAFillValue, &fillFloat))   != NC_NOERR)
				{
					if ((status = nc_get_att_double (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAMissingVal, &missingFloat)) != NC_NOERR)
						missingFloat = fillFloat  = FLOAT_NOVALUE;
					else
						fillFloat  = missingFloat;
				}
				else if ((status = nc_get_att_double (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAMissingVal, &missingFloat))  != NC_NOERR)
					missingFloat = fillFloat;
				break;
		}
		if ((status = nc_get_att_double (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAScaleFactor,&scale))  != NC_NOERR) scale  = 1.0;
		if ((status = nc_get_att_double (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAAddOffset,  &offset)) != NC_NOERR) offset = 0.0;
		if (((status = nc_inq_attlen   (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAUnits,&unitlen)) == NC_NOERR) &&
		    ((status = nc_get_att_text (gCont->NCIds [i], gCont->GVarIds [i], NCGnameVAUnits, unitstr)) == NC_NOERR)) unitstr [unitlen] = '\0';
		if (i == 0)
		{
		 	if ((status == NC_NOERR) && (utScan (unitstr,&(gCont->GUnit)) == 0))
			{ gCont->DoGUnit  = true; strcpy (gunitstr,unitstr); }
			else gCont->DoGUnit = false;
			switch (gCont->GType)
			{
				case NC_BYTE:
				case NC_SHORT:
				case NC_INT:    gCont->FillValue.Int   = fillInt;   gCont->MissingVal.Int   = missingInt;   break;
				default:
				case NC_FLOAT:
				case NC_DOUBLE: gCont->FillValue.Float = fillFloat; gCont->MissingVal.Float = missingFloat; break;
			}
			gCont->Scale  = scale;
			gCont->Offset = offset;
		}
		else
		{
			if (gCont->DoGUnit && ((status != NC_NOERR) || strcmp (unitstr,gunitstr) != 0))
			{ fprintf (stderr,"Inconsistent data bundle (units) in: _NCGdsHandleGetGContDefine ()\n"); return (NCGfailed); }
			switch (gCont->GType)
			{
				case NC_BYTE:
				case NC_SHORT:
				case NC_INT:
					if ((gCont->FillValue.Int  != fillInt) || (gCont->MissingVal.Int != missingInt))
					{ fprintf (stderr,"Inconsistent data bundle (int fill value) in: _NCGdsHandleGetGContDefine ()\n"); return (NCGfailed); }
					break;
				default:
				case NC_FLOAT:
				case NC_DOUBLE:
					if ((NCGmathEqualValues (gCont->FillValue.Float,  fillFloat)    == false) ||
					    (NCGmathEqualValues (gCont->MissingVal.Float, missingFloat) == false))
					{ fprintf (stderr,"Inconsistent bundle (float fill value) in: _NCGdsHandleGetGContDefine ()\n"); return (NCGfailed); }
					break;
			}
			if ((NCGmathEqualValues (gCont->Scale,  scale)  == false) ||
		       (NCGmathEqualValues (gCont->Offset, offset) == false))
			{ fprintf (stderr,"Inconsistent bundle (scale and offset) in: _NCGdsHandleGetGContDefine ()\n"); return (NCGfailed); }
		}
		gCont->MeanIds [i] = nc_inq_varid (ncids [i], NCGnameVAAverage, &varid) == NC_NOERR ? varid : NCGundefined;
		gCont->MaxIds  [i] = nc_inq_varid (ncids [i], NCGnameVAMaximum, &varid) == NC_NOERR ? varid : NCGundefined;
		gCont->MinIds  [i] = nc_inq_varid (ncids [i], NCGnameVAMinimum, &varid) == NC_NOERR ? varid : NCGundefined;
		gCont->StdIds  [i] = nc_inq_varid (ncids [i], NCGnameVAStdDev,  &varid) == NC_NOERR ? varid : NCGundefined;
	}
	for (row = 0;row < gCont->RowNum;row++) for (col = 0;col < gCont->ColNum;col++) NCGdsHandleGContSetFill (gCont,row,col);
	return (NCGsucceeded);
}

void NCGdsHandleGContClear (NCGdsHandleGCont_t *gCont)
{
	NCGdsHandleGridClear ((NCGdsHandleGrid_t *) gCont);
	if (gCont->Data    != (double *) NULL) free (gCont->Data);
	if (gCont->AuxData != (double *) NULL) free (gCont->AuxData);
	if (gCont->ObsNum  != (size_t *) NULL) free (gCont->ObsNum);
	gCont->Data     = (double *) NULL;
	gCont->AuxData  = (double *) NULL;
	gCont->ObsNum   = (size_t *) NULL;
}

#define NCGGridNeighborNum 4

NCGstate NCGdsHandleGContReference (const NCGdsHandleGCont_t *gCont, const NCGcoordinate_t *coordIn, NCGreference_t *ref)
{
	int i, j, k, l, num;
	double maxDist, minDist [NCGGridNeighborNum], dist;
	int  row, col, sRow, sCol, rows [NCGGridNeighborNum], cols [NCGGridNeighborNum];
	NCGcoordinate_t gCoord, coord;

	coord.X = coordIn->X;
	coord.Y = coordIn->Y;
	if ((gCont->Projection == NCGprojSpherical) &&
		 (gCont->XCoords [0] >= 0.0) &&
		 (gCont->XCoords [gCont->ColNum - 1] > 180.0) &&
		 (coord.X < 0.0)) coord.X = coord.X + 360.0;

	ref->Num    = 0;
	ref->Idx    = (int *)    NULL;
	ref->Weight = (double *) NULL;

	if (gCont->XCoords [0] < gCont->XCoords [gCont->ColNum - 1])
	{ for (col = 0;col < gCont->ColNum;col++) if (gCont->XCoords [col] > coord.X) break; }
	else
	{ for (col = 0;col < gCont->ColNum;col++) if (gCont->XCoords [col] < coord.X) break; }

	if (gCont->YCoords [0] < gCont->YCoords [gCont->RowNum - 1])
	{ for (row = 0;row < gCont->RowNum;row++) if (gCont->YCoords [row] > coord.Y) break; }
	else
	{ for (row = 0;row < gCont->RowNum;row++) if (gCont->YCoords [row] < coord.Y) break; }

	num = 0;
	maxDist = 0.0;
	for (j = -2;j <= 3;j++) for (i = -2;i <= 3; i++)
	{
		sCol = col + i;
		sRow = row + j;
		if (gCont->Projection == NCGprojSpherical)
		{
			if (sCol >= (int) gCont->ColNum) sCol = sCol % gCont->ColNum;
			else if (sCol < 0)               sCol = sCol + gCont->ColNum;
		}
		if ((sCol < 0) || (sCol >= gCont->ColNum) || (sRow < 0) || (sRow >= gCont->RowNum)) continue;
		gCoord.X = gCont->XCoords [sCol];
		gCoord.Y = gCont->YCoords [sRow];
		dist = NCGmathCoordinateDistance (gCont->Projection, &gCoord, &coord);
		if (dist > maxDist) maxDist = dist;
		for (k = 0; k < num; k++)
			if (minDist [k] > dist)
			{ 
				if (num < NCGGridNeighborNum) num++;
				for (l = num - 1;l > k;l--)
				{
					minDist [l] = minDist [l - 1];
					cols    [l] = cols    [l - 1];
					rows    [l] = rows    [l - 1];
				}
				minDist  [k] = dist;
				cols     [k] = sCol;
				rows     [k] = sRow;
				break;
			}
		if ((k == num) && (num < NCGGridNeighborNum))
		{	
			minDist  [num] = dist;
			cols     [num] = sCol;
			rows     [num] = sRow;
			num++;
		}
	}
	if ((ref->Idx = (int *) calloc (num * 2,sizeof (int))) == (int *) NULL)
	{
		perror ("Memory allocation error in: NCGdsHandleGContReference ()");
		return (NCGfailed);
	}
	if ((ref->Weight = (double *) calloc (num,sizeof (double))) == (double *) NULL)
	{
		perror ("Memory allocation error in: NCGdsHandleGContReference ()");
		free (ref->Idx);
		NCGreferenceInitialize (ref);
		return (NCGfailed);
	}
	ref->Num = num;
	if (num > 1.0)
		for (k = 0;k < num;k++)
		{
			ref->Idx [k * 2]     = cols [k];
			ref->Idx [k * 2 + 1] = rows [k];
			ref->Weight [k]      = minDist [k];
		}
	else minDist [0] = 0.0;
	return (NCGsucceeded);
}

static bool _NCGdsHandleGContTestNodata (const NCGdsHandleGCont_t *gCont, double val)
{
	switch (gCont->GType)
	{
		default: fprintf (stderr,"Invalid data NetCDF type in: _NCGdsHandleGContTestNodata ()\n"); break;
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    return ((gCont->MissingVal.Int == (int) val) || (gCont->FillValue.Int == (int) val) ? true : false);
		case NC_FLOAT:
		case NC_DOUBLE: return (NCGmathEqualValues (gCont->MissingVal.Float, val) || NCGmathEqualValues (gCont->FillValue.Float,  val) ? true : false);
	}
	return (true);
}

NCGstate NCGdsHandleGContLoadCache (NCGdsHandleGCont_t *gCont,size_t sTStep, size_t nTSteps, size_t sLevel, size_t nLevels)
{
	int status, start [4], count [4];
	size_t ncidx, tStep, level, row, col;
	double val;

	count [0] = 1;
	if (gCont->LVarIds [ncidx] == NCGundefined)
	{
		start [1]   = (size_t) 0; count [1] = gCont->RowNum;
		start [2]   = (size_t) 0; count [2] = gCont->ColNum;
	}
	else
	{
		count [1] = 1; 
		start [2]   = (size_t) 0; count [2] = gCont->RowNum;
		start [3]   = (size_t) 0; count [3] = gCont->ColNum;
	}
	for (row = 0;row < gCont->RowNum;row++)
		for (col = 0;col < gCont->ColNum;col++)
		{
			gCont->ObsNum [gCont->ColNum * row + col] = (size_t) 0;
			gCont->Data   [gCont->ColNum * row + col] = (double) 0.0;
		}
	for (tStep = sTStep;tStep < sTStep + nTSteps; tStep++)
		for (level = sLevel;level < sLevel + nLevels; level++)
		{
			ncidx = gCont->NCindex [tStep];
			start [0]   = tStep - gCont->NCoffset [ncidx];
			if (gCont->LVarIds [ncidx] != NCGundefined) start [1] = level;
			if ((status =  nc_sync (gCont->NCIds [ncidx])) != NC_NOERR)
			{ NCGprintNCError (status,"NCGdsHandleGContLoadCache"); return (NCGfailed); }
			if ((status = nc_get_vara_double (gCont->NCIds [ncidx], gCont->GVarIds [ncidx],start,count,gCont->AuxData)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGdsHandleGContLoadCache"); return (NCGfailed); }
			for (row = 0;row < gCont->RowNum;row++)
				for (col = 0;col < gCont->ColNum;col++)
					if (_NCGdsHandleGContTestNodata (gCont,(val = gCont->AuxData [gCont->ColNum * row + col])) == false)
					{
						gCont->Data   [gCont->ColNum * row + col] += val;
						gCont->ObsNum [gCont->ColNum * row + col] += 1;
					}
		}
	for (row = 0;row < gCont->RowNum;row++)
		for (col = 0;col < gCont->ColNum;col++)
			if (gCont->ObsNum [gCont->ColNum * row + col] == 0) NCGdsHandleGContSetFill (gCont, row, col);
			else if (gCont->ObsNum [gCont->ColNum * row + col] > 1) 
				gCont->Data   [gCont->ColNum * row + col] /= (double) gCont->ObsNum [gCont->ColNum * row + col];
	return (NCGsucceeded);
}

NCGstate NCGdsHandleGContSaveCache (NCGdsHandleGCont_t *gCont,size_t tStep, size_t level)
{
	int status, i = 0, start [4], count [4];
	size_t ncidx;

	if (NCGdsHandleGContCLStats (gCont,tStep,level) != NCGsucceeded) return (NCGfailed);
	ncidx = gCont->NCindex [tStep];
	start [i]   = tStep - gCont->NCoffset [ncidx];      count [i++] = 1;
	if (gCont->LVarIds [ncidx] != NCGundefined)
	{ start [i] = level;      count [i++] = 1; }
	start [i]   = (size_t) 0; count [i++] = gCont->RowNum;
	start [i]   = (size_t) 0; count [i++] = gCont->ColNum;

	if ((status = nc_put_vara_double (gCont->NCIds [ncidx], gCont->GVarIds [ncidx],start,count,gCont->Data)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGdsHandleGContSaveCache"); }
	if ((gCont->TVarIds [ncidx] != NCGundefined) &&
	    ((status = nc_put_var1_double (gCont->NCIds [ncidx], gCont->TVarIds [ncidx], start,     gCont->Times  + tStep)) != NC_NOERR))
	{ NCGprintNCError (status,"NCGdsHandleGContSaveCache"); }
	if ((gCont->LVarIds [ncidx] != NCGundefined) &&
	    ((status = nc_put_var1_double (gCont->NCIds [ncidx], gCont->LVarIds [ncidx], start + 1, gCont->Levels + level)) != NC_NOERR))
	{ NCGprintNCError (status,"NCGdsHandleGContSaveCache"); }
	if ((status =  nc_sync (gCont->NCIds [ncidx])) != NC_NOERR)
	{ NCGprintNCError (status,"NCGdsHandleGContLoadCache"); return (NCGfailed); }
	return (NCGsucceeded);
}

bool NCGdsHandleGContGetFloat (const NCGdsHandleGCont_t *gCont, NCGreference_t *ref, double *val)
{
	int i, obsNum = 0;
	double retVal, sumVal = 0.0, sumWeight = 0.0;

	if (ref->Num == 0) goto NOVALUE;

	retVal = gCont->Data [gCont->ColNum * ref->Idx [1] + ref->Idx [0]];
	if (_NCGdsHandleGContTestNodata (gCont,retVal) == false)
	{
		if (NCGmathEqualValues (ref->Weight [0],0.0)) { *val = retVal * gCont->Scale + gCont->Offset; return (true); }
		sumVal    = retVal / ref->Weight [0];
		sumWeight = 1.0 / ref->Weight [0];
		obsNum    = 1;
	}
	for (i = 1;i < ref->Num;i++)
	{
		retVal = gCont->Data [gCont->ColNum * ref->Idx [i * 2 + 1] + ref->Idx [i * 2]];
		if (_NCGdsHandleGContTestNodata (gCont, retVal)) continue;
		sumVal    += retVal / ref->Weight [i];
		sumWeight += 1.0 / ref->Weight [i];
		obsNum++;
	} 
	if (obsNum == 0) goto NOVALUE;
	*val = (sumVal / sumWeight) * gCont->Scale + gCont->Offset;
	return (true);

NOVALUE:
	*val = FLOAT_NOVALUE;
	return (false);
}

NCGstate NCGdsHandleGContSetFloat (NCGdsHandleGCont_t *gCont, size_t row,size_t col, double val)
{
	if (row > gCont->RowNum) return (NCGfailed);
	if (col > gCont->ColNum) return (NCGfailed);
	val = (val - gCont->Offset) / gCont->Scale;
	gCont->Data [gCont->ColNum * row + col] = val;
	return (NCGsucceeded);
}

NCGstate NCGdsHandleGContSetFill (NCGdsHandleGCont_t *gCont, size_t row,size_t col)
{
	if (row > gCont->RowNum) return (NCGfailed);
	if (col > gCont->ColNum) return (NCGfailed);
	switch (gCont->GType)
	{
		default: fprintf (stderr,"Invalide NetCDF type in NCGdsHandleGContSetFill ()\n"); return (NCGfailed);
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    gCont->Data [gCont->ColNum * row + col] = (double) gCont->FillValue.Int; break;
		case NC_FLOAT:
		case NC_DOUBLE: gCont->Data [gCont->ColNum * row + col] = gCont->FillValue.Float;        break;
	}
	return (NCGsucceeded);
}

NCGstate NCGdsHandleGContCLStats (NCGdsHandleGCont_t *gCont,size_t tStep, size_t level)
{
	int status;
	size_t row, col, obsNum = 0, index [2], ncidx;
	double val, weight = 1.0, sumWeight = 0.0;
	double min = HUGE_VAL, max = -HUGE_VAL, avg = 0.0, stdDev = 0.0;

	for (row = 0;row < gCont->RowNum;row++)
		for (col = 0;col < gCont->ColNum;col++)
		{
			val = gCont->Data [gCont->ColNum * row + col];
			if (_NCGdsHandleGContTestNodata (gCont,val)) continue;
			sumWeight += weight; 
			avg = avg + val * weight;
			min = min < val ? min : val;
			max = max > val ? max : val;
			stdDev = stdDev + val * val * weight;
			obsNum++;
		}
	if (obsNum > 0)
		{
		avg    = avg / sumWeight;
		stdDev = stdDev / sumWeight;
		stdDev = stdDev - avg * avg;
		stdDev = sqrt (stdDev);
		}
	else
		switch (gCont->GType)
		{
			default: fprintf (stderr,"Invalide NetCDF type in NCGdsHandleGContCLStats ()\n"); return (NCGfailed);
			case NC_BYTE:
			case NC_SHORT:
			case NC_INT:    avg = stdDev = min = max = (double) gCont->FillValue.Int;   break;
			case NC_FLOAT:
			case NC_DOUBLE: avg = stdDev = min = max =          gCont->FillValue.Float; break;
		}
	ncidx = gCont->NCindex [tStep];
	index [0] = tStep - gCont->NCoffset [ncidx];
	index [1] = level;
	if ((gCont->MeanIds [ncidx] != NCGundefined) &&
	    ((status = nc_put_var1_double (gCont->NCIds [ncidx], gCont->MeanIds [ncidx], index, &avg))    != NC_NOERR))
		NCGprintNCError (status,"NCGdsHandleGContCLStats");
	if ((gCont->MinIds  [ncidx] != NCGundefined) &&
	    ((status = nc_put_var1_double (gCont->NCIds [ncidx], gCont->MinIds  [ncidx], index, &min))    != NC_NOERR))
		NCGprintNCError (status,"NCGdsHandleGContCLStats");
	if ((gCont->MaxIds  [ncidx] != NCGundefined) &&
	    ((status = nc_put_var1_double (gCont->NCIds [ncidx], gCont->MaxIds  [ncidx], index, &max))    != NC_NOERR))
		NCGprintNCError (status,"NCGdsHandleGContCLStats");
	if ((gCont->StdIds  [ncidx] != NCGundefined) &&
	    ((status = nc_put_var1_double (gCont->NCIds [ncidx], gCont->StdIds  [ncidx], index, &stdDev)) != NC_NOERR))
		NCGprintNCError (status,"NCGdsHandleGContCLStats");
	return (NCGsucceeded);
}

NCGstate NCGdsHandleGContUpdateRanges  (const NCGdsHandleGCont_t *gCont)
{
	int status;
	double *minArray = (double *) NULL, *maxArray = (double *) NULL, *meanArray = (double *) NULL, meanRange [2], minRange [2], maxRange [2], varRange [2], glbRange [2];
	size_t i, offset, ncidx, level;
	size_t start [2], count [2];

	if ((gCont->MinIds == (int *) NULL) || (gCont->MaxIds == (int *) NULL))
	{ fprintf (stderr,"Missing minimum or maximum variables in:  _NCGdsHandleGContUpdateRanges ()\n"); goto ABORT; }

	if (((meanArray = (double *) calloc (gCont->TNum, sizeof (double))) == (double *) NULL) ||
	    ((minArray  = (double *) calloc (gCont->TNum, sizeof (double))) == (double *) NULL) ||
	    ((maxArray  = (double *) calloc (gCont->TNum, sizeof (double))) == (double *) NULL))
	{ perror ("Memory allocation error in: _NCGdsHandleGContUpdateRange ()"); goto ABORT; }

	glbRange [0] =  HUGE_VAL;
	glbRange [1] = -HUGE_VAL;
	start [0] = 0;
	count [1] = 1;
	for (ncidx = 0;ncidx < gCont->NCnum;ncidx++)
	{
		meanRange [0] = minRange [0] = maxRange [0] =  HUGE_VAL;
		meanRange [1] = minRange [1] = maxRange [1] = -HUGE_VAL;
		if ((gCont->MinIds [ncidx] == NCGundefined) || (gCont->MaxIds [ncidx] == NCGundefined))
		{ fprintf (stderr,"Missing minimum or maximum variables in:  _NCGdsHandleGContUpdateRange ()\n"); goto ABORT; }
		count [0] = (ncidx + 1 < gCont->NCnum ? gCont->NCoffset [ncidx + 1] : gCont->TNum) - gCont->NCoffset [ncidx];
		offset = gCont->NCoffset [ncidx];
		for (level = 0;level < gCont->LNum;level++)
		{
			start [1] = level;
			if (((status = nc_get_vara_double (gCont->NCIds [ncidx], gCont->MeanIds  [ncidx], start, count, meanArray + offset)) != NC_NOERR) ||
			    ((status = nc_get_vara_double (gCont->NCIds [ncidx], gCont->MinIds   [ncidx], start, count, minArray  + offset)) != NC_NOERR) ||
			    ((status = nc_get_vara_double (gCont->NCIds [ncidx], gCont->MaxIds   [ncidx], start, count, maxArray  + offset)) != NC_NOERR))
			{ NCGprintNCError (status,"_NCGdsHandleGContUpdateRange"); goto ABORT; }
			for (i = 0;i < count [0]; i++)
			{
				if (meanArray [i + offset] < meanRange [0]) meanRange [0] = meanArray [i + offset];
				if (meanArray [i + offset] > meanRange [1]) meanRange [1] = meanArray [i + offset];
				if (minArray  [i + offset] < minRange  [0]) minRange  [0] = minArray  [i + offset];
				if (minArray  [i + offset] > minRange  [1]) minRange  [1] = minArray  [i + offset];
				if (maxArray  [i + offset] < maxRange  [0]) maxRange  [0] = maxArray  [i + offset];
				if (maxArray  [i + offset] > maxRange  [1]) maxRange  [1] = maxArray  [i + offset];
			}
			varRange [0] = minRange [0];
			varRange [1] = maxRange [1];
			if (varRange [0] < glbRange [0]) glbRange [0] = varRange [0];
			if (varRange [1] > glbRange [1]) glbRange [1] = varRange [1];
			if (((status = nc_redef (gCont->NCIds [ncidx])) != NC_NOERR) ||
				 ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->GVarIds  [ncidx], NCGnameVAActualRange, NC_FLOAT, 2, varRange))  != NC_NOERR) ||
		       ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->MeanIds  [ncidx], NCGnameVAActualRange, NC_FLOAT, 2, meanRange)) != NC_NOERR) ||
			    ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->MinIds   [ncidx], NCGnameVAActualRange, NC_FLOAT, 2, minRange))  != NC_NOERR) ||
			    ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->MaxIds   [ncidx], NCGnameVAActualRange, NC_FLOAT, 2, maxRange))  != NC_NOERR) ||
				 ((status = nc_enddef (gCont->NCIds [ncidx])) != NC_NOERR))
			{ NCGprintNCError (status,"_NCGdsHandleGContUpdateRange"); goto ABORT; }
		}
	}
	for (ncidx = 0;ncidx < gCont->NCnum;ncidx++)
		if (((status = nc_redef (gCont->NCIds [ncidx])) != NC_NOERR) ||
		    ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->GVarIds  [ncidx], NCGnameVAValidRange, NC_FLOAT, 2, glbRange)) != NC_NOERR) ||
		    ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->MeanIds  [ncidx], NCGnameVAValidRange, NC_FLOAT, 2, glbRange)) != NC_NOERR) ||
		    ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->MinIds   [ncidx], NCGnameVAValidRange, NC_FLOAT, 2, glbRange)) != NC_NOERR) ||
		    ((status = nc_put_att_double (gCont->NCIds [ncidx], gCont->MaxIds   [ncidx], NCGnameVAValidRange, NC_FLOAT, 2, glbRange)) != NC_NOERR) ||
			 ((status = nc_enddef (gCont->NCIds [ncidx])) != NC_NOERR))
		{ NCGprintNCError (status,"_NCGdsHandleGContUpdateRange"); goto ABORT; }
	free (meanArray);
	free (minArray);
	free (maxArray);
	return (NCGsucceeded);

ABORT:
	if (meanArray != (double *) NULL) free (meanArray);
	if (minArray  != (double *) NULL) free (minArray);
	if (maxArray  != (double *) NULL) free (maxArray);
	return (NCGfailed);
}
