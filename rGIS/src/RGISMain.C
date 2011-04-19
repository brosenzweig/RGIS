/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGISMain.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<rgis.H>
#include<cm.h>

extern UIMenuItem RGISFileMenu [];
extern UIMenuItem RGISEditMenu [];
extern UIMenuItem RGISAnalyseMenu [];
extern UIMenuItem RGISToolsMenu [];
extern UIMenuItem RGISMetaDBMenu [];
extern UIMenuItem RGISDisplayMenu [];

UIMenuItem RGISMainMenu [] = {
	UIMenuItem ((char *) "File",    UIMENU_NORULE, UIMENU_NORULE, RGISFileMenu),
	UIMenuItem ((char *) "Edit",    UIMENU_NORULE, RGISDataGroup, RGISEditMenu),
	UIMenuItem ((char *) "Analyze", RGISDataGroup, UIMENU_NORULE, RGISAnalyseMenu),
	UIMenuItem ((char *) "Tools",   UIMENU_NORULE, UIMENU_NORULE, RGISToolsMenu),
	UIMenuItem ((char *) "MetaDB",  UIMENU_NORULE, UIMENU_NORULE, RGISMetaDBMenu),
	UIMenuItem ((char *) "Display", UIMENU_NORULE, RGISDataGroup, RGISDisplayMenu),
	UIMenuItem ()};

int main (int argc,char **argv)

	{
	int argPos, argNum = argc;
	char *metaDB = (char *) NULL;
	int spin = true, progress = true;
	const char *modes [] = { (char *) "yes", (char *) "no", NULL };
	int codes [] = { true, false };
	Widget mainForm;
	RGISWorkspace *workspace;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-m","--metadb"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing metadb argument!\n"); return (CMfailed); }
			metaDB = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-p","--progress"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing progress mode!\n");   return (CMfailed); }
			if ((progress = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid progress mode!\n");   return (CMfailed); }
			progress = codes [progress];
			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--spin"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing spin mode!\n");       return (CMfailed); }
			if ((spin = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid spin mode!\n");	     return (CMfailed); }
			spin = codes [spin];
			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-P","--planet"))
			{
			int planet;
			const char *planets [] = { "Earth", "Mars", "Venus", NULL };
			DBFloat radius [] = { 6371.2213, 6371.2213 * 0.53264, 6371.2213 * 0.94886 };

			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing planet!\n");	       return (CMfailed); }
			if ((planet = CMoptLookup (planets,argv [argPos],true)) == DBFault)
				{
				if (sscanf (argv [argPos],"%lf",radius) != 1)
					{ CMmsgPrint (CMmsgUsrError,"Invalid planet!\n");      return (CMfailed); }
				planet = 0;
				}
			DBMathSetGlobeRadius (radius [planet]);
			if ((argNum = CMargShiftLeft (argPos,argv,argc)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			fprintf (stderr,"rgis [options]\n");
			fprintf (stderr,"     -m, --metadb   [meta database]\n");
			fprintf (stderr,"     -p, --progress [yes|no]\n");
			fprintf (stderr,"     -s, --spin     [yes|no]\n");
			fprintf (stderr,"     -P, --planet   [Earth|Mars|Venus|radius]\n");
			fprintf (stderr,"     -h, --help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ fprintf (stderr,"Unknown option: %s!\n",argv [argPos]); return (DBFault); }
		argPos++;
		}

	workspace = new RGISWorkspace;
	mainForm = UIInitialize ((char *) "GHAAS V2.1 - RiverGIS",(char *) "GHAASrgis",(char *) "RGISMain.html",
									 RGISMainMenu,(void *) workspace,&argc,argv,720,500,(bool) spin,(bool) progress);

	UIDataset ((char *) "GHAASrgis",metaDB);

	XtVaSetValues (mainForm,XmNkeyboardFocusPolicy,		XmPOINTER, NULL);

	workspace->Initialize (mainForm);

	if (argNum > 1)
		{
		DBDataset *dataset;
		DBObjData *data;
		DBObjectLIST<DBObjMetaEntry> *metaList;
		DBObjMetaEntry *metaEntry;

		dataset  = UIDataset ();
		metaList = dataset->MetaList ();

		for (argPos = 1;argPos < argNum; ++argPos)
			{
			data = new DBObjData ();
			if (data->Read (argv [argPos]) == DBSuccess)
				{
				workspace->CurrentData (data);
				metaEntry = metaList->Item (data->Name ());
				metaEntry->FileName (data->FileName ());
				}
			else delete data;
			}
		}
	while (UILoop ());
	delete UIDataset ();
	return (DBSuccess);
	}
