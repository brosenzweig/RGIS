#include<NCdsHandle.h>

NCGstate NCGdsHandleGridDefine (NCGdsHandleGrid_t *grid, int *ncids, size_t n)
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
	if (n < 1) return (NCGfailed);
	if (NCGdsHandleGLayoutDefine ((NCGdsHandleGLayout_t *) grid, ncids, n) == NCGfailed) return (NCGfailed);

	if ((grid->DataType != NCGtypeGCont) && (grid->DataType != NCGtypeGDisc)) 
	{ fprintf (stderr,"Invalid data in: NCDdsHanderGridDefine ()\n"); goto ABORT; }

	if (((grid->TVarIds  = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((grid->LVarIds  = (int *)    calloc (n, sizeof (int)))    == (int *)    NULL) ||
	    ((grid->NCoffset = (size_t *) calloc (n, sizeof (size_t))) == (size_t *) NULL) ||
		 ((grid->LUnits   = (utUnit *) calloc (n, sizeof (utUnit))) == (utUnit *) NULL) ||
		 ((grid->TUnits   = (utUnit *) calloc (n, sizeof (utUnit))) == (utUnit *) NULL))
	{ perror ("Memory allocation error in: NCGdsHandleGridDefine\n"); goto ABORT; }

	for (i = 0;i < n; ++i) grid->TVarIds [i] = grid->LVarIds [i] = NCGundefined;
	for (i = 0;i < n; ++i)
	{
		if ((status = nc_inq_dimid (ncids [i], NCGnameDNTime, &dimid)) != NC_NOERR) 
		{ NCGprintNCError (status,"NCGdsHandleGridDefine");          goto ABORT; }
		if ((status = nc_inq_dimlen (ncids [i],dimid, &tnum)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGdsHandleGridDefine");          goto ABORT; }
		if ((status = nc_inq_varid (ncids [i], NCGnameDNTime, grid->TVarIds + i)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGdsHandleGridDefine");          goto ABORT; }

		if (((status = nc_inq_attlen   (ncids [i],grid->TVarIds [i],NCGnameVAUnits,&unitlen)) == NC_NOERR) &&
		    ((status = nc_get_att_text (ncids [i],grid->TVarIds [i],NCGnameVAUnits,unitstr))  == NC_NOERR))
			unitstr [unitlen] = '\0';
		if (i == 0) grid->DoTUnit = status == NC_NOERR ? true : false;
		if (grid->DoTUnit && (status != NC_NOERR)) grid->DoTUnit = false;
		else if (utScan (unitstr,grid->TUnits + i) != 0)
		{ fprintf (stderr,"Invalid time unit string [%s] in: NCGdsHandleGridDefine ()\n",unitstr); grid->DoTUnit = false; }
		else
		{
			if      (strncmp (unitstr,"seconds",strlen ("seconds")) == 0) grid->TSmode = NCGtimeSecond;
			else if (strncmp (unitstr,"minutes",strlen ("minutes")) == 0) grid->TSmode = NCGtimeMinute;
			else if (strncmp (unitstr,"hours",  strlen ("hours"))   == 0) grid->TSmode = NCGtimeHour;
			else if (strncmp (unitstr,"days",   strlen ("days"))    == 0) grid->TSmode = NCGtimeDay;
			else if (strncmp (unitstr,"months", strlen ("months"))  == 0) grid->TSmode = NCGtimeMonth;
			else if (strncmp (unitstr,"years",  strlen ("years"))   == 0) grid->TSmode = NCGtimeYear;
			if ((dateStr = strstr (unitstr,"since")) == (char *) NULL)
			{ fprintf (stderr,"Invalid time unit string [%s] in: NCGdsHandleGridDefine ()\n",unitstr); grid->DoTUnit = false; }
			else if (sscanf (dateStr + strlen ("since"),"%04d",&year) != 1)
			{ fprintf (stderr,"Invalid time unit string [%s] in: NCGdsHandleGridDefine ()\n",unitstr); grid->DoTUnit = false; }
			grid->Climatology = year == 0 ? true : false;
		}

		grid->NCoffset [i] = grid->TNum;
		if (tnum > 0)
		{
			if (((grid->Times    = (double *) realloc (grid->Times,    (grid->TNum + tnum) * sizeof (double))) == (double *) NULL) ||
			    ((grid->NCindex  = (size_t *) realloc (grid->NCindex,  (grid->TNum + tnum) * sizeof (size_t))) == (size_t *) NULL))
			{ perror ("Memory allocation error in: NCGdsHandleGridDefine ()\n"); goto ABORT; }

			if ((status = nc_get_var_double (ncids [i],grid->TVarIds [i],grid->Times + grid->TNum)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGdsHandleGridDefine");       goto ABORT; }

			for (j = 0;j < tnum;++j) grid->NCindex  [grid->TNum + j] = i;
			grid->TNum += tnum;
		}

		if ((status = nc_inq_dimid (ncids [i],NCGnameDNLevel, &dimid)) == NC_NOERR) 
		{
			if ((status = nc_inq_dimlen (ncids [i],dimid, &lnum)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGdsHandleGridDefine");       goto ABORT; }
			if ((status = nc_inq_varid (ncids [i], NCGnameDNLevel, grid->LVarIds + i)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGdsHandleGridDefine");       goto ABORT; }

			if (((status = nc_inq_attlen   (ncids [i],grid->LVarIds [i],NCGnameVAUnits,&unitlen)) == NC_NOERR) &&
			    ((status = nc_get_att_text (ncids [i],grid->LVarIds [i],NCGnameVAUnits,unitstr))  == NC_NOERR))
				unitstr [unitlen] = '\0';
			if (i == 0)
			{
				grid->DoLUnit = status == NC_NOERR ? true : false;
				if ((grid->Levels = (double *) realloc (grid->Levels, lnum * sizeof (double))) == (double *) NULL)
				{ perror ("Memory allocation error in: NCGdsHandle.h\n"); goto ABORT; }
				if ((status = nc_get_var_double (ncids [i], grid->LVarIds [i], grid->Levels)) != NC_NOERR)
				{	NCGprintNCError (status,"NCGdsHandleGridDefine");      goto ABORT; }
				grid->LNum = lnum;
			}
			else if (grid->LNum != lnum)
			{ fprintf (stderr,"Inconsistent NetCDF bundle in: NCGdsHandleGridDefine ()\n"); goto ABORT; }

			if (grid->DoLUnit && (status != NC_NOERR)) grid->DoLUnit = false;
			else if (utScan (unitstr,grid->LUnits + i) != 0)
			{ fprintf (stderr,"Invalid level unit string [%s] in: NCGdsHandleGridDefine ()\n",unitstr); grid->DoLUnit = false; }
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
				{ fprintf (stderr,"Invalid time conversion in: NCGdsHandleGridDefine ()\n"); break; }
			}
			x1 = grid->Times [i] * scale + offset;
			if (i == 1) delta = x1 - x0;
			else if (NCGmathEqualValues (delta, x1 - x0) != true)
			{ fprintf (stderr,"Irregular time step in: NCGdsHandleGridDefine ()\n"); break; }
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
				{ fprintf (stderr,"Invalid level conversion in: NCGdsHandleGridDefine ()\n"); break; }
			}
			x1 = grid->Levels [i] * scale + offset;
			if (i == 1) delta = x1 - x0;
			else if (NCGmathEqualValues (delta,x1 - x0) != true)
			{ fprintf (stderr,"Irregular time step in: NCGdsHandleGridDefine ()\n"); break; }
			x0 = x1;
		}
	}
	return (NCGsucceeded);
ABORT:
	NCGdsHandleGridClear (grid);
	return (NCGfailed);
}

void NCGdsHandleGridClear (NCGdsHandleGrid_t *grid)
{
	NCGdsHandleGLayoutClear ((NCGdsHandleGLayout_t *) grid);
	grid->LNum = grid->TNum = (size_t) 0;
	if (grid->LVarIds != (int *)    NULL) { free (grid->LVarIds); grid->LVarIds = (int *)    NULL; }
	if (grid->TVarIds != (int *)    NULL) { free (grid->TVarIds); grid->TVarIds = (int *)    NULL; }
	if (grid->Times   != (double *) NULL) { free (grid->Times);   grid->Times   = (double *) NULL; }
	if (grid->Levels  != (double *) NULL) { free (grid->Levels);  grid->Levels  = (double *) NULL; }
	if (grid->LUnits  != (utUnit *) NULL) { free (grid->LUnits);  grid->LUnits  = (utUnit *) NULL; }
	if (grid->TUnits  != (utUnit *) NULL) { free (grid->TUnits);  grid->TUnits  = (utUnit *) NULL; }
}

NCGstate NCGdsHandleGridGetTime (const NCGdsHandleGrid_t *grid, size_t layerID, utUnit *tUnit, double *time)
{
	double scale, offset;
	int year, month, day, hour, minute, nMonths;
	float second;

	if (layerID >= grid->TNum) return (NCGfailed);
	if (grid->DoTUnit)
	{
		if (grid->TSmode > NCGtimeMonth)
		{
			if (utConvert (grid->TUnits,tUnit, &scale, &offset) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetStartTime ()\n"); return (NCGfailed); }
			*time = grid->Times [layerID] * scale + offset;
		}
		else
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetStartTime ()\n"); return (NCGfailed); }
			if (grid->TSmode == NCGtimeYear)
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
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetStartTime ()\n"); return (NCGfailed); }
		}
	}
	else *time = grid->Times [layerID];
	return (NCGsucceeded);
}

int NCGdsHandleGridGetTimeStep (const NCGdsHandleGrid_t *grid, size_t layerID, utUnit *tUnit, double *tStep)
{
	double scale, offset;
	int year, month, day, hour, minute, nMonths;
	float second;
	double time;

	if (grid->TNum < (layerID > 2 ? layerID : 2)) return (NCGfailed);
	if (grid->DoTUnit)
	{
		if (grid->TSmode == NCGtimeMonth)
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTimeStep ()\n"); return (NCGfailed); }
			nMonths = year * 12 + month - 1 + (int) grid->Times [layerID];
			year    = nMonths / 12;
			month   = nMonths % 12 + 1;
			if (utInvCalendar (year, month, day, hour, minute, second, tUnit, &time) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTimeStep ()\n"); return (NCGfailed); }
			nMonths += 1;
			year    = nMonths / 12;
			month   = nMonths % 12 + 1;
			if (utInvCalendar (year, month, day, hour, minute, second, tUnit, tStep) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTimeStep ()\n"); return (NCGfailed); }
			*tStep -= time;
		}
		else if (grid->TSmode == NCGtimeYear)
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTimeStep ()\n"); return (NCGfailed); }

			year += (int) grid->Times [layerID];
			if (utInvCalendar (year, month, day, hour, minute, second, tUnit, &time) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTimeStep ()\n"); return (NCGfailed); }
			if (utInvCalendar (year + 1, month, day, hour, minute, second, tUnit, tStep) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTimeStep ()\n"); return (NCGfailed); }
			*tStep -= time;
		}
		else
		{
			if (utConvert (grid->TUnits,tUnit, &scale, &offset) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTimeStep ()\n"); return (NCGfailed); }
			*tStep = (grid->Times [1] - grid->Times [0]) * scale;
		}
	}
	else
		*tStep = grid->Times [1] - grid->Times [0];

	return (grid->TSmode);
}

int NCGdsHandleGridGetTLayerID (const NCGdsHandleGrid_t *grid, utUnit *tUnit, double time)
{
	size_t i;
	double scale, offset;
	int year, month, day, hour, minute;
	float second;

	if (grid->DoTUnit)
	{
		if (grid->TSmode > NCGtimeMonth)
		{
			if (utConvert (tUnit, grid->TUnits, &scale, &offset) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTLayerID ()\n"); return (NCGfailed); }
			time = time * scale + offset;
		}
		else
		{
			if (utCalendar ((double) 0.0,grid->TUnits, &year, &month, &day, &hour, &minute, &second) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTLayerID ()\n"); return (NCGfailed); }
			offset = grid->TSmode == NCGtimeYear ? year : year * 12 + month - 1;
			if (utCalendar (time, tUnit, &year, &month, &day, &hour, &minute, &second) != 0)
			{ fprintf (stderr,"Unit conversion error in: NCGdsHandleGridGetTLayerID ()\n"); return (NCGfailed); }
			time = grid->TSmode == NCGtimeYear ? year - offset : year * 12 + month - 1 - offset;
		}
	}
	for (i = 0;i < grid->TNum;++i) if (time <= grid->Times [i]) break;
	return (i);
}
