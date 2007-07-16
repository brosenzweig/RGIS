/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

cmBufGetLine.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<cm.h>

#define BLOCKSIZE 128

char *CMbufGetLine (char *buffer, int *size, FILE *fp) {
	char *ptr;
	int cont = false;

	if (buffer == (char *) NULL) {
		if ((buffer = (char *) malloc (BLOCKSIZE * sizeof (char))) == (char *) NULL) {
			perror ("Memory Allocation Error in: FGetLine ()");
			*size = 0;
			return ((char *) NULL);
		}
		else	*size = BLOCKSIZE;
	}

	ptr = buffer;
	do {
		if (fgets (ptr,*size - (ptr - buffer),fp) == NULL) { free (buffer); *size = 0; return ((char *) NULL); }
		if (((int) strlen (buffer) == *size - 1) && (buffer [strlen (buffer) - 1] != '\n')) {
			if ((buffer = (char *) realloc (buffer,(*size + BLOCKSIZE) * sizeof (char))) == (char *) NULL) {
				perror ("Memory Allocation Error in: FGetLine ()");
				*size = 0;
				return ((char *) NULL);
			}
			else *size += BLOCKSIZE;
			ptr = buffer + strlen (buffer);
			cont = true;
		}
		else cont = false;
	} while (cont);

	if (buffer [strlen (buffer) - 1] == '\n') buffer [strlen (buffer) - 1] = '\0';
	return (buffer);
}

char *CMbufTrim (char *buffer) {
	int i, len;

	if (buffer != (char *) NULL) {
		len = strlen (buffer);
		while ((len > 0) && (buffer [len - 1] == ' ')) {
			buffer [len - 1] = '\0';
			len--;
		}
		while ((len > 0) && (buffer [0] == ' ')) {
			for (i = 1;i < len; ++i) buffer [i - 1] = buffer [i];
			buffer [i - 1] = '\0';
			len--;
		}
	}
	return (buffer);
}

char *CMbufStripChar (char *buffer, char ch) {
	int i, len;

	if (buffer != (char *) NULL) {
		len = strlen (buffer);
		if ((len > 2) && (buffer [0] == ch) && (buffer [len - 1] == ch)) {
			for (i = 1;i < len - 1; ++i) buffer [i - 1] = buffer [i];
			buffer [i - 1] = '\0';
		}
	}
	return (buffer);
}
