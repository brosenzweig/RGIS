#include <string.h>
#include <cm.h>
#include <NCdsHandle.h>

NCstate NCdsHandleGridDefine (NCdsHandleGrid_t *grid, int *ncids, size_t n)
{
	int status;
	int dimid;
	int year;
	size_t i, j, tnum, lnum, unitlen;
	char unitstr [NC_MAX_NAME], *dateStr;
	double x0, x1, delta, scale, offset;

	grid->LVarIds  = grid->TVarIds  = (int *)    NULL;
	grid->LNum     = grid->TNum     = (size_t)   0;
	grid->Levels   = grid->Times    = (double *) NULL;
	grid->NCindex  = grid->NCoffset = (size_t *) NULL;
	grid->DoLUnit  = grid->DoTUnit  = false;
	grid->LUnits   = grid->TUnits   = (utUnit *) NULL;
	grid->Climatology = false;
	if (n < 1) return (NCfailed);
	if (NCdsHandleGLayoutDefine ((NCdsHandleGLayout_t *) grid, ncids, n) == NCfailed) return (NCfailed);

	if ((grid->DataType != NCtypeGCont) && (grid->DataType != NCtypeGDisc)) 
	{ CMmsgPrint (CMmsgAppError, "Invalid data in: %s %d",__FILE__,__LINE__); goto ABORT; }

	if (((grid->TVarIds  = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((grid->LVarIds  = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((grid->NCoffset = (size_t *) calloc (n, sizeof (size_t))) == (size_t *) NULL) ||
		((grid->LUnits   = (utUnit *) calloc (n, sizeof (utUnit))) == (utUnit *) NULL) ||
		((grid->TUnits   = (utUnit *) calloc (n, sizeof (utUnit))) == (utUnit *) NULL))
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }

	for (i = 0;i < n; ++i) grid->TVarIds [i] = grid->LVarIds [i] = NCundefined;
	for (i = 0;i < n; ++i)
	{
		if ((status = nc_inq_dimid (ncids [i], NCnameDNTime, &dimid)) != NC_NOERR) 
		{ NCprintNCError (status,"NCdsHandleGridDefine");          goto ABORT; }
		if ((status = nc_inq_dimlen (ncids [i],dimid, &tnum)) != NC_NOERR)
		{ NCprintNCError (status,"NCdsHandleGridDefine");          goto ABORT; }
		if ((status = nc_inq_varid (ncids [i], NCnameDNTime, grid->TVarIds + i)) != NC_NOERR)
		{ NCprintNCError (status,"NCdsHandleGridDefine");          goto ABORT; }

		if (((status = nc_inq_attlen   (ncids [i],grid->TVarIds [i],NCnameVAUnits,&unitlen)) == NC_NOERR) &&
		    ((status = nc_get_att_text (ncids [i],grid->TVarIds [i],NCnameVAUnits,unitstr))  == NC_NOERR))
			unitstr [unitlen] = '\0';
		if (i == 0) grid->DoTUnit = status == NC_NOERR ? true : false;
		if (grid->DoTUnit && (status != NC_NOERR)) grid->DoTUnit = false;
		else if (utScan (unitstr,grid->TUnits + i) != 0)
		{ CMmsgPrint (CMmsgAppError, "Invalid time unit string [%s] in: %s %d",__FILE__,__LINE__,unitstr); grid->DoTUnit = false; }
		else
		{
			if      (strncmp (unitstr,"seconds",strlen ("seconds")) == 0) grid->TSmode = NCtimeSecond;
			else if (strncmp (unitstr,"minutes",strlen ("minutes")) == 0) grid->TSmode = NCtimeMinute;
			else if (strncmp (unitstr,"hours",  strlen ("hours"))   == 0) grid->TSmode = NCtimeHour;
			else if (strncmp (unitstr,"days",   strlen ("days"))    == 0) grid->TSmode = NCtimeDay;
			else if (strncmp (unitstr,"months", strlen ("months"))  == 0) grid->TSmode = NCtimeMonth;
			else if (strncmp (unitstr,"years",  strlen ("years"))   == 0) grid->TSmode = NCtimeYear;
			if ((dateStr = strstr (unitstr,"since")) == (char *) NULL)
			{ CMmsgPrint (CMmsgAppError, "Invalid time unit string [%s] in: %s %d",__FILE__,__LINE__,unitstr); grid->DoTUnit = false; }
			else if (sscanf (dateStr + strlen ("since"),"%04d",&year) != 1)
			{ CMmsgPrint (CMmsgAppError, "Invalid time unit string [%s] in: %s %d",__FILE__,__LINE__,unitstr); grid->DoTUnit = false; }
			grid->Climatology = year == 0 ? true : false;
		}

		grid->NCoffset [i] = grid->TNum;
		if (tnum > 0)
		{
			if (((grid->Times    = (double *) realloc (grid->Times,    (grid->TNum + tnum) * sizeof (double))) == (double *) NULL) ||
			    ((grid->NCindex  = (size_t *) realloc (grid->NCindex,  (grid->TNum + tnum) * sizeof (size_t))) == (size_t *) NULL))
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }

			if ((status = nc_get_var_double (ncids [i],grid->TVarIds [i],grid->Times + grid->TNum)) != NC_NOERR)
			{ NCprintNCError (status,"NCdsHandleGridDefine");       goto ABORT; }

			for (j = 0;j < tnum;++j) grid->NCindex  [grid->TNum + j] = i;
			grid->TNum += tnum;
		}

		if ((status = nc_inq_dimid (ncids [i],NCnameDNLevel, &dimid)) == NC_NOERR) 
		{
			if ((status = nc_inq_dimlen (ncids [i],dimid, &lnum)) != NC_NOERR)
			{ NCprintNCError (status,"NCdsHandleGridDefine");       goto ABORT; }
			if ((status = nc_inq_varid (ncids [i], NCnameDNLevel, grid->LVarIds + i)) != NC_NOERR)
			{ NCprintNCError (status,"NCdsHandleGridDefine");       goto ABORT; }

			if (((status = nc_inq_attlen   (ncids [i],grid->LVarIds [i],NCnameVAUnits,&unitlen)) == NC_NOERR) &&
			    ((status = nc_get_att_text (ncids [i],grid->LVarIds [i],NCnameVAUnits,unitstr))  == NC_NOERR))
				unitstr [unitlen] = '\0';
			if (i == 0)
			{
				grid->DoLUnit = status == NC_NOERR ? true : false;
				if ((grid->Levels = (double *) realloc (grid->Levels, lnum * sizeof (double))) == (double *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }
				if ((status = nc_get_var_double (ncids [i], grid->LVarIds [i], grid->Levels)) != NC_NOERR)
				{	NCprintNCError (status,"NCdsHandleGridDefine");      goto ABORT; }
				grid->LNum = lnum;
			}
			else if (grid->LNum != lnum)
			{ CMmsgPrint (CMmsgAppError, "Inconsistent NetCDF bundle in: %s %d",__FILE__,__LINE__); goto ABORT; }

			if (grid->DoLUnit && (status != NC_NOERR)) grid->DoLUnit = false;
			else if (utScan (unitstr,grid->LUnits + i) != 0)
			{ CMmsgPrint (CMmsgAppError, "Invalid level unit string [%s] in: %s %d",__FILE__,__LINE__,unitstr); grid->DoLUnit = false; }
		}
		else grid->LNum = 1;

	}
	if (grid->TNum > 1)
	{
		scale  = 1.0;
		offset = delta = 0.0;
		x0 = grid->Times [0];
		for (i = 1;i < grid->TNum;++i)
		{
			if (grid->DoTUnit && ((j = grid->NCindex [i]) != 0))
			{
				if (utConvert (grid->TUnits + j,grid->TUnits, &scale, &offset) != 0)
				{ CMmsgPrint (CMmsgAppError, "Invalid time conversion in: %s %d",__FILE__,__LINE__); break; }
			}
			x1 = grid->Times [i] * scale + offset;
			if (i == 1) delta = x1 - x0;
			else if (NCmathEqualValues (delta, x1 - x0) != true)
			{ CMmsgPrint (CMmsgAppError, "Irregular time step in: %s %d",__FILE__,__LINE__); break; }
			x0 = x1;
		}
	}

	if (grid->LNum > 1)
	{
		scale  = 1.0;
		offset = delta = 0.0;
		x0 = grid->Levels [0];
		for (i = 1;i < grid->LNum;++i)
		{
			if (grid->DoLUnit && ((j = grid->NCindex [i]) != 0))
			{
				if (utConvert (grid->LUnits + j,grid->LUnits, &scale, &offset) != 0)
				{ CMmsgPrint (CMmsgAppError, "Invalid level conversion in: %s %d",__FILE__,__LINE__); break; }
			}
			x1 = grid->Levels [i] * scale + offset;
			if (i == 1) delta = x1 - x0;
			else if (NCmathEqualValues (delta,x1 - x0) != true)
			{ CMmsgPrint (CMmsgAppError, "Irregular time step in: %s %d",__FILE__,__LINE__); break; }
			x0 = x1;
		}
	}
	return (NCsucceeded);
ABORT:
	NCdsHandleGridClear (grid);
	return (NCfailed);
}

void NCdsHandleGridClear (NCdsHandleGrid_t *grid)
{
	NCdsHandleGLayoutClear ((NCdsHandleGLayout_t *) grid);
	grid->LNum = grid->TNum = (size_t) 0;
	if (grid->LVarIds != (int *)    NULL) { free (grid->LVarIds); grid->LVarIds = (int *)    NULL; }
	if (grid->TVarIds != (int *)    NULL) { free (grid->TVarIds); grid->TVarIds = (int *)    NULL; }
	if (grid->Times   != (double *) NULL) { free (grid->Times);   grid->Times   = (double *) NULL; }
	if (grid->Levels  != (double *) NULL) { free (grid->Levels);  grid->Levels  = (double *) NULL; }
	if (grid->LUnits  != (utUnit *) NULL) { free (grid->LUnits);  grid->LUnits  = (utUnit *) NULL; }
	if (grid->TUnits  != (utUnit *) NULL) { free (grid->TUnits);  grid->TUnits  = (utUnit *) NULL; }
}

NCstate NCdsHandleGridGetTime (const NCdsHandleGrid_t *grid, size_t layerID, utUnit *tUnit, double *time)
{
	double scale, offset;
	int year, month, day, hour, minute, nMonths;
	float second;

	if (layerID >= grid->TNum) return (NCfailed);
	if (grid->DoTUnit)
	{
		if (grid->TSmode > NCtimeMonth)
		{
			if (utConvert (grid->TUnits,tUnit, &scale, &offset) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			*time = grid->Times [layerID] * scale + offset;
		}
		else
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			if (grid->TSmode == NCtimeYear)
			{
				*time = (double) year + grid->Times [layerID];
				month = 1;
			}
			else
			{
				nMonths = year * 12 + month - 1 + (int) grid->Times [layerID];
				year  = nMonths / 12;
				month = nMonths % 12 + 1;
			}
			day    = 1;
			hour   = 0;
			minute = 0;	
			second = 0.0;
			if (utInvCalendar (year, month, day, hour, minute, second, tUnit, time) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
		}
	}
	else *time = grid->Times [layerID];
	return (NCsucceeded);
}

int NCdsHandleGridGetTimeStep (const NCdsHandleGrid_t *grid, size_t layerID, utUnit *tUnit, double *tStep)
{
	double scale, offset;
	int year, month, day, hour, minute, nMonths;
	float second;
	double time;

	if (grid->TNum < (layerID > 2 ? layerID : 2)) return (NCfailed);
	if (grid->DoTUnit)
	{
		if (grid->TSmode == NCtimeMonth)
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			nMonths = year * 12 + month - 1 + (int) grid->Times [layerID];
			year    = nMonths / 12;
			month   = nMonths % 12 + 1;
			if (utInvCalendar (year, month, day, hour, minute, second, tUnit, &time) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			nMonths += 1;
			year    = nMonths / 12;
			month   = nMonths % 12 + 1;
			if (utInvCalendar (year, month, day, hour, minute, second, tUnit, tStep) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			*tStep -= time;
		}
		else if (grid->TSmode == NCtimeYear)
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }

			year += (int) grid->Times [layerID];
			if (utInvCalendar (year, month, day, hour, minute, second, tUnit, &time) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			if (utInvCalendar (year + 1, month, day, hour, minute, second, tUnit, tStep) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			*tStep -= time;
		}
		else
		{
			if (utConvert (grid->TUnits,tUnit, &scale, &offset) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			*tStep = (grid->Times [1] - grid->Times [0]) * scale;
		}
	}
	else
		*tStep = grid->Times [1] - grid->Times [0];

	return (grid->TSmode);
}

int NCdsHandleGridGetTLayerID (const NCdsHandleGrid_t *grid, utUnit *tUnit, double time)
{
	size_t i;
	double scale, offset;
	int year, month, day, hour, minute;
	float second;

	if (grid->DoTUnit)
	{
		if (grid->TSmode > NCtimeMonth)
		{
			if (utConvert (tUnit, grid->TUnits, &scale, &offset) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			time = time * scale + offset;
		}
		else
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			offset = grid->TSmode == NCtimeYear ? year : year * 12 + month - 1;
			if (utCalendar (time, tUnit, &year, &month, &day, &hour, &minute, &second) != 0)
			{ CMmsgPrint (CMmsgAppError, "Unit conversion error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
			time = grid->TSmode == NCtimeYear ? year - offset : year * 12 + month - 1 - offset;
		}
	}
	for (i = 0;i < grid->TNum;++i) if (time <= grid->Times [i]) break;
	return (i);
}
