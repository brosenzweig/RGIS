/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgetHeader.C

andras@ob.sr.unh.edu

*******************************************************************************/
/*
 Aug. 29, 2003
*/

#include <stdio.h>
#include <cm.h>
#include <DB.H>
#include <RG.H>
#define FIELD_SIZE 100000

void Show_Usage(char *);

int main(int argc, char* argv[]){
	bool All_Fields = false;
	char *title    = (char *) NULL, *subject   = (char *) NULL, *domain = (char *) NULL, *version = (char *) NULL;
	char *citation = (char *) NULL, *institute = (char *) NULL, *source = (char *) NULL, *person  = (char *) NULL;
	char *comment  = (char *) NULL, *type      = (char *) NULL, ch = 'x';
	int argPos, argNum = argc, verbose = false;
	FILE *file = (FILE *) NULL;
	DBObjData *dbData = new DBObjData();
	DBDataHeader header;

	if (argc == 1) { Show_Usage(argv[0]); return 0; }

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--all")) {
   		All_Fields = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose")) {
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help")) { Show_Usage(argv[0]); return 0; }
		if ((file == (FILE *) NULL) && (access(argv[argPos],R_OK) == 0)) file = fopen(argv[argPos],"r");
						// ugly but working method to search for input filename.
		argPos++;
		}

	if (file != (FILE *) NULL)
		{
		if (header.Read(file) == DBFault) { delete dbData; return(DBFault); }
		fseek(file,0,SEEK_SET); //go back to beginning of file
		if (dbData->Read(file) == DBFault) { delete dbData; return(DBFault); }
		fclose(file);
		}
	else
		{
		CMmsgPrint (CMmsgUsrError,"No data file, reading from stdin!");
		if (header.Read(stdin) == DBFault) { delete dbData; return(DBFault); }
		if (dbData->Read(stdin) == DBFault) { delete dbData; return(DBFault); }
		}

	if(All_Fields)
		{
		title     = dbData->Name();
		domain    = dbData->Document(DBDocGeoDomain);
		subject   = dbData->Document(DBDocSubject);
		version   = dbData->Document(DBDocVersion);
		type      = DBDataTypeString(header.Type ());
		institute = dbData->Document(DBDocCitationInst); 
		person    = dbData->Document(DBDocOwnerPerson);
		citation  = dbData->Document(DBDocCitationRef);
		comment   = dbData->Document(DBDocComment);
		source    = dbData->Document(DBDocSourcePerson);
		}
	else
		{
		for (argPos = 1;argPos < argNum; ) {
			if (CMargTest (argv [argPos],"-t","--title")) {
				title = dbData->Name();
				if(title == (char *) NULL) title = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-d","--domain")) {
				domain = dbData->Document(DBDocGeoDomain);
				if(domain == (char *) NULL) domain = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-s","--subject")) {
				subject = dbData->Document(DBDocSubject);
				if(subject == (char *) NULL) subject = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-v","--version")) {
				version = dbData->Document(DBDocVersion);
				if(version == (char *) NULL) version = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-n","--institute")) {
				institute = dbData->Document(DBDocCitationInst); 
				if(institute == (char *) NULL) institute = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-p","--person")) {
				person = dbData->Document(DBDocOwnerPerson);
				if(person == (char *) NULL) person = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-i","--citation")) {
				citation = dbData->Document(DBDocCitationRef);
				if(citation == (char *) NULL) citation = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-c","--comment")) {
				comment = dbData->Document(DBDocComment);
				if(comment == (char *) NULL) comment = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}
			if (CMargTest (argv [argPos],"-S","--source")) {
				source = dbData->Document(DBDocSourcePerson);
				if(source == (char *) NULL) source = &ch;
				if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
				continue;
			}	
			if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
				{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv[argPos]); return (CMfailed); }
			argPos++;
			}
		}
	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);


	if (title     == &ch) printf("Title: N/A\n");              else if (title     != (char *) NULL) printf("Title: %s\n",             title);
	if (domain    == &ch) printf("Domain: N/A\n");             else if (domain    != (char *) NULL) printf("Domain: %s\n",            domain);
	if (subject   == &ch) printf("Subject: N/A\n");            else if (subject   != (char *) NULL) printf("Subject: %s\n",           subject);
	if (version   == &ch) printf("Version: N/A\n");            else if (version   != (char *) NULL) printf("Version: %s\n",           version);
	if (type      == &ch) printf("Data Type: N/A\n");          else if (type      != (char *) NULL) printf("Data Type: %s\n",         type);
	if (institute == &ch) printf("Citation Institute: N/A\n"); else if (institute != (char *) NULL) printf("Citation Institute: %s\n",institute); 
	if (citation  == &ch) printf("Citation Reference: N/A\n"); else if (citation  != (char *) NULL) printf("Citation Reference: %s\n",citation);
	if (person    == &ch) printf("Person: N/A\n");             else if (person    != (char *) NULL) printf("Person: %s\n",            person);
	if (source    == &ch) printf("Source: N/A\n");             else if (source    != (char *) NULL) printf("Source: %s\n",            source);
	if (comment   == &ch) printf("Comment: N/A\n");            else if (comment   != (char *) NULL) printf("Comment: %s\n",           comment);

	if (verbose) RGlibPauseClose ();
	return (0);
}

void Show_Usage(char *arg0) {
	CMmsgPrint (CMmsgInfo,"%s [options] <RiverGIS file>",CMprgName(arg0));
	CMmsgPrint (CMmsgInfo,"       => Gets RiverGIS file header informations.");
	CMmsgPrint (CMmsgInfo,"       -a,--all");
	CMmsgPrint (CMmsgInfo,"       -c,--comment");
	CMmsgPrint (CMmsgInfo,"       -C,--citation");
	CMmsgPrint (CMmsgInfo,"       -d,--domain");
	CMmsgPrint (CMmsgInfo,"       -i,--institute");
	CMmsgPrint (CMmsgInfo,"       -p,--person");
	CMmsgPrint (CMmsgInfo,"       -s,--subject");
	CMmsgPrint (CMmsgInfo,"       -S,--source");
	CMmsgPrint (CMmsgInfo,"       -t,--title");
	CMmsgPrint (CMmsgInfo,"       -v,--version");
	CMmsgPrint (CMmsgInfo,"       -V,--verbose");
	CMmsgPrint (CMmsgInfo,"       -h,--help");
	exit(0);
}
