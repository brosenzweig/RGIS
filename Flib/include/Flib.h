/******************************************************************************

GHAAS F Function Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

Flib.h

balazs.fekete@unh.edu

*******************************************************************************/

#ifndef _FLIB_H
#define _FLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*maximum size allocated for each cell*/
#define CELL_SIZE 500
/************************************************
 * FLib header file
 * Shared resource of functions for all of the "F"
 * commands.
 */
/**************Balazs's code*********/
#define FTrue  	1
#define FFalse 	0
#define FSuccess 0
#define FFault  -1

char *FGetLine (char *,int *,FILE *);
int   FGetFieldID (char *,char *);
int   FGetFieldPos (char *,int);
char *FGetField (char *,int,char *,int *);
/************end Balazs's code******/

/*
  FLIB.H
  
  Created by pete weissbrod for complex systems 2002
*/

//structures
struct _fData;
struct _fDataType;

//structure definitions
//---------------------------------------------------
typedef struct _fData
{
  char*** data; //the actual data
  char** fieldNames; //the names of the fields
  int numCols; //number of fields
  int numRows; //number of rows
  struct _fDataType* types;//the type of each column
} fData;
//---------------------------------------------------
typedef enum{ INT, STRING, FLOAT, DATE } TYPE;
//---------------------------------------------------
typedef struct _fDataType
{
  TYPE type;
  int sigDigits;
  int inSigDigits;
} fDataType;
//---------------------------------------------------

//functions
//---------------------------------------------------
/*
  dataFile is the pointer the the tabular data file.
  dataTypesFile is the pointer to the types file (NULL if no types file).
  err - this is used to check if the types in the types file agree
  with the data file. If err == 1, there was a conflict.
  all conflicts are reported to stdout.
*/
fData* readFile(FILE* dataFile, FILE* dataTypesFile, int* err);

/* frees an allocated data table */
void freeFData(fData* r);

/* prints the data table to standard out*/
void printDataTable( const fData t );

/* frees an allocated record */
void freeRow(char** r, const int numCols);

/*returns a list of row one. Secondarily returns the number of columns*/
char** readRowOne( FILE* fp, int* numCols );

/*given an empty char*, it allocates memory and returns a line of data, NULL on EOF*/
char* getRow( FILE* fp );

/* reads a row of data and returns the data in char** form. returns NULL on EOF or error.*/
char** readRow( char* rowPtr, const int numCols );

/*reads the data table, given a file pointer, and a fdata struct. returns the number of rows*/
int readFData( FILE* fp, fData* t, const struct _fDataType* );

/*
  Fdata.h
  
  created by pete weissbrod for complex systems of UNH in 2002
  This is a set of commands used for determining data types.
  According to the F functions, there can only be a set of different types
  there is int, string, float, and date.
*/

/* given an array or types, and an array of strings, this modifies the types */
void getDataTypes( const int numCols, fDataType* types, char** rowData );

/* given a string and a pointer to a single datatype, this modifies the type */
void findType( fDataType* out, const char* in );

/* given a data type, this returns a string for the format of the type */
char* formatString( const fDataType t );

/* prints the types of a data table to stdout */
void printDataTypes(  const fData t );

void findNumberType( fDataType* out, const char* in );

int floatCmp( const char* a, const char* b );

int intCmp(const int a, const int b);

//initializes a new fDataType Object
fDataType newFDataType();

int verifyRow(char** data, const fDataType* types, const int numCols);

fData* readDataTypes( FILE* typesFP );

#if defined(__cplusplus)
}
#endif

#endif
