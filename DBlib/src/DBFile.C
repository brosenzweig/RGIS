/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBFile.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <DB.H>

DBInt DBFileSize (const char *fileName)

	{
	struct stat fileStat;
	
	if (stat (fileName,&fileStat) == DBFault)	return (DBFault);
	return (fileStat.st_size);
	}
