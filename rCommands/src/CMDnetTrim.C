/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDnetTrim.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title      = (char *) NULL, *subject = (char *) NULL;
	char *domain     = (char *) NULL, *version = (char *) NULL;
	DBObjData *netData;
   DBNetworkIF *netIF;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!");        return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!");      return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!");       return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!");      return (CMfailed); }
			version  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input network> <output network>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	netData = new DBObjData ();

	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? netData->Read (argv [1]) : netData->Read (stdin);
	if (netData->Type () != DBTypeNetwork)
		{
      CMmsgPrint (CMmsgUsrError,"Not a network input!");
      delete netData;
      return (CMfailed);
      }
   if (title	!= (char *) NULL) netData->Name (title);
	if (subject != (char *) NULL) netData->Document (DBDocSubject,   subject);
	if (domain	!= (char *) NULL) netData->Document (DBDocGeoDomain, domain);
   if (version != (char *) NULL) netData->Document (DBDocVersion,   version);
   
   netIF = new DBNetworkIF (netData);   
   ret = netIF->Trim ();
   delete netIF;

   if (ret == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? netData->Write (argv [2]) : netData->Write (stdout);

	delete netData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
