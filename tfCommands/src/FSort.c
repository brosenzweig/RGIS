/******************************************************************************

GHAAS F Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

TFSort.c

andras@ob.sr.unh.edu

*******************************************************************************/
#include <cm.h>
#include <Flib.h>
#include <ctype.h>

struct FieldDsc_s
	{
	int ID;
	int Flag;
	char *Name;
	int Type;
	int q;
	struct FieldDsc_s *Prev;
	struct FieldDsc_s *Next;
	};

typedef struct FieldDsc_s FieldDsc;


static FieldDsc *firstDsc = (FieldDsc *) NULL;
static FieldDsc *fDsc;

static int _Compare (const void *ptr0, const void *ptr1)
	{
	char *line0 = *((char **) ptr0),*line1 = *((char **) ptr1);
	char *field0 = (char *) NULL, *field1 = (char *) NULL;
	static int fbSize0 = 0, fbSize1 = 0;
	int ret, num0, num1;
	double no0,no1;
	for (fDsc = firstDsc;fDsc != NULL; fDsc = fDsc->Next)
		{
		if ((field0 = FGetField (line0,fDsc->ID,field0,&fbSize0)) == (char *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Invalid Field in line %s\n",line0); break; }
		if ((field1 = FGetField (line1, fDsc->ID,field1,&fbSize1)) == (char *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Invalid Field in line %s\n",line1); break; }
		switch (fDsc->Type)
			{
			case 1:
				num0 = atoi(field0);
				num1 = atoi(field1);
				if (num0 > num1) { ret = 1; }
				else if (num0 < num1) { ret = -1; }
				else { ret = 0; }
				break;
			case 2:
				no0 = atof(field0);
				no1 = atof(field1);
				if (no0 > no1) { ret = 1; }
				else if (no0 < no1) { ret = -1; }
				else { ret = 0; }
				break;
			case 3:
				ret = strcmp (field0,field1);
				break;
			default:
				CMmsgPrint (CMmsgUsrError,"Cannot define column!\n");
				exit(1);
				break;
			}
		if (fDsc->Flag) { ret = ret * -1; }
		if (ret) {break;}
		}
	return (ret);
	}

int main (int argc, char *argv [])
	{
	char *argStr;
	int argPos, argNum = argc;
	char *buffer = (char *) NULL, *qname = (char *) NULL;
	char *fieldBuffer = (char *) NULL, *name = (char *) NULL;
	char **lines = (char **) NULL, *line = (char *) NULL;
	int i = 0,a = 0, bSize, fbSize = 0, fieldID, rowNum = 0, force = 0;
	FieldDsc *lastDsc = (FieldDsc *) NULL;

	if ((argNum == 1) || ((argNum == 2) && (CMargTest(argv[1],"-h","--help"))))
		{
		printf("Usage: %s [-hvnrf [field | name] [name]] ... < [inputfile] ...\n",argv[0]);
		printf ("Where fieldnames must match fieldnames in first line of the datafile.\n");
		printf ("-a, --ascending field\n\tPrescribes ascending sort order on field \'field\'.\n\tRepeat for multiple fields.\n");
		printf ("-c, --calculate *NOT IMPLEMENTED*\n\tInhibits printing of input columns, printing only enumerated\n\tsort columns.\n");
		printf ("-d, --descending field\n\tPrescribes descending sort order on field \'field\'. \n\tRepeat for multiple fields\n");
		printf ("-s, --string\n\tForce the next sort field to sort as string.\n\tEx: %s -sa field1.\n",argv[0]);
		printf ("-n, --numeric\n\tForce next sort field to sort on numeric values.\n\tEx: %s -nd field2.\n",argv[0]);
		printf ("-i, --integer\n\tForce next sort field to sort as integer.\n\t Ex: %s -id field1.\n",argv[0]);
		printf ("-h, --help\n\tPrint this usage information.\n");
		exit(0);
		}

	if ((buffer = FGetLine (buffer,&bSize,stdin)) == (char *) NULL) 
		{
		perror ("Empty File in: main ()");
		return (CMfailed);
		}
	for (argPos = 1;argPos < argNum; argPos++)
		{
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			printf("Usage: %s [-hvnrf [field | name] [name]] ... < [inputfile] ...\n",argv[0]);
			printf ("Where fieldnames must match fieldnames in first line of the datafile.\n");
			printf ("-a, --ascending field\n\tPrescribes ascending sort order on field \'field\'.\n\tRepeat for multiple fields.\n");
			printf ("-c, --calculate *NOT IMPLEMENTED*\n\tInhibits printing of input columns, printing only enumerated\n\tsort columns.\n");
			printf ("-d, --descending field\n\tPrescribes descending sort order on field \'field\'. \n\tRepeat for multiple fields\n");
			printf ("-s, --string\n\tForce the next sort field to sort as string.\n\tEx: %s -sa field1.\n",argv[0]);
			printf ("-n, --numeric\n\tForce next sort field to sort on numeric values.\n\tEx: %s -nd field2.\n",argv[0]);
			printf ("-i, --integer\n\tForce next sort field to sort as integer.\n\t Ex: %s -id field1.\n",argv[0]);
			printf ("-h, --help\n\tPrint this usage information.\n");
			exit(0);
			}
		}
	for (argPos = 1;argPos < argNum; argPos++)
		{
		if (CMargTest (argv [argPos],"-a","--ascending"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!\n");       return (CMfailed); }
			argStr = argv [argPos];
			if ((qname = (char *) realloc (qname, strlen(argStr)+3)) == (char *) NULL)
            {
            perror ("Memory Allocation Error in: main ()");
            break;
            }
         sprintf (qname,"\"%s\"",argStr);
         if ((fieldID = FGetFieldID (buffer, qname)) == FFault)
            {
            if ((fieldID = FGetFieldID (buffer, argStr)) == FFault)
               {
               CMmsgPrint (CMmsgUsrError,"Invalid Field Name: %s\n",argStr);
               break;
               }
               else { name = argStr; }
               qname = (char *) NULL;
               }
         else { name = qname; qname = (char *) NULL;}
         if ((fDsc = (FieldDsc *) malloc (sizeof (FieldDsc))) == (FieldDsc *) NULL)
            {
            perror ("Memory Allocation Error in: main ()");
            return (CMfailed);
            }
         fDsc->Name = name;
			fDsc->ID = fieldID;
         fDsc->Flag = 0;
         fDsc->Type = force;
         fDsc->Next = (FieldDsc *) NULL;
         if (firstDsc == (FieldDsc *) NULL) { firstDsc = lastDsc = fDsc; fDsc->Prev = (FieldDsc *) NULL; }
         else { lastDsc->Next = fDsc; fDsc->Prev = lastDsc; lastDsc = fDsc; }
         force = 0;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--descending"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!\n");     return (CMfailed); }
			argStr = argv [argPos];
			if ((qname = (char *) realloc (qname, strlen(argStr)+3)) == (char *) NULL)
            {
            perror ("Memory Allocation Error in: main ()");
            break;
            }
         sprintf (qname,"\"%s\"",argStr);
         if ((fieldID = FGetFieldID (buffer, qname)) == FFault)
            {
            if ((fieldID = FGetFieldID (buffer, argStr)) == FFault)
               {
               CMmsgPrint (CMmsgUsrError,"Invalid Field Name: %s\n",argStr);
               break;
               }
               else { name = argStr; }
         	   qname = (char *) NULL;
            }
         else { name = qname; qname = (char *) NULL;}
         if ((fDsc = (FieldDsc *) malloc (sizeof (FieldDsc))) == (FieldDsc *) NULL)
            {
            perror ("Memory Allocation Error in: main ()");
            return (CMfailed);
            }
			fDsc->Name = name;
         fDsc->ID = fieldID;
         fDsc->Flag = 1;
         fDsc->Type = force;
         fDsc->Next = (FieldDsc *) NULL;
         if (firstDsc == (FieldDsc *) NULL) { firstDsc = lastDsc = fDsc; fDsc->Prev = (FieldDsc *) NULL; }
         else { lastDsc->Next = fDsc; fDsc->Prev = lastDsc; lastDsc = fDsc; }
         force = 0;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--string"))
			{
			force = 3;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--numeric"))
			{
			force = 2;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-i","--integer"))
			{
			force = 1;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (argv [argPos][0] == '-')
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		}

	printf ("%s\n",buffer); 
	while ((line = FGetLine(line,&bSize,stdin)) != (char *) NULL)
		{
		if ((lines = (char **) realloc (lines,(rowNum + 1) * sizeof(char *))) == (char **) NULL)
			{ perror ("Memory Allocation Error in: main ()"); return (CMfailed); }
		if ((lines[rowNum] = (char *) malloc (strlen(line) + 1)) == (char *) NULL)
			{ perror ("Memory Allocation Error in: main ()"); return (CMfailed); }
		strcpy (lines[rowNum], line);
		rowNum++;
		}
	fDsc = firstDsc;
	for (i = 0;i < rowNum; ++i)
		{
		while (fDsc != (FieldDsc *) NULL)
			{
			if ((fieldBuffer = FGetField (lines[i],fDsc->ID,fieldBuffer,&fbSize)) == (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Invalid Field in line# %d\n",i); break; }
			if (fDsc->Type == 0)
				{
				for (a = 0;a < (int) strlen (fieldBuffer);++a)
					{
					if (fDsc->Type < 3)
						{
						if ((fieldBuffer[a] < '0') || (fieldBuffer[a] > '9'))
							{
							/*String*/
							fDsc->Type = 3;
							}
						else
							{
							if (fieldBuffer[a] != '.')
								{
								/*Integer*/
								fDsc->Type = 1;
								}
							else
								{
								/*Float*/
								if (fDsc->Type != 2) { fDsc->Type = 2; } else { fDsc->Type = 3; }
								}
							}
						}
					}
				}
			fDsc = fDsc->Next;
			}
		}
	qsort (lines,rowNum,sizeof (char *),_Compare);
	for (i=0;i < rowNum; ++i)
		{
		printf ("%s\n",lines[i]);
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
	}
