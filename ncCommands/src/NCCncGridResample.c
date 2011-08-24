#include <string.h>
#include <unistd.h>
#include <cm.h>
#include <NC.h>
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
		if (CMargTest (argv [argPos],"-T","--template"))
		{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing template!"); return (CMfailed); }
			template  = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-h","--help"))
		{
			printf ("%s [options] <ncgis grid> <ncgis grid>", CMprgName(argv [0]));
			printf ("     -T,--template");
			printf ("     -h,--help");
			return (CMsucceeded);
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
		{
			CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv [argPos]);
			return (CMfailed);
		}
		argPos++;
	}
	if (template == (char *) NULL)
	{ CMmsgPrint (CMmsgUsrError, "%s: Missing template!",  CMprgName (argv [0])); return (CMfailed); }
	if (argNum < 3)
	{ CMmsgPrint (CMmsgUsrError, "%s: Too few arguments!", CMprgName (argv [0])); return (CMfailed); }

	if ((status = nc_open (argv [1], NC_NOWRITE, &sncid)) != NC_NOERR)
	{ NCprintNCError (status,"main"); return (CMfailed); }

	if ((dataType = NCdataGetType (sncid)) == CMfailed) { nc_close (sncid); return (CMfailed); }
	if ((dataType != NCtypeGCont) && (dataType != NCtypeGDisc))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Non-grid input coverage!", CMprgName (argv [0]));
		nc_close (sncid);
		return (CMfailed);
	}
	if ((status = nc_open (template, NC_NOWRITE, &tncid)) != NC_NOERR)
	{
		NCprintNCError (status,"main");
		nc_close (sncid);
		return (CMfailed);
	}
	if ((dataType = NCdataGetType (tncid)) == CMfailed)
	{ nc_close (sncid); nc_close (sncid); return (CMfailed); }
	if ((dataType != NCtypeGCont) && (dataType != NCtypeGDisc) && (dataType != NCtypeNetwork))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Non-grid template coverage!", CMprgName (argv [0]));
		nc_close (sncid);
		nc_close (tncid);
		return (CMfailed);
	}
	if ((dncid = NCfileCreate (argv [2],tncid)) == CMfailed)
	{ nc_close (sncid); nc_close (tncid); return (CMfailed); }

	if ((title    == (char *) NULL) && 
		 ((title   = NCdataGetTextAttribute (sncid,NC_GLOBAL,NCnameGATitle))   == (char *) NULL)) title   = "Undefined";
	if (( subject == (char *) NULL) && 
		 ((subject = NCdataGetTextAttribute (sncid,NC_GLOBAL,NCnameGASubject)) == (char *) NULL)) subject = "Undefined";
	if ((domain   == (char *) NULL) && 
		 ((domain  = NCdataGetTextAttribute (sncid,NC_GLOBAL,NCnameGADomain))  == (char *) NULL)) domain  = "Undefined";

	if (NCdataSetTextAttribute (dncid,NC_GLOBAL,NCnameGATitle,  title)   == CMfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (CMfailed); }
	if (NCdataSetTextAttribute (dncid,NC_GLOBAL,NCnameGASubject,subject) == CMfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (CMfailed); }
	if (NCdataSetTextAttribute (dncid,NC_GLOBAL,NCnameGADomain, domain)  == CMfailed)
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (CMfailed); }

	if ((dvarid = NCfileVarClone (sncid, dncid)) == CMfailed) 
	{ nc_close (sncid); nc_close (tncid); nc_close (dncid); unlink (argv [2]); return (CMfailed); }
	ret = NCGridContSampling (sncid, dncid);
	nc_close (sncid);
	nc_close (tncid);
	nc_close (dncid);
	if (ret == CMfailed) unlink (argv [2]);
	return (ret);
}
