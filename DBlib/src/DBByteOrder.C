/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

DBByteOrder.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

DBInt DBByteOrder ()

	{
	int i;

	i = 1;
	switch (((char *) &i) [0])
		{
		case 0: return (DBByteOrderBIG);
		case 1: return (DBByteOrderLITTLE);
		}
	return (DBByteOrderBIG);
	}

DBInt DBByteOrder (DBInt byteOrder)

	{ return (byteOrder == DBByteOrder () ? true : false); }

void DBByteOrderSwapHalfWord (void *data)

	{ char ch; ch = ((char *) data) [0]; ((char *) data) [0] = ((char *) data) [1]; ((char *) data) [1] = ch; }

void DBByteOrderSwapWord (void *data)

	{
	char ch;
	ch =  ((char *) data) [0];		((char *) data) [0] =  ((char *) data) [3];  ((char *) data) [3] = ch;
	ch =  ((char *) data) [1]; 	((char *) data) [1] =  ((char *) data) [2];  ((char *) data) [2] = ch;
	}

void DBByteOrderSwapLongWord (void *data)

	{
	char ch;
	ch = ((char *) data) [0]; ((char *) data) [0] = ((char *) data) [7]; ((char *) data) [7] = ch;
	ch = ((char *) data) [1]; ((char *) data) [1] = ((char *) data) [6]; ((char *) data) [6] = ch;
	ch = ((char *) data) [2]; ((char *) data) [2] = ((char *) data) [5]; ((char *) data) [5] = ch;
	ch = ((char *) data) [3]; ((char *) data) [3] = ((char *) data) [4]; ((char *) data) [4] = ch;
	}
