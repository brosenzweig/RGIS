/******************************************************************************

GHAAS F Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

FPrintDataTypes.c

pfw@unh.edu

*******************************************************************************/
/*
  This function simply prints out the data types of a given table.
  If you specify a dataTypes file that already exists, it will print
  out the data type conflicts to stdout.
*/
#include <cm.h>
#include <Flib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define DATAINPUTSHORT "-d"
#define DATAINPUTLONG "--data"

FILE* dataFile = NULL;
FILE* dataTypesFile = NULL;

int findArg( char* shortName, char* longName, int argc, char* argv[] )
{
  int i = 1;
  while( i < argc )
    { if( argv[i] && (!strcmp(argv[i],shortName) || !strcmp(argv[i],longName )) ) return i; i++; }
  return 0; //search failed
}

void setInputFiles( int argc, char* argv [] )
{
  int i,j;
  /* trying to open the data types file */
  if( (i = findArg( DATAINPUTSHORT, DATAINPUTLONG, argc, argv )) && (i + 1 < argc) &&
      access( argv[i+1], R_OK ) == 0 )
    dataTypesFile = fopen( argv[i+1], "rb" );
  
  /* trying to open the data file */
  for( j=1;j<argc;j++ )
    {
      if( ((dataTypesFile != NULL && i+1 != j) || (!dataTypesFile)) && access( argv[j], R_OK ) == 0 )
	dataFile = fopen( argv[j], "rb" );
    }
  if( dataFile == NULL )
    dataFile = stdin;
}



int main( int argc, char* argv[] )
{
  fData* theData;
  int errCheck;

  setInputFiles( argc,argv );
  
  theData = readFile( dataFile, dataTypesFile, &errCheck );
  
  if(errCheck)
    CMmsgPrint (CMmsgUsrError, "errors were reported, but continuing anyways.");
  
  printDataTypes( *theData );
  freeFData( theData );
  
  return 0;
}




