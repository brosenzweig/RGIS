/******************************************************************************

GHAAS F Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

FExpDBObj.C

Unknown

*******************************************************************************/
#include <Flib.h>
#include <DB.H>
#include <cm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define DATAINPUTSHORT "-d"
#define DATAINPUTLONG "--data"

DBObjData* exportDBObj( const fData* theData );
int findNearestPowerOfTwo( int x );

void showUsage( char *arg0 )
{
	CMmsgPrint (CMmsgInfo, "This function will read any ascii table from stdin, and");
	CMmsgPrint (CMmsgInfo, "output the RGIS table equivalent to stdout.");
	CMmsgPrint (CMmsgInfo, "Usage:%s [-o] <inputFileName>",CMprgName(arg0));
	CMmsgPrint (CMmsgInfo, "  -o,--output    = Output to specified file instead of stdout");
	CMmsgPrint (CMmsgInfo, "  -D,--descFile  = Read column types from this file");
	CMmsgPrint (CMmsgInfo, "  -p,--printDT   = Print out column types before writing output");
	CMmsgPrint (CMmsgInfo, "<inputFileName> := <FileName> | -");
	exit(1);
}

int main( int argc, char* argv[] )
{
	DBObjData *dbData;
	fData *theData;
	int errCheck;
	int argPos, argNum = argc;
	FILE *outFile = (FILE *) NULL, *inFile = (FILE *) NULL, *descFile = (FILE *) NULL;
	bool printDT = false;
	
	for (argPos = 1;argPos < argNum;)
		{
		if (CMargTest(argv[argPos],"-o","--output"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing output filename!"); return (CMfailed); }
			if((outFile = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ CMmsgPrint (CMmsgUsrError, "Cannot open file %s",argv[argPos]); return(DBFault); }
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-D","--descfile"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing description filename!"); return (CMfailed); }
			if((descFile = fopen(argv[argPos],"r")) == (FILE *) NULL)
				{ CMmsgPrint (CMmsgUsrError, "Cannot open file %s",argv[argPos]); return(DBFault); }
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-p","--printdt"))
			{
			printDT = true;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv[argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 1)
		{
		if (strcmp(argv[1],"-") == 0) { inFile = stdin; } else { inFile = fopen(argv[1],"r"); }
		if(((theData = readFile( inFile , descFile, &errCheck )) == (fData *) NULL) || errCheck)
			{ CMmsgPrint (CMmsgUsrError, "errors were reported, halting."); return -1; }
		}
	else showUsage(argv[0]);

	if(outFile == (FILE *) NULL) { outFile = stdout; }

	if(printDT) printDataTypes(*theData);

	dbData = exportDBObj( theData );
	freeFData( theData );

	dbData->Write(outFile);
  	return 0;
}

/* this is the function that converts FLib data tables to rgis DBObjects */
DBObjData* exportDBObj( const fData* theData )
{
	DBObjData *data = new DBObjData ("",DBTypeTable);
	DBObjTable *table = data->Table (DBrNItems);
	DBObjTableField *field;
	DBObjRecord *record;
	int intValue;
	double floatValue;

	int i,j;
	
	/*FIRST READ THE COLUMN NAMES AND THE DATA TYPES*/
	
	for(i=0;i<theData->numCols;i++)
	{
		switch(theData->types[i].type)
		{
		case(INT):
		field = new DBObjTableField(theData->fieldNames[i], DBTableFieldInt, formatString( theData->types[i] ),
				sizeof (DBInt), false);
		break;
		case(FLOAT):
		field = new DBObjTableField(theData->fieldNames[i], DBTableFieldFloat, formatString( theData->types[i] ),
				sizeof (DBFloat), false);
		break;
		case(STRING):
		field = new DBObjTableField(theData->fieldNames[i], DBTableFieldString, "%s",
				findNearestPowerOfTwo( theData->types[i].sigDigits + 1 ), false);
		break;
		default: CMmsgPrint (CMmsgUsrError, "FAscii2GDBC::createRGISTable: strange things are heppening here!!"); break;
		}
		table->AddField (field);
	}
	
	/*SECOND, READ THE DATA TABLE*/	
	/*for each row*/
	for(i=0;i<theData->numRows;i++)
	{
		record = table->Add ();
		
		/*for each column*/
		for(j=0;j<theData->numCols;j++)
		{
			/*if the data cell is empty*/
			if(strlen(theData->data[i][j])==0)
			{
				switch(theData->types[j].type)
				{
				default:
				case(STRING): table->Field(j)->String (record,"");                             break;
				case(FLOAT):  table->Field(j)->Float (record, table->Field(j)->FloatNoData()); break;
				case(INT):    table->Field(j)->Int (record, table->Field(j)->IntNoData());     break;

				}
			}
			else
			{/*the data cell has a value*/
				switch (theData->types[j].type)
				{
				default:
				case (STRING): table->Field(j)->String (record,theData->data[i][j]);	break;
				case (INT):
					sscanf (theData->data[i][j],"%d",&intValue); 
					table->Field(j)->Int (record,intValue);
					break;
				case (FLOAT):
					sscanf (theData->data[i][j],"%lf",&floatValue);
					table->Field(j)->Float (record,floatValue);
					break;
				/*
				case DBTableFieldDate:
					dateValue.Set (fieldToken);
					fields [fieldID]->Date (record,dateValue);
					break;
				*/
				}
			}
		}/*for each column*/
	}/*for each row*/

	/*return the Table*/
	return data;
}

/* this is a simple function to find the storage length of a given string in RGIS. */
int findNearestPowerOfTwo( int x )
{
	int i = 1;
	while( i < x ) i*=2;
	return i;
}
