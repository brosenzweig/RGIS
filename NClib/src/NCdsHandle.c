#include<unistd.h>
#include<NCdsHandle.h>

NCGdsHandle_t *NCGdsHandleOpenByIds (int *ncids, size_t n)
{
	int ret;
	NCGdsHandle_t *dsHandle = (NCGdsHandle_t *) NULL;
	NCGdataType dataType;

	if (n < 1) return ((NCGdsHandle_t *) NULL);
	switch (dataType = NCGdataGetType (ncids [0]))
	{
		default: fprintf (stderr, "Invalid data type in NCGdsHandleOpen ()"); return ((NCGdsHandle_t *) NULL);
		case NCGtypeGCont:    dsHandle = (NCGdsHandle_t *) malloc (sizeof (NCGdsHandleGCont_t));    break;
//		case NCGtypeGDisc:    dsHandle = (NCGdsHandle_t *) malloc (sizeof (NCGdsHandleGDisc_t));    break;
		case NCGtypeNetwork:  dsHandle = (NCGdsHandle_t *) malloc (sizeof (NCGdsHandleNetwork_t));  break;
		case NCGtypePoint:    dsHandle = (NCGdsHandle_t *) malloc (sizeof (NCGdsHandleVPoint_t));   break;
//		case NCGtypeLine:     dsHandle = (NCGdsHandle_t *) malloc (sizeof (NCGdsHandleVLine_t));    break;
//		case NCGtypePolygon:  dsHandle = (NCGdsHandle_t *) malloc (sizeof (NCGdsHandleVPolygon_t)); break;
	}
	if (dsHandle == (NCGdsHandle_t *) NULL) 
	{
		fprintf (stderr, "Memory allocation error in NCGdsHandleOpen ()");
		return ((NCGdsHandle_t *) NULL);
	}
	switch (dataType)
	{
		default: free (dsHandle); return ((NCGdsHandle_t *) NULL); 
		case NCGtypeGCont:   ret = NCGdsHandleGContDefine    ((NCGdsHandleGCont_t *)    dsHandle, ncids, n); break;
//		case NCGtypeGDisc:   ret = NCGdsHandleGDiscDefine    ((NCGdsHandleGDisc_t *)    dsHandle, ncids, n); break;
		case NCGtypeNetwork: ret = NCGdsHandleNetworkDefine  ((NCGdsHandleNetwork_t *)  dsHandle, ncids [0]); break;
		case NCGtypePoint:   ret = NCGdsHandleVPointDefine   ((NCGdsHandleVPoint_t *)   dsHandle, ncids [0]); break;
//		case NCGtypeLine:    ret = NCGdsHandleVLineDefine    ((NCGdsHandleVLine_t *)    dsHandle, ncids [0]); break;
//		case NCGtypePolygon: ret = NCGdsHandleVPolygonDefine ((NCGdsHandleVPolygon_t *) dsHandle, ncids [0]); break;
	}
	return (dsHandle);
}

NCGdsHandle_t *NCGdsHandleOpen (const char *pattern)
{
	int status, *ncids = (int *) NULL;
	size_t i = 0, ncNum = 0;
	char **fileList = (char **) NULL;
	NCGdsHandle_t *dsHandle = (NCGdsHandle_t *) NULL;

	if ((fileList = NCGfileList (pattern,&ncNum)) == (char **) NULL) goto ABORT;

	if ((ncids = (int *) calloc (ncNum, sizeof (int))) == (int *) NULL)
	{ perror ("Memory allocation error in: NCGdsHandleOpenByPattern ()"); goto ABORT; }

	for (i = 0;i < ncNum;++i)
		if ((status = nc_open (fileList [i],NC_NOWRITE,ncids + i)) != NC_NOERR)
		{ fprintf (stderr,"Filename: %s\n",fileList [0]); NCGprintNCError (status,"NCGdsHandleOpenByPattern"); ncids [i] = NCGundefined; goto ABORT; }

	dsHandle = NCGdsHandleOpenByIds (ncids,ncNum);
	free (ncids);
	NCGfileListFree (fileList,ncNum);
	return (dsHandle);

ABORT:
	if (fileList != (char **) NULL) NCGfileListFree (fileList,ncNum);
	if (ncids != (int *) NULL)
	{
		for (i = i - 1;i >= 0;--i) if (ncids [i] != NCGundefined) nc_close (ncids [i]);
		free (ncids);
	}
	return (dsHandle);
}

NCGdsHandle_t *NCGdsHandleOpenById (int ncid) { return (NCGdsHandleOpenByIds (&ncid,1)); }

NCGdsHandle_t *NCGdsHandleCreate (const char *pattern, const char *name, int dncid, NCGtimeStep tsMode, utUnit *tUnitIn, double sTime, double eTime)
{
	size_t i, j, strLen, strOffset, ncNum = 0, index;
	char *str [] = { "{year}", "{month}", "{day}", "{hour}", "{minute}", "{second}" };
	char *searchStr, *subStr, **fileNames = (char **) NULL, tsUnitStr [NCGsizeString];
	int *ncids = (int *) NULL, tvarid, status;
	int endYear, endMonth, year, month, day, hour, minute;
	float second;
	double time = 0.0, scale, offset;
	utUnit tUnitOut;
	NCGdsHandle_t *dsh;

	if ((searchStr = malloc (strlen (pattern) + 1)) == (char *) NULL)
	{ perror ("Memory allocation error in: NCGdsHandleCreate ()!"); return ((NCGdsHandle_t *) NULL); }

	if ((utCalendar (eTime,tUnitIn,&endYear,&endMonth,&day,&hour,&minute,&second) != 0) ||
	    (utCalendar (sTime,tUnitIn,&year,   &month,   &day,&hour,&minute,&second) != 0))
	{ fprintf (stderr,"Calender scanning error in: NCGdsHandleCreate ()\n"); goto ABORT; }

	switch (tsMode)
	{
		case NCGtimeYear:   sprintf (tsUnitStr,"years since %04d-01-01 00:00 0.0",year);  break;
		case NCGtimeMonth:  sprintf (tsUnitStr,"months since %04d-%02d-01 00:00 0.0",year, month); break;
		case NCGtimeDay:    sprintf (tsUnitStr,"days since %04d-%02d-%02d 00:00 0.0",year, month, day);   break;
		case NCGtimeHour:   sprintf (tsUnitStr,"hours since %04d-%02d-%02d %02d:00 0.0",year, month, day, hour);  break;
		case NCGtimeMinute: sprintf (tsUnitStr,"minutes since %04d-%02d-%02d %02d:%02d 0.0", year, month, day, hour, minute); break;
		case NCGtimeSecond: sprintf (tsUnitStr,"seconds since %04d-%02d-%02d %02d:%02d %.1f", year, month, day, hour, minute, second); break;
	}
	if (tsMode > NCGtimeMonth)
	{
		if ((utScan (tsUnitStr, &tUnitOut) != 0) || (utConvert (tUnitIn, &tUnitOut, &scale, &offset) != 0))
		{ fprintf (stderr,"Time unit scanning error in: NCGdsHandleCreate ()\n"); goto ABORT; }
		sTime = sTime * scale + offset;
		eTime = eTime * scale + offset;
	}
	else
	{
		sTime = 0.0;
		eTime = tsMode > NCGtimeYear ? (endYear - year) * 12 + (endMonth - month + 1) : (double) (year - endYear);
	}
	do
	{
		if (tsMode > NCGtimeMonth)
		{
			if (utCalendar (sTime + time,&tUnitOut,&year,&month,&day,&hour,&minute,&second) != 0)
			{ fprintf (stderr,"Time unit scaning error in: NCGdsHandleCreate ()\n"); goto ABORT; }
		}
		strcpy (searchStr, pattern);
		for (i = 0;i < tsMode; ++i)
			if ((subStr = strstr (searchStr,str [i])) == (char *) NULL) break;
			else
			{
				strOffset = strlen (str [i]);
				strLen = strlen (subStr) - strOffset;
				switch (i)
				{
					case NCGtimeYear:   sprintf (subStr,"%04d", year);    subStr += 4; strOffset -= 4; break;
					case NCGtimeMonth:  sprintf (subStr,"%02d", month);   subStr += 2; strOffset -= 2; break;
					case NCGtimeDay:    sprintf (subStr,"%02d", day);     subStr += 2; strOffset -= 2; break;
					case NCGtimeHour:   sprintf (subStr,"%02d", hour);    subStr += 2; strOffset -= 2; break;
					case NCGtimeMinute: sprintf (subStr,"%02d", minute);  subStr += 2; strOffset -= 2; break;
					case NCGtimeSecond: sprintf (subStr,"%04.1f",second); subStr += 4; strOffset -= 4; break;
				}
				for (j = 0;j <= strLen; j++) subStr [j] = subStr [j + strOffset];
			} 
		if ((ncNum == 0) || (strcmp (fileNames [ncNum - 1], searchStr) != 0))
		{
			if (((fileNames = (char **) realloc (fileNames, (ncNum + 1) * sizeof (char *))) == (char **) NULL) ||
				 ((ncids     = (int *)   realloc (ncids,     (ncNum + 1) * sizeof (int)))    == (int *)   NULL))
			{ perror ("Memory allocation error in: NCGdsHandleCreate"); goto ABORT; }
			else ncNum++;
			if  ((fileNames [ncNum - 1] = (char *) malloc (strlen (searchStr) + 1)) == (char *) NULL)
			{ perror ("Memory allocation error in: NCGdsHandleCreate"); goto ABORT; }
			strcpy (fileNames [ncNum - 1], searchStr);
			if (((ncids [ncNum - 1] = NCGfileCreate (fileNames [ncNum - 1], dncid)) == NCGfailed) ||
			    (NCGfileVarAdd (ncids [ncNum - 1], name, NC_FLOAT, NC_DOUBLE, NC_FLOAT) == NCGfailed) ||
				 (NCGfileSetTimeUnit   (ncids [ncNum - 1], tsUnitStr)  == NCGfailed) ||
				 (NCGfileSetMissingVal (ncids [ncNum - 1], -9999.0)    == NCGfailed) ||
				 ((tvarid = NCGdataGetTVarId (ncids [ncNum - 1])) == NCGfailed)) goto ABORT;
			index = 0;
		}
		if ((status = nc_put_var1_double (ncids [ncNum - 1],tvarid,&index, &time)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGdsHandleCreate"); goto ABORT; }
		index++;
		
		if (tsMode == NCGtimeMonth) { month++; if (month > 12) { month = 1; year++;} }
		if (tsMode == NCGtimeYear)  { year++; }
		time = time + 1.0;
	} while ((sTime + time) < eTime);
	for (i = 0;i < ncNum; i++) nc_sync (ncids [i]);
	if ((dsh = NCGdsHandleOpenByIds (ncids, ncNum)) == (NCGdsHandle_t *) NULL) goto ABORT;

	for (i = 0;i < ncNum; i++) free (fileNames [i]);
	utClear (&tUnitOut);
	free (fileNames);
	free (ncids);
	free (searchStr);
	return (dsh);
ABORT:
	for (i = 0;i < ncNum;i++) { nc_abort (ncids [i]); unlink (fileNames [i]); if (fileNames [i] != (char *) NULL) free (fileNames [i]); }
	utClear (&tUnitOut);
	if (fileNames != (char **) NULL) free (fileNames);
	free (searchStr);
	return ((NCGdsHandle_t *) NULL);
}

NCGstate NCGdsHandleClose (NCGdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    NCGdsHandleGContClear    ((NCGdsHandleGCont_t *)    dsh); break;
//		case NCGtypeGDisc:    NCGdsHandleGDiscClear    ((NCGdsHandleGDisc_t *)    dsh); break; 
		case NCGtypeNetwork:  NCGdsHandleNetworkClear  ((NCGdsHandleNetwork_t *)  dsh); break;
		case NCGtypePoint:    NCGdsHandleVPointClear   ((NCGdsHandleVPoint_t *)   dsh); break;
//		case NCGtypeLine:     NCGdsHandleVLineClear    ((NCGdsHandleVLine_t *)    dsh); break;
//		case NCGtypePolygon:  NCGdsHandleVPolygonClear ((NCGdsHandleVPolygon_t *) dsh); break;
	}
	NCGdsHandleClear (dsh);
	free (dsh);
	return (NCGsucceeded);
}

NCGstate NCGdsHandleDefine (NCGdsHandle_t *dsh, int *ncids, size_t n)
{
	NCGdataType dataType;
	int i;

	dsh->NCIds   = (int *)  NULL;
	NCGdsHandleClear (dsh);

	if (n < 1) return (NCGfailed);
	if ((dataType = NCGdataGetType (ncids [0])) == NCGundefined) return (NCGfailed);
	dsh->DataType = dataType;
	if ((dsh->NCIds = (int *) calloc (n,sizeof (int))) == (int *) NULL)
	{ perror ("Memory allocation error in: NCGdsHandleDefine ()"); return (NCGfailed); }
	for (i = 0;i < n;++i) dsh->NCIds [i] = ncids [i];
	dsh->NCnum = n;
	return (NCGsucceeded);
}

void NCGdsHandleClear (NCGdsHandle_t *dsh)
{
	if (dsh->NCIds   != (int *)  NULL) free (dsh->NCIds);
	dsh->DataType = NCGundefined;
	dsh->NCIds    = (int *) NULL;
	dsh->NCnum    = 0;
}

NCGstate NCGdsHandleReference (const NCGdsHandle_t *dsh,const NCGcoordinate_t *coord,NCGreference_t *ref)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    return (NCGdsHandleGContReference  ((const NCGdsHandleGCont_t *)  dsh, coord, ref));
//		case NCGtypeGDisc:    return (NCGdsHandleGDiscReference  ((const NCGdsHandleGDisc_t *)  dsh, coord, ref));
		case NCGtypePoint:    return (NCGdsHandleVPointReference ((const NCGdsHandleVPoint_t *) dsh, coord, ref));
	}
	return (NCGfailed);
}

int NCGdsHandleGetLNum  (const NCGdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break;
		case NCGtypeGCont: return (((const NCGdsHandleGCont_t *) dsh)->LNum);
	}
	return (NCGfailed);
}

int NCGdsHandleGetTNum  (const NCGdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break;
		case NCGtypeGCont: return (((const NCGdsHandleGCont_t *) dsh)->TNum);
	}
	return (NCGfailed);
}

NCGstate NCGdsHandleUpdateRanges (const NCGdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    return (NCGdsHandleGContUpdateRanges   ((NCGdsHandleGCont_t *)  dsh));
	}
	return (NCGfailed);
}

NCGstate NCGdsHandleLoadCache (NCGdsHandle_t *dsh,size_t sTStep, size_t nTSteps, size_t sLevel, size_t nLevels)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    return (NCGdsHandleGContLoadCache  ((NCGdsHandleGCont_t *)  dsh, sTStep, nTSteps, sLevel, nLevels));
//		case NCGtypeGDisc:    return (NCGdsHandleGDiscLoadCache  ((NCGdsHandleGDisc_t *)  dsh, sTime, eTime, sLevel, eLevel));
//		case NCGtypePoint:    return (NCGdsHandleVPointLoadCache ((NCGdsHandleVPoint_t *) dsh, sTime, eTime, sLevel, eLevel));
	}
	return (NCGfailed);
}

NCGstate NCGdsHandleSaveCache (NCGdsHandle_t *dsh, size_t time, size_t level)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    return (NCGdsHandleGContSaveCache  ((NCGdsHandleGCont_t *)  dsh, time, level));
//		case NCGtypeGDisc:    return (NCGdsHandleGDiscSaveCache  ((NCGdsHandleGDisc_t *)  dsh, time, level));
//		case NCGtypePoint:    return (NCGdsHandleVPointSaveCache ((NCGdsHandleVPoint_t *) dsh, time, level));
	}
	return (NCGfailed);
}

bool NCGdsHandleGetFloat (const NCGdsHandle_t *dsh, NCGreference_t *ref, double *val)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    return (NCGdsHandleGContGetFloat  ((const NCGdsHandleGCont_t *)  dsh, ref, val));
//		case NCGtypeGDisc:    return (NCGdsHandleGDiscGetFloat  ((const NCGdsHandleGDisc_t *)  dsh, ref, val));
//		case NCGtypePoint:    return (NCGdsHandleVPointFloat    ((const NCGdsHandleVPoint_t *) dsh, ref, val));
	}
	return (FLOAT_NOVALUE);
}

NCGstate NCGdsHandleSetFloat (NCGdsHandle_t *dsh, size_t *index, double val)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    return (NCGdsHandleGContSetFloat  ((NCGdsHandleGCont_t *)  dsh, index [0], index [1], val));
//		case NCGtypeGDisc:    return (NCGdsHandleGDiscSetFloat  ((NCGdsHandleGDisc_t *)  dsh, index [0], index [1], val));
//		case NCGtypePoint:    return (NCGdsHandleVPoinSetFloat  ((NCGdsHandleVPoint_t *) dsh, index [0], val));
	}
	return (FLOAT_NOVALUE);
}

NCGstate NCGdsHandleSetFill (NCGdsHandle_t *dsh, size_t *index)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:    return (NCGdsHandleGContSetFill  ((NCGdsHandleGCont_t *)  dsh, index [0], index [1]));
//		case NCGtypeGDisc:    return (NCGdsHandleGDiscSetFloat ((NCGdsHandleGDisc_t *)  dsh, index [0], index [1], val));
//		case NCGtypePoint:    return (NCGdsHandleVPoinSetFloat ((NCGdsHandleVPoint_t *) dsh, index [0], val));
	}
	return (FLOAT_NOVALUE);
}

NCGstate NCGdsHandleGetTime (const NCGdsHandle_t *dsh, size_t layerID, utUnit *tUnit, double *time)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:   
		case NCGtypeGDisc:   return (NCGdsHandleGridGetTime ((const NCGdsHandleGrid_t *)  dsh, layerID,tUnit, time));
	}
	fprintf (stderr, "Data set does not have time series in: NCGdsHandleGetTime ()\n");
	return (NCGfailed);
}

int NCGdsHandleGetTimeStep (const NCGdsHandle_t *dsh, size_t layerID, utUnit *tUnit, double *tStep)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:   
		case NCGtypeGDisc:   return (NCGdsHandleGridGetTimeStep ((const NCGdsHandleGrid_t *)  dsh, layerID, tUnit, tStep));
	}
	fprintf (stderr, "Data set does not have time series in: NCGdsHandleGetTimeStep ()\n");
	return (NCGfailed);
}

NCGstate NCGdsHandleGetTLayerID (const NCGdsHandle_t *dsh, utUnit *tUnit, double time)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCGtypeGCont:   
		case NCGtypeGDisc:   return (NCGdsHandleGridGetTLayerID ((const NCGdsHandleGrid_t *)  dsh, tUnit, time));
	}
	fprintf (stderr, "Data set does not have time series in: NCGdsHandleGetTLayerID ()\n");
	return (NCGfailed);
}

NCGstate NCGdsHandleGetUnitConv (const NCGdsHandle_t *dsh, const utUnit *unit, double *scale, double *offset)
{
	switch (dsh->DataType)
	{
		case NCGtypeGDisc:
		default:
			fprintf (stderr, "Data set does not have time series in: NCGdsHandleGetTimeStep ()\n");
			return (NCGfailed);
		case NCGtypeGCont:
			if (((NCGdsHandleGCont_t *) dsh)->DoGUnit)
			{
				if (utConvert (&(((NCGdsHandleGCont_t *) dsh)->GUnit),unit, scale, offset) != 0)
				{
					fprintf (stderr,"Unit Conversion error in: NCGdsHandleGetUnitConv ()\n");
					*scale = 1.0; *offset = 0.0;
					return (NCGfailed);
				} 
			}
			else { *scale = 1.0; *offset = 0.0; }
			break;
	}
	return (NCGsucceeded);
}

NCGstate NCGdsHandleSetLevelUnit (const NCGdsHandle_t *dsh, const char *unitStr)
{
	size_t i;

	for (i = 0;i < dsh->NCnum; i++) if (NCGfileSetLevelUnit (dsh->NCIds [i],unitStr) == NCGfailed) return (NCGfailed);
	return (NCGsucceeded);
}

NCGstate NCGdsHandleSetVarUnit (const NCGdsHandle_t *dsh, const char *unitStr)
{
	size_t i;

	for (i = 0;i < dsh->NCnum; i++) if (NCGfileSetVarUnit (dsh->NCIds [i],unitStr) == NCGfailed) return (NCGfailed);
	return (NCGsucceeded);
}

//NCGstate       NCGdsHandleVarSetAttributes (const NCGdsHandle_t *, double, double, double, double, double);

