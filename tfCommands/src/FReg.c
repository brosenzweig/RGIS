/******************************************************************************

GHAAS F Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

FReg.c

pfw@unh.edu

*******************************************************************************/
#define MISSINGDATA -9999

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <DBtf.h>
#include <Flib.h>

#define GROUPSHOURT "-g"
#define GROUPLONG "--group"
#define XSHORT "-x"
#define XLONG  "--xField"
#define YSHORT "-y"
#define YLONG  "--yField"
#define WEIGHTSHORT "-w"
#define WEIGHTLONG  "--weight"
#define HELPSHORT "-h"
#define HELPLONG "--help"
#define DATAINPUTSHORT "-d"
#define DATAINPUTLONG "--data"
#define WEIGHTSHORT "-w"
#define WEIGHTLONG "--weight"

/*****globals***/
FILE* dataFile = NULL; /*this will point to the actual data file*/
FILE* dataTypesFile = NULL; /*this points to the data types file (If there is one)*/

/*****functions****/
void reg( const int xFieldNum, const int yFieldNum, const int wFieldNum, const fData* theData );
int isMissingDataVal(float x){return (DBMathEqualValues(x,MISSINGDATA));}

void showUsage()
	{
	printf("Calculates linear regression.\n");
	printf("Usage:\n");
	printf("regf -g [field] -x [field]] -y [field] [inputfile] ...\n\n");
	printf("where fieldnames must match fieldnames in first line of the datafile.\n");
	printf("Operations are:\n");
	printf("-g, --group  field\n");
	printf("Perform selected operations on data groups, selected by having\n");
	printf("the same value in the group field.  Process whole\n");
	printf("input if no group is given.  Select groups based on groups of\n");
	printf("fields if multiple groups are given.\n");
	printf("-x, --xField  field\n");
	printf("x variable field\n");
	printf("-y, --yField  field\n");
	printf("y variable field\n");
	printf("-w, --weight field\n");
	printf("-h, --help\n");
	printf("Print this usage information.\n");
	printf("The output fields are:\n");
	printf("b0 - Intercept;\n");
	printf("b1 - Slope;\n");
	printf("r  - (x,y) correlation coefficient;\n");
	printf("R2 - squared multiple (y,yHat) correlation coefficient;\n");
	printf("n  - number of observation;\n");
	printf("S - square root of sum of the residuals sqrt (SSE/(n-2));\n");
	printf("b0Bound - confidence boundary for intersept;\n");
	printf("b1Bound - confidence boundary for slope;\n");
	}

/* findarg is a simple serch function that finds arguments in argv[]*/
int findArg( char* shortName, char* longName, int argc, char* argv[] )
{
  int i = 1;
  while( i < argc )
    { 
	    if( argv[i] && (!strcmp(argv[i],shortName) || !strcmp(argv[i],longName )) ) 
	    	return i; 
	    i++; 
    }
  return 0; 
}

/* setInputFiles is a function that automatically finds the data file and the
   data type file (if there is one)*/

void setInputFiles( int argc, char* argv [] )
{
  int i,j;
  /* trying to open the data types file */
  if( 
     (i = findArg( DATAINPUTSHORT, DATAINPUTLONG, argc, argv )) &&
     (i + 1 < argc) &&
     access( argv[i+1], R_OK ) == 0 
    )
    dataTypesFile = fopen( argv[i+1], "rb" );
  
  /* trying to open the data file */
  for( j=1;j<argc;j++ )
    {
      if( access( argv[j], R_OK ) == 0 )
	if( !dataTypesFile || j != i+1 )
	  dataFile = fopen( argv[j], "rb" );
    }
  if( dataFile == NULL )
    dataFile = stdin;
}

/*given a fieldName, this function will return the field number in the table,
 * -1 on failure.
 */
int getFieldNum(const fData* theData, const char* name)
{
  int i;
  for(i=0;i<theData->numCols;i++)
    if(!strcmp(theData->fieldNames[i], name)) return i;
  return -1; 
}

/* main function */
int main( int argc, char* argv[] )
{
  fData* theData;
  int errCheck;
  int xField = -1; 
  int yField = -1;
  int weightField = -1;

  if(argc<=1)
    { showUsage(); return 0; }

  setInputFiles( argc,argv );
  
  theData = readFile( dataFile, dataTypesFile, &errCheck );
  
  if(errCheck)
    fprintf(stderr,"errors were reported, but continuing anyways.\n");
  
  xField = findArg(XSHORT,XLONG,argc,argv);
  yField = findArg(YSHORT,YLONG,argc,argv);
  weightField = findArg(WEIGHTSHORT,WEIGHTLONG,argc,argv);

  if( xField && xField+1 < argc )
	xField = getFieldNum( theData, argv[xField+1] );
  else	xField = -1;
  
  if( yField && yField+1 < argc )
	yField = getFieldNum( theData, argv[yField+1] );
  else	yField = -1;
  
  if( weightField && weightField+1 < argc )
	weightField = getFieldNum( theData, argv[weightField+1] );
  else	weightField = -1;

  if(xField == -1){ fprintf(stderr,"Could not get the X field\n"); return 0; }
  if(yField == -1){ fprintf(stderr,"Could not get the Y field\n"); return 0; }
  /*(weight field is not mandatory)*/
  
  reg( xField, yField, weightField, theData );

  return 0;
}


/*REGRESSION FUNCTION*/
/*this is the function where all of the numeric calculations are done*/
void reg( const int xFieldNum, const int yFieldNum, const int wFieldNum, const fData* theData )
{
  float sumX = 0;
  float sumY = 0;
  float sumX2 = 0;
  float sumY2 = 0;
  float sumXY = 0;
  float sumW = 0;
  int i;
  
  if( wFieldNum == -1 ) /*all records are equally weighted*/
  { 
	float x;
	float y;
	for(i=0;i<theData->numRows;i++)
	{
		x = strtod(theData->data[i][xFieldNum], NULL);
		y = strtod(theData->data[i][yFieldNum], NULL);

		if(!isMissingDataVal(x) && !isMissingDataVal(y))
		{
			sumX += x;
			sumY += y;
			sumX2 += x*x;
			sumY2 += y*y;
			sumXY += x*y;
			sumW++;
		}
	}
  }
  else   /*there is a weight field that we must factor in*/
  {
	float w;
	float x;
	float y;
	
  	for(i=0;i<theData->numRows;i++)
	{	
		w = strtod(theData->data[i][wFieldNum],NULL);
		x = strtod(theData->data[i][xFieldNum], NULL);
		y = strtod(theData->data[i][yFieldNum], NULL);

		if(!isMissingDataVal(x) && !isMissingDataVal(y))
		{
			sumX += x * w;
			sumY += y * w;
			sumX2 += x*x * w;
			sumY2 += y*y * w;
			sumXY += x*y * w; 
			sumW += w;
		}
	}	
  }
  
  /*I added this in so we dont get any "divide by zero" fatal errors*/
  if(sumW==0)
  	{ fprintf(stderr,"I could not read the \"weight field\" as numeric values.\n"); return; }
  if(sumX==0)
  	fprintf(stderr,"Warning: it looks like the X field is not numeric\n");
  if(sumY==0)
	fprintf(stderr,"Warning: it looks like the Y field is not numeric\n");
  

  {
	 /*
	  * this is based upon the equation for linear regression
	  * slope = n*SUM(xy) - SUM(x)*SUM(y) / n*SUM(x*x) - SUM(x)*SUM(x)
	  * intercept = (SUM(y) - slope*SUM(x))/n
	  */
	   
	float b1 = ((sumW*sumXY) - (sumX*sumY)) / ((sumW*sumX2) - (sumX*sumX));
	
      	float b0 = (sumY - (b1 * sumX)) / sumW;
	
	float r = (sumXY - sumX * sumY / sumW) / sqrt ((sumX2 - sumX * sumX / sumW) * (sumY2 - sumY * sumY / sumW ));

	float se2 = (sumY2 - 2 * b1 * sumXY - 2 * b0 * sumY + 2 * b0 * b1 * sumX + b1 * b1 * sumX2 + sumW * b0 * b0);

	float R2 = 1.0 - se2 / (sumY2 - sumY * sumY / sumW);

	float S = sqrt(se2 / (sumW - 2));
	
	float t, b0Bound, b1Bound;

	se2 = se2 / (sumW - 2 );

	t = DBMathStudentsT((double) sumW - 2 );
	
 	b0Bound = t * sqrt( sumX2 / ( sumW * ( sumX2 - sumX * sumX / sumW ))) * S;

	b1Bound = t * sqrt( se2 / ( sumX2 - sumX * sumX / sumW ));

	printf("\"bo\"\t\"b1\"\t\"r\"\t\"R2\"\t\"N\"\t\"S\"\t\"b0Bound\"\t\"b1Bound\"\n");
	printf("%f\t%f\t%f\t%f\t%f  \t%f\t%f\t%f\n", b0, b1, r, R2, sumW, S, b0Bound, b1Bound );

  }
}

