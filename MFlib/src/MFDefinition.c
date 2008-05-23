/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

MFDomain.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <string.h>
#include <cm.h>
#include <MF.h>

static int _MFDefLevel = 0;

void MFDefEntering (char *text) {
	char formatStr [MFNameLength];

	sprintf (formatStr,"%c%ds %cs\n",'%',_MFDefLevel * 3 + (int) strlen ("Entering"),'%');
	CMmsgPrint (CMmsgInfo,formatStr,"Entering:",text);
	_MFDefLevel++;
}

void MFDefLeaving (char *text) {
	char formatStr [MFNameLength];

	_MFDefLevel--;
	sprintf (formatStr,  "%c%ds %cs\n",'%',_MFDefLevel * 3 + (int) strlen ("Entering"),'%');
	CMmsgPrint (CMmsgInfo,formatStr,"Leaving:",text);
}
