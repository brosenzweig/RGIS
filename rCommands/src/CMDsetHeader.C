/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDsetHeader.C

balazs.fekete@unh.edu

*******************************************************************************/
/*
 also modified by pete weissbrod starting april of 2001
 later modified by Fekete, Andras on Feb. 15 2002
 Updated again by Fekete, Andras on Aug. 29 2003
 tweaked again by Fekete, Andras on Jan, 07 2004
*/

#include <stdio.h>
#include <cm.h>
#include <DB.H>
#include <RG.H>

int CMDgetDelim(char **in, size_t *len,char delim, FILE *file)
{
	char ch;
	if(*in == (char *) NULL) *in = (char *) malloc((*len = 1) * sizeof(char));
	else *in = (char *) realloc(*in,(*len = 1) * sizeof(char));
	ch = getc(file);
	while((ch != delim) && (ch != EOF) && (ch != '\0') && (ch != (char) NULL))
	{
		(*in)[*len - 1] = ch; // change last character to 'ch'
		*in = (char *) realloc(*in,++(*len) * sizeof(char));
		ch = getc(file);
	}
	(*in)[*len - 1] = '\0';
	if((ch == (char) NULL) || (ch == '\0')) return -1;
	return (int) (*len) - 1;
}

void CMDgetInfoInteractive(char **in, char *prompt, bool useMultipleLines)
{ 
	char ch;
	size_t len = 2;
	printf("%s ",prompt);
	if ((*in = (char *) realloc (*in,sizeof(char) * len)) == (char *) NULL)
		{ perror ("Memory allocation error in: CMDsetHeader()"); exit(-1); }

	if(useMultipleLines)
	{
		if(CMDgetDelim(in,&len,'$',stdin) == -1) { fprintf(stderr,"Error reading input!\n"); exit(-1); }
		ch = getchar();
		if (ch != '\n') { ungetc(ch,stdin); }
	}
	else if(CMDgetDelim(in,&len,'\n',stdin) == -1) { fprintf(stderr,"Error reading input!\n"); exit(-1); }
}

void CMDshowUsage(char *arg0)
{
	CMmsgPrint (CMmsgInfo,"%s [options] <RiverGIS file>\n",CMprgName(arg0));
	CMmsgPrint (CMmsgInfo,"       => Sets RiverGIS file header informations.\n");
	CMmsgPrint (CMmsgInfo,"       -a,--all\n");
	CMmsgPrint (CMmsgInfo,"       -c,--comment\n");
	CMmsgPrint (CMmsgInfo,"       -i,--citation\n");
	CMmsgPrint (CMmsgInfo,"       -d,--domain\n");
	CMmsgPrint (CMmsgInfo,"       -n,--institute\n");
	CMmsgPrint (CMmsgInfo,"       -i,--interactive\n");
	CMmsgPrint (CMmsgInfo,"       -p,--person\n");
	CMmsgPrint (CMmsgInfo,"       -s,--subject\n");
	CMmsgPrint (CMmsgInfo,"       -S,--source\n");
	CMmsgPrint (CMmsgInfo,"       -t,--title\n");
	CMmsgPrint (CMmsgInfo,"       -v,--version\n");
	CMmsgPrint (CMmsgInfo,"       -V,--verbose\n");
	CMmsgPrint (CMmsgInfo,"       -h,--help\n");
	exit(1);
}

int main(int argc, char* argv[])
{
	bool Interactive_Mode = false, All_Fields = false;
	char *title = (char *) NULL, *subject = (char *) NULL, *domain = (char *) NULL, *version = (char *) NULL;
	char *citation = (char *) NULL, *institute = (char *) NULL, *source = (char *) NULL, *person = (char *) NULL;
	char *comment = (char *) NULL;
	int argPos, argNum = argc, verbose = false;
	FILE *file = (FILE *) NULL;
	DBObjData *dbData = new DBObjData();

	if (argc == 1) { CMDshowUsage(argv[0]); return 0; }

	for (argPos = 1;argPos < argNum; )
	{
		if (CMargTest (argv [argPos],"-a","--all"))
		{
			All_Fields = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-i","--interactive"))
		{
			Interactive_Mode = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-V","--verbose"))
		{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-h","--help")) { CMDshowUsage(argv[0]); return 0; }
		argPos++;
	}
	if(All_Fields)
	{
		CMDgetInfoInteractive(&title,    "title:>",false); 
		CMDgetInfoInteractive(&subject,  "subject:>",false); 
		CMDgetInfoInteractive(&domain,   "domain:>",false); 
		CMDgetInfoInteractive(&version,  "version:>",false); 
		CMDgetInfoInteractive(&citation, "citation (multiple lines finish with $):>",true); 
		CMDgetInfoInteractive(&institute,"institute:>",false); 
		CMDgetInfoInteractive(&person,   "person:>",false); 
		CMDgetInfoInteractive(&source,   "source (multiple lines finish with $):>",true); 
		CMDgetInfoInteractive(&comment,  "comment (multiple lines finish with $):>",true); 
	}
	else
	{
		for (argPos = 1;argPos < argNum; )
		{
			if (CMargTest (argv [argPos],"-t","--title"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
			 	if(Interactive_Mode) CMDgetInfoInteractive(&title,"title:>",false); 
			 	else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing title!\n"); return (CMfailed); }
					title = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-d","--domain"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
				if(Interactive_Mode) CMDgetInfoInteractive(&domain,"domain:>",false); 
				else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing domain!\n"); return (CMfailed); }
					domain = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-s","--subject"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
			 	if(Interactive_Mode) CMDgetInfoInteractive(&subject,"subject:>",false); 
				else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing subject!\n"); return (CMfailed); }
					subject = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-v","--version"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
				if(Interactive_Mode) CMDgetInfoInteractive(&version,"version:>",false); 
				else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing version!\n"); return (CMfailed); }
					version = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-n","--institute"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
				if(Interactive_Mode) CMDgetInfoInteractive(&institute,"institute:>",false); 
				else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing institute!\n"); return (CMfailed); }
					institute = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-p","--person"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
				if(Interactive_Mode) CMDgetInfoInteractive(&person,"person:>",false); 
				else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing person!\n"); return (CMfailed); }
					person = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-i","--citation"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
				if(Interactive_Mode) CMDgetInfoInteractive(&citation,"citation (multiple lines finish with $):>",true); 
				else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing citation!\n"); return (CMfailed); }
					citation = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-c","--comment"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
				if(Interactive_Mode) CMDgetInfoInteractive(&comment,"comment (multiple lines finish with $):>",true); 
				else
				{
					if (argNum <= argPos) { CMmsgPrint (CMmsgUsrError,"Missing comment!\n"); return (CMfailed); }
					comment = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}
			if (CMargTest (argv [argPos],"-S","--source"))
			{
				argNum = CMargShiftLeft (argPos,argv,argNum);
				if(Interactive_Mode) CMDgetInfoInteractive(&source,"source (multiple lines finish with $):>",true); 
				else
				{
					if (argNum < argPos) { CMmsgPrint (CMmsgUsrError,"Missing source!\n"); return (CMfailed); }
					source = argv[argPos];
					argNum = CMargShiftLeft (argPos,argv,argNum);
				}
				continue;
			}	
			if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
				{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
			argPos++;
			}
		}
	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if ((argNum > 1) && (strcmp(argv[1],"-") != 0))
	{
		file = fopen (argv[1],"r");
		if(dbData->Read(file) == DBFault) { delete dbData; return(DBFault); }
		fclose(file);
		file = (FILE *) NULL;
	}
	else
	{
		fprintf(stderr,"No data file, reading from stdin and writing to stdout!\n");
		file = stdout;
		if(dbData->Read(stdin) == DBFault) { delete dbData; return(DBFault); }
	}

	if(title     != (char *) NULL) dbData->Name(title);
	if(domain    != (char *) NULL) dbData->Document(DBDocGeoDomain,domain);
	if(subject   != (char *) NULL) dbData->Document(DBDocSubject,subject);
	if(version   != (char *) NULL) dbData->Document(DBDocVersion,version);
	if(institute != (char *) NULL) dbData->Document(DBDocCitationInst,institute); 
	if(person    != (char *) NULL) dbData->Document(DBDocOwnerPerson,person);
	if(citation  != (char *) NULL) dbData->Document(DBDocCitationRef,citation);
	if(comment   != (char *) NULL) dbData->Document(DBDocComment,comment);
	if(source    != (char *) NULL) dbData->Document(DBDocSourcePerson,source);

	if (file != stdout) file = fopen(argv[1],"wb");
	if(dbData->Write(file) == DBFault) return(-1);
	if (file != stdout) fclose(file);
	if (verbose) RGlibPauseClose ();
	if (Interactive_Mode)
	{
		if (title     != (char *) NULL) delete title;
		if (domain    != (char *) NULL) delete domain;
		if (subject   != (char *) NULL) delete subject;
		if (version   != (char *) NULL) delete version;
		if (institute != (char *) NULL) delete institute; 
		if (person    != (char *) NULL) delete person;
		if (citation  != (char *) NULL) delete citation;
		if (comment   != (char *) NULL) delete comment;
		if (source    != (char *) NULL) delete source;
	}
	return (0);
}
