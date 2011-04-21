/******************************************************************************

GHAAS F Function Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

FTableRead.c

balazs.fekete@unh.edu

*******************************************************************************/
#include <cm.h>
#include <Flib.h>

#define BLOCKSIZE 128

char *FGetLine(char *buffer, int *size, FILE *inFile)
	{
	char *ptr;
	int cont = FFalse;

	if (buffer == (char *) NULL)
		{
		if ((buffer = (char *) malloc (BLOCKSIZE * sizeof (char))) == (char *) NULL)
			{
			CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
			*size = 0;
			return ((char *) NULL);
			}
		else	*size = BLOCKSIZE;
		}

	ptr = buffer;
	do
		{
		if (fgets (ptr,*size - (ptr - buffer),inFile) == NULL)
			{ free (buffer); *size = 0; return ((char *) NULL); }
		if (((int) strlen (buffer) == *size - 1) && (buffer [strlen (buffer) - 1] != '\n'))
			{
			if ((buffer = (char *) realloc (buffer,(*size + BLOCKSIZE) * sizeof (char))) == (char *) NULL)
				{
				CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
				*size = 0;
				return ((char *) NULL);
				}
			else *size += BLOCKSIZE;
			ptr = buffer + strlen (buffer);
			cont = FTrue;
			}
		else cont = FFalse;
		} while (cont);

	if (buffer [strlen (buffer) - 1] == '\n') buffer [strlen (buffer) - 1] = '\0';
	return (buffer);
	}

int FGetFieldID(char *buffer,char *searchString)
	{
	int i = 0, colID = 0;

	do	{
		if (strncmp (buffer + i,searchString,strlen (searchString)) == 0) return (colID);
		for (;(i < (int) strlen (buffer)) && (buffer [i] != '\t');++i);
		i++;
		colID++;
		} while (i < (int) strlen (buffer));
	return (FFault);
	}

int FGetFieldPos (char *buffer,int fieldID)
	{
	int i = 0, colID;

	for (colID = 0;colID < fieldID;++colID)
		{
		for (;(i < (int) strlen (buffer)) && (buffer [i] != '\t');++i);
		i++;
		}
	return (i <= (int) strlen (buffer) ? i : FFault);
	}


char *FGetField (char *buffer,int fieldID,char *fieldBuffer,int *fbSize)
	{
	int i, b;
	
	if ((i = FGetFieldPos (buffer,fieldID)) == FFault)
		{
		fprintf (stderr,"Field [%d] not found in: FGetField ()",fieldID);
		free (fieldBuffer);
		*fbSize = 0;
		return ((char *) NULL);
		}
	if (i > (int) strlen (buffer))
		{
		if (fieldBuffer != (char *) NULL) free (fieldBuffer);
		*fbSize = 0;
		return ((char *) NULL);
		}

	for (b = 0;(b < (int) strlen (buffer + i)) && (buffer [b + i] != '\t'); ++b);
	
	if ((b > 0 ? b : 1) > *fbSize)
		{
		if ((fieldBuffer = (char *) realloc (fieldBuffer,(b + 1) * sizeof (char))) == (char *) NULL)
			{
			CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
			*fbSize = 0;
			return ((char *) NULL);
			}
		*fbSize = b + 1;
		}

	strncpy (fieldBuffer,buffer + i,b);
	fieldBuffer[b] = '\0';
	return (fieldBuffer);
	}
