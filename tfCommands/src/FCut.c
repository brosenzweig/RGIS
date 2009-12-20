/******************************************************************************

GHAAS F Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

TFCut.c

andras@ob.sr.unh.edu

*******************************************************************************/
#include <cm.h>
#include <Flib.h>

struct FieldDsc_s
	{
	int ID;
	int Keep;
	char *Name;
	struct FieldDsc_s *Prev;
	struct FieldDsc_s *Next;
	};

typedef struct FieldDsc_s FieldDsc;

int main (int argc, char *argv [])
	{
	char *buffer = (char *) NULL, *optarg;
	char *fieldBuffer = (char *) NULL, *name = (char *) NULL;
	char *qname = (char *) NULL;
	int argPos, argNum = argc;
	int bSize = 0, fbSize = 0;
	int inverse = FFalse;
	int keep, fieldID;
	FieldDsc *firstDsc = (FieldDsc *) NULL, *lastDsc = (FieldDsc *) NULL;
	FieldDsc *revFirstDsc = (FieldDsc *) NULL, *revLastDsc = (FieldDsc *) NULL;
	FieldDsc *fDsc, *rfDsc;

	if ((buffer = FGetLine (buffer,&bSize,stdin)) == (char *) NULL) 
		{ perror ("Empty File in: main ()"); return (CMfailed); }

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-f","--field"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing field!\n");       return (CMfailed); }
			optarg = argv [argPos];
			if ((qname = (char *) realloc (qname, strlen(optarg)+3)) == (char *) NULL)
				{ perror ("Memory Allocation Error in: main ()"); return (CMfailed); }
			sprintf (qname,"\"%s\"",optarg);
			name = (char *) NULL;
			if ((fieldID = FGetFieldID (buffer, qname)) == FFault)
				{
				if ((fieldID = FGetFieldID (buffer, optarg)) == FFault)
					{ CMmsgPrint (CMmsgUsrError,"Invalid Field Name: %s\n",optarg); return (CMfailed); }
				else name = optarg;
				}
			else name = qname;
			qname = (char *) NULL;
			if ((fDsc = (FieldDsc *) malloc (sizeof (FieldDsc))) == (FieldDsc *) NULL)
				{ perror ("Memory Allocation Error in: main ()"); return (CMfailed); }
			fDsc->ID = fieldID;
			fDsc->Name = name;
			fDsc->Keep = keep;
			fDsc->Next = (FieldDsc *) NULL;
			if (firstDsc == (FieldDsc *) NULL) { firstDsc = lastDsc = fDsc; fDsc->Prev = (FieldDsc *) NULL; }
			else { lastDsc->Next = fDsc; fDsc->Prev = lastDsc; lastDsc = fDsc; }
			keep = FFalse;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--rename"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing New Name!\n");    return (CMfailed); }
			name = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--nfield"))
			{
			keep = FTrue;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--inverse"))
			{
			inverse = FTrue;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help")) goto Usage;
		if (argv [argPos][0] == '-')
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (inverse)
		{
		for (fieldID = 0;FGetFieldPos (buffer,fieldID) != FFault;++fieldID)
			{
			for (fDsc = firstDsc;fDsc != (FieldDsc *) NULL; fDsc = fDsc->Next)
				if (fDsc->ID == fieldID) break;
			if ((fDsc == (FieldDsc *) NULL) || (fDsc->Keep == FTrue))
				{
				if (fDsc != (FieldDsc *) NULL)
					{
					if ((name = (char *) malloc (strlen (fDsc->Name) + 1)) == (char *) NULL)
						{ perror ("Memory Allocation Error in: main ()"); return (CMfailed); }
					strcpy (name,fDsc->Name);
					}
				else
					{
					if ((fieldBuffer = FGetField (buffer,fieldID,fieldBuffer,&fbSize)) == (char *) NULL)
						{ CMmsgPrint (CMmsgUsrError,"Total Gebasz in: main ()"); return (CMfailed); }
					if ((name = (char *) malloc (strlen (fieldBuffer) + 1)) == (char *) NULL)
						{ perror ("Memory Allocation Error in: main ()"); return (CMfailed); }
					strcpy (name,fieldBuffer);
					}
				if ((rfDsc = (FieldDsc *) malloc (sizeof (FieldDsc))) == (FieldDsc *) NULL)
					{ perror ("Memory Allocation Error in: main ()"); return (CMfailed); }
				rfDsc->ID = fieldID;
				rfDsc->Next = (FieldDsc *) NULL;
				rfDsc->Name = name;
				if (revFirstDsc == (FieldDsc *) NULL)
					{ revFirstDsc = revLastDsc = rfDsc; rfDsc->Prev = (FieldDsc *) NULL; }
				else
					{ revLastDsc->Next = rfDsc; rfDsc->Prev = revLastDsc; revLastDsc = rfDsc; }
				}
			if (fDsc != (FieldDsc *) NULL)
				{
				if (fDsc->Next == (FieldDsc *) NULL) lastDsc  = fDsc->Prev;
				else { fDsc->Next->Prev = fDsc->Prev; }
				if (fDsc->Prev == (FieldDsc *) NULL) firstDsc = fDsc->Next;
				else fDsc->Prev->Next = fDsc->Next;
				free (fDsc);
				}
			}
		firstDsc = revFirstDsc;
		lastDsc  = revLastDsc;
		}

	if (firstDsc->Name[0] == '"') { printf ("%s",firstDsc->Name); } else { printf ("\"%s\"",firstDsc->Name); }
	fDsc = firstDsc->Next;
	while (fDsc != (FieldDsc *) NULL)
		{
		if (fDsc->Name[0] == '"') { printf ("\t%s",fDsc->Name); } else { printf ("\t\"%s\"",fDsc->Name); }
		fDsc = fDsc->Next;
		}
	printf ("\n");

	while ((buffer = FGetLine (buffer,&bSize,stdin)) != (char *) NULL)
		{
		if (firstDsc != (FieldDsc *) NULL)
			{
			if ((fieldBuffer = FGetField (buffer,firstDsc->ID,fieldBuffer,&fbSize)) == (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Total Gebasz\n"); return (CMfailed); }
			printf ("%s",fieldBuffer);
			fDsc = firstDsc->Next;
			}
		while (fDsc != (FieldDsc *) NULL)
			{
			if ((fieldBuffer = FGetField (buffer,fDsc->ID,fieldBuffer,&fbSize)) == (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Total Gebasz\n"); return (CMfailed); }
			printf ("\t%s",fieldBuffer);
			fDsc = fDsc->Next;
			}
		printf ("\n");
		}

	fDsc = firstDsc->Next;
	free (firstDsc);

	while (fDsc != (FieldDsc *) NULL)
		{
		firstDsc = fDsc;
		fDsc = fDsc->Next;
		free (firstDsc);
		}

	return (FSuccess);

	Usage:
		printf("Usage: %s [-hvnrf [field | name] [name]] ... < [inputfile] ...\n",argv[0]);
		fprintf(stderr,"where fieldnames must match fieldnames in first line of the datafile.");
		fprintf(stderr,"-f, --field  field\n\tSpecifies field (column) to display.  Repeat for multiple fields.\n");
		fprintf(stderr,"-h, --help\n\tPrint this usage information.\n");
		fprintf(stderr,"-r, --rename  name\n\tRename field.  Sets the field name of the next `-f' or '-n' field.\n\tIf negated operation (`-v') is requested, '-n' or '--nfield' must be used.\n");
		fprintf(stderr,"-v, --inverse\n\tNegated operation, suppressing given fields only.  Ignored if\n\tno fields are given.\n\n");

	return (FSuccess);
	}
