#include<unistd.h>
#include<NCdsHandle.h>

int main (int argc,char *argv [])
{
	int argPos, argNum = argc, ret;
	char *template = (char *) NULL;
	char *title    = (char *) NULL, *subject = (char *) NULL, *domain = (char *) NULL;
	NCGdataType dataType;
	int   status, sncid, dncid, tncid, dvarid;

	for (argPos = 1;argPos < argNum; )
	{
		if (NCGcmArgTest (argv [argPos],"-T","--template"))
		{
			NCGcmArgShiftLeft (argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing template!\n");		return (NCGfailed); }
			template  = argv [argPos];
			NCGcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest (argv [argPos],"-h","--help"))
		{
			fprintf (stdout,"%s [options] <ncgis grid> <ncgis grid>\n", NCGcmProgName(argv [0]));
			fprintf (stdout,"     -T,--template\n");
			fprintf (stdout,"     -h,--help\n");
			return (NCGsucceeded);
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
		{
			fprintf (stderr,"Unknown option: %s!\n",argv [argPos]);
			return (NCGfailed);
		}
		argPos++;
	}
	if (template == (char *) NULL)
	{ fprintf (stderr,"%s: Missing template!\n",  NCGcmProgName (argv [0])); return (NCGfailed); }
	if (argNum < 3)
	{ fprintf (stderr,"%s: Too few arguments!\n", NCGcmProgName (argv [0])); return (NCGfailed); }

	if ((status = nc_open (argv [1], NC_NOWRITE, &sncid)) != NC_NOERR)
	{ NCGprintNCError (status,"main"); return (NCGfailed); }

	if ((dataType = NCGdataGetType (sncid)) == NCGfailed) { nc_close (sncid); return (NCGfailed); }
	if ((dataType != NCGtypeGCont) && (dataType != NCGtypeGDisc))
	{
		fprintf (stderr,"%s: Non-grid input coverage!\n", NCGcmProgName (argv [0]));
		nc_close (sncid);
		return (NCGfailed);
	}
	if ((status = nc_open (template, NC_NOWRITE, &tncid)) != NC_NOERR)
	{
		NCGprintNCError (status,"main");
		nc_close (sncid);
		return (NCGfailed);
	}
	if ((dataType = NCGdataGetType (tncid)) == NCGfailed)
	{ nc_close (sncid); nc_close (sncid); return (NCGfailed); }
	if ((dataType != NCGtypeGCont) && (dataType != NCGtypeGDisc) && (dataType != NCGtypeNetwork))
	{
		fprintf (stderr,"%s: Non-grid template coverage!\n", NCGcmProgName (argv [0]));
		nc_close (sncid);
		nc_close (tncid);
		return (NCGfailed);
	}
	if ((dncid = NCGfileCreate (argv [2],tncid)) == NCGfailed)
	{ nc_close (sncid); nc_close (tncid); return (NCGfailed); }

	if ((title    == (char *) NULL) && 
		 ((title   = NCGdataGetTextAttribute (sncid,NC_GLOBAL,NCGnameGATitle))   == (char *) NULL)) title   = "Undefined";
	if (( subject == (char *) NULL) && 
		 ((subject = NCGdataGetTextAttribute (sncid,NC_GLOBAL,NCGnameGASubject)) == (char *) NULL)) subject = "Undefined";
	if ((domain   == (char *) NULL) && 
		 ((domain  = NCGdataGetTextAttribute (sncid,NC_GLOBAL,NCGnameGADomain))  == (char *) NULL)) domain  = "Undefined";

	if (NCGdataSetTextAttribute (dncid,NC_GLOBAL,NCGnameGATitle,  title)   == NCGfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCGfailed); }
	if (NCGdataSetTextAttribute (dncid,NC_GLOBAL,NCGnameGASubject,subject) == NCGfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCGfailed); }
	if (NCGdataSetTextAttribute (dncid,NC_GLOBAL,NCGnameGADomain, domain)  == NCGfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCGfailed); }

	if ((dvarid = NCGfileVarClone (sncid, dncid)) == NCGfailed) 
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCGfailed); }
	ret = NCGGridContSampling (sncid, dncid);
	nc_close (sncid);
	nc_close (tncid);
	nc_close (dncid);
	if (ret == NCGfailed) unlink (argv [2]);
	return (ret);
}
