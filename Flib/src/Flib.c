/******************************************************************************

GHAAS F Function Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

Flib.c

Unknown

*******************************************************************************/
#include <Flib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char delim = '\t';
static int EOFDETECTED;

//----------------------------------------
fData* readFile( FILE* dataFile, FILE* dataTypesFile, int* err )
{
  fData* dataTypes = NULL;
  fData* data;
  int errFlag = 0; //this errflag turns to 1 when there is a type conflict
  
  if( !dataFile )
    { fprintf(stderr,"Flib::readFile: The data file pointer is invalid.\n"); return (fData *) NULL; }
  
  if( dataTypesFile )
    if( (dataTypes = readDataTypes( dataTypesFile )) == NULL )
      { fprintf(stderr,"Flib::readAndVerify: Could not read the dataTypes file.\n"); return (fData *) NULL; }

  data = (fData*)malloc(sizeof(fData));

  if((data->fieldNames = readRowOne( dataFile, &(data->numCols) )) == NULL) return (fData *) NULL;

  /* if we are verifying the types, use it as an argument to reading the data */
  if( dataTypes )
    errFlag = readFData( dataFile, data, dataTypes->types );
  else
    errFlag = readFData( dataFile, data, NULL );

  /* if there was some sort of an error in the reading process */
  if(data->data==NULL) return (fData *) NULL;
  
  *err = errFlag;
  
  return data;
}
//----------------------------------------
void freeRow(char** r, const int numCols)
{
  int i;
  for(i=0;i<numCols;i++) free(r[i]); free(r);
}
//----------------------------------------
void freeFData(fData* r)
{
  int i;
  for(i=0;i<r->numRows;i++) { freeRow((r->data)[i], r->numCols); }
  free(r->data); free(r);
}
//----------------------------------------
char** readRowOne( FILE* fp, int* numCols )
{
  char** res;
  char* rowOne = getRow(fp);
  char* cp;
  int colCount = 1;
  
  cp = rowOne;
  //first pass, count the number of tabs to figure out the number of columns
  while( *cp != '\n' )
    {
      if( *cp == delim ) colCount++;
      cp++;
    }
  //second pass, read the actual column names
  res = readRow( rowOne, colCount );
  free(rowOne);
  *numCols = colCount;
  return res;
}


//=============================================================
//this is just whet I normally use for reading a row from a file.
//==============================================================
char* getRow( FILE* fp )
{
  char* row = (char*)malloc (100000);
  if( fgets(row,100000,fp) == NULL ) return (char *) NULL; return row;
}

//==========================================================
//this function reads in a row of strings
//==========================================================
char** readRow( char* rowPtr, const int numCols )
{
  char** res = (char**)malloc( sizeof(char*) * numCols );
  char* cp = rowPtr;
  char* tempCell = (char*)malloc(500);
  int i,j;

  while(*rowPtr == '\n') rowPtr++;
  if(*rowPtr == '\0') { EOFDETECTED = 1; return (char **) NULL; }
  
  for(i=0;i<numCols;i++) res[i] = (char*)malloc(CELL_SIZE);
  j = 0;
  while( j < numCols )
    { 
      i = 0;
      if(*cp == '\0') return (char **) NULL;
      if(*cp ==delim || *cp == '\n') cp++;
    while( *cp != delim && *cp != '\0' && *cp != '\n' )
      { 
	if(*cp != '\"')
	  tempCell[i++] = *cp; 
	cp++; 
      }
    tempCell[i] = '\0';
    strcpy( res[j++], tempCell );
    
    }
  free(tempCell);
  return res;
}
//==============================================================
//this is the function that reads in the table
//==============================================================
int readFData( FILE* fp, fData* t, const fDataType* types )
{
  char** currentRow;
  char*  rowPtr;
  int rowCount = 0;
  int i;
  int errFlag = 0;//not used, but possibly for future usage. set when data types disagree
  
  //allocate the datatypes table, and set the default values
  t->types = (fDataType*)malloc( sizeof(fDataType) * t->numCols );
  for(i=0;i<t->numCols;i++) { t->types[i].type = INT; t->types[i].sigDigits = 0; t->types[i].inSigDigits = 0; }

  //allocate the first row of data
  t->data = (char***)malloc((sizeof( char** )) );
  
  while(!EOFDETECTED)
    {
      //read in a row
      if((rowPtr = getRow(fp))==NULL) break; //this is not really an error
      //parse the row
      currentRow = readRow( rowPtr, t->numCols );
      if(types != NULL)//check to see if the dataTypes in TYPES are OK with the data read.
	{
	  if(!verifyRow(currentRow, types, t->numCols)) {fprintf(stderr,"On row %d\n", rowCount);errFlag = 1;}
	}
      if(EOFDETECTED) break;
      if(! currentRow )
	{ fprintf(stderr,"Flib::readRawData: I cant read line #%d\n",rowCount); t->data = NULL; return 1;; }
      
      //make space for another row
      t->data = (char***) realloc( t->data, (sizeof(char**) * (rowCount + 1)) );
      //set the values for the row, and increment rowCount
      t->data[rowCount++] = currentRow;
      
      getDataTypes( t->numCols, t->types, currentRow );
      free(rowPtr);
    }

  t->numRows = rowCount;
  return errFlag;
}
//=======================================================
//prints the contents of the table to stdout
void printDataTable( const fData t )
{
  int i,j;
  
  for(i=0;i<t.numCols;i++)
    printf("%s\t",t.fieldNames[i]);
  printf("\n");

  for(i=0;i<t.numRows;i++)
    {
      for(j=0;j<t.numCols;j++)
	printf("%s\t",(t.data[i][j]));
      printf("\n");
    }
}
//========================================================
/*void writeDataTable( FILE* fp )
{
	int i,j;
  
  for(i=0;i<t.numCols;i++)
    fprintf(fp,"%s\t",t.fieldNames[i]);
  fprintf(fp,"\n");

  for(i=0;i<t.numRows;i++)
    {
      for(j=0;j<t.numCols;j++)
	fprintf(fp,"%s\t",(t.data[i][j]));
      fprintf(fp,"\n");
    }
}*/

