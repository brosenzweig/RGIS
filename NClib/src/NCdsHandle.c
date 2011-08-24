#include <string.h>
#include <cm.h>
#include <unistd.h>
#include <NCdsHandle.h>

NCdsHandle_t *NCdsHandleOpenByIds (int *ncids, size_t n)
{
	int ret;
	NCdsHandle_t *dsHandle = (NCdsHandle_t *) NULL;
	NCdataType dataType;

	if (n < 1) return ((NCdsHandle_t *) NULL);
	switch (dataType = NCdataGetType (ncids [0]))
	{
		default: CMmsgPrint (CMmsgAppError,  "Invalid data type in: %s %d",__FILE__,__LINE__); return ((NCdsHandle_t *) NULL);
		case NCtypeGCont:    dsHandle = (NCdsHandle_t *) malloc (sizeof (NCdsHandleGCont_t));    break;
//		case NCtypeGDisc:    dsHandle = (NCdsHandle_t *) malloc (sizeof (NCdsHandleGDisc_t));    break;
		case NCtypeNetwork:  dsHandle = (NCdsHandle_t *) malloc (sizeof (NCdsHandleNetwork_t));  break;
		case NCtypePoint:    dsHandle = (NCdsHandle_t *) malloc (sizeof (NCdsHandleVPoint_t));   break;
//		case NCtypeLine:     dsHandle = (NCdsHandle_t *) malloc (sizeof (NCdsHandleVLine_t));    break;
//		case NCtypePolygon:  dsHandle = (NCdsHandle_t *) malloc (sizeof (NCdsHandleVPolygon_t)); break;
	}
	if (dsHandle == (NCdsHandle_t *) NULL) 
	{
		CMmsgPrint (CMmsgAppError,  "Memory allocation error in: %s %d",__FILE__,__LINE__);
		return ((NCdsHandle_t *) NULL);
	}
	switch (dataType)
	{
		default: free (dsHandle); return ((NCdsHandle_t *) NULL); 
		case NCtypeGCont:   ret = NCdsHandleGContDefine    ((NCdsHandleGCont_t *)    dsHandle, ncids, n); break;
//		case NCtypeGDisc:   ret = NCdsHandleGDiscDefine    ((NCdsHandleGDisc_t *)    dsHandle, ncids, n); break;
		case NCtypeNetwork: ret = NCdsHandleNetworkDefine  ((NCdsHandleNetwork_t *)  dsHandle, ncids [0]); break;
		case NCtypePoint:   ret = NCdsHandleVPointDefine   ((NCdsHandleVPoint_t *)   dsHandle, ncids [0]); break;
//		case NCtypeLine:    ret = NCdsHandleVLineDefine    ((NCdsHandleVLine_t *)    dsHandle, ncids [0]); break;
//		case NCtypePolygon: ret = NCdsHandleVPolygonDefine ((NCdsHandleVPolygon_t *) dsHandle, ncids [0]); break;
	}
	return (dsHandle);
}

NCdsHandle_t *NCdsHandleOpen (const char *pattern)
{
	int status, *ncids = (int *) NULL;
	size_t i = 0, ncNum = 0;
	char **fileList = (char **) NULL;
	NCdsHandle_t *dsHandle = (NCdsHandle_t *) NULL;

	if ((fileList = NCfileList (pattern,&ncNum)) == (char **) NULL) goto ABORT;

	if ((ncids = (int *) calloc (ncNum, sizeof (int))) == (int *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: NCdsHandleOpenByPattern ()"); goto ABORT; }

	for (i = 0;i < ncNum;++i)
		if ((status = nc_open (fileList [i],NC_NOWRITE,ncids + i)) != NC_NOERR)
		{ CMmsgPrint (CMmsgUsrError, "Filename: %s",fileList [0]); NCprintNCError (status,"NCdsHandleOpenByPattern"); ncids [i] = NCundefined; goto ABORT; }

	dsHandle = NCdsHandleOpenByIds (ncids,ncNum);
	free (ncids);
	NCfileListFree (fileList,ncNum);
	return (dsHandle);

ABORT:
	if (fileList != (char **) NULL) NCfileListFree (fileList,ncNum);
	if (ncids != (int *) NULL)
	{
		for (i = i - 1;i >= 0;--i) if (ncids [i] != NCundefined) nc_close (ncids [i]);
		free (ncids);
	}
	return (dsHandle);
}

NCdsHandle_t *NCdsHandleOpenById (int ncid) { return (NCdsHandleOpenByIds (&ncid,1)); }

NCdsHandle_t *NCdsHandleCreate (const char *pattern, const char *name, int dncid, NCtimeStep tsMode, utUnit *tUnitIn, double sTime, double eTime)
{
	size_t i, j, strLen, strOffset, ncNum = 0, index;
	char *str [] = { "{year}", "{month}", "{day}", "{hour}", "{minute}", "{second}" };
	char *searchStr, *subStr, **fileNames = (char **) NULL, tsUnitStr [NCsizeString];
	int *ncids = (int *) NULL, tvarid, status;
	int endYear, endMonth, year, month, day, hour, minute;
	float second;
	double time = 0.0, scale, offset;
	utUnit tUnitOut;
	NCdsHandle_t *dsh;

	if ((searchStr = malloc (strlen (pattern) + 1)) == (char *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: NCdsHandleCreate ()!"); return ((NCdsHandle_t *) NULL); }

	if ((utCalendar (eTime,tUnitIn,&endYear,&endMonth,&day,&hour,&minute,&second) != 0) ||
	    (utCalendar (sTime,tUnitIn,&year,   &month,   &day,&hour,&minute,&second) != 0))
	{ CMmsgPrint (CMmsgAppError, "Calender scanning error in:%s %d",__FILE__,__LINE__); goto ABORT; }

	switch (tsMode)
	{
		case NCtimeYear:   sprintf (tsUnitStr,"years since %04d-01-01 00:00 0.0",year);  break;
		case NCtimeMonth:  sprintf (tsUnitStr,"months since %04d-%02d-01 00:00 0.0",year, month); break;
		case NCtimeDay:    sprintf (tsUnitStr,"days since %04d-%02d-%02d 00:00 0.0",year, month, day);   break;
		case NCtimeHour:   sprintf (tsUnitStr,"hours since %04d-%02d-%02d %02d:00 0.0",year, month, day, hour);  break;
		case NCtimeMinute: sprintf (tsUnitStr,"minutes since %04d-%02d-%02d %02d:%02d 0.0", year, month, day, hour, minute); break;
		case NCtimeSecond: sprintf (tsUnitStr,"seconds since %04d-%02d-%02d %02d:%02d %.1f", year, month, day, hour, minute, second); break;
	}
	if (tsMode > NCtimeMonth)
	{
		if ((utScan (tsUnitStr, &tUnitOut) != 0) || (utConvert (tUnitIn, &tUnitOut, &scale, &offset) != 0))
		{ CMmsgPrint (CMmsgAppError, "Time unit scanning error in: %s %d",__FILE__,__LINE__); goto ABORT; }
		sTime = sTime * scale + offset;
		eTime = eTime * scale + offset;
	}
	else
	{
		sTime = 0.0;
		eTime = tsMode > NCtimeYear ? (endYear - year) * 12 + (endMonth - month + 1) : (double) (year - endYear);
	}
	do
	{
		if (tsMode > NCtimeMonth)
		{
			if (utCalendar (sTime + time,&tUnitOut,&year,&month,&day,&hour,&minute,&second) != 0)
			{ CMmsgPrint (CMmsgAppError, "Time unit scaning error in: %s %d",__FILE__,__LINE__); goto ABORT; }
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
					case NCtimeYear:   sprintf (subStr,"%04d", year);    subStr += 4; strOffset -= 4; break;
					case NCtimeMonth:  sprintf (subStr,"%02d", month);   subStr += 2; strOffset -= 2; break;
					case NCtimeDay:    sprintf (subStr,"%02d", day);     subStr += 2; strOffset -= 2; break;
					case NCtimeHour:   sprintf (subStr,"%02d", hour);    subStr += 2; strOffset -= 2; break;
					case NCtimeMinute: sprintf (subStr,"%02d", minute);  subStr += 2; strOffset -= 2; break;
					case NCtimeSecond: sprintf (subStr,"%04.1f",second); subStr += 4; strOffset -= 4; break;
				}
				for (j = 0;j <= strLen; j++) subStr [j] = subStr [j + strOffset];
			} 
		if ((ncNum == 0) || (strcmp (fileNames [ncNum - 1], searchStr) != 0))
		{
			if (((fileNames = (char **) realloc (fileNames, (ncNum + 1) * sizeof (char *))) == (char **) NULL) ||
				 ((ncids     = (int *)   realloc (ncids,     (ncNum + 1) * sizeof (int)))    == (int *)   NULL))
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }
			else ncNum++;
			if  ((fileNames [ncNum - 1] = (char *) malloc (strlen (searchStr) + 1)) == (char *) NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }
			strcpy (fileNames [ncNum - 1], searchStr);
			if (((ncids [ncNum - 1] = NCfileCreate (fileNames [ncNum - 1], dncid)) == NCfailed) ||
			    (NCfileVarAdd (ncids [ncNum - 1], name, NC_FLOAT, NC_DOUBLE, NC_FLOAT) == NCfailed) ||
				 (NCfileSetTimeUnit   (ncids [ncNum - 1], tsUnitStr)  == NCfailed) ||
				 (NCfileSetMissingVal (ncids [ncNum - 1], -9999.0)    == NCfailed) ||
				 ((tvarid = NCdataGetTVarId (ncids [ncNum - 1])) == NCfailed)) goto ABORT;
			index = 0;
		}
		if ((status = nc_put_var1_double (ncids [ncNum - 1],tvarid,&index, &time)) != NC_NOERR)
		{ NCprintNCError (status,"NCdsHandleCreate"); goto ABORT; }
		index++;
		
		if (tsMode == NCtimeMonth) { month++; if (month > 12) { month = 1; year++;} }
		if (tsMode == NCtimeYear)  { year++; }
		time = time + 1.0;
	} while ((sTime + time) < eTime);
	for (i = 0;i < ncNum; i++) nc_sync (ncids [i]);
	if ((dsh = NCdsHandleOpenByIds (ncids, ncNum)) == (NCdsHandle_t *) NULL) goto ABORT;

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
	return ((NCdsHandle_t *) NULL);
}

NCstate NCdsHandleClose (NCdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    NCdsHandleGContClear    ((NCdsHandleGCont_t *)    dsh); break;
//		case NCtypeGDisc:    NCdsHandleGDiscClear    ((NCdsHandleGDisc_t *)    dsh); break; 
		case NCtypeNetwork:  NCdsHandleNetworkClear  ((NCdsHandleNetwork_t *)  dsh); break;
		case NCtypePoint:    NCdsHandleVPointClear   ((NCdsHandleVPoint_t *)   dsh); break;
//		case NCtypeLine:     NCdsHandleVLineClear    ((NCdsHandleVLine_t *)    dsh); break;
//		case NCtypePolygon:  NCdsHandleVPolygonClear ((NCdsHandleVPolygon_t *) dsh); break;
	}
	NCdsHandleClear (dsh);
	free (dsh);
	return (NCsucceeded);
}

NCstate NCdsHandleDefine (NCdsHandle_t *dsh, int *ncids, size_t n)
{
	NCdataType dataType;
	int i;

	dsh->NCIds   = (int *)  NULL;
	NCdsHandleClear (dsh);

	if (n < 1) return (NCfailed);
	if ((dataType = NCdataGetType (ncids [0])) == NCundefined) return (NCfailed);
	dsh->DataType = dataType;
	if ((dsh->NCIds = (int *) calloc (n,sizeof (int))) == (int *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
	for (i = 0;i < n;++i) dsh->NCIds [i] = ncids [i];
	dsh->NCnum = n;
	return (NCsucceeded);
}

void NCdsHandleClear (NCdsHandle_t *dsh)
{
	if (dsh->NCIds   != (int *)  NULL) free (dsh->NCIds);
	dsh->DataType = NCundefined;
	dsh->NCIds    = (int *) NULL;
	dsh->NCnum    = 0;
}

NCstate NCdsHandleReference (const NCdsHandle_t *dsh,const NCcoordinate_t *coord,NCreference_t *ref)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    return (NCdsHandleGContReference  ((const NCdsHandleGCont_t *)  dsh, coord, ref));
//		case NCtypeGDisc:    return (NCdsHandleGDiscReference  ((const NCdsHandleGDisc_t *)  dsh, coord, ref));
		case NCtypePoint:    return (NCdsHandleVPointReference ((const NCdsHandleVPoint_t *) dsh, coord, ref));
	}
	return (NCfailed);
}

int NCdsHandleGetLNum  (const NCdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break;
		case NCtypeGCont: return (((const NCdsHandleGCont_t *) dsh)->LNum);
	}
	return (NCfailed);
}

int NCdsHandleGetTNum  (const NCdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break;
		case NCtypeGCont: return (((const NCdsHandleGCont_t *) dsh)->TNum);
	}
	return (NCfailed);
}

NCstate NCdsHandleUpdateRanges (const NCdsHandle_t *dsh)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    return (NCdsHandleGContUpdateRanges   ((NCdsHandleGCont_t *)  dsh));
	}
	return (NCfailed);
}

NCstate NCdsHandleLoadCache (NCdsHandle_t *dsh,size_t sTStep, size_t nTSteps, size_t sLevel, size_t nLevels)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    return (NCdsHandleGContLoadCache  ((NCdsHandleGCont_t *)  dsh, sTStep, nTSteps, sLevel, nLevels));
//		case NCtypeGDisc:    return (NCdsHandleGDiscLoadCache  ((NCdsHandleGDisc_t *)  dsh, sTime, eTime, sLevel, eLevel));
//		case NCtypePoint:    return (NCdsHandleVPointLoadCache ((NCdsHandleVPoint_t *) dsh, sTime, eTime, sLevel, eLevel));
	}
	return (NCfailed);
}

NCstate NCdsHandleSaveCache (NCdsHandle_t *dsh, size_t time, size_t level)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    return (NCdsHandleGContSaveCache  ((NCdsHandleGCont_t *)  dsh, time, level));
//		case NCtypeGDisc:    return (NCdsHandleGDiscSaveCache  ((NCdsHandleGDisc_t *)  dsh, time, level));
//		case NCtypePoint:    return (NCdsHandleVPointSaveCache ((NCdsHandleVPoint_t *) dsh, time, level));
	}
	return (NCfailed);
}

bool NCdsHandleGetFloat (const NCdsHandle_t *dsh, NCreference_t *ref, double *val)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    return (NCdsHandleGContGetFloat  ((const NCdsHandleGCont_t *)  dsh, ref, val));
//		case NCtypeGDisc:    return (NCdsHandleGDiscGetFloat  ((const NCdsHandleGDisc_t *)  dsh, ref, val));
//		case NCtypePoint:    return (NCdsHandleVPointFloat    ((const NCdsHandleVPoint_t *) dsh, ref, val));
	}
	return (FLOAT_NOVALUE);
}

NCstate NCdsHandleSetFloat (NCdsHandle_t *dsh, size_t *index, double val)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    return (NCdsHandleGContSetFloat  ((NCdsHandleGCont_t *)  dsh, index [0], index [1], val));
//		case NCtypeGDisc:    return (NCdsHandleGDiscSetFloat  ((NCdsHandleGDisc_t *)  dsh, index [0], index [1], val));
//		case NCtypePoint:    return (NCdsHandleVPoinSetFloat  ((NCdsHandleVPoint_t *) dsh, index [0], val));
	}
	return (CMfailed);
}

NCstate NCdsHandleSetFill (NCdsHandle_t *dsh, size_t *index)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:    return (NCdsHandleGContSetFill  ((NCdsHandleGCont_t *)  dsh, index [0], index [1]));
//		case NCtypeGDisc:    return (NCdsHandleGDiscSetFloat ((NCdsHandleGDisc_t *)  dsh, index [0], index [1], val));
//		case NCtypePoint:    return (NCdsHandleVPoinSetFloat ((NCdsHandleVPoint_t *) dsh, index [0], val));
	}
	return (CMfailed);
}

NCstate NCdsHandleGetTime (const NCdsHandle_t *dsh, size_t layerID, utUnit *tUnit, double *time)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:   
		case NCtypeGDisc:   return (NCdsHandleGridGetTime ((const NCdsHandleGrid_t *)  dsh, layerID,tUnit, time));
	}
	CMmsgPrint (CMmsgAppError,  "Data set does not have time series in: %s %d",__FILE__,__LINE__);
	return (NCfailed);
}

int NCdsHandleGetTimeStep (const NCdsHandle_t *dsh, size_t layerID, utUnit *tUnit, double *tStep)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:   
		case NCtypeGDisc:   return (NCdsHandleGridGetTimeStep ((const NCdsHandleGrid_t *)  dsh, layerID, tUnit, tStep));
	}
	CMmsgPrint (CMmsgAppError,  "Data set does not have time series in: %s %d",__FILE__,__LINE__);
	return (NCfailed);
}

NCstate NCdsHandleGetTLayerID (const NCdsHandle_t *dsh, utUnit *tUnit, double time)
{
	switch (dsh->DataType)
	{
		default: break; 
		case NCtypeGCont:   
		case NCtypeGDisc:   return (NCdsHandleGridGetTLayerID ((const NCdsHandleGrid_t *)  dsh, tUnit, time));
	}
	CMmsgPrint (CMmsgAppError,  "Data set does not have time series in: %s %d",__FILE__,__LINE__);
	return (NCfailed);
}

NCstate NCdsHandleGetUnitConv (const NCdsHandle_t *dsh, const utUnit *unit, double *scale, double *offset)
{
	switch (dsh->DataType)
	{
		case NCtypeGDisc:
		default:
			CMmsgPrint (CMmsgAppError,  "Data set does not have time series in: %s %d",__FILE__,__LINE__);
			return (NCfailed);
		case NCtypeGCont:
			if (((NCdsHandleGCont_t *) dsh)->DoGUnit)
			{
				if (utConvert (&(((NCdsHandleGCont_t *) dsh)->GUnit),unit, scale, offset) != 0)
				{
					CMmsgPrint (CMmsgAppError, "Unit Conversion error in: %s %d",__FILE__,__LINE__);
					*scale = 1.0; *offset = 0.0;
					return (NCfailed);
				} 
			}
			else { *scale = 1.0; *offset = 0.0; }
			break;
	}
	return (NCsucceeded);
}

NCstate NCdsHandleSetLevelUnit (const NCdsHandle_t *dsh, const char *unitStr)
{
	size_t i;

	for (i = 0;i < dsh->NCnum; i++) if (NCfileSetLevelUnit (dsh->NCIds [i],unitStr) == NCfailed) return (NCfailed);
	return (NCsucceeded);
}

NCstate NCdsHandleSetVarUnit (const NCdsHandle_t *dsh, const char *unitStr)
{
	size_t i;

	for (i = 0;i < dsh->NCnum; i++) if (NCfileSetVarUnit (dsh->NCIds [i],unitStr) == NCfailed) return (NCfailed);
	return (NCsucceeded);
}

//NCstate       NCdsHandleVarSetAttributes (const NCdsHandle_t *, double, double, double, double, double);

