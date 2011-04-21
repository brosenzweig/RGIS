/******************************************************************************

GHAAS F Function Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

FData.c

Unknown

*******************************************************************************/
#include <cm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Flib.h>
//----------------------------------------
char* formatString( const fDataType t )
{
  char* ret = (char*)malloc(100);

  switch(t.type)
    {
    case(INT):
      sprintf(ret,"%%%dd",t.sigDigits); break;
    case(STRING):
      sprintf(ret,"%%s"); break;
    case(FLOAT):
      sprintf(ret,"%%%d.%df",(t.sigDigits),(t.inSigDigits)); break;
    default:
      sprintf(ret,"unknown");
    }
  return ret;
}
//---------------------------------------
int is_Integer( const char*in )
{
  long x;
  char* remainder;
  
  x = strtol(in, & remainder,0);
  if(strlen(remainder)) //we have not read in an integer value
    return 0;
  return (int)x;
}
//---------------------------------------
int findFieldNum( const char* key, const char** args )
{
  int i;
  if( is_Integer( key ) ) return( is_Integer( key ) );
  for(i=0; i < ((int)sizeof(args)); i++)
    if(strcmp(args[i],key)==0) return i;
  return 0;
}
//---------------------------------------
fDataType readDataType(char* line)
{
  fDataType t;
  char* val = (char*)malloc(50);
  char* c = line;
  int i = 0;
  t = newFDataType();
  while( (*c != '\t') && (*c != '\n') && (*c != '\0') ) c++;
  if(*c=='\n' || *c=='\0') return t; c++; c++;
  
  if(*c=='s'){ t.type = STRING; free(val); return t; }

  while(*c >= '0' && *c <= '9')  val[i++] = *(c++); val[i] = '\0';
  t.sigDigits = atoi(val);
  
  if(*c != '.'){ t.type = INT; free(val); return t; } c++; i = 0;
  
  while(*c >= '0' && *c <= '9')  val[i++] = *(c++); val[i] = '\0';
  t.inSigDigits = atoi(val);
  t.type = FLOAT; free(val); return t;
}
//=======================================================
fData* readDataTypes( FILE* typesFP )
{
  char* currentLine = (char*)malloc(1000);
  fData* data = (fData*)malloc(sizeof(fData));
  int i=0,j=0;
  char* c;

  if(!typesFP) return NULL;
  if( fgets( currentLine, 100, typesFP ) == NULL ) return NULL;
  
  sscanf( currentLine, "%d", &(data->numCols) );
  data->types = (fDataType*)malloc(sizeof(fDataType) * data->numCols);
  data->fieldNames = (char**)malloc(sizeof(char*) * data->numCols);
  for(i=0;i<data->numCols;i++)data->fieldNames[i] = (char*)malloc(1000);
  i=0;
  while(i<data->numCols)
    {
      if( fgets( currentLine, 100, typesFP ) == NULL ) return NULL;
      c = currentLine; j = 0;
      while(*c!='\t'&&*c!='\0') data->fieldNames[i][j++] = *(c++); 
      data->fieldNames[i][j] = '\0';
      data->types[i++] = readDataType( c );
    }
  free(currentLine);
  return data;
}
//=======================================================
//prints the data types to stdout
void printDataTypes( const fData table )
{
  char** fieldNames = table.fieldNames;
  const fDataType* t = table.types;
  const int numCols = table.numCols;
  int i;
  printf("%d\n",numCols);
  for(i=0;i<numCols;i++)
      printf("%s\t%s\n",fieldNames[i], formatString(t[i]) );
}
//=======================================================
void getDataTypes( const int numCols, fDataType* types, char** rowData )
{
  int i;

  for(i=0;i<numCols;i++)
    findType( &(types[i]), rowData[i] );
}
//=======================================================
void findType( fDataType* out, const char* in )
{  
  double x;
  char* remainder;
  
  //if the length is 0, dont make any assumption about the type
  if(strlen(in) == 0 || strcmp(in,"\0") == 0)
    return;
  
  //else try to convert the string to double and 
  //look for no remaining characters
  x = strtod(in, & remainder);
  if(strcmp(remainder,"") == 0)
    {
      findNumberType( out, in );
      return;
    }
  
  out->type = STRING;
  out->sigDigits = intCmp( strlen(in), out->sigDigits );
      
}
//----------------------------------------
void findNumberType( fDataType* out, const char* in )
{
  long x;
  char* remainder;
  
  x = strtol(in, & remainder,0);
  if(strcmp(remainder,"") == 0) //we have read in an integer value
    {
	out->sigDigits = intCmp( out->sigDigits, strlen(in) );
    }
  else //we have read in a floating point value.
    {
      if( out->type == STRING )//if it is not already declared a string
	{
	  out->sigDigits = intCmp( out->sigDigits, strlen(in) );
	  return;
	}
      if(out->type == FLOAT)//if it is already declared a float
	{
	  out->sigDigits = intCmp( out->sigDigits, (strlen(in) - strlen(remainder)) );
	  out->inSigDigits = intCmp( out->inSigDigits, strlen(remainder) - 1 );
	}
      if(out->type == INT)//changing from an int to a float
	{
	  out->sigDigits = intCmp( out->sigDigits, (strlen(in) - strlen(remainder)) );
	  out->inSigDigits = strlen(remainder) - 1;
	  out->type = FLOAT;
	}
    }
}
//-------------------------------------
int verifyType( const char* data, const fDataType type )
{
  fDataType comp; 
  
  if(type.type==STRING) return 1;
  
  comp = newFDataType();
  findType( &comp, data );
  
  if(comp.type==STRING) {CMmsgPrint (CMmsgUsrError, "\twrong type"); return 0;}
  if(comp.type==FLOAT && type.type==INT){CMmsgPrint (CMmsgUsrError, "\twrong type"); return 0;}
  if(comp.type==INT && type.type==FLOAT)return 1;
  
  if(comp.type==FLOAT)
    {
      if(comp.sigDigits > type.sigDigits) {CMmsgPrint (CMmsgUsrError, "\tsig.digits too low"); return 0;}
      else if(comp.inSigDigits > type.inSigDigits)
	{CMmsgPrint (CMmsgUsrError, "\tinsig.digits too low"); return 0;}
      else return 1;
    }
  if(comp.sigDigits > type.sigDigits) {CMmsgPrint (CMmsgUsrError, "\tsig.digits too low"); return 0;}
  else return 1;
}
//-------------------------------------
int verifyRow(char** data, const fDataType* types, const int numCols)
{
  int i;
  int errFlag = 0;

  for(i=0;i<numCols;i++)
    if(!verifyType(data[i],types[i]))
      { CMmsgPrint (CMmsgUsrError, ": column %d\n", i); errFlag = 1; }
  return errFlag == 0;
}
//-------------------------------------
int intCmp(const int a, const int b){ if(a>b) return a; return b; }
//-------------------------------------
//initializes a new fDataType Object
fDataType newFDataType()
{
  fDataType* t = (fDataType*)malloc(sizeof(fDataType));
  t->type = INT; t->sigDigits = 0; t->inSigDigits = 0;
  return *t;
}










