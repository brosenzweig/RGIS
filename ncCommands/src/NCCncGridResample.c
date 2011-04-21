#include <cm.h>
#include <unistd.h>
#include <NCdsHandle.h>

int main (int argc,char *argv [])
{
	int argPos, argNum = argc, ret;
	char *template = (char *) NULL;
	char *title    = (char *) NULL, *subject = (char *) NULL, *domain = (char *) NULL;
	NCdataType dataType;
	int   status, sncid, dncid, tncid, dvarid;

	for (argPos = 1;argPos < argNum; )
	{
		if (NCcmArgTest (argv [argPos],"-T","--template"))
		{
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing template!");		return (NCfailed); }
			template  = argv [argPos];
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest (argv [argPos],"-h","--help"))
		{
			printf ("%s [options] <ncgis grid> <ncgis grid>", NCcmProgName(argv [0]));
			printf ("     -T,--template");
			printf ("     -h,--help");
			return (NCsucceeded);
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
		{
			CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv [argPos]);
			return (NCfailed);
		}
		argPos++;
	}
	if (template == (char *) NULL)
	{ CMmsgPrint (CMmsgUsrError, "%s: Missing template!",  NCcmProgName (argv [0])); return (NCfailed); }
	if (argNum < 3)
	{ CMmsgPrint (CMmsgUsrError, "%s: Too few arguments!", NCcmProgName (argv [0])); return (NCfailed); }

	if ((status = nc_open (argv [1], NC_NOWRITE, &sncid)) != NC_NOERR)
	{ NCprintNCError (status,"main"); return (NCfailed); }

	if ((dataType = NCdataGetType (sncid)) == NCfailed) { nc_close (sncid); return (NCfailed); }
	if ((dataType != NCtypeGCont) && (dataType != NCtypeGDisc))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Non-grid input coverage!", NCcmProgName (argv [0]));
		nc_close (sncid);
		return (NCfailed);
	}
	if ((status = nc_open (template, NC_NOWRITE, &tncid)) != NC_NOERR)
	{
		NCprintNCError (status,"main");
		nc_close (sncid);
		return (NCfailed);
	}
	if ((dataType = NCdataGetType (tncid)) == NCfailed)
	{ nc_close (sncid); nc_close (sncid); return (NCfailed); }
	if ((dataType != NCtypeGCont) && (dataType != NCtypeGDisc) && (dataType != NCtypeNetwork))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Non-grid template coverage!", NCcmProgName (argv [0]));
		nc_close (sncid);
		nc_close (tncid);
		return (NCfailed);
	}
	if ((dncid = NCfileCreate (argv [2],tncid)) == NCfailed)
	{ nc_close (sncid); nc_close (tncid); return (NCfailed); }

	if ((title    == (char *) NULL) && 
		 ((title   = NCdataGetTextAttribute (sncid,NC_GLOBAL,NCnameGATitle))   == (char *) NULL)) title   = "Undefined";
	if (( subject == (char *) NULL) && 
		 ((subject = NCdataGetTextAttribute (sncid,NC_GLOBAL,NCnameGASubject)) == (char *) NULL)) subject = "Undefined";
	if ((domain   == (char *) NULL) && 
		 ((domain  = NCdataGetTextAttribute (sncid,NC_GLOBAL,NCnameGADomain))  == (char *) NULL)) domain  = "Undefined";

	if (NCdataSetTextAttribute (dncid,NC_GLOBAL,NCnameGATitle,  title)   == NCfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCfailed); }
	if (NCdataSetTextAttribute (dncid,NC_GLOBAL,NCnameGASubject,subject) == NCfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCfailed); }
	if (NCdataSetTextAttribute (dncid,NC_GLOBAL,NCnameGADomain, domain)  == NCfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCfailed); }

	if ((dvarid = NCfileVarClone (sncid, dncid)) == NCfailed) 
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (NCfailed); }
	ret = NCGridContSampling (sncid, dncid);
	nc_close (sncid);
	nc_close (tncid);
	nc_close (dncid);
	if (ret == NCfailed) unlink (argv [2]);
	return (ret);
}
