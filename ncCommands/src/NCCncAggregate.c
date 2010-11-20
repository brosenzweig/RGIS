#include <netcdf.h>
#include <NCcore.h>
#include <NCcm.h>
#include <NCstring.h>
#include <NCstdlib.h>
#include <NCmath.h>
#include <NCdsHandle.h>

enum { NONE = 0, HOUR = -1, DAY = -2, MONTH = -3, YEAR = -4 };

void do_help(char *progName)
{
	fprintf(stderr,"Usage: %s [OPTIONS] \"<expression>\"\n",progName);
	fprintf(stderr,"  Flags:\n");
	fprintf(stderr,"  -d,--debug                        => initiate debug output\n");
	fprintf(stderr,"  -f,--file <FILENAME>              => specify a NetCDF file to read data from\n");
	fprintf(stderr,"  -s,--set <INT> <VARNAME>          => specify the interval and the variable to aggregate\n");
	fprintf(stderr,"           <INT> = hour | day | month | year | <time in seconds>\n");
	fprintf(stderr,"  -r,--rename <VARNAME>             => specify a name for the aggregated variable\n");
}

// Checks to see if a new interval has been reached by the difference in the dates
bool NCnewInterval( const char *date1, const char *date2, const int i)
{
	int t1, t2, tmp;
	static int numberOfDays [12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	switch (i)
		{
		case HOUR:
			return strncmp(date1, date2, 11) != 0;
		case DAY:
			return strncmp(date1, date2, 8) != 0;
		case MONTH:
			return strncmp(date1, date2, 5) != 0;
		case YEAR:
			return strncmp(date1, date2, 3) != 0;
		case NONE:
			fprintf(stderr,"Invalid interval!\n");
			abort();
		default:
			if ((t1 = (strlen (date1) > 3) && (sscanf (date1,"%2d",&t1) == 1) ? t1 * 365 * 24 : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }
			if ((t2 = (strlen (date2) > 3) && (sscanf (date2,"%2d",&t2) == 1) ? t2 * 365 * 24 : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }

			if ((tmp = (strlen (date1) >  5) && (sscanf (date1+5, "%2d",&tmp) == 1)? tmp - 1  : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }
			if ((tmp == 1) && (t1 % 400 == 0)) t1 += (numberOfDays[tmp] + 1) * 24; else t1 += numberOfDays[tmp] * 24;
			if ((tmp = (strlen (date2) >  5) && (sscanf (date2+5, "%2d",&tmp) == 1)? tmp - 1  : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }
			if ((tmp == 1) && (t2 % 400 == 0)) t2 += (numberOfDays[tmp] + 1) * 24; else t2 += numberOfDays[tmp] * 24;

			if ((tmp = (strlen (date1) >  8) && (sscanf (date1+8, "%2d",&tmp) == 1)? tmp - 1  : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }
			t1 += tmp * 24;
			if ((tmp = (strlen (date2) >  8) && (sscanf (date2+8, "%2d",&tmp) == 1)? tmp - 1  : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }
			t2 += tmp * 24;

			if ((tmp = (strlen (date1) > 11) && (sscanf (date1+11, "%2d",&tmp) == 1)? tmp     : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }
			t1 += tmp;
			if ((tmp = (strlen (date2) > 11) && (sscanf (date2+11, "%2d",&tmp) == 1)? tmp     : -1) == -1)
				{ fprintf(stderr,"Problem with dates: %s and %s in CMDdsAggregate()\n",date1,date2); exit(-1); }
			t2 += tmp;

			if(t1 > t2) return (t1 - t2) >= i;
			return (t2 - t1) >= i;
		}
}

// *** MAIN

#define cleanup(ret) if(ncid != -1 && nc_close(ncid) != NC_NOERR) fprintf(stderr,"Error closing file: %s!\n",filename);\
							free(dimIDs); printMemInfo(); return ret;

int main(int argc, char* argv[])
{
	char *tmp = (char *) NULL, *filename = (char *) NULL, *rename = (char *) NULL, *varname = (char *) NULL;
	int argPos = 0, argNum = argc, interval = NONE, ncid = 0, *dimIDs = (int *) NULL, ndims, bLen, idx[3], inVar = 0, outVar = 0, i;
	double val;
	NCdsHandle_t *dsHandle;

	if(argNum == 1) { do_help(NCcmProgName(argv[0])); return (NCsucceeded); }
	if ((argNum == 2) && (argv[1][0] == '-'))
	{
		if (NCcmArgTest(argv[1],"-d","--debug")) SetDebug();
		do_help(NCcmProgName(argv[0])); return (NCsucceeded);
	}
	initMemInfo();
	for(argPos = 1; argPos < argNum;)
	{
		if (NCcmArgTest(argv[argPos],"-d","--debug")) { SetDebug(); NCcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCcmArgTest(argv[argPos],"-h","--help")) { do_help(NCcmProgName(argv[0])); cleanup(NCsucceeded); }
		if (NCcmArgTest(argv[argPos],"-f","--file"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			filename = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-r","--rename"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(rename == (char *) NULL) rename = argv[argPos];
			else { fprintf(stderr,"Output field name defined twice!\n"); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-s","--set"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(interval != NONE) { fprintf(stderr,"Interval already set!\n"); cleanup(NCfailed); }
			if(NCmathIsNumber(argv[argPos])) interval = atoi(argv[argPos]);
			else if (strcmp(argv[argPos],"hour") == 0) interval = HOUR;
			else if (strcmp(argv[argPos],"day") == 0) interval = DAY;
			else if (strcmp(argv[argPos],"month") == 0) interval = MONTH;
			else if (strcmp(argv[argPos],"year") == 0) interval = YEAR;
			else { fprintf(stderr,"Undefined time interval '%s'\n",argv[argPos]); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(varname == (char *) NULL) varname = argv[argPos];
			else { fprintf(stderr,"Input field name defined twice!\n"); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); cleanup(NCfailed); }
		argPos++;
	}
	if(interval == NONE) { fprintf(stderr,"Invalid interval!\n"); cleanup(NCfailed); }
	if (filename != (char *) NULL) {
		if(nc_open(filename,NC_WRITE,&ncid) != NC_NOERR)
			{ fprintf(stderr,"Error opening file: %s!\n",filename); return (NCfailed); }
	} else if ((argNum > 1) && (strcmp(argv[1],"-") != 0)) {
		if(nc_open(argv[1],NC_WRITE,&ncid) != NC_NOERR) { fprintf(stderr,"Error opening file: %s!\n",argv[1]); return (NCfailed); }
	} else do_help(NCcmProgName(argv[0]));

	dsHandle = NCdsHandleOpenById (ncid);
/*	if((nc_inq_varid(ncid,varname,&inVar)) != NC_NOERR) { fprintf(stderr,"NC: Error getting varID!\n"); cleanup(NCfailed); }
	if(nc_inq_ndims(ncid,&ndims) != NC_NOERR) { perror("Cannot get the ndims!\n"); cleanup(NCfailed); }
	dimIDs = malloc(sizeof(int) * ndims);
	if((nc_inq_vardimid(ncid,inVar,dimIDs)) != NC_NOERR) { fprintf(stderr,"NC: Error getting dimIDs!\n"); cleanup(NCfailed); }
	if(nc_redef(ncid) != NC_NOERR) { perror("Cannot place into redef mode!\n"); cleanup(NCfailed); }
	fprintf(stderr,"id=%d ndims=%d, dimIDs[0]=%d dimIDs[1]=%d dimIDs[2]=%d\n",ncid,ndims, dimIDs[0],dimIDs[1],dimIDs[2]);
	if(rename == (char *) NULL) {
		if((nc_def_var(ncid,"Aggregate",NC_DOUBLE,ndims,dimIDs,&outVar)) != NC_NOERR)
			{ fprintf(stderr,"NC: Error creating new variable!\n"); cleanup(NCfailed); }
	} else {
		if((nc_def_var(ncid,rename,NC_DOUBLE,ndims,dimIDs,&outVar)) != NC_NOERR)
			{ fprintf(stderr,"NC: Error creating new variable!\n"); cleanup(NCfailed); }
	}
	fprintf(stderr,"id=%d ndims=%d, dimIDs[0]=%d dimIDs[1]=%d dimIDs[2]=%d\n",ncid,ndims, dimIDs[0],dimIDs[1],dimIDs[2]);
	//if(nc_enddef(ncid) != NC_NOERR) { perror("Cannot get out of redef mode!\n"); cleanup(NCfailed); }
	cleanup(NCsucceeded);

	if((nc_inq_dimlen(ncid,dimIDs[0],&bLen)) != NC_NOERR)
		{ fprintf(stderr,"NC: Error getting length of dimension!\n"); cleanup(NCfailed); }
	for(i = 0; i < bLen; i++)
	{ 
		idx[0] = i;
		if(nc_get_var1_double(ncid,inVar,idx, &val) != NC_NOERR)
			{ fprintf(stderr,"NC: There's a problem getting a value!\n"); cleanup(NCfailed); }
		printf("inVar: %d, outVar: %d, val: %f\n",inVar,outVar,val);
	}*/
//	for (i = 0; i < dsHandle->ColNum * dsHandle->RowNum * dsHandle->TimeNum; i++) printf("i: %f\n",data[i]);
	cleanup(NCsucceeded);
}
