/******************************************************************************

GHAAS F Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

FStat.c

balazs.fekete@unh.edu

*******************************************************************************/

/*
  Fstat (formerly known as statf) version 3.0
  Written by Pete weissbrod for complex systems UNH
  pfw@unh.edu
  Revisited by: Fekete Andras
*/

#include <cm.h>
#include <Flib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

void showUsage()
	{
	printf("FStat - statistical operations on a data file\n");
	printf("USAGE: FStat <inputfile>  <option> <rownumber>\n");
	printf("\tOPTIONS:\n");
	printf("\t-d,--data:<filename>\tread in and verify the expected dataTypes\n");
	printf("\t-r,--rgis:\toutput in RGISDB format\n");
	printf("\t-g,--group:<columnNumber>\tgroup by column number\n");
	printf("\t-n,--num:\treturn the number of rows in the column\n");
	printf("\t-m,--min:<columnNumber>\treturn the minimum value for the column\n");
	printf("\t-M,--max:<columnNumber>\treturn the maximum value for the column\n");
	printf("\t-a,--avg:<columnNumber>\treturn the average value for the column\n");
	printf("\t-s,--sum:<columnNumber>\treturn the sum value for the column\n");
	printf("\t-p,--pct:<q1,q2,...> <columnNumber>\treturn a set of quantiles for a column\n");
	printf("\t-p,--pct all <columnNumber>\treturns a the original table with a new column of quantiles for <columnNumber>\n");
	printf("\t-e,--med:<columnNumber>\treturn the median value for the column\n");
	printf("\t-o,--mod:<columnNumber>\treturn the modal value for the column\n");
	printf("\nEXAMPLES:\n");
	printf("%% FStat --min 0 --pct 1,5,25,50,75,95,100 5 test.txt\n");
	printf("  -output the minimum value of column 0 and a set of quantiles for column 5 all out of \"test.txt\"\n");
	printf("%% FStat --num --pct all 5 < test.txt\n");
	printf("  -this will output the number of lines in \"test.txt\", and then output the table with\n   a new column of quantiles for every entry in column 5.\n");
	}

/**********globals********/
int debug = 1;
int* groupVar = NULL; //the listing of field numbers to be grouped by
int opField = -1; //the field undergoing the current operation
const fDataType* types; //the listing of data types for each field 
FILE* dataFile = NULL; //this will point to the actual data file
FILE* dataTypesFile = NULL; //this points to the data types file (If there is one)

/********functions*********/
int compareCell( const char* cellA, const char* cellB, const int fieldNum );
int compareRow( const void* A, const void* B );
void setInputFiles( int argc, char* argv [] );
fData* sort( fData* theData, const int colNumber );
float* readPctFields( char* command );
int* readGroupFields( const fData* theData, char* command );

void num( fData* theData );
void min( fData* theData, const int colNumber );
void max( fData* theData, const int colNumber );
void avg( fData* theData, const int colNumber );
void sum( fData* theData, const int colNumber );
void pct( fData* theData, float* pctMark, const int colNumber );
void med( fData* theData, const int colNumber );
void mod( fData* theData, const int colNumber );
void dev( fData* theData, const int colNumber );


/********code**************/
/* findarg is a simple serch function that finds arguments in argv[]*/
int findArg( char* shortName, char* longName, int argc, char* argv[] )
	{
	int i = 1;
	while( i < argc )
		{ if( argv[i] && (!strcmp(argv[i],shortName) || !strcmp(argv[i],longName )) ) return i; i++; }
	return 0; //search failed
	}

/* setInputFiles is a function that automatically finds the data file and the
   data type file (if there is one)*/

void setInputFiles( int argc, char* argv [] )
	{
	int i,j;
	/* trying to open the data types file */
	if( (i = findArg( "-d", "--data", argc, argv )) && (i + 1 < argc) &&
		access( argv[i+1], R_OK ) == 0 ) dataTypesFile = fopen( argv[i+1], "rb" );
	
	/* trying to open the data file */
	for( j=1;j<argc;j++ )
		{
		if( access( argv[j], R_OK ) == 0 )
			if( !dataTypesFile || j != i+1 ) dataFile = fopen( argv[j], "rb" );
		}
	if( dataFile == NULL ) dataFile = stdin;
	}


/***********************************************************************************/
/* MAIN */
/***********************************************************************************/
int main( int argc, char* argv[] )
	{
	fData* theData;
	int errCheck;
	int argPos, argNum = argc;
	
	if(argc<=1) { showUsage(); return 0; }
	
	setInputFiles( argc,argv );
	
	theData = readFile( dataFile, dataTypesFile, &errCheck );
	if(errCheck) fprintf(stderr,"errors were reported, but continuing anyway.\n");
	
	/*set the data types for sorting*/
	types = theData->types;
	
	/*iterate through the list of commands from left to right, executing as you go*/
	for (argPos = 1;argPos < argNum; )
		{
		/*set the grouping variable (if there is one) from the command line*/
		if (CMargTest (argv [argPos],"-g","--group"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing group!\n");      return (CMfailed); }
			groupVar = readGroupFields(theData, argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the NUM command
		if (CMargTest (argv [argPos],"-n","--num"))
			{
			num( theData );
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the MIN command
		if (CMargTest (argv [argPos],"-m","--min"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing MIN value!\n");   return (CMfailed); }
			if(atoi(argv[argPos]) < theData->numCols ) min( theData, atoi(argv[argPos]) );
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the MAX command
		if (CMargTest (argv [argPos],"-M","--max"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing MAX value!\n"); return (CMfailed); }
			if(atoi(argv[argPos]) < theData->numCols ) max( theData, atoi(argv[argPos]) ); 
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the AVG command
		if (CMargTest (argv [argPos],"-a","--avg"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing AVG value!\n");   return (CMfailed); }
			if(atoi(argv[argPos]) < theData->numCols ) avg( theData, atoi(argv[argPos]) ); 
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the SUM command
		if (CMargTest (argv [argPos],"-s","--sum"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing SUM value!\n");   return (CMfailed); }
			if(atoi(argv[argPos]) < theData->numCols ) sum( theData, atoi(argv[argPos]) ); 
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the PCT command
		if (CMargTest (argv [argPos],"-p","--pct"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
			if (argPos >= argNum) { CMmsgPrint (CMmsgUsrError,"Missing PCT value!\n"); return (CMfailed); }
			if(!strcmp(argv[argPos],"all"))
				{
				if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
					{ CMmsgPrint (CMmsgUsrError,"Missing pct argument!\n");   return (CMfailed); }
				if(atoi(argv[argPos]) > theData->numCols) break;
				pct( theData, NULL, atoi(argv[argPos]) ); 
				}
			else
				{
				if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
					{ CMmsgPrint (CMmsgUsrError,"Missing pct argument!\n");   return (CMfailed); }
				if(atoi(argv[argPos+1]) > theData->numCols) break;
				pct( theData, readPctFields(argv[argPos]), atoi(argv[argPos+1]) );
				}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the MED command
		if (CMargTest (argv [argPos],"-e","--med"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing MED value!\n");   return (CMfailed); }
			if(atoi(argv[argPos]) < theData->numCols ) med( theData, atoi(argv[argPos]) );
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the MOD command
		if (CMargTest (argv [argPos],"-o","--mod"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing MOD value!\n");   return (CMfailed); }
			if(atoi(argv[argPos]) < theData->numCols ) mod( theData, atoi(argv[argPos]) ); 
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		//read and execute the DEV command
		if (CMargTest (argv [argPos],"-d","--dev"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing PCT value!\n");   return (CMfailed); }
			if(atoi(argv[argPos]) < theData->numCols ) dev( theData, atoi(argv[argPos]) );
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (argv [argPos][0] == '-')
         { CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv[argPos]); return (CMfailed); }
      argPos++;
		}
	freeFData( theData );
	
	return 0;
	}
void min( fData* theData, const int colNumber )
	{
	theData = sort( theData, colNumber );
	printf("min for %s: %s\n", theData->fieldNames[colNumber], theData->data[0][colNumber]);
	}
void max( fData* theData, const int colNumber )
	{
	theData = sort( theData, colNumber );
	printf("max for %s: %s\n", theData->fieldNames[colNumber], theData->data[(theData->numRows-1)][colNumber]);
	}
void num( fData* theData )
	{
	printf("the table has %d records\n", theData->numRows);
	}
void avg( fData* theData, const int colNumber )
	{
	double sum = 0;
	int i;
	theData = sort( theData, colNumber );
	for(i=0;i<theData->numRows;i++) sum += strtod( theData->data[i][colNumber], NULL );
	sum = sum / theData->numRows;
	printf("The average for %s is %19.19e\n", theData->fieldNames[colNumber], sum);
	}

void sum( fData* theData, const int colNumber )
	{
	double sum = 0;
	int i;
	theData = sort( theData, colNumber );
	for(i=0;i<theData->numRows;i++) sum += strtod( theData->data[i][colNumber], NULL );
	printf("The sum for %s is %19.19e\n", theData->fieldNames[colNumber], sum);
	}

void pct( fData* theData, float* pctMark, const int colNumber )
	{ 
	int i,j;
	theData = sort( theData, colNumber );
	
	if(!pctMark) /*--pct "all"*/
		{
		for(j=0;j<theData->numCols;j++)
		printf("%s\t",theData->fieldNames[j]);
		printf("quantiles (sorted by %s)\n", theData->fieldNames[colNumber]);
		
		for(i=0;i<theData->numRows;i++)
			{
			  for(j=0;j<theData->numCols;j++)
			    printf("%s\t",theData->data[i][j]);
			  printf("%f\n",i/(float)theData->numRows*100.0);
			}
		}
	else /*--pct "q1,q2,q3,...*/
		{
		float currentNum;
		printf("quantiles for %s\n-------------\n",  theData->fieldNames[colNumber]);
		j = 0;
		while(j++ < pctMark[0] )
			{
			  currentNum = pctMark[j];
			  for( i=0; (((float)i/((float)theData->numRows)) * 100.0) < currentNum && (i+1) < theData->numRows; i++ );
			  printf("%f: %s\n",currentNum, theData->data[i][colNumber]);
			}
		}
	}

void med( fData* theData, const int colNumber )
	{
	int i;
	theData = sort( theData, colNumber );
	for(i=0;i<(theData->numRows/2);i++);
	printf("The med for %s is %s\n", theData->fieldNames[colNumber], theData->data[i][colNumber]);
	}

void mod( fData* theData, const int colNumber )
	{
	int i = 0, highCount = 0, currentCount = 0;
	char* currentVal;
	char* highVal;
	
	theData = sort( theData, colNumber );
	currentVal = theData->data[0][colNumber];
	highVal = theData->data[0][colNumber];
	
	for(i=0;i<theData->numRows;i++)
		{
		if( strcmp(theData->data[i][colNumber], currentVal) )
			{ 
			  if(currentCount > highCount)
			    { highVal = currentVal; highCount = currentCount; }
			  currentVal = theData->data[i][colNumber]; 
			  currentCount = 0;
			}
		else currentCount++;
		}
	printf("The mode for %s is %s\n", theData->fieldNames[colNumber], highVal);
	}

void dev( fData* theData, const int colNumber )
	{ /*note this is NON-BIASED standard deviation*/
	float avg = 0, sum = 0;
	int i = 0;
	
	theData = sort( theData, colNumber );
	/*pass 1: get the average*/
	for(i=0;i<theData->numRows;i++) sum += strtod(theData->data[i][colNumber],NULL);
	avg = sum/theData->numRows;
	/*pass 2: sum the deviations*/
	sum = 0;
	for(i=0;i<theData->numRows;i++)
		sum += ((strtod(theData->data[i][colNumber],NULL) - avg) * (strtod(theData->data[i][colNumber],NULL) - avg));
	avg = sum/(theData->numRows - 1);
	printf("The variance for %s is %f\n", theData->fieldNames[colNumber], avg);
	}

fData* sort( fData* theData, const int colNumber )
	{
	opField = colNumber;
	if( opField < theData->numCols )
	qsort( theData->data, theData->numRows, (sizeof(char**)), compareRow );
	opField = -1;
	return theData;
	}

/*
  in order to sort the rows, we have to take a few things into consideration:
  1. the operation field --TOP sorting priority
  2. the groupings-- in order of their priority
  3. the data types. numbers cant be sorted with strcmp.
*/

int compareRow( const void* A, const void* B )
	{
	int numberOfGroupings;
	int comp;
	char*** rA = (char***)A;
	char*** rB = (char***)B;
	char** rowA = *rA;
	char** rowB = *rB;
	
	if(opField != (-1)) /* the sorting by the operation field is the highest priority */
	if((comp = compareCell(rowA[opField],rowB[opField],opField)) != 0) return comp;
	
	/* either the opField is -1, or compareCell returned 0 */
	if( groupVar )/*if we are dealing with grouping*/
		{
		numberOfGroupings = 0;
		
		while( numberOfGroupings <= groupVar[0] )
			{
			  if( (comp =  compareCell(
			rowA[(groupVar[numberOfGroupings])], 
			rowB[(groupVar[numberOfGroupings])], 
			(groupVar[numberOfGroupings]) 
			)) != 0
			  )
			    return comp; 
			  numberOfGroupings++;
			}
		}
	/* if the rows are ultimately equal by all of the factors */
	return 0;
	}

int compareCell( const char* cellA, const char* cellB, const int fieldNum )
	{
	float a,b;
	switch(types[fieldNum].type)
		{
		case(DATE): /*not sure what to do here yet*/
		case(STRING): return strcmp(cellA, cellB);
		case(INT): /*dealing with numbers*/
		case(FLOAT):
		sscanf(cellA,"%f",&a);
		sscanf(cellB,"%f",&b);
		if( a == b ) return 0;
		if( a >  b ) return 1;
		}
	return -1;
	}

float* readPctFields( char* command )
	{
	int numFields = 1, i,j;
	float* res;
	char* parse = command;
	char currentNum[10];
	
	if(!strcmp(command,"all")) return NULL;
	
	/* count the commas to get the number of percentiles*/
	while( *parse != '\0' )
		{ 
		if(*parse==',') 
		numFields++;
		parse++;
		}
	
	res = (float*)malloc(sizeof(float) * numFields + 1);
	res[0] = numFields;
	parse = command;
	/* read the numbers in and try to convert them to float*/
	for(i=1;i<=numFields;i++)
		{
		j=0;
		while( *parse != ',' && *parse != '\0' )
			{ currentNum[j++] = *parse; parse++; }
		currentNum[j] = '\0';
		res[i] = strtod( currentNum, NULL );
		parse++;
		}
	return res;
	}

int* readGroupFields( const fData* theData, char* command )
	{
	int numFields = 1,i,j;
	int* ret;
	char* parse = command;
	char currentNum[10];
	
	/* count the commas to get the number of percentiles*/
	while( *parse != '\0' )
		{ 
		if(*parse==',') 
		numFields++;
		parse++;
		}
	
	ret = (int*)malloc(sizeof(int) * (numFields + 1));
	ret[0] = numFields;
	parse = command;
	/* read the numbers in and try to convert them to float*/
	for(i=1;i<=numFields;i++)
		{
		j=0;
		while( *parse != ',' && *parse != '\0' )
			{ currentNum[j++] = *parse; parse++; }
		currentNum[j] = '\0';
		if(atoi(currentNum)<= theData->numCols)
		ret[i] = atoi(currentNum);
		parse++;
		}
	return ret;
	}
