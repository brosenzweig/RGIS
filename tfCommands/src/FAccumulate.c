/******************************************************************************

GHAAS F Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

TFAccumulate.c

andras@ob.sr.unh.edu

*******************************************************************************/
#include <cm.h>
#include <Flib.h>

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

int main (int argc, char *argv [])
	{
	char *argStr;
	char *buffer = (char *) NULL, *line = (char *) NULL;
	char *fieldBuffer = (char *) NULL, *name = (char *) NULL;
	char *rename = (char *) NULL;
	int argPos, argNum = argc;
	int bSize = 0, fbSize = 0, rowNum = 1, oldType = 0, a;
	int avg = 0,sum = 0, next = 0, begin = FFalse, fieldID;
	int Type = 0, force = 0, count = 0, typerr = 0;
	double num = 0.0, fsum = 0.0, asum = 0;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--avg"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!\n");       return (CMfailed); }
			argStr = argv [argPos];
			if ((name = (char *) realloc (name, strlen(argStr)+3)) == (char *) NULL)
               {
               perror ("Memory Allocation Error in: main ()");
               break;
               }
			sprintf (name,"\"%s\"",argStr);
			if ((fieldID = FGetFieldID (buffer, name)) == FFault)
				{
				CMmsgPrint (CMmsgUsrError,"###Invalid Field Name: %s creating RowNumbers!###\n",name);
				count = 1;
				if (rename == (char *) NULL) { rename = "RowNum"; }
				break;
				}
			avg = 1;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--sum"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!\n");     return (CMfailed); }
			argStr = argv [argPos];
			if ((name = (char *) realloc (name, strlen(argStr)+3)) == (char *) NULL)
               {
               perror ("Memory Allocation Error in: main ()");
               break;
               }
            sprintf (name,"\"%s\"",argStr);
				if ((fieldID = FGetFieldID (buffer, name)) == FFault)
					{
					CMmsgPrint (CMmsgUsrError,"###Invalid Field Name: %s creating RowNumbers!###\n",name);
					count = 1;
					if (rename == (char *) NULL) { rename = "RowNum"; }
					break;
					}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--rename"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!\n");     return (CMfailed); }
			rename = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-i","--int"))
			{
			force = 1;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--float"))
			{
			force = 2;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-b","--begin"))
			{
			begin = FTrue;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help")) goto Usage;
		if (argv [argPos][0] == '-')
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if ((buffer = FGetLine (buffer,&bSize,stdin)) == (char *) NULL) 
		{
		perror ("Empty File in: main ()");
		return (CMfailed);
		}

	if ((name == (char *) NULL) && (rename == (char *) NULL)) rename = "RowNum";

	if (rename != (char *) NULL)
		{
		if (begin) { printf ("\"%s\"\t%s\n",rename,buffer); } else { printf ("%s\t\"%s\"\n",buffer,rename); }
		} else {
		if (avg)
			{
			if (begin) { printf ("\"avgof(%s)\"\t%s\n",name,buffer); } else { printf ("%s\tavgof(%s)\n",buffer,name); }
			} else {
			if (begin) { printf ("\"sumof(%s)\"\t%s\n",name,buffer); } else { printf ("%s\tsumof(%s)\n",buffer,name); }
			}
		}

	if (name == (char *) NULL)
		{
		name = "ifyouhaveafieldcalledthisyouareafreak";
		count = 1;
		while ((line = FGetLine(line,&bSize,stdin)) != (char *) NULL)
			{
			if (count)
				{
				if (sum == 0) sum = 1;
				if (begin) { printf ("%d\t%s\n",sum,line); } else { printf ("%s\t%d\n",line,sum); }
				sum++;
				}
			}
		exit(0);
		}
	
	while ((line = FGetLine(line,&bSize,stdin)) != (char *) NULL)
		{
		if ((fieldBuffer = FGetField (line,fieldID,fieldBuffer,&fbSize)) == (char *) NULL)
			{
			CMmsgPrint (CMmsgUsrError,"###Invalid Field Name: %s creating RowNumbers!###\n",argStr);
			count = 1;
			if (rename == (char *) NULL) { rename = "RowNum"; }
			exit(1);
			}
		if (count)
			{
			if (sum == 0) sum = 1;
			if (begin) { printf ("%d\t%s\n",sum,line); } else { printf ("%s\t%d\n",line,sum); }
			sum++;
			}
		else
			{
			for (a=0;a <(int) strlen(fieldBuffer);++a)
				{
				if (((fieldBuffer[a] < '0') && (fieldBuffer[a] != '.')) || ((fieldBuffer[a] > '9') && (fieldBuffer[a] != '.')))
					{
					/*String*/
					Type = 3;
					}
				else
					{
					if (fieldBuffer[a] != '.')
						{
						/*Integer*/
						if (Type != 2) { Type = 1;}
						}
					else
						{
						/*Float*/
						if (Type < 2) { Type = 2; }
						}
					}
				if (force) { Type = force; }
				if (Type == 3)
					{
					CMmsgPrint (CMmsgUsrError,"String (or two decimal points) found where Integer or Float was expected in main()!\nfield: \'%s\', row#: %d, col#: %d.\n",fieldBuffer,rowNum,fieldID+1);
					exit(1);
					}
				}
			if (oldType == 0) oldType = Type;
			if (oldType != Type) typerr = 1;
			oldType = Type;
			if (Type == 2)
				{
				num = atof(fieldBuffer);
				if (sum) { fsum = fsum + num + sum; sum = 0;} else { fsum = fsum + num; }
				if (avg)
					{
					if (begin) { printf ("%.3f\t%s\n",fsum / rowNum,line); } else { printf ("%s\t%.3f\n",line,fsum / rowNum); }
					}
				else
					{
					if (begin) { printf ("%.3f\t%s\n",fsum,line); } else { printf ("%s\t%.3f\n",line,fsum); }
					}
				}
			else if (Type == 1)
				{
				next = atoi(fieldBuffer);
				sum = sum + next;
				if (avg)
					{
					asum = (double) sum / (double) rowNum;
					if (begin) { printf ("%.3f\t%s\n",asum,line); } else { printf ("%s\t%f\n",line,asum); }
					}
				else
					{
					if (begin) { printf ("%d\t%s\n",sum,line); } else { printf ("%s\t%d\n",line,sum); }
					}
				}
			else
				{ CMmsgPrint (CMmsgUsrError,"Error with definition of field: %s!Type: %d\n",fieldBuffer,Type);
					exit(1);
				}
			rowNum++;
			}
		}
	if (typerr) { CMmsgPrint (CMmsgUsrError,"##Warning!##> The type of the field has changed, you might\nwant to use the \'i\' and \'f\' flags to force the type of the field!\n"); } 
	return (FSuccess);

	Usage:
		printf("Usage: %s [-raf ... < [inputfile] ...\n",argv[0]);
		printf ("Where fieldnames must match fieldnames in first line of the datafile.\n If it doesn't match, then it creates writes the RowNumber in a new\nfield, which can be defined with '-r', by default it's called 'RowNum'.\n");
		printf ("-s, --sum field\n\tSpecifies field (column) to add.\n");
		printf ("-a, --avg field\n\tSpecifies field (column) to average.\n");
		printf ("-r, --rename name\n\tRenames field.\n");
		printf ("-i, --int\n\tForce input type to integer.\n");
		printf ("-f, --float\n\tForce input type to float.\n");
		printf ("-b, --begin\n\tPuts results in beginning of table. Default is at the end.\n");
		printf ("-h, --help\n\tPrints out this help.\n\n");
	return (FSuccess);
	}
